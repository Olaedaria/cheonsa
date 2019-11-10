#include "cheonsa_scene.h"
#include "cheonsa_scene_object.h"
#include "cheonsa_scene_component.h"
#include "cheonsa_scene_component_model.h"
#include "cheonsa_scene_component_sprite.h"
#include "cheonsa_scene_component_sound.h"
#include "cheonsa_scene_component_light_probe.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	scene_pick_c::scene_pick_c()
		: depth( 0.0f )
		, component()
	{
	}

	float64_c scene_pick_c::quick_sort_function( scene_pick_c const & pick )
	{
		return pick.depth;
	}

	scene_c::scene_c()
		: _user_interface( nullptr )
		, _scene_camera()
		, _scene_object_list()
		, _scene_component_tree()
		, _global_lights_list()
		, _local_lights_tree()
		, _render_frame( 1 )
		, _time_counter( 0.0 )
		, _random_number( 0.0 )
		, _audio_scene( nullptr )
		, _audio_scene_listener( nullptr )
		, _physics_scene( nullptr )
		, _light_probe_list()
		, _last_baked_light_probe( nullptr )
		, sky_models()
		, colors()
		, _suspend_automatic_light_probe_invalidation( false )
		, _light_probe_clip_near( 0.05f )
		, _light_probe_clip_far( 1000.0f )
		, _pick_on_next_render( false )
		, _pick_canvas_coordinates()
		, _pick_list_original()
		, _pick_list_final()
		, _target_pick_depth( nullptr )
		, _target_pick_depth_readable( nullptr )
		, _user_pointer( nullptr )
	{
		_scene_component_tree.initialize( 512.0, 5 ); // 512, 256, 128, 64, 32
		_local_lights_tree.initialize( 512.0, 5 );

		_audio_scene_listener = new audio2_scene_listener_c();

		_audio_scene = new audio2_scene_c();
		_audio_scene->set_scene_listener( _audio_scene_listener );

		_physics_scene = new physics_scene_c();
	}

	scene_c::~scene_c()
	{
		_audio_scene->remove_reference();
		_audio_scene = nullptr;

		_audio_scene_listener->remove_reference();
		_audio_scene_listener = nullptr;

		delete _physics_scene;
		_physics_scene = nullptr;
	}

	scene_camera_c & scene_c::get_scene_camera()
	{
		return _scene_camera;
	}

	void_c scene_c::update_audio( float32_c time_step )
	{
		_audio_scene_listener->set_world_space_transform( _scene_camera.get_world_space_transform() );
		core_linked_list_c< scene_component_sound_c * >::node_c const * sound_list_node = _sound_list.get_first();
		while ( sound_list_node != nullptr )
		{
			scene_component_sound_c * sound = sound_list_node->get_value();
			sound_list_node = sound_list_node->get_next();
			sound->update_audio( time_step );
		}
	}

	void_c scene_c::add_object( scene_object_c * object )
	{
		assert( object );
		assert( !object->_scene );
		_scene_object_list.insert_at_end( &object->_scene_object_list_node );
		object->_scene = this;
		object->_handle_after_added_to_scene();
	}

	void_c scene_c::remove_object( scene_object_c * object )
	{
		assert( object );
		assert( object->_scene == this );
		object->_handle_before_removed_from_scene();
		_scene_object_list.remove( &object->_scene_object_list_node );
		object->_scene = nullptr;
	}

	void_c scene_c::add_light( scene_light_c * light )
	{
		assert( light->_scene == nullptr );
		light->_scene = this;
		if ( light->_type == scene_light_type_e_direction )
		{
			_global_lights_list.insert_at_end( light );
		}
		else
		{
			_local_lights_tree.insert_or_update_item( light );
		}
		invalidate_light_probes_with_light( light );
	}

	void_c scene_c::remove_light( scene_light_c * light )
	{
		assert( light->_scene == this );
		invalidate_light_probes_with_light( light );
		light->_scene = nullptr;
		if ( light->_type == scene_light_type_e_direction )
		{
			_global_lights_list.remove( light );
		}
		else
		{
			_local_lights_tree.remove_item( light );
		}
	}

	void_c scene_c::_gather_pickables_along_segment( segment64_c const & pick_segment, core_list_c< scene_component_model_c * > & models_of_interest, core_list_c< scene_component_sprite_c * > & sprites_of_interest )
	{
		_pick_list_original.remove_all();

		float64_c t;

		for ( sint32_c i = 0; i < models_of_interest.get_length(); i++ )
		{
			scene_component_model_c * model = models_of_interest[ i ];

			if ( ops::sweep_point_vs_box( pick_segment, box64x3_c( model->get_local_space_obb() ), model->get_scene_object()->get_world_space_transform(), t ) )
			{
				scene_pick_c * pick = _pick_list_original.emplace_at_end();
				pick->component = model;
			}
		}

		for ( sint32_c sprite_index = 0; sprite_index < sprites_of_interest.get_length(); sprite_index++ )
		{
			scene_component_sprite_c * sprite = sprites_of_interest[ sprite_index ];
			if ( ops::sweep_point_vs_sphere( pick_segment, sphere64_c( sprite->get_scene_object()->get_world_space_transform().position, sprite->_radius ), t ) )
			{
				scene_pick_c * pick = _pick_list_original.emplace_at_end();
				pick->component = sprite;
			}
		}
	}

	void_c scene_c::pixel_pick_during_next_render( vector32x2_c const & screen_coordinates )
	{
		_pick_on_next_render = true;
		_pick_canvas_coordinates = screen_coordinates;
	}

	scene_component_c * scene_c::get_pixel_pick_result_component() const
	{
		if ( _pick_list_final.get_length() == 0 )
		{
			return nullptr;
		}
		return _pick_list_final[ 0 ].component;
	}

	scene_object_c * scene_c::get_pixel_pick_result_object( scene_object_c * after_object ) const
	{
		if ( _pick_list_final.get_length() == 0 )
		{
			return nullptr;
		}

		if ( after_object == nullptr )
		{
			return _pick_list_final[ 0 ].component->get_scene_object();
		}

		sint32_c result_index = -1;
		boolean_c found_after_object = false;
		for ( sint32_c i = 0; i < _pick_list_final.get_length(); i++ )
		{
			if ( _pick_list_final[ i ].component->get_scene_object() == after_object )
			{
				found_after_object = true;
			}
			else if ( found_after_object )
			{
				break;
			}
		}
		if ( result_index < 0 || result_index >= _pick_list_final.get_length() )
		{
			result_index = 0;
		}

		return _pick_list_final[ result_index ].component->get_scene_object();
	}

	void_c scene_c::suspend_automatic_light_probe_invalidation()
	{
		_suspend_automatic_light_probe_invalidation = true;
	}

	void_c scene_c::resume_automatic_light_probe_invalidation()
	{
		_suspend_automatic_light_probe_invalidation = false;
	}

	void_c scene_c::invalidate_light_probes_every_where()
	{
		core_linked_list_c< scene_component_light_probe_c * >::node_c const * light_probe_list_node = _light_probe_list.get_first();
		while ( light_probe_list_node )
		{
			light_probe_list_node->get_value()->_is_up_to_date = false;
			light_probe_list_node = light_probe_list_node->get_next();
		}
	}

	void_c scene_c::invalidate_light_probes_with_light( scene_light_c * light )
	{
		if ( light && light->_scene && !light->_scene->_suspend_automatic_light_probe_invalidation )
		{
			if ( light->_type == scene_light_type_e_direction )
			{
				light->_scene->invalidate_light_probes_every_where();
			}
			else if ( light->_type == scene_light_type_e_point || light->_type == scene_light_type_e_cone )
			{
				light->_scene->invalidate_light_probes_with_box( box64x3_c( light->_local_space_obb ), light->_world_space_transform );
			}
		}
	}

	void_c scene_c::invalidate_light_probes_with_box( box64x3_c const & bounding_box, space_transform_c const & bounding_box_transform )
	{
		core_linked_list_c< scene_component_light_probe_c * >::node_c const * light_probe_list_node = _light_probe_list.get_first();
		while ( light_probe_list_node )
		{
			vector64x3_c light_probe_position = light_probe_list_node->get_value()->get_scene_object()->_world_space_transform.position;
			vector64x3_c extent = vector64x3_c( _light_probe_clip_far, _light_probe_clip_far, _light_probe_clip_far );
			box64x3_c light_probe_box = box64x3_c( light_probe_position - extent, light_probe_position + extent ); // light probes are shaped like axis aligned boxes, not spheres.
			if ( ops::intersect_box_vs_box( light_probe_box, space_transform_c(), bounding_box, bounding_box_transform ) )
			{
				light_probe_list_node->get_value()->_is_up_to_date = false;
			}
			light_probe_list_node = light_probe_list_node->get_next();
		}
	}

	void_c scene_c::invalidate_light_probes_with_before_and_after_box( box64x3_c const & bounding_box, space_transform_c const & before_bounding_box_transform, space_transform_c & after_bounding_box_transform )
	{
		core_linked_list_c< scene_component_light_probe_c * >::node_c const * light_probe_list_node = _light_probe_list.get_first();
		while ( light_probe_list_node )
		{
			vector64x3_c light_probe_position = light_probe_list_node->get_value()->get_scene_object()->get_world_space_transform().position;
			vector64x3_c extent = vector64x3_c( _light_probe_clip_far, _light_probe_clip_far, _light_probe_clip_far );
			box64x3_c light_probe_box = box64x3_c( light_probe_position - extent, light_probe_position + extent ); // light probes are shaped like axis aligned boxes, not spheres.
			if ( ops::intersect_box_vs_box( light_probe_box, space_transform_c(), bounding_box, before_bounding_box_transform ) || ops::intersect_box_vs_box( light_probe_box, space_transform_c(), bounding_box, after_bounding_box_transform ) )
			{
				light_probe_list_node->get_value()->_is_up_to_date = false;
			}
			light_probe_list_node = light_probe_list_node->get_next();
		}
	}

	void_c scene_c::invalidate_light_probes_with_sphere( sphere64_c const & sphere )
	{
		core_linked_list_c< scene_component_light_probe_c * >::node_c const * light_probe_list_node = _light_probe_list.get_first();
		while ( light_probe_list_node )
		{
			vector64x3_c light_probe_position = light_probe_list_node->get_value()->get_scene_object()->get_world_space_transform().position;
			vector64x3_c extent = vector64x3_c( _light_probe_clip_far, _light_probe_clip_far, _light_probe_clip_far );
			box64x3_c light_probe_box = box64x3_c( light_probe_position - extent, light_probe_position + extent ); // light probes are shaped like axis aligned boxes, not spheres.
			if ( ops::intersect_box_vs_sphere( light_probe_box, sphere ) )
			{
				light_probe_list_node->get_value()->_is_up_to_date = false;
			}
			light_probe_list_node = light_probe_list_node->get_next();
		}
	}

	void_c scene_c::invalidate_light_probes_with_before_and_after_sphere( sphere64_c const & before_sphere, sphere64_c const & after_sphere )
	{
		core_linked_list_c< scene_component_light_probe_c * >::node_c const * light_probe_list_node = _light_probe_list.get_first();
		while ( light_probe_list_node )
		{
			vector64x3_c light_probe_position = light_probe_list_node->get_value()->get_scene_object()->get_world_space_transform().position;
			vector64x3_c extent = vector64x3_c( _light_probe_clip_far, _light_probe_clip_far, _light_probe_clip_far );
			box64x3_c light_probe_box = box64x3_c( light_probe_position - extent, light_probe_position + extent ); // light probes are shaped like axis aligned boxes, not spheres.
			if ( ops::intersect_box_vs_sphere( light_probe_box, before_sphere ) || ops::intersect_box_vs_sphere( light_probe_box, after_sphere ) )
			{
				light_probe_list_node->get_value()->_is_up_to_date = false;
			}
			light_probe_list_node = light_probe_list_node->get_next();
		}
	}

	scene_component_light_probe_c * scene_c::find_light_probe_with_point( vector64x3_c const & position )
	{
		core_linked_list_c< scene_component_light_probe_c * >::node_c const * light_probe_list_node = _light_probe_list.get_first();
		if ( light_probe_list_node == nullptr )
		{
			return nullptr;
		}
		scene_component_light_probe_c * closest_value = light_probe_list_node->get_value();
		float64_c closest_value_distance = 0.0;
		light_probe_list_node = light_probe_list_node->get_next();
		while ( light_probe_list_node )
		{
			float64_c value_distance = ops::make_float64_length( light_probe_list_node->get_value()->get_scene_object()->get_world_space_transform().position - position );
			if ( value_distance < closest_value_distance )
			{
				closest_value = light_probe_list_node->get_value();
				closest_value_distance = value_distance;
			}
			light_probe_list_node = light_probe_list_node->get_next();
		}
		return closest_value;
	}

	//boolean_c scene_c::physics_ray_cast( vector64x3_c & result, segment64_c const segment )
	//{
	//	physics_sweep_test_result_c sweep_test_result;
	//	uint16_c collision_filter_group = physics_layer_e_dynamic;
	//	uint16_c collision_filter_mask = physics_get_layer_mask_for_layer( collision_filter_group );
	//	_physics_scene.find_nearest_hit( sweep_test_result, collision_filter_group, collision_filter_mask, segment.point_a, segment.point_b );
	//	if ( sweep_test_result.hit_list.get_length() )
	//	{
	//		result = sweep_test_result.hit_list[ 0 ].position;
	//		return true;
	//	}
	//	return false;
	//}

	user_interface_c * scene_c::get_user_interface() const
	{
		return _user_interface;
	}

	audio2_scene_c * scene_c::get_audio_scene()
	{
		return _audio_scene;
	}

	physics_scene_c * scene_c::get_physics_scene()
	{
		return _physics_scene;
	}

	core_linked_list_c< scene_object_c * > const & scene_c::get_scene_object_list() const
	{
		return _scene_object_list;
	}

	scene_tree_octo_c< scene_component_c * > const & scene_c::get_scene_component_tree() const
	{
		return _scene_component_tree;
	}

	void_c * scene_c::get_user_pointer()
	{
		return _user_pointer;
	}

	void_c scene_c::set_user_pointer( void_c * value )
	{
		_user_pointer = value;
	}

}
