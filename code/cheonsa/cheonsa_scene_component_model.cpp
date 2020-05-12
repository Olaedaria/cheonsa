#include "cheonsa_scene_component_model.h"
#include "cheonsa_scene_component_light_probe.h"
#include "cheonsa_scene_object.h"
#include "cheonsa_scene.h"
#include "cheonsa_ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{




	//
	// mother/daughter relations.
	//

	scene_component_model_c * scene_component_model_c::get_mother_model_root()
	{
		scene_component_model_c * result = this;
		while ( result->_mother_model )
		{
			result = result->_mother_model;
		}
		return result;
	}

	scene_component_model_c * scene_component_model_c::get_mother_model()
	{
		return _mother_model;
	}

	void_c scene_component_model_c::add_daughter_model( scene_component_model_c * model )
	{
		assert( model != nullptr );
		assert( model->_mother_model == nullptr );
		model->add_reference();
		model->_mother_model = this;
		_daughter_model_list.insert( -1, model );
		_update_bone_skin_matrix_list_recursive();
		_restart_bone_logics_recursive();
	}

	void_c scene_component_model_c::remove_daughter_model( scene_component_model_c * model )
	{
		assert( model );
		assert( model->_mother_model == this );
		_daughter_model_list.remove_value( model );
		model->_mother_model = nullptr;
		if ( !model->remove_reference() )
		{
			model->_update_bone_skin_matrix_list_recursive();
			model->_restart_bone_logics_recursive();
		}
	}




	//
	// model file resource binding.
	//

	void_c scene_component_model_c::_handle_model_resource_on_loaded( resource_file_c * resource )
	{
		assert( resource == _model_resource );
		assert( _model_resource_is_bound == false );
		assert( _model_resource.get_is_value_set_and_loaded() == true );

		_model_resource_is_bound = true;

		_local_space_obb = box32x3_c( _model_resource->get_data().mesh_box );

		// instantiate meshes.
		if ( _model_resource->get_data().mesh_list.get_length() )
		{
			_mesh_list.set_length_absolute( _model_resource->get_data().mesh_list.get_length() );
			for ( sint32_c i = 0; i < _mesh_list.get_length(); i++ )
			{
				mesh_c * mesh = &_mesh_list[ i ];
				mesh->_mother_model = this;
				mesh->_render_enabled = true;
				mesh->_source_material_map_thing = nullptr;
				mesh->_custom_material_assignment = nullptr;
				mesh->_source_mesh = &_model_resource->get_data().mesh_list[ i ];

				//// instantiate shape keys.
				//for ( sint32_c j = 0; j < _model_resource->get_data().mesh_shape_key_list.get_length(); j++ )
				//{
				//	mesh_c::shape_key_c * mesh_shape_key = mesh->_shape_key_list.emplace_at_end();
				//	mesh_shape_key->_mother_mesh = mesh;
				//	mesh_shape_key->_source_mesh_shape_key = &_model_resource->get_data().mesh_shape_key_list[ j ];
				//	mesh_shape_key->_value = _model_resource->get_data().mesh_shape_key_list[ j ].minimum_value;
				//}
			}
		}

		// apply material map materials.
		if ( _materials_resource.get_is_value_set_and_loaded() )
		{
			_handle_materials_resource_on_loaded( _materials_resource );
		}

		// apply custom material assignments.
		apply_custom_material_list();

		// instantiate bones.
		if ( _model_resource->get_data().bone_list.get_length() > 0 )
		{
			_bone_list.set_length_absolute( _model_resource->get_data().bone_list.get_length() );
			for ( sint32_c i = 0; i < _bone_list.get_length(); i++ )
			{
				bone_c * bone = &_bone_list[ i ];
				bone->_mother_model = this;
				bone->_source_bone = &_model_resource->get_data().bone_list[ i ];
				bone->_source_bone_extras = &_model_resource->get_data().bone_extras_list[ i ];
				sint32_c mother_bone_index = _model_resource->find_bone_index_with_bone_name( bone->_source_bone->mother_bone_name );
				if ( mother_bone_index >= 0 )
				{
					assert( mother_bone_index < i );
					bone->_mother_bone = &_bone_list[ mother_bone_index ];
					bone->_mother_bone->_daughter_bone_list.insert_at_end( &bone->_daughter_bone_list_node );
				}
				else
				{
					_root_bone_list.insert_at_end( &bone->_daughter_bone_list_node );
				}
			}
		}

		// initialize bone skin matrix list.
		_update_bone_skin_matrix_list_recursive();

		// instantiate bone logics.
		for ( sint32_c i = 0; i < _model_resource->get_data().bone_logic_list.get_length(); i++ )
		{
			bone_logic_c * bone_logic = _instantiate_new_bone_logic( &_model_resource->get_data().bone_logic_list[ i ] );
			if ( bone_logic != nullptr )
			{
				_bone_logic_list.insert( -1, bone_logic );
			}
		}

		// instantiate attachment points.
		if ( _model_resource->get_data().attachment_point_list.get_length() > 0 )
		{
			sint32_c count = _model_resource->get_data().attachment_point_list.get_length();
			_attachment_point_list.set_length_absolute( count );
			for ( sint32_c i = 0; i < count; i++ )
			{
				attachment_point_c * attachment_point = &_attachment_point_list[ i ];
				attachment_point->_source_attachment_point = &_model_resource->get_data().attachment_point_list[ i ];
				sint32_c bone_index = _model_resource->find_bone_index_with_bone_name( attachment_point->_source_attachment_point->mother_bone_name );
				attachment_point->_mother_bone = bone_index >= 0 ? &_bone_list[ bone_index ] : nullptr;
				transform3d_c object_space_rest_transform;
				object_space_rest_transform.position = vector64x3_c( attachment_point->_source_attachment_point->position );
				object_space_rest_transform.rotation = quaternion32_c( attachment_point->_source_attachment_point->rotation );
				object_space_rest_transform.set_uniform_scale( attachment_point->_source_attachment_point->scale );
				attachment_point->_local_space_transform = object_space_rest_transform * attachment_point->_mother_bone->_source_bone_extras->object_space_rest_transform_inverted;
			}
		}

		// instantiate lights.
		if ( _model_resource->get_data().light_list.get_length() > 0 )
		{
			sint32_c count = _model_resource->get_data().light_list.get_length();
			_light_list.set_length_absolute( count );
			for ( sint32_c i = 0; i < count; i++ )
			{
				light_c * light = &_light_list[ i ];
				light->_source_light = &_model_resource->get_data().light_list[ i ];
				sint32_c bone_index = _model_resource->find_bone_index_with_bone_name( light->_source_light->mother_bone_name );
				light->_mother_bone = bone_index >= 0 ? &_bone_list[ bone_index ] : nullptr;
				light->_local_space_transform.position = vector64x3_c( light->_source_light->position );
				light->_local_space_transform.rotation = quaternion32_c( light->_source_light->rotation );
				light->_scene_light.set_type( light->_source_light->get_scene_light_type() );
				light->_scene_light.set_color( vector32x3_c( light->_source_light->color ) );
				light->_scene_light.set_strength( light->_source_light->strength );
				light->_scene_light.set_cone_angle( light->_source_light->cone_angle );
				light->_scene_light.set_render_enabled( ( light->_source_light->flags & static_cast< uint16_c >( resource_file_model_c::light_c::flags_e_render_enabled ) ) != 0 );
				light->_scene_light.set_shadow_cast_enabled( ( light->_source_light->flags & static_cast< uint16_c >( resource_file_model_c::light_c::flags_e_shadow_cast_enabled ) ) != 0 );
				if ( _scene_object != nullptr && _scene_object->get_scene() != nullptr )
				{
					_scene_object->get_scene()->add_light( &light->_scene_light );
				}
			}
		}

		// apply rest pose.
		_update_world_transforms();

		// instantiate physics rigid bodies.
		core_list_c< physics_rigid_body_c * > physics_rigid_body_list;
		if ( _model_resource->get_data().physics_body_list.get_length() > 0 )
		{
			sint32_c count = _model_resource->get_data().physics_body_list.get_length();

			physics_rigid_body_list.set_length_absolute( count );

			for ( sint32_c i = 0; i < count; i++ )
			{
				resource_file_model_c::physics_rigid_body_c const * source_physics_body = &_model_resource->get_data().physics_body_list[ i ];
				physics_rigid_body_list[ i ] = nullptr;

				sint32_c shape_count = source_physics_body->shape_end - source_physics_body->shape_start;
				if ( shape_count == 1 ) // file format supports compound shapes, but we don't (yet), so we'll only process if there's one shape.
				{
					resource_file_model_c::physics_shape_c const * source_physics_shape = &_model_resource->get_data().physics_shape_list[ source_physics_body->shape_start ];
					physics_shape_c * physics_shape = nullptr;

					transform3d_c physics_shape_local_space_transform;
					physics_shape_local_space_transform.position = vector64x3_c( source_physics_shape->position[ 0 ], source_physics_shape->position[ 1 ], source_physics_shape->position[ 2 ] );
					physics_shape_local_space_transform.rotation = quaternion32_c( source_physics_shape->rotation[ 0 ], source_physics_shape->rotation[ 1 ], source_physics_shape->rotation[ 2 ], source_physics_shape->rotation[ 3 ] );

					if ( source_physics_shape->get_physics_shape_type() == physics_shape_type_e_sphere )
					{
						physics_shape = physics_shape_c::make_new_instance();
						physics_shape->initialize_sphere( physics_shape_local_space_transform, source_physics_shape->sphere.radius, source_physics_shape->margin );
					}
					else if ( source_physics_shape->get_physics_shape_type() == physics_shape_type_e_box )
					{
						physics_shape = physics_shape_c::make_new_instance();
						physics_shape->initialize_box( physics_shape_local_space_transform, vector64x3_c( source_physics_shape->box.width, source_physics_shape->box.depth, source_physics_shape->box.height ), source_physics_shape->margin );
					}
					else if ( source_physics_shape->get_physics_shape_type() == physics_shape_type_e_capsule )
					{
						float32_c capsule_segment_length = source_physics_shape->capsule.height - ( source_physics_shape->capsule.radius * 2.0f );
						if ( capsule_segment_length < 0.0f )
						{
							capsule_segment_length = 0.0f;
						}
						physics_shape = physics_shape_c::make_new_instance();
						physics_shape->initialize_capsule( physics_shape_local_space_transform, source_physics_shape->capsule.radius, capsule_segment_length, source_physics_shape->margin );
					}
					else if ( source_physics_shape->get_physics_shape_type() == physics_shape_type_e_cylinder )
					{
						physics_shape = physics_shape_c::make_new_instance();
						physics_shape->initialize_cylinder( physics_shape_local_space_transform, source_physics_shape->cylinder.radius, source_physics_shape->cylinder.height, source_physics_shape->margin );
					}
					else if ( source_physics_shape->get_physics_shape_type() == physics_shape_type_e_cone )
					{
						physics_shape = physics_shape_c::make_new_instance();
						physics_shape->initialize_cone( physics_shape_local_space_transform, source_physics_shape->cone.radius, source_physics_shape->cone.height, source_physics_shape->margin );
					}
					else if ( source_physics_shape->get_physics_shape_type() == physics_shape_type_e_convex_hull )
					{
						sint32_c vertex_count = source_physics_shape->convex_hull.vertex_end - source_physics_shape->convex_hull.vertex_start;
						core_list_c< vector64x3_c > vertex_list;
						vertex_list.set_length_absolute( vertex_count );
						resource_file_model_c::physics_vertex_c const * source_physics_vertex_list = &_model_resource->get_data().physics_vertex_list[ source_physics_shape->convex_hull.vertex_start ];
						for ( sint32_c k = 0; k < vertex_count; k++ )
						{
							vertex_list[ k ].a = source_physics_vertex_list[ k ].position[ 0 ];
							vertex_list[ k ].b = source_physics_vertex_list[ k ].position[ 1 ];
							vertex_list[ k ].c = source_physics_vertex_list[ k ].position[ 2 ];
						}
						physics_shape = physics_shape_c::make_new_instance();
						physics_shape->initialize_convex_hull( physics_shape_local_space_transform, vertex_count, vertex_list.get_internal_array(), source_physics_shape->margin );
					}
					else if ( source_physics_shape->get_physics_shape_type() == physics_shape_type_e_triangle_mesh )
					{
						sint32_c vertex_count = source_physics_shape->triangle_mesh.vertex_end - source_physics_shape->triangle_mesh.vertex_start;
						core_list_c< vector64x3_c > vertex_list;
						vertex_list.set_length_absolute( vertex_count );
						resource_file_model_c::physics_vertex_c const * source_physics_vertex_list = &_model_resource->get_data().physics_vertex_list[ source_physics_shape->triangle_mesh.vertex_start ];
						for ( sint32_c k = 0; k < vertex_count; k++ )
						{
							vertex_list[ k ].a = source_physics_vertex_list[ k ].position[ 0 ];
							vertex_list[ k ].b = source_physics_vertex_list[ k ].position[ 1 ];
							vertex_list[ k ].c = source_physics_vertex_list[ k ].position[ 2 ];
						}
						sint32_c index_count = source_physics_shape->triangle_mesh.index_end - source_physics_shape->triangle_mesh.index_start;
						core_list_c< uint16_c > index_list;
						index_list.set_length_absolute( index_count );
						resource_file_model_c::physics_index_c const * source_physics_index_list = &_model_resource->get_data().physics_index_list[ source_physics_shape->triangle_mesh.index_start ];
						for ( sint32_c k = 0; k < index_count; k++ )
						{
							index_list[ k ] = source_physics_index_list[ k ].value;
						}
						physics_shape = physics_shape_c::make_new_instance();
						physics_shape->initialize_triangle_mesh( physics_shape_local_space_transform, vertex_count, vertex_list.get_internal_array(), index_count, index_list.get_internal_array(), source_physics_shape->margin );
					}

					if ( physics_shape != nullptr )
					{
						if ( source_physics_body->mother_bone_name == 0 )
						{
							if ( _physics_rigid_body == nullptr )
							{
								_physics_rigid_body = physics_rigid_body_c::make_new_instance();
								_physics_rigid_body->add_reference();
								_physics_rigid_body->initialize( this, &scene_component_model_c::_physics_copy_world_space_transform_from_game_to_physics, &scene_component_model_c::_physics_copy_world_space_transform_from_physics_to_game, physics_shape, source_physics_body->mass, false, source_physics_body->get_physics_layer(), source_physics_body->get_physics_layer_mask() );
								_physics_shape = physics_shape;
								_physics_shape->add_reference();
								physics_rigid_body_list[ i ] = _physics_rigid_body;
								continue;
							}
						}
						/*
						else
						{
							string8_c mother_bone_name = string8_c( core_list_mode_e_static_volatile, &_model_resource->get_data().string_table[ source_physics_body->mother_bone_name ] );
							bone_c * mother_bone = find_bone_with_name( mother_bone_name, false );
							if ( mother_bone != nullptr )
							{
								if ( mother_bone->_physics_rigid_body == nullptr )
								{
									mother_bone->_physics_rigid_body = physics_rigid_body_c::make_new_instance();
									mother_bone->_physics_rigid_body->add_reference();
									mother_bone->_physics_rigid_body->initialize( mother_bone, &bone_c::_physics_copy_world_space_transform_from_game_to_physics, &bone_c::_physics_copy_world_space_transform_from_physics_to_game, physics_shape, source_physics_body->mass, false, source_physics_body->get_physics_layer(), source_physics_body->get_physics_layer_mask() );
									mother_bone->_physics_shape = physics_shape;
									mother_bone->_physics_shape->add_reference();
									physics_rigid_body_list[ i ] = mother_bone->_physics_rigid_body;
									continue;
								}
							}
						}
						*/
						delete physics_shape;
						physics_shape = nullptr;
					}
				}
			}
		}

		/*
		// instantiate physics constraints.
		if ( _model_resource->get_data().physics_constraint_list.get_length() > 0 )
		{
			sint32_c count = _model_resource->get_data().physics_constraint_list.get_length();
			for ( sint32_c i = 0; i < count; i++ )
			{
				resource_file_model_c::physics_constraint_c const * source_physics_constraint = &_model_resource->get_data().physics_constraint_list[ i ];
				physics_constraint_c * physics_constraint = nullptr;

				physics_rigid_body_c * physics_rigid_body_a = physics_rigid_body_list[ source_physics_constraint->body_a_index ];
				physics_rigid_body_c * physics_rigid_body_b = physics_rigid_body_list[ source_physics_constraint->body_b_index ];

				if ( physics_rigid_body_a != nullptr && physics_rigid_body_b != nullptr )
				{
					transform3d_c frame;
					frame.position = vector64x3_c( source_physics_constraint->frame_position[ 0 ], source_physics_constraint->frame_position[ 1 ], source_physics_constraint->frame_position[ 2 ] );
					frame.rotation = quaternion32_c( source_physics_constraint->frame_rotation[ 0 ], source_physics_constraint->frame_rotation[ 1 ], source_physics_constraint->frame_rotation[ 2 ], source_physics_constraint->frame_rotation[ 3 ] );
					if ( _scene_object != nullptr )
					{
						frame = frame * _scene_object->get_world_space_transform();
					}

					if ( source_physics_constraint->get_physics_constraint_type() == physics_constraint_type_e_fixed )
					{
						physics_constraint = physics_constraint_c::make_new_instance();
						physics_constraint->initialize_fixed( physics_rigid_body_a, physics_rigid_body_b, frame );
					}
					else if ( source_physics_constraint->get_physics_constraint_type() == physics_constraint_type_e_point )
					{
						physics_constraint = physics_constraint_c::make_new_instance();
						physics_constraint->initialize_point( physics_rigid_body_a, physics_rigid_body_b, frame );
					}
					else if ( source_physics_constraint->get_physics_constraint_type() == physics_constraint_type_e_hinge )
					{
						physics_constraint = physics_constraint_c::make_new_instance();
						physics_constraint->initialize_hinge( physics_rigid_body_a, physics_rigid_body_b, frame );
						float32_c lower_limit = 0.0f;
						float32_c upper_limit = 0.0f;
						if ( source_physics_constraint->parameters_end - source_physics_constraint->parameters_start == 2 )
						{
							lower_limit = 
						}
						for ( sint32_c j = source_physics_constraint->parameters_start; j < source_physics_constraint->parameters_end; j++ )
						{
							_model_resource->get_data().physics_parameter_list[ j ].value
						}
						physics_constraint->set_hinge_limits( lower_limit, upper_limit );
					}
					else if ( source_physics_constraint->get_physics_constraint_type() == physics_constraint_type_e_cone )
					{
					}
					else if ( source_physics_constraint->get_physics_constraint_type() == physics_constraint_type_e_generic )
					{
					}

					if ( physics_constraint != nullptr )
					{
						physics_constraint->add_reference();
						_physics_constraint_list.insert_at_end( physics_constraint );
					}
				}
			}
		}
		*/

		if ( _scene_object && _scene_object->get_scene() )
		{
			// add physics objects to physics scene.
			_physics_add_to_simulation();

			// invalidate light probes if needed.
			if ( _scene_object->get_scene()->_automatic_light_probe_invalidation_enabled && _automatic_light_probe_invalidation_enabled && _light_cast_enabled )
			{
				invalidate_light_probes();
			}
		}

		// restart all bone logics in the hierarchy network.
		get_mother_model_root()->_restart_bone_logics_recursive();

		// refresh animation map dictionary.
		refresh_animation_map_dictionary();

		// notify listeners that we are now bound.
		on_model_resource_bound.invoke( this );

		// notify animation players that we are now bound.
		_on_animation_resources_updated.invoke( this );

		// update with 0 timestep to sample animations so that model doesn't spawn in with rest pose.
		update_animations( 0.0f );
	}

	void_c scene_component_model_c::_handle_model_resource_on_unloaded( resource_file_c * resource )
	{
		assert( resource == _model_resource );

		// mark our base as un bound.
		_model_resource_is_bound = false;

		// notify animation players that we are now un bound.
		_on_animation_resources_updated.invoke( this );

		// notify listeners that we are now un bound.
		on_model_resource_unbound.invoke( this );

		// update animation lookup dictionary.
		refresh_animation_map_dictionary();

		// remove and delete physics.
		if ( _physics_is_added_to_simulation )
		{
			_physics_remove_from_simulation();
		}
		for ( sint32_c i = 0; i < _physics_constraint_list.get_length(); i++ )
		{
			delete _physics_constraint_list[ i ];
		}
		_physics_constraint_list.remove_all();
		for ( sint32_c i = 0; i < _bone_list.get_length(); i++ )
		{
			bone_c * bone = &_bone_list[ i ];
			if ( bone->_physics_rigid_body || bone->_physics_shape )
			{
				delete bone->_physics_rigid_body;
				bone->_physics_rigid_body = nullptr;
				delete bone->_physics_shape;
				bone->_physics_shape = nullptr;
			}
		}
		if ( _physics_rigid_body || _physics_shape )
		{
			delete _physics_rigid_body;
			_physics_rigid_body = nullptr;
			delete _physics_shape;
			_physics_shape = nullptr;
		}

		// make daughters let go of any references that they might have to our bones (since we set our _model_resource_is_bound to false, our daughters won't try to look us up again).
		for ( sint32_c i = 0; i < _daughter_model_list.get_length(); i++ )
		{
			_daughter_model_list[ i ]->_restart_bone_logics_recursive();
		}

		// delete attachment points.
		_attachment_point_list.remove_all();

		// delete bone logics.
		for ( sint32_c i = 0; i < _bone_logic_list.get_length(); i++ )
		{
			delete _bone_logic_list[ i ];
		}
		_bone_logic_list.remove_all();

		// delete bones.
		_bone_skin_matrix_list.remove_all();
		_root_bone_list.remove_all_quick_and_dirty();
		_bone_list.remove_all();
		_bone_skin_matrix_list.remove_all();

		// delete mesh stuff.
		if ( _gpu_vertex_buffer_mesh_base_shape_and_bone_transformed )
		{
			delete _gpu_vertex_buffer_mesh_base_shape_and_bone_transformed;
			_gpu_vertex_buffer_mesh_base_shape_and_bone_transformed = nullptr;
		}
		_cpu_vertex_buffer_mesh_base_shape_and_bone_transformed.remove_all();

		if ( _gpu_vertex_buffer_mesh_base_shape_transformed )
		{
			delete _gpu_vertex_buffer_mesh_base_shape_transformed;
			_gpu_vertex_buffer_mesh_base_shape_transformed = nullptr;
		}
		_cpu_vertex_buffer_mesh_base_shape_transformed.remove_all();

		_mesh_list.remove_all();

		// invalidate light probes.
		if ( _automatic_light_probe_invalidation_enabled && _light_cast_enabled )
		{
			invalidate_light_probes();
		}

		// reset local space bounding box.
		_reset_local_space_obb();
	}

	resource_file_model_c * scene_component_model_c::get_model_resource()
	{
		return _model_resource;
	}

	void_c scene_component_model_c::set_model_resource( resource_file_model_c * resource )
	{
		if ( _model_resource == resource )
		{
			return;
		}

		if ( _model_resource )
		{
			_model_resource->on_loaded.unsubscribe( this, &scene_component_model_c::_handle_model_resource_on_loaded );
			_model_resource->on_unloaded.unsubscribe( this, &scene_component_model_c::_handle_model_resource_on_unloaded );
			if ( _model_resource_is_bound )
			{
				_handle_model_resource_on_unloaded( _model_resource );
			}
		}

		_model_resource = resource;

		if ( _model_resource )
		{
			_model_resource->on_loaded.subscribe( this, &scene_component_model_c::_handle_model_resource_on_loaded );
			_model_resource->on_unloaded.subscribe( this, &scene_component_model_c::_handle_model_resource_on_unloaded );
			if ( _model_resource->get_is_loaded() == true )
			{
				_handle_model_resource_on_loaded( _model_resource );
			}
		}
	}

	boolean_c scene_component_model_c::get_model_resource_is_bound() const
	{
		return _model_resource_is_bound;
	}




	//
	// animation file resource binding.
	//

	scene_component_model_c::animation_map_c::animation_map_c()
		: source_model( nullptr )
		, source_animation( nullptr )
	{
	}

	void_c scene_component_model_c::_handle_animations_resource_on_loaded( resource_file_c * resource )
	{
		refresh_animation_map_dictionary();
		_on_animation_resources_updated.invoke( this );
	}

	void_c scene_component_model_c::_handle_animations_resource_on_unloaded( resource_file_c * resource )
	{
		refresh_animation_map_dictionary();
		_on_animation_resources_updated.invoke( this );
	}

	void_c scene_component_model_c::add_animations_resource( resource_file_model_c * resource )
	{
		assert( _animations_resource_list.find_index_of( resource ) < 0 );
		resource->on_loaded.subscribe( this, &scene_component_model_c::_handle_animations_resource_on_loaded );
		resource->on_unloaded.subscribe( this, &scene_component_model_c::_handle_animations_resource_on_unloaded );
		_animations_resource_list.insert( -1, resource );
	}

	void_c scene_component_model_c::remove_animations_resource( resource_file_model_c * resource )
	{
		assert( _animations_resource_list.find_index_of( resource ) >= 0 );
		resource->on_loaded.unsubscribe( this, &scene_component_model_c::_handle_animations_resource_on_loaded );
		resource->on_unloaded.unsubscribe( this, &scene_component_model_c::_handle_animations_resource_on_unloaded );
		_animations_resource_list.remove_value( resource );
	}

	sint32_c scene_component_model_c::get_animations_resource_count()
	{
		return _animations_resource_list.get_length();
	}

	resource_file_model_c * scene_component_model_c::get_animations_resource_at_index( sint32_c index )
	{
		return _animations_resource_list[ index ];
	}

	void_c scene_component_model_c::remove_all_animations_resources()
	{
		for ( sint32_c i = 0; i < _animations_resource_list.get_length(); i++ )
		{
			resource_file_model_c * animations_resource = _animations_resource_list[ i ];
			animations_resource->on_loaded.unsubscribe( this, &scene_component_model_c::_handle_animations_resource_on_loaded );
			animations_resource->on_unloaded.unsubscribe( this, &scene_component_model_c::_handle_animations_resource_on_unloaded );
		}
		_animations_resource_list.remove_all();
		refresh_animation_map_dictionary();
	}

	void_c scene_component_model_c::refresh_animation_map_dictionary()
	{
		core_list_c< resource_file_model_c * > animations_resource_list;
		if ( _model_resource->get_is_loaded() )
		{
			animations_resource_list.insert( -1, _model_resource );
		}
		for ( sint32_c i = 0; i < _animations_resource_list.get_length(); i++ )
		{
			if ( _animations_resource_list[ i ]->get_is_loaded() )
			{
				animations_resource_list.insert( -1, _animations_resource_list[ i ] );
			}
		}

		_animation_map_dictionary.remove_all();
		for ( sint32_c j = 0; j < animations_resource_list.get_length(); j++ )
		{
			resource_file_model_c * animation_resource = animations_resource_list[ j ];
			for ( sint32_c i = 0; i < animation_resource->get_data().animation_list.get_length(); i++ )
			{
				animation_map_c animation_map;
				animation_map.source_model = animation_resource;
				animation_map.source_animation = &animation_resource->get_data().animation_list[ i ];
				_animation_map_dictionary.insert( string8_c( core_list_mode_e_static, &animation_resource->get_data().string_table[ animation_map.source_animation->name ] ), animation_map );
			}
		}
	}




	//
	// material map file resource binding.
	//

	void_c scene_component_model_c::_handle_materials_resource_on_loaded( resource_file_c * resource )
	{
		assert( resource == _materials_resource );

		for ( sint32_c i = 0; i < _materials_resource->get_thing_list().get_length(); i++ )
		{
			resource_file_materials_c::thing_c const * material_map_thing = _materials_resource->get_thing_list()[ i ];
			core_list_c< mesh_c * > mesh_list;
			find_meshes( material_map_thing->get_target_names(), mesh_list );
			for ( sint32_c j = 0; j < mesh_list.get_length(); j++ )
			{
				mesh_c * mesh = mesh_list[ j ];
				mesh->_source_material_map_thing = material_map_thing;
			}
		}
	}

	void_c scene_component_model_c::_handle_materials_resource_on_unloaded( resource_file_c * resource )
	{
		assert( resource == _materials_resource );

		for ( sint32_c i = 0; i < _mesh_list.get_length(); i++ )
		{
			mesh_c * mesh = &_mesh_list[ i ];
			mesh->_source_material_map_thing = nullptr;
		}
	}

	resource_file_materials_c * scene_component_model_c::get_materials_resource()
	{
		return _materials_resource;
	}

	void_c scene_component_model_c::set_materials_resource( resource_file_materials_c * value )
	{
		if ( _materials_resource )
		{
			_materials_resource->on_loaded.unsubscribe( this, & scene_component_model_c::_handle_materials_resource_on_loaded );
			_materials_resource->on_unloaded.unsubscribe( this, & scene_component_model_c::_handle_materials_resource_on_unloaded );
			if ( _materials_resource->get_is_loaded() )
			{
				_handle_materials_resource_on_unloaded( _materials_resource );
			}
		}

		_materials_resource = value;

		if ( _materials_resource )
		{
			_materials_resource->on_loaded.subscribe( this, & scene_component_model_c::_handle_materials_resource_on_loaded );
			_materials_resource->on_unloaded.subscribe( this, & scene_component_model_c::_handle_materials_resource_on_unloaded );
			if ( _materials_resource->get_is_loaded() )
			{
				_handle_materials_resource_on_loaded( _materials_resource );
			}
		}
	}




	//
	// meshes.
	//

	scene_component_model_c::mesh_c::shape_key_c::shape_key_c()
		: _mother_mesh( nullptr )
		, _source_mesh_shape_key( nullptr )
		, _value( 0.0f )
	{
	}

	scene_component_model_c::mesh_c * scene_component_model_c::mesh_c::shape_key_c::get_mother_mesh() const
	{
		return _mother_mesh;
	}

	string8_c scene_component_model_c::mesh_c::shape_key_c::get_name() const
	{
		return string8_c( core_list_mode_e_static_volatile, &_mother_mesh->get_mother_model()->get_model_resource()->get_data().string_table[ _source_mesh_shape_key->name ] );
	}

	float32_c scene_component_model_c::mesh_c::shape_key_c::get_value() const
	{
		return _value;
	}

	void_c scene_component_model_c::mesh_c::shape_key_c::set_value( float32_c value )
	{
		_value = value;
	}

	float32_c scene_component_model_c::mesh_c::shape_key_c::get_minimum_value() const
	{
		return _source_mesh_shape_key->minimum_value;
	}

	float32_c scene_component_model_c::mesh_c::shape_key_c::get_maximum_value() const
	{
		return _source_mesh_shape_key->maximum_value;
	}

	scene_component_model_c::mesh_c::mesh_c()
		: _mother_model( nullptr )
		, _source_mesh( nullptr )
		, _render_enabled( true )
		, _custom_material_assignment( nullptr )
		//, _shape_key_list()
	{
	}

	scene_component_model_c * scene_component_model_c::mesh_c::get_mother_model() const
	{
		return _mother_model;
	}

	string8_c scene_component_model_c::mesh_c::get_name() const
	{
		return string8_c( core_list_mode_e_static_volatile, &_mother_model->get_model_resource()->get_data().string_table[ _source_mesh->name ] );
	}

	boolean_c scene_component_model_c::mesh_c::get_render_enabled() const
	{
		return _render_enabled;
	}

	void_c scene_component_model_c::mesh_c::set_render_enabled( boolean_c value )
	{
		_render_enabled = value;
	}

	void_c scene_component_model_c::find_meshes( string8_c const & names, core_list_c< mesh_c * > & result )
	{
		// split the potentially semi-colon separated list of names into a list of single names.
		core_list_c< string8_c > name_list;
		//ops::string8_split_at_delimiter( names, string8_c( core_list_mode_e_static, ";" ), name_list ); // this is the quick and easy way, but not as efficient.
		string8_c names_copy( core_list_mode_e_dynamic, names.character_list.get_internal_array(), names.character_list.get_length() ); // create a copy that we can edit in-place.
		result.remove_all();
		char8_c * string_scan = names_copy.character_list.get_internal_array();
		char8_c * string_start = string_scan;
		while ( true )
		{
			if ( *string_scan == ';' || *string_scan == 0 )
			{
				*string_scan = 0;
				name_list.insert( -1, string8_c( core_list_mode_e_static, string_start, static_cast< sint32_c >( string_scan - string_start ) ) );
				if ( *string_scan == 0 ) // first check, in case the list did not end with a delimiter.
				{
					break;
				}
				string_scan++;
				if ( *string_scan == 0 ) // redundant second check, in case the list did end with a delimiter.
				{
					break;
				}
				string_start = string_scan;
			}
			else
			{
				string_scan++;
			}
		}

		// go through each name.
		for ( sint32_c i = 0; i < name_list.get_length(); i++ )
		{
			string8_c name = ops::string8_trim( name_list[ i ] );
			if ( ops::string8_ends_with( name.character_list.get_internal_array(), "*" ) )
			{
				name = ops::string8_sub_string( name, 0, name.get_length() - 1 );
				for ( sint32_c j = 0; j < _mesh_list.get_length(); j++ )
				{
					if ( ops::string8_starts_with( &_model_resource->get_data().string_table[ _mesh_list[ j ]._source_mesh->name ], name.character_list.get_internal_array() ) )
					{
						result.insert( -1, &_mesh_list[ j ] );
					}
				}
			}
			else
			{
				for ( sint32_c j = 0; j < _mesh_list.get_length(); j++ )
				{
					if ( name == &_model_resource->get_data().string_table[ _mesh_list[ j ]._source_mesh->name ] )
					{
						result.insert( -1, &_mesh_list[ j ] );
					}
				}
			}
		}
	}




	//
	// bones.
	//

	void_c scene_component_model_c::bone_c::_physics_copy_world_space_transform_from_game_to_physics( void_c * object, vector64x3_c & world_space_position, matrix32x3x3_c & world_space_basis )
	{
		bone_c * bone = reinterpret_cast< bone_c * >( object );
		world_space_position = bone->_world_space_transform.position + ( bone->_world_space_transform.get_scaled_basis_y() * ( bone->_source_bone_extras->length * 0.5f ) );
		world_space_basis = bone->_world_space_transform.get_unscaled_basis();
	}

	void_c scene_component_model_c::bone_c::_physics_copy_world_space_transform_from_physics_to_game( void_c * object, vector64x3_c const & world_space_position, matrix32x3x3_c const & world_space_basis )
	{
		bone_c * bone = reinterpret_cast< bone_c * >( object );
		bone->_world_space_transform.set_rotation_from_unscaled_basis( world_space_basis );
		bone->_world_space_transform.position = world_space_position - ( bone->_world_space_transform.get_scaled_basis_y() * ( bone->_source_bone_extras->length * 0.5f ) );
	}

	scene_component_model_c::bone_c::bone_c()
		: _mother_model( nullptr )
		, _mother_bone( nullptr )
		, _source_bone( nullptr )
		, _source_bone_extras( nullptr )
		, _local_space_transform()
		, _world_space_transform()
		, _object_space_skin_matrix()
		, _daughter_bone_list()
		, _daughter_bone_list_node( this )
		, _physics_rigid_body( nullptr )
		, _physics_shape( nullptr )
	{
	}

	scene_component_model_c::bone_c::~bone_c()
	{
		assert( _physics_rigid_body == nullptr && _physics_shape == nullptr );
	}

	void_c scene_component_model_c::bone_c::update_transforms_recursive()
	{
		if ( _mother_bone )
		{
			uint8_c mask = resource_file_model_c::bone_c::flags_e_inherit_position | resource_file_model_c::bone_c::flags_e_inherit_rotation | resource_file_model_c::bone_c::flags_e_inherit_scale;
			if ( ( _source_bone->flags & mask ) == mask )
			{
				_world_space_transform = _local_space_transform * _mother_bone->_world_space_transform;
			}
			else
			{
				if ( _source_bone->flags & resource_file_model_c::bone_c::flags_e_inherit_position )
				{
					_world_space_transform.position = ops::rotate_scale_and_translate_vector64x3( _local_space_transform.position, _mother_bone->_world_space_transform );
				}
				else
				{
					_world_space_transform.position = ops::rotate_scale_and_translate_vector64x3( _local_space_transform.position, _mother_model->get_scene_object()->get_world_space_transform() );
				}
				if ( _source_bone->flags & resource_file_model_c::bone_c::flags_e_inherit_rotation )
				{
					_world_space_transform.rotation = _local_space_transform.rotation * _mother_bone->_world_space_transform.rotation;
				}
				else
				{
					_world_space_transform.rotation = _local_space_transform.rotation * _mother_model->get_scene_object()->get_world_space_transform().rotation;
				}
				if ( _source_bone->flags & resource_file_model_c::bone_c::flags_e_inherit_scale )
				{
					_world_space_transform.scale = _local_space_transform.scale * _mother_bone->_world_space_transform.scale;
				}
				else
				{
					_world_space_transform.scale = _local_space_transform.scale * _mother_model->get_scene_object()->get_world_space_transform().scale;
				}
			}
		}
		else
		{
			_world_space_transform = _local_space_transform * _mother_model->get_scene_object()->get_world_space_transform();
		}

		build_object_space_skin_matrix_from_world_space_transform();

		core_linked_list_c< bone_c * >::node_c const * daughter_bone_list_node = _daughter_bone_list.get_first();
		while ( daughter_bone_list_node != nullptr )
		{
			daughter_bone_list_node->get_value()->update_transforms_recursive();
			daughter_bone_list_node = daughter_bone_list_node->get_next();
		}
	}

	void_c scene_component_model_c::bone_c::build_object_space_skin_matrix_from_world_space_transform()
	{
		transform3d_c object_space_transform = _world_space_transform * _mother_model->_world_space_transform_inverse;
		_object_space_skin_matrix = ops::transform_matrix32x4x4_c_from_transform3d( _source_bone_extras->object_space_rest_transform_inverted * object_space_transform );
	}

	transform3d_c const & scene_component_model_c::bone_c::get_local_space_transform() const
	{
		return _local_space_transform;
	}

	transform3d_c const & scene_component_model_c::bone_c::get_world_space_transform() const
	{
		return _world_space_transform;
	}

	void_c scene_component_model_c::_update_bone_skin_matrix_list_recursive()
	{
		if ( _model_resource_is_bound == false )
		{
			_bone_skin_matrix_list.remove_all();
			return;
		}

		if ( _model_resource->get_data().mesh_bone_name_list.get_length() > 0 )
		{
			// instantiate transform list if needed.
			if ( _bone_skin_matrix_list.get_length() == 0 )
			{
				_bone_skin_matrix_list.set_length_absolute( _model_resource->get_data().mesh_bone_name_list.get_length() );
			}

			// resolve bone skin matrices.
			// prioritize our model first, and traverse up the hierarchy until we find the bone.
			for ( sint32_c i = 0; i < _bone_skin_matrix_list.get_length(); i++ )
			{
				string8_c & bone_name = string8_c( core_list_mode_e_static, &_model_resource->get_data().string_table[ _model_resource->get_data().mesh_bone_name_list[ i ].name ] );
				bone_c * bone = nullptr;
				scene_component_model_c * model_component = this;
				while ( model_component != nullptr )
				{
					if ( model_component->_model_resource_is_bound )
					{
						bone = model_component->find_bone_with_name( bone_name, false );
						if ( bone != nullptr )
						{
							break;
						}
					}
					model_component = model_component->_mother_model;
				}
				_bone_skin_matrix_list[ i ] = bone != nullptr ? & bone->_object_space_skin_matrix : nullptr;
			}
		}

		for ( sint32_c i = 0; i < _daughter_model_list.get_length(); i++ )
		{
			_daughter_model_list[ i ]->_update_bone_skin_matrix_list_recursive();
		}
	}

	scene_component_model_c::bone_c * scene_component_model_c::find_bone_at_path( string8_c const & path )
	{
		core_list_c< string8_c > path_nodes;
		ops::string8_split_at_delimiter( path, string8_c( core_list_mode_e_static, "/" ), path_nodes );
		bone_c * bone = nullptr;
		core_linked_list_c< bone_c * >::node_c const * bone_list_node = _root_bone_list.get_first();
		sint32_c i = 0;
		while ( bone_list_node != nullptr && i < path_nodes.get_length() )
		{
			while ( bone_list_node != nullptr )
			{
				if ( path_nodes[ i ] == &_model_resource->get_data().string_table[ bone_list_node->get_value()->_source_bone->name ] )
				{
					bone = bone_list_node->get_value();
					if ( i + 1 == path_nodes.get_length() )
					{
						return bone;
					}
					bone_list_node = bone->_daughter_bone_list.get_first();
					break;
				}
				bone_list_node = bone_list_node->get_next();
			}
			i++;
		}
		return nullptr;
	}

	scene_component_model_c::bone_c * scene_component_model_c::find_bone_with_name( string8_c const & name, boolean_c search_mother_first )
	{
		assert( _model_resource != nullptr && _model_resource_is_bound );

		if ( search_mother_first && _mother_model != nullptr && _mother_model->get_model_resource_is_bound() )
		{
			bone_c * bone = _mother_model->find_bone_with_name( name, false );
			if ( bone )
			{
				return bone;
			}
		}

		for ( sint32_c i = 0; i < _bone_list.get_length(); i++ )
		{
			bone_c * bone = &_bone_list[ i ];
			if ( name == &_model_resource->get_data().string_table[ bone->_source_bone->name ] )
			{
				return bone;
			}
		}
		return nullptr;
	}

	void_c scene_component_model_c::find_bones( string8_c const & names, core_list_c< bone_c * > & result )
	{
		result.remove_all();
		core_list_c< string8_c > name_list;
		ops::string8_split_at_delimiter( names, string8_c( core_list_mode_e_static, "," ), name_list );
		for ( sint32_c i = 0; i < name_list.get_length(); i++ )
		{
			string8_c name = ops::string8_trim( name_list[ i ] );
			if ( ops::string8_ends_with( name.character_list.get_internal_array(), "*" ) )
			{
				name = ops::string8_sub_string( name, 0, name.get_length() - 1 );
				for ( sint32_c j = 0; j < _bone_list.get_length(); j++ )
				{
					if ( ops::string8_starts_with( &_model_resource->get_data().string_table[ _bone_list[ j ]._source_bone->name ], name.character_list.get_internal_array() ) )
					{
						result.insert( -1, &_bone_list[ j ] );
					}
				}
			}
			else
			{
				for ( sint32_c j = 0; j < _bone_list.get_length(); j++ )
				{
					if ( name == &_model_resource->get_data().string_table[ _bone_list[ j ]._source_bone->name ] )
					{
						result.insert( -1, &_bone_list[ j ] );
					}
				}
			}
		}
	}

	sint32_c scene_component_model_c::find_bone_index_with_name( string8_c const & name )
	{
		assert( _model_resource != nullptr );
		for ( sint32_c i = 0; i < _bone_list.get_length(); i++ )
		{
			bone_c * bone = &_bone_list[ i ];
			if ( name == &_model_resource->get_data().string_table[ bone->_source_bone->name ] )
			{
				return i;
			}
		}
		return -1;
	}




	//
	// bone logics.
	//

	scene_component_model_c::bone_logic_c::bone_logic_c( scene_component_model_c * mother_model, resource_file_model_c::bone_logic_c const * source_bone_logic )
		: _mother_model( mother_model )
		, _source_bone_logic( source_bone_logic )
		, _is_ready( false )
		, _is_simulated( false )
		, _this_bone_name()
		, _this_bone( nullptr )
		, _that_bone_name()
		, _that_bone( nullptr )
	{
		_mother_model->_model_resource->find_bone_logic_property_as_string8( _source_bone_logic, string8_c( core_list_mode_e_static, "this_bone_name" ), _this_bone_name );
		_mother_model->_model_resource->find_bone_logic_property_as_string8( _source_bone_logic, string8_c( core_list_mode_e_static, "that_bone_name" ), _that_bone_name );
	}

	void_c scene_component_model_c::bone_logic_c::start()
	{
	}

	void_c scene_component_model_c::bone_logic_c::update( float32_c time_step )
	{
	}

	scene_component_model_c::bone_logic_copy_transform_c::bone_logic_copy_transform_c( scene_component_model_c * model, resource_file_model_c::bone_logic_c const * source_bone_logic )
		: bone_logic_c( model, source_bone_logic )
	{
	}

	void_c scene_component_model_c::bone_logic_copy_transform_c::start()
	{
		_is_ready = false;
		_this_bone = _mother_model->find_bone_with_name( _this_bone_name, false );
		_that_bone = _mother_model->find_bone_with_name( _that_bone_name, true );
		_is_ready = _this_bone && _that_bone;
	}

	void_c scene_component_model_c::bone_logic_copy_transform_c::update( float32_c time_step )
	{
		if ( !_is_ready )
		{
			return;
		}

		_this_bone->_world_space_transform = _that_bone->_world_space_transform;
	}

	scene_component_model_c::bone_logic_copy_animation_c::bone_logic_copy_animation_c( scene_component_model_c * model, resource_file_model_c::bone_logic_c const * source_bone_logic )
		: bone_logic_c( model, source_bone_logic )
		, position_weight( 1.0f )
		, rotation_weight( 1.0f )
		, scale_weight( 1.0f )
	{
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( core_list_mode_e_static, "position_weight" ), position_weight );
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( core_list_mode_e_static, "rotation_weight" ), rotation_weight );
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( core_list_mode_e_static, "scale_weight" ), scale_weight );
	}

	void_c scene_component_model_c::bone_logic_copy_animation_c::start()
	{
		_is_ready = false;
		_this_bone = _mother_model->find_bone_with_name( _this_bone_name, false );
		_that_bone = _mother_model->find_bone_with_name( _that_bone_name, true );
		_is_ready = _this_bone && _that_bone;
	}

	void_c scene_component_model_c::bone_logic_copy_animation_c::update( float32_c time_step )
	{
		if ( !_is_ready )
		{
			return;
		}

		_this_bone->_local_space_transform.position = _that_bone->_local_space_transform.position * position_weight + _this_bone->_local_space_transform.position * ( 1.0f - position_weight );
		_this_bone->_local_space_transform.rotation = _that_bone->_local_space_transform.rotation * rotation_weight + _this_bone->_local_space_transform.rotation * ( 1.0f - rotation_weight );
		_this_bone->_local_space_transform.scale = _that_bone->_local_space_transform.scale * scale_weight + _this_bone->_local_space_transform.scale * ( 1.0f - scale_weight );
		_this_bone->update_transforms_recursive();
	}

	scene_component_model_c::bone_logic_muscle_c::bone_logic_muscle_c( scene_component_model_c * model, resource_file_model_c::bone_logic_c const * source_bone_logic )
		: bone_logic_c( model, source_bone_logic )
		, compression_factor( 1.0f )
	{
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( core_list_mode_e_static, "compression_factor" ), compression_factor );
	}

	void_c scene_component_model_c::bone_logic_muscle_c::start()
	{
		_is_ready = false;
		_this_bone = _mother_model->find_bone_with_name( _this_bone_name, false );
		_that_bone = _mother_model->find_bone_with_name( _that_bone_name, true );
		_is_ready = _this_bone && _that_bone;
	}

	void_c scene_component_model_c::bone_logic_muscle_c::update( float32_c time_step )
	{
		if ( !_is_ready )
		{
			return;
		}

		// work in world space.
		vector64x3_c target_tail_position = ops::rotate_scale_and_translate_vector64x3( vector64x3_c( _this_bone->_source_bone->tail_position ), _that_bone->_world_space_transform * _that_bone->_source_bone_extras->object_space_rest_transform_inverted );
		vector32x3_c target_axis = vector32x3_c( target_tail_position - _this_bone->_world_space_transform.position ); // tail position minus head position.
		float32_c tail_axis_length = ops::length_float32( target_axis ); // get length, we'll need it to calculate compression.
		target_axis = target_axis / tail_axis_length; // normalize target axis.

		float32_c scale_along = tail_axis_length / _this_bone->_source_bone_extras->length; // scale of bone along main axis needed to stretch/compress the muscle.
		float32_c scale_across = 1.0f / ( ( ( scale_along - 1.0f ) * compression_factor ) + 1.0f ); // scale of bone around main axis needed to shrink/grow the muscle.

		_this_bone->_world_space_transform.set_rotation_from_unscaled_basis( ops::joint_rotate_basis_matrix32x3x3_z( _this_bone->_world_space_transform.get_unscaled_basis(), target_axis ) );
		_this_bone->_world_space_transform.scale.b = scale_along;
		_this_bone->_world_space_transform.scale.a = scale_across;
		_this_bone->_world_space_transform.scale.c = scale_across;

		_this_bone->build_object_space_skin_matrix_from_world_space_transform();
	}

	scene_component_model_c::bone_logic_jiggle_c::particle_c::particle_c()
		: tension( 0.25f )
		, damping( 0.15f )
		, limit( 0.1f )
		, gravity( 0.0f )
		, position()
		, velocity()
	{
	}

	void_c scene_component_model_c::bone_logic_jiggle_c::particle_c::update( float32_c time_step, vector64x3_c const & position_target, vector64x3_c const & scene_gravity )
	{
		vector64x3_c position_start = position;
		position += velocity;
		position += scene_gravity * ( gravity * time_step );
		float64_c extra_tension = ops::math_saturate( ( ops::length_float64( position - position_target ) / limit ) + tension );
		position = ( position * ( 1.0 - extra_tension ) ) + ( position_target * extra_tension );
		velocity = ( position - position_start );
		velocity -= velocity * damping;
	}

	scene_component_model_c::bone_logic_jiggle_c::bone_logic_jiggle_c( scene_component_model_c * model, resource_file_model_c::bone_logic_c const * source_bone_logic )
		: bone_logic_c( model, source_bone_logic )
		, _head_particle()
		, _tail_particle()
	{
		_is_simulated = true;

		float32_c t;
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( core_list_mode_e_static, "head_tension" ), t ); _head_particle.tension = t;
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( core_list_mode_e_static, "head_damping" ), t ); _head_particle.damping = t;
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( core_list_mode_e_static, "head_limit" ), t ); _head_particle.limit = t;
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( core_list_mode_e_static, "head_gravity" ), t ); _head_particle.gravity = t;
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( core_list_mode_e_static, "tail_tension" ), t ); _tail_particle.tension = t;
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( core_list_mode_e_static, "tail_damping" ), t ); _tail_particle.damping = t;
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( core_list_mode_e_static, "tail_limit" ), t ); _tail_particle.limit = t;
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( core_list_mode_e_static, "tail_gravity" ), t ); _tail_particle.gravity = t;
	}

	void_c scene_component_model_c::bone_logic_jiggle_c::start()
	{
		_is_ready = false;
		_this_bone = _mother_model->find_bone_with_name( _this_bone_name, false );
		if ( _this_bone != nullptr )
		{
			_head_particle.position = _this_bone->_world_space_transform.position;
			_tail_particle.position = _this_bone->_world_space_transform.position + ( _this_bone->_world_space_transform.get_scaled_basis_z() * _this_bone->_source_bone_extras->length );
			_is_ready = true;
		}
	}

	void_c scene_component_model_c::bone_logic_jiggle_c::update( float32_c time_step )
	{
		if ( !_is_ready )
		{
			return;
		}

		//vector64x3_c scene_gravity = _model->_scene_object->_scene->_physics_scene.sample_gravity_force( _model->_scene_object->_world_transform.d );
		vector64x3_c scene_gravity = vector64x3_c( 0.0, 0.0, -9.8 );

		_head_particle.update( time_step, _this_bone->_world_space_transform.position, scene_gravity );
		_tail_particle.update( time_step, _head_particle.position + _this_bone->_world_space_transform.get_scaled_basis_z() * _this_bone->_source_bone_extras->length, scene_gravity );

		vector64x3_c target_axis = _tail_particle.position - _head_particle.position;
		float64_c target_axis_length = ops::length_float64( target_axis );
		target_axis_length = ops::length_float64( target_axis ) / _this_bone->_source_bone_extras->length;
		target_axis = ops::normal_vector64x3( target_axis );

		_this_bone->_world_space_transform = resource_file_model_c::make_bone_transform( _head_particle.position, _tail_particle.position, 0.0f, _this_bone->_world_space_transform.get_scaled_basis_z() );
	}

	scene_component_model_c::bone_logic_c * scene_component_model_c::_instantiate_new_bone_logic( resource_file_model_c::bone_logic_c const * source_bone_logic )
	{
		string8_c source_bone_logic_type( core_list_mode_e_static, &_model_resource->get_data().string_table[ source_bone_logic->type ] );
		if ( source_bone_logic_type == bone_logic_copy_transform_c::get_type_static() )
		{
			return new bone_logic_copy_transform_c( this, source_bone_logic );
		}
		else if ( source_bone_logic_type == bone_logic_copy_animation_c::get_type_static() )
		{
			return new bone_logic_copy_animation_c( this, source_bone_logic );
		}
		else if ( source_bone_logic_type == bone_logic_jiggle_c::get_type_static() )
		{
			return new bone_logic_jiggle_c( this, source_bone_logic );
		}
		return nullptr;
	}

	void_c scene_component_model_c::_restart_bone_logics_recursive()
	{
		for ( sint32_c i = 0; i < _bone_logic_list.get_length(); i++ )
		{
			_bone_logic_list[ i ]->start();
		}

		for ( sint32_c i = 0; i < _daughter_model_list.get_length(); i++ )
		{
			_daughter_model_list[ i ]->_restart_bone_logics_recursive();
		}
	}




	//
	// bone attachments.
	//

	scene_component_model_c::attachment_point_c::attachment_point_c()
		: _source_attachment_point( nullptr )
		, _local_space_transform()
		, _world_space_transform()
		, _mother_bone( nullptr )
	{
	}

	transform3d_c const & scene_component_model_c::attachment_point_c::get_local_space_transform() const
	{
		return _local_space_transform;
	}

	transform3d_c const & scene_component_model_c::attachment_point_c::get_world_space_transform() const
	{
		return _world_space_transform;
	}

	scene_component_model_c::attachment_point_c * scene_component_model_c::find_attachment_point( string8_c const & name )
	{
		assert( _model_resource != nullptr );
		for ( sint32_c i = 0; i < _attachment_point_list.get_length(); i++ )
		{
			attachment_point_c * attachment_point = &_attachment_point_list[ i ];
			if ( name == &_model_resource->get_data().string_table[ attachment_point->_source_attachment_point->name ] )
			{
				return attachment_point;
			}
		}
		return nullptr;
	}

	void_c scene_component_model_c::find_attachment_points( string8_c const & names, core_list_c< attachment_point_c * > & result )
	{
		result.remove_all();
		core_list_c< string8_c > name_list;
		ops::string8_split_at_delimiter( names, string8_c( core_list_mode_e_static, "," ), name_list );
		for ( sint32_c i = 0; i < name_list.get_length(); i++ )
		{
			string8_c name = ops::string8_trim( name_list[ i ] );
			if ( ops::string8_ends_with( name.character_list.get_internal_array(), "*" ) )
			{
				name = ops::string8_sub_string( name, 0, name.get_length() - 1 );
				for ( sint32_c j = 0; j < _attachment_point_list.get_length(); j++ )
				{
					if ( ops::string8_starts_with( &_model_resource->get_data().string_table[ _attachment_point_list[ j ]._source_attachment_point->name ], name.character_list.get_internal_array() ) )
					{
						result.insert( -1, &_attachment_point_list[ j ] );
					}
				}
			}
			else
			{
				for ( sint32_c j = 0; j < _attachment_point_list.get_length(); j++ )
				{
					if ( name == &_model_resource->get_data().string_table[ _attachment_point_list[ j ]._source_attachment_point->name ] )
					{
						result.insert( -1, &_attachment_point_list[ j ] );
					}
				}
			}
		}
	}




	//
	// lights.
	//

	scene_component_model_c::light_c::light_c()
		: _source_light( nullptr )
		, _mother_bone( nullptr )
		, _local_space_transform()
		, _world_space_transform()
		, _scene_light()
	{
	}

	scene_light_c & scene_component_model_c::light_c::get_scene_light() const
	{
		return const_cast< scene_light_c & >( _scene_light );
	}

	scene_component_model_c::bone_c * scene_component_model_c::light_c::get_mother_bone() const
	{
		return _mother_bone;
	}

	transform3d_c const & scene_component_model_c::light_c::get_local_space_transform() const
	{
		return _local_space_transform;
	}

	transform3d_c const & scene_component_model_c::light_c::get_world_space_transform() const
	{
		return _world_space_transform;
	}




	//
	// vertex buffers and skinning.
	//

	video_vertex_buffer_c * scene_component_model_c::_get_vertex_buffer_mesh_base_for_rendering() const
	{
		assert( _model_resource_is_bound );
		if ( _bone_skin_matrix_list.get_length() > 0 )
		{
			assert( _gpu_vertex_buffer_mesh_base_shape_and_bone_transformed != nullptr );
			return _gpu_vertex_buffer_mesh_base_shape_and_bone_transformed;
		}
		else
		{
			assert( _model_resource != nullptr );
			assert( _model_resource->get_is_loaded() );
			assert( _model_resource->get_data()._gpu_vertex_buffer_mesh_base != nullptr );
			return _model_resource->get_data()._gpu_vertex_buffer_mesh_base;
		}
	}

	scene_component_model_c::vertex_skin_mode_e scene_component_model_c::get_vertex_skin_mode() const
	{
		return _vertex_skin_mode;
	}

	void_c scene_component_model_c::set_vertex_skin_mode( vertex_skin_mode_e value )
	{
		if ( _vertex_skin_mode != value )
		{
			_vertex_skin_mode = value;
			if ( _gpu_vertex_buffer_mesh_base_shape_transformed )
			{
				delete _gpu_vertex_buffer_mesh_base_shape_transformed;
				_gpu_vertex_buffer_mesh_base_shape_transformed = nullptr;
			}
			if ( _gpu_vertex_buffer_mesh_base_shape_and_bone_transformed )
			{
				delete _gpu_vertex_buffer_mesh_base_shape_and_bone_transformed;
				_gpu_vertex_buffer_mesh_base_shape_and_bone_transformed = nullptr;
			}
		}
	}



	
	//
	// physics.
	//

	void_c scene_component_model_c::_physics_add_to_simulation()
	{
		assert( _physics_is_added_to_simulation == false );
		assert( _scene_object );
		assert( _scene_object->get_scene() );

		_physics_is_added_to_simulation = true;
		physics_scene_c * physics_scene = _scene_object->get_scene()->get_physics_scene();
		if ( _physics_rigid_body && _physics_shape )
		{
			_physics_rigid_body->set_world_space_transform( _scene_object->get_world_space_transform() );
			physics_scene->add_rigid_body( _physics_rigid_body );
		}
		for ( sint32_c i = 0; i < _bone_list.get_length(); i++ )
		{
			bone_c * bone = &_bone_list[ i ];
			if ( bone->_physics_rigid_body && bone->_physics_shape )
			{
				bone->_physics_rigid_body->set_world_space_transform( bone->get_world_space_transform() );
				physics_scene->add_rigid_body( bone->_physics_rigid_body );
			}
		}
		for ( sint32_c i = 0; i < _physics_constraint_list.get_length(); i++ )
		{
			physics_scene->add_constraint( _physics_constraint_list[ i ] );
		}
	}

	void_c scene_component_model_c::_physics_remove_from_simulation()
	{
		assert( _physics_is_added_to_simulation );
		assert( _scene_object );
		assert( _scene_object->get_scene() );

		_physics_is_added_to_simulation = false;
		physics_scene_c * physics_scene = _scene_object->get_scene()->get_physics_scene();
		for ( sint32_c i = 0; i < _physics_constraint_list.get_length(); i++ )
		{
			physics_scene->remove_constraint( _physics_constraint_list[ i ] );
		}
		for ( sint32_c i = 0; i < _bone_list.get_length(); i++ )
		{
			bone_c * bone = &_bone_list[ i ];
			if ( bone->_physics_rigid_body && bone->_physics_shape )
			{
				physics_scene->remove_rigid_body( bone->_physics_rigid_body );
			}
		}
		if ( _physics_rigid_body && _physics_shape )
		{
			physics_scene->remove_rigid_body( _physics_rigid_body );
		}
	}

	void_c scene_component_model_c::_physics_copy_world_space_transform_from_game_to_physics( void_c * object, vector64x3_c & world_space_position, matrix32x3x3_c & world_space_basis )
	{
		scene_component_model_c * instance = reinterpret_cast< scene_component_model_c * >( object );
		world_space_position = instance->get_scene_object()->get_world_space_transform().position;
		world_space_basis = instance->get_scene_object()->get_world_space_transform().get_unscaled_basis();
	}

	void_c scene_component_model_c::_physics_copy_world_space_transform_from_physics_to_game( void_c * object, vector64x3_c const & world_space_position, matrix32x3x3_c const & world_space_basis )
	{
		scene_component_model_c * instance = reinterpret_cast< scene_component_model_c * >( object );
		transform3d_c transform = instance->get_scene_object()->get_world_space_transform();
		transform.position = world_space_position;
		transform.set_rotation_from_unscaled_basis( world_space_basis );
		instance->get_scene_object()->set_world_space_transform( transform );
	}

	boolean_c scene_component_model_c::get_physics_enable() const
	{
		return _physics_is_enabled;
	}

	void_c scene_component_model_c::set_physics_enable( boolean_c value )
	{
		_physics_is_enabled = value;
		if ( _scene_object && _scene_object->get_scene() )
		{
			if ( _physics_is_enabled && !_physics_is_added_to_simulation )
			{
				_physics_add_to_simulation();
			}
			else if ( !_physics_is_enabled && _physics_is_added_to_simulation )
			{
				_physics_remove_from_simulation();
			}
		}
	}




	//
	// custom object colors.
	//

	boolean_c scene_component_model_c::get_custom_object_colors_enabled() const
	{
		return _custom_object_colors_enabled;
	}

	void_c scene_component_model_c::set_custom_object_colors_enabled( boolean_c value )
	{
		_custom_object_colors_enabled = value;
	}

	vector32x4_c const & scene_component_model_c::get_custom_object_colors( sint32_c index ) const
	{
		assert( index >= 0 && index < 4 );
		return _custom_object_colors[ index ];
	}

	void_c scene_component_model_c::set_custom_object_colors( sint32_c index, vector32x4_c const & value )
	{
		assert( index >= 0 && index < 4 );
		_custom_object_colors[ index ] = value;
	}



	
	//
	// custom object textures.
	//

	boolean_c scene_component_model_c::get_custom_object_textures_enabled() const
	{
		return _custom_object_textures_enabled;
	}

	void_c scene_component_model_c::set_custom_object_textures_enabled( boolean_c value )
	{
		_custom_object_textures_enabled = value;
	}

	resource_file_texture_c * scene_component_model_c::get_custom_object_textures( sint32_c index ) const
	{
		assert( index >= 0 && index < 4 );
		return _custom_object_textures[ index ];
	}

	void_c scene_component_model_c::set_custom_object_textures( sint32_c index, resource_file_texture_c * value )
	{
		assert( index >= 0 && index < 4 );
		_custom_object_textures[ index ] = value;
	}




	//
	// custom material assignments.
	//

	scene_component_model_c::custom_material_c::custom_material_c( string8_c const & target_mesh_name )
		: _target_mesh_names()
		, _colors_enabled( false )
		, _colors()
		, _textures_enabled( false )
		, _textures()
		, _pixel_shader_enabled( false )
		, _pixel_shader()
	{
	}

	string8_c const & scene_component_model_c::custom_material_c::get_target_mesh_names() const
	{
		return _target_mesh_names;
	}

	void_c scene_component_model_c::custom_material_c::set_target_mesh_names( string8_c const & value )
	{
		_target_mesh_names = value;
	}

	boolean_c scene_component_model_c::custom_material_c::get_colors_enabled() const
	{
		return _colors_enabled;
	}

	void_c scene_component_model_c::custom_material_c::set_colors_enabled( boolean_c value )
	{
		_colors_enabled = value;
	}

	vector32x4_c const & scene_component_model_c::custom_material_c::get_colors( sint32_c index )
	{
		assert( index >= 0 && index < 4 );
		return _colors[ index ];
	}

	void_c scene_component_model_c::custom_material_c::set_colors( sint32_c index, vector32x4_c const & value )
	{
		assert( index >= 0 && index < 4 );
		_colors[ index ] = value;
	}

	boolean_c scene_component_model_c::custom_material_c::get_textures_enabled() const
	{
		return _textures_enabled;
	}

	void_c scene_component_model_c::custom_material_c::set_textures_enabled( boolean_c value )
	{
		_textures_enabled = value;
	}

	resource_file_texture_c * scene_component_model_c::custom_material_c::get_textures( sint32_c index )
	{
		assert( index >= 0 && index < 4 );
		return _textures[ index ];
	}

	void_c scene_component_model_c::custom_material_c::set_textures( sint32_c index, resource_file_texture_c * value )
	{
		assert( index >= 0 && index < 4 );
		_textures[ index ] = value;
	}

	boolean_c scene_component_model_c::custom_material_c::get_pixel_shader_enabled() const
	{
		return _pixel_shader_enabled;
	}

	void_c scene_component_model_c::custom_material_c::set_pixel_shader_enabled( boolean_c value )
	{
		_pixel_shader_enabled = value;
	}

	video_renderer_pixel_shader_c * scene_component_model_c::custom_material_c::get_pixel_shader() const
	{
		return _pixel_shader;
	}

	void_c scene_component_model_c::custom_material_c::set_pixel_shader( video_renderer_pixel_shader_c * value )
	{
		_pixel_shader = value;
	}

	core_list_c< scene_component_model_c::custom_material_c * > & scene_component_model_c::get_custom_material_list()
	{
		assert( _custom_material_list_is_locked == false );
		return _custom_material_list;
	}

	void_c scene_component_model_c::unapply_custom_material_list()
	{
		for ( sint32_c i = 0; i < _mesh_list.get_length(); i++ )
		{
			_mesh_list[ i ]._custom_material_assignment = nullptr;
		}
		_custom_material_list_is_locked = false;
	}

	void_c scene_component_model_c::apply_custom_material_list()
	{
		for ( sint32_c i = 0; i < _custom_material_list.get_length(); i++ )
		{
			custom_material_c * material_assignment = _custom_material_list[ i ];
			core_list_c< mesh_c * > mesh_list;
			find_meshes( material_assignment->_target_mesh_names, mesh_list );
			for ( sint32_c j = 0; j < mesh_list.get_length(); j++ )
			{
				mesh_list[ j ]->_custom_material_assignment = material_assignment;
			}
		}
		_custom_material_list_is_locked = true;
	}




	//
	// animation player stack.
	//

	scene_component_model_c::animation_player_c::object_player_c::property_player_c::property_player_c()
		: property_type( 0 )
		//, property_index( 0 )
		, key_current( 0 )
		, key_minimum( 0 )
		, key_maximum( 0 )
	{
	}

	scene_component_model_c::animation_player_c::object_player_c::object_player_c()
		: object_name()
		, object_index_for_bone( -1 )
		, property_player_list()
		, property_player_for_position( nullptr )
		, property_player_for_rotation( nullptr )
		, property_player_for_scale( nullptr )
	{
	}

	void_c scene_component_model_c::animation_player_c::_handle_on_animation_resources_updated( scene_component_model_c * model )
	{
		// save current time before we unbind.
		if ( _time_to_bind_type == time_to_bind_type_e_seconds )
		{
			_time_to_bind = _time;
		}
		else
		{
			_time_to_bind = 0.5f;
			float32_c span = ( _source_animation->time_out - _source_animation->time_in );
			if ( span > 0.0f )
			{
				_time_to_bind = ( _time - _source_animation->time_in ) / span;
			}
		}

		// unbind.
		_source_model = nullptr;
		_source_animation = nullptr;
		_object_player_list.remove_all();

		// acquire new animation.
		animation_map_c * animation_map = _mother_model->_animation_map_dictionary.find_value_pointer( _animation_name );
		if ( animation_map != nullptr )
		{
			_source_model = animation_map->source_model;
			_source_animation = animation_map->source_animation;

			// instantiate and bind object players.
			uint32_c animation_object_count = _source_animation->object_end - _source_animation->object_start;
			_object_player_list.set_length_absolute( animation_object_count );
			for ( uint32_c i = 0; i < animation_object_count; i++ )
			{
				resource_file_model_c::animation_object_c const * source_animation_object = &_source_model->get_data().animation_object_list[ _source_animation->object_start + i ];

				object_player_c * animation_object_player = &_object_player_list[ i ];

				// bind animation to bone.
				animation_object_player->object_name = string8_c( core_list_mode_e_static, &_source_model->get_data().string_table[ source_animation_object->name ] );
				animation_object_player->object_index_for_bone = _mother_model->find_bone_index_with_name( animation_object_player->object_name );
				if ( animation_object_player->object_index_for_bone >= 0 )
				{
					// object being bound to is a bone.
					animation_object_player->property_player_list.set_length_absolute( 3 );
					animation_object_player->property_player_list[ 0 ].property_type = static_cast< uint32_c >( resource_file_model_c::animation_property_c::type_e_position );
					//animation_object_player->property_player_list[ 0 ].property_index = 0;
					animation_object_player->property_player_list[ 1 ].property_type = static_cast< uint32_c >( resource_file_model_c::animation_property_c::type_e_rotation );
					//animation_object_player->property_player_list[ 1 ].property_index = 1;
					animation_object_player->property_player_list[ 2 ].property_type = static_cast< uint32_c >( resource_file_model_c::animation_property_c::type_e_scale );
					//animation_object_player->property_player_list[ 2 ].property_index = 2;
					uint32_c animation_property_count = source_animation_object->property_end - source_animation_object->property_start;
					for ( uint32_c j = 0; j < animation_property_count; j++ )
					{
						resource_file_model_c::animation_property_c const * source_animation_property = &_source_model->get_data().animation_property_list[ source_animation_object->property_start + j ];
						for ( sint32_c k = 0; k < animation_object_player->property_player_list.get_length(); k++ )
						{
							if ( source_animation_property->type == animation_object_player->property_player_list[ k ].property_type )
							{
								object_player_c::property_player_c * animation_property_player = &animation_object_player->property_player_list[ k ];
								animation_property_player->key_current = source_animation_property->key_start;
								animation_property_player->key_minimum = source_animation_property->key_start;
								animation_property_player->key_maximum = source_animation_property->key_end;
								break;
							}
						}
					}
					animation_object_player->property_player_for_position = animation_object_player->property_player_list[ 0 ].key_minimum >= 0 ? & animation_object_player->property_player_list[ 0 ] : 0;
					animation_object_player->property_player_for_rotation = animation_object_player->property_player_list[ 1 ].key_minimum >= 0 ? & animation_object_player->property_player_list[ 1 ] : 0;
					animation_object_player->property_player_for_scale = animation_object_player->property_player_list[ 2 ].key_minimum >= 0 ? & animation_object_player->property_player_list[ 2 ] : 0;
				}
				// bind animation to particle emmitter
				// ...
				// bind animation to material
				// ...
				// bind animation to light
				// ...
			}

			// bind time.
			if ( _time_to_bind_type == time_to_bind_type_e_seconds )
			{
				set_time( _time_to_bind );
			}
			else
			{
				set_time_percent( _time_to_bind );
			}
		}
	}

	scene_component_model_c::animation_player_c::animation_player_c( scene_component_model_c * mother_model )
		: _mother_model( mother_model )
		, _animation_name()
		, _source_model( nullptr )
		, _source_animation( nullptr )
		, _object_player_list()
		, _blend_type( blend_type_e_mix )
		, _blend_weight( 0.0f )
		, _loop_count_current( 0 )
		, _loop_count_target( 0 )
		, _speed( 1.0f )
		, _time( 0.0f )
		, _time_to_bind_type( time_to_bind_type_e_seconds )
		, _time_to_bind( 0.0f )
		//, _time_from_other( nullptr )
	{
		assert( _mother_model != nullptr );
		_mother_model->_on_animation_resources_updated.subscribe( this, &animation_player_c::_handle_on_animation_resources_updated );
	}

	scene_component_model_c::animation_player_c::~animation_player_c()
	{
		_mother_model->_on_animation_resources_updated.unsubscribe( this, &animation_player_c::_handle_on_animation_resources_updated );
	}

	void_c scene_component_model_c::animation_player_c::update( float32_c time_step )
	{
		assert( _source_animation != nullptr );

		if ( _blend_weight == 0.0f )
		{
			return;
		}

		//if ( _time_from_other == nullptr )
		{
			if ( _loop_count_target > 0 && _loop_count_current >= _loop_count_target )
			{
				return;
			}
			//if ( _speed_type == speed_type_e_normal )
			//{
			//	time_step *= _speed;
			//}
			//else
			//{
			//	time_step *= ( _speed / _animation->locomotion_speed );
			//}
			time_step *= _speed;
			if ( time_step != 0.0f )
			{
				_time = static_cast< float32_c >( _time + time_step );
				if ( _time >= _source_animation->time_out || _time < _source_animation->time_in )
				{
					// time went past bounds.
					_loop_count_current++;
					if ( _loop_count_target > 0 && _loop_count_current >= _loop_count_target )
					{
						if ( _time >= _source_animation->time_out )
						{
							// clamp time to end.
							set_time( _source_animation->time_out );
						}
						else
						{
							// clamp time to start.
							set_time( _source_animation->time_in );
						}
					}
					else
					{
						// wrap time around.
						set_time( ops::math_wrap( _time - _source_animation->time_in, _source_animation->time_out - _source_animation->time_in ) + _source_animation->time_in );
					}
				}
				else
				{
					if ( time_step > 0.0f )
					{
						// moving forward, update key indices where needed
						for ( sint32_c object_player_index = 0; object_player_index < _object_player_list.get_length(); object_player_index++ )
						{
							animation_player_c::object_player_c & object_player = _object_player_list[ object_player_index ];
							for ( sint32_c property_player_index = 0; property_player_index < object_player.property_player_list.get_length(); property_player_index++ )
							{
								animation_player_c::object_player_c::property_player_c & property_player = object_player.property_player_list[ property_player_index ];
								if ( property_player.key_current >= 0 )
								{
									for ( sint32_c key_index = property_player.key_current; key_index < property_player.key_maximum; key_index++ )
									{
										if ( _source_model->get_data().animation_key_list[ key_index ].time > _time )
										{
											break;
										}
										property_player.key_current = key_index;
									}
								}
							}
						}
					}
					else
					{
						// moving backward
						sint32_c object_count = _object_player_list.get_length();
						for ( sint32_c i = 0; i < object_count; i++ )
						{
							object_player_c & object_player = _object_player_list[ i ];
							for ( sint32_c j = 0; j < object_player.property_player_list.get_length(); j++ )
							{
								object_player_c::property_player_c & property_player = object_player.property_player_list[ j ];
								if ( property_player.key_current >= 0 )
								{
									for ( sint32_c k = property_player.key_current; k >= property_player.key_minimum; k-- )
									{
										if ( _source_model->get_data().animation_key_list[ k ].time < _time )
										{
											property_player.key_current = k;
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		//else
		//{
		//	set_time( _time_from_other->get_time() );
		//}
	}

	void_c scene_component_model_c::animation_player_c::play( sint32_c loop_count )
	{
		_loop_count_current = 0;
		_loop_count_target = loop_count;
		if ( _source_animation != nullptr )
		{
			set_time( _source_animation->time_in );
		}
	}

	void_c scene_component_model_c::animation_player_c::set_animation( string8_c const & value )
	{
		_animation_name = value;
		_handle_on_animation_resources_updated( nullptr );
	}

	void_c scene_component_model_c::animation_player_c::clear_animation()
	{
		_animation_name = "";
		_source_animation = nullptr;
	}

	scene_component_model_c::animation_player_c::blend_type_e scene_component_model_c::animation_player_c::get_blend_type()
	{
		return _blend_type;
	}

	void_c scene_component_model_c::animation_player_c::set_blend_type( blend_type_e value )
	{
		_blend_type = value;
	}

	float32_c scene_component_model_c::animation_player_c::get_blend_weight()
	{
		return _blend_weight;
	}

	void_c scene_component_model_c::animation_player_c::set_blend_weight( float32_c value )
	{
		_blend_weight = value;
	}

	void_c scene_component_model_c::animation_player_c::traverse_blend_weight( float32_c value, float32_c amount )
	{
		_blend_weight = ops::traverse( _blend_weight, value, amount );
	}

	float32_c scene_component_model_c::animation_player_c::get_animation_move_speed()
	{
		if ( _source_animation )
		{
			return _source_animation->move_speed;
		}
		return 1.0f;
	}

	float32_c scene_component_model_c::animation_player_c::get_speed()
	{
		return _speed;
	}

	void_c scene_component_model_c::animation_player_c::set_speed( float32_c value )
	{
		_speed = value;
	}

	float32_c scene_component_model_c::animation_player_c::get_time()
	{
		return _time;
	}

	void_c scene_component_model_c::animation_player_c::set_time( float32_c value )
	{
		//_time_from_other = nullptr;
		_time_to_bind_type = time_to_bind_type_e_seconds;
		if ( _source_animation != nullptr )
		{
			_time = value;
			for ( sint32_c object_player_index = 0; object_player_index < _object_player_list.get_length(); object_player_index++ )
			{
				object_player_c & object_player = _object_player_list[ object_player_index ];
				for ( sint32_c property_player_index = 0; property_player_index < object_player.property_player_list.get_length(); property_player_index++ )
				{
					object_player_c::property_player_c & property_player = object_player.property_player_list[ property_player_index ];
					for ( sint32_c key_index = property_player.key_minimum; key_index < property_player.key_maximum; key_index++ )
					{
						if ( _source_model->get_data().animation_key_list[ key_index ].time > _time )
						{
							break;
						}
						property_player.key_current = key_index;
					}
				}
			}
		}
		else
		{
			_time_to_bind = value;
		}
	}

	float32_c scene_component_model_c::animation_player_c::get_time_percent()
	{
		if ( _source_animation )
		{
			return ( _time - _source_animation->time_in ) / ( _source_animation->time_out - _source_animation->time_in );
		}
		return 0.0f;
	}

	void_c scene_component_model_c::animation_player_c::set_time_percent( float32_c value )
	{
		//_time_from_other = nullptr;
		_time_to_bind_type = time_to_bind_type_e_percent;
		if ( _source_animation != nullptr )
		{
			set_time( value * ( _source_animation->time_out - _source_animation->time_in ) + _source_animation->time_in );
		}
		else
		{
			_time_to_bind = value;
		}
	}

	void_c scene_component_model_c::animation_player_c::sample_property_player( animation_player_c::object_player_c::property_player_c * property_player, vector32x4_c & result )
	{
		sint32_c key_current = property_player->key_current;
		sint32_c key_minimum = property_player->key_minimum;
		sint32_c key_maximum = property_player->key_maximum - 1;

		//sint32_c key_a_index = ops::clamp( key_current - 1, key_minimum, key_maximum );
		sint32_c key_b_index = ops::math_clamp( key_current, key_minimum, key_maximum );
		sint32_c key_c_index = ops::math_clamp( key_current + 1, key_minimum, key_maximum );
		//sint32_c key_d_index = ops::clamp( key_current + 2, key_minimum, key_maximum );

		//const resource_file_model_c::animation_key_c & key_a = _source_model->_header_extras.animation_key_list[ key_a_index ];
		const resource_file_model_c::animation_key_c & key_b = _source_model->get_data().animation_key_list[ key_b_index ];
		const resource_file_model_c::animation_key_c & key_c = _source_model->get_data().animation_key_list[ key_c_index ];
		//const resource_file_model_c::animation_key_c & key_d = _source_model->_header_extras.animation_key_list[ key_d_index ];

		float32_c weight = 1.0f;
		if ( key_c.time > key_b.time )
		{
			weight = ( _time - key_b.time ) / ( key_c.time - key_b.time );
		}

		result.a = ops::interpolate_cosine( key_b.value[ 0 ], key_c.value[ 0 ], weight );
		result.b = ops::interpolate_cosine( key_b.value[ 1 ], key_c.value[ 1 ], weight );
		result.c = ops::interpolate_cosine( key_b.value[ 2 ], key_c.value[ 2 ], weight );
		result.d = ops::interpolate_cosine( key_b.value[ 3 ], key_c.value[ 3 ], weight );
		//result.a = ops::interpolate_hermite( key_a.value.a, key_b.value.a, key_c.value.a, key_d.value.a, weight, 0.0f, 0.0f );
		//result.b = ops::interpolate_hermite( key_a.value.b, key_b.value.b, key_c.value.b, key_d.value.b, weight, 0.0f, 0.0f );
		//result.c = ops::interpolate_hermite( key_a.value.c, key_b.value.c, key_c.value.c, key_d.value.c, weight, 0.0f, 0.0f );
		//result.d = ops::interpolate_hermite( key_a.value.d, key_b.value.d, key_c.value.d, key_d.value.d, weight, 0.0f, 0.0f );
	}

	boolean_c scene_component_model_c::animation_player_c::sample_position( sint32_c object_player_index, vector32x3_c & result )
	{
		object_player_c & object_player = _object_player_list[ object_player_index ];
		if ( object_player.property_player_for_position != nullptr )
		{
			vector32x4_c intermediate_result;
			sample_property_player( object_player.property_player_for_position, intermediate_result );
			result.a = intermediate_result.a;
			result.b = intermediate_result.b;
			result.c = intermediate_result.c;
			return true;
		}
		return false;
	}

	boolean_c scene_component_model_c::animation_player_c::sample_rotation( sint32_c object_player_index, quaternion32_c & result )
	{
		object_player_c & object_player = _object_player_list[ object_player_index ];
		if ( object_player.property_player_for_rotation != nullptr )
		{
			vector32x4_c intermediate_result;
			sample_property_player( object_player.property_player_for_rotation, intermediate_result );
			result.a = intermediate_result.a;
			result.b = intermediate_result.b;
			result.c = intermediate_result.c;
			result.d = intermediate_result.d;
			result = ops::normal_quaternion32( result );
			return true;
		}
		return false;
	}

	boolean_c scene_component_model_c::animation_player_c::sample_scale( sint32_c object_player_index, vector32x3_c & result )
	{
		object_player_c & object_player = _object_player_list[ object_player_index ];
		if ( object_player.property_player_for_scale != nullptr )
		{
			vector32x4_c intermediate_result;
			sample_property_player( object_player.property_player_for_scale, intermediate_result );
			result.a = intermediate_result.a;
			result.b = intermediate_result.b;
			result.c = intermediate_result.c;
			return true;
		}
		return false;
	}




	//
	// miscellaneous options.
	//

	float32_c scene_component_model_c::get_opacity() const
	{
		return _opacity;
	}

	void_c scene_component_model_c::set_opacity( float32_c value )
	{
		_opacity = value;
	}

	boolean_c scene_component_model_c::get_render_enabled() const
	{
		return _render_enabled;
	}

	void_c scene_component_model_c::set_render_enabled( boolean_c value )
	{
		_render_enabled = value;
	}

	boolean_c scene_component_model_c::get_shadow_cast_enabled() const
	{
		return _shadow_cast_enabled;
	}

	void_c scene_component_model_c::set_shadow_cast_enabled( boolean_c value )
	{
		_shadow_cast_enabled = value;
	}

	boolean_c scene_component_model_c::get_light_cast_enabled() const
	{
		return _light_cast_enabled;
	}

	void_c scene_component_model_c::set_light_cast_enabled( boolean_c value )
	{
		_light_cast_enabled = value;
	}

	boolean_c scene_component_model_c::get_automatic_light_probe_invalidation_enabled() const
	{
		return _automatic_light_probe_invalidation_enabled;
	}

	void_c scene_component_model_c::set_automatic_light_probe_invalidation_enabled( boolean_c value )
	{
		_automatic_light_probe_invalidation_enabled = value;
	}

	boolean_c scene_component_model_c::get_local_light_probe_enabled() const
	{
		return _local_light_probe_enabled;
	}

	void_c scene_component_model_c::set_local_light_probe_enabled( boolean_c value )
	{
		if ( _local_light_probe_enabled != value )
		{
			_local_light_probe_enabled = value;
			if ( _local_light_probe_enabled )
			{
				assert( _local_light_probe_component == nullptr );
				_local_light_probe_component = scene_component_light_probe_c::make_new_instance();
				_local_light_probe_component->set_local_model( this );
				_scene_object->add_component( _local_light_probe_component );
			}
			else
			{
				assert( _local_light_probe_component );
				_scene_object->remove_component( _local_light_probe_component );
				_local_light_probe_component = nullptr; // we don't need to delete it, as removing it from the scene object will remove a reference count from it.
			}
		}
	}




	//
	// normal scene component stuff.
	//

	void_c scene_component_model_c::_update_world_transforms()
	{
		// update world space transforms of bones from animated local space properties.
		core_linked_list_c< bone_c * >::node_c const * root_bone_list_node = _root_bone_list.get_first();
		while ( root_bone_list_node != nullptr )
		{
			root_bone_list_node->get_value()->update_transforms_recursive();
			root_bone_list_node = root_bone_list_node->get_next();
		}

		// update bone attachments.
		for ( sint32_c i = 0; i < _attachment_point_list.get_length(); i++ )
		{
			attachment_point_c * attachment_point = &_attachment_point_list[ i ];
			if ( attachment_point->_mother_bone )
			{
				attachment_point->_world_space_transform = attachment_point->_local_space_transform * attachment_point->_mother_bone->_world_space_transform;
			}
			else
			{
				attachment_point->_world_space_transform = attachment_point->_local_space_transform * _scene_object->get_world_space_transform();
			}
		}

		// update lights.
		for ( sint32_c i = 0; i < _light_list.get_length(); i++ )
		{
			light_c * light = &_light_list[ i ];
			if ( light->_mother_bone )
			{
				light->_world_space_transform = light->_local_space_transform * light->_mother_bone->_world_space_transform;
			}
			else
			{
				light->_world_space_transform = light->_local_space_transform * _scene_object->get_world_space_transform();
			}
		}
	}

	void_c scene_component_model_c::_handle_before_removed_from_scene()
	{
		scene_component_c::_handle_before_removed_from_scene();
		for ( sint32_c i = 0; i < _light_list.get_length(); i++ )
		{
			_scene_object->get_scene()->remove_light( &_light_list[ i ]._scene_light );
		}
		if ( _scene_object->get_scene()->_automatic_light_probe_invalidation_enabled )
		{
			invalidate_light_probes();
		}
	}

	void_c scene_component_model_c::_handle_after_added_to_scene()
	{
		scene_component_c::_handle_after_added_to_scene();
		if ( _scene_object->get_scene()->_automatic_light_probe_invalidation_enabled )
		{
			invalidate_light_probes();
		}
		for ( sint32_c i = 0; i < _light_list.get_length(); i++ )
		{
			_scene_object->get_scene()->add_light( &_light_list[ i ]._scene_light );
		}
	}

	void_c scene_component_model_c::_handle_on_world_space_transform_modified( transform3d_c const & old_world_space_transform, scene_component_c * initiator )
	{
		scene_component_c::_handle_on_world_space_transform_modified( old_world_space_transform, initiator );
		if ( _automatic_light_probe_invalidation_enabled && _scene_object->get_scene()->_automatic_light_probe_invalidation_enabled && old_world_space_transform != _scene_object->get_world_space_transform() )
		{
			_scene_object->get_scene()->invalidate_light_probes_with_box( box64x3_c( _local_space_obb ), old_world_space_transform );
			_scene_object->get_scene()->invalidate_light_probes_with_box( box64x3_c( _local_space_obb ), _scene_object->get_world_space_transform() );
		}
	}

	scene_component_model_c::scene_component_model_c()
		: scene_component_c()
		, _mother_model( nullptr )
		, _daughter_model_list()
		, _model_resource_is_bound( false )
		, _model_resource()
		, _animations_resource_list()
		, _materials_resource()
		, _animation_map_dictionary()
		, _on_animation_resources_updated()
		, _mesh_list()
		, _bone_list()
		, _root_bone_list()
		, _bone_skin_matrix_list()
		, _bone_logic_list ()
		, _attachment_point_list()
		, _light_list()
		, _vertex_skin_mode( vertex_skin_mode_e_gpu )
		, _cpu_vertex_buffer_mesh_base_shape_transformed()
		, _gpu_vertex_buffer_mesh_base_shape_transformed( nullptr )
		, _cpu_vertex_buffer_mesh_base_shape_and_bone_transformed()
		, _gpu_vertex_buffer_mesh_base_shape_and_bone_transformed( nullptr )
		, _physics_rigid_body( nullptr )
		, _physics_shape( nullptr )
		, _physics_constraint_list()
		, _physics_is_enabled( true )
		, _physics_is_added_to_simulation( false ) 
		, _custom_object_colors_enabled( false )
		, _custom_object_colors()
		, _custom_object_textures_enabled( false )
		, _custom_object_textures()
		, _custom_material_list_is_locked( false )
		, _custom_material_list()
		, animation_player_list()
		, _world_space_transform_inverse()
		, _opacity( 1.0f )
		, _render_enabled( true )
		, _shadow_cast_enabled( true )
		, _light_cast_enabled( true )
		, _automatic_light_probe_invalidation_enabled( true )
		, _local_light_probe_enabled( false )
		, _local_light_probe_component( nullptr )
		, _fixed_light_probe_component( nullptr )
		, _render_light_indices_count( 0 )
		, _render_light_indices()
		, _render_frame_last( 0 )
		, _render_lod_scale( 0.0f )
		, on_model_resource_bound()
		, on_model_resource_unbound()
	{
	}

	scene_component_model_c::~scene_component_model_c()
	{
		assert( _mother_model == nullptr ); // should be disassociated before being deleted.
		assert( animation_player_list.get_length() == 0 ); // we don't want to risk taking responsibility for managing dangling animation player instances, so we assert here to warn about potential for leak. these animation players could be on the heap or the stack, there may be others that were allocated for us but are not currently in our list, etc. paranoid.
		set_materials_resource( nullptr ); // releases reference and unsubscribes from events.
		remove_all_animations_resources(); // releases reference and unsubscribes from events.
		set_model_resource( nullptr ); // releases reference and unsubscribes from events. also removes physics objects from physics simulation scene, and deletes them.
	}

	scene_component_model_c * scene_component_model_c::make_new_instance()
	{
		return new scene_component_model_c();
	}

	void_c scene_component_model_c::update_animations( float32_c time_step )
	{
		// 
		_world_space_transform_inverse = _scene_object->get_world_space_transform().get_inverted();

		// reset pose properties so we don't inadvertently accumulate values from the last frame.
		for ( sint32_c i = 0; i < _bone_list.get_length(); i++ )
		{
			_bone_list[ i ]._local_space_transform.reset();
		}

		// update animation players and sample animated properties.
		vector32x3_c sampled_position;
		quaternion32_c sampled_orientation;
		vector32x3_c sampled_scale;
		for ( sint32_c i = 0; i < animation_player_list.get_length(); i++ )
		{
			animation_player_c * animation_player = animation_player_list[ i ];
			if ( animation_player == nullptr || animation_player->_source_animation == nullptr || animation_player->_blend_weight == 0.0f )
			{
				continue;
			}
			animation_player->update( time_step );
			if ( animation_player->get_blend_type() == animation_player_c::blend_type_e_mix )
			{
				for ( sint32_c object_player_index = 0; object_player_index < animation_player->_object_player_list.get_length(); object_player_index++ )
				{
					animation_player_c::object_player_c & object_player = animation_player->_object_player_list[ object_player_index ];
					if ( object_player.object_index_for_bone >= 0 )
					{
						bone_c * bone = &_bone_list[ object_player.object_index_for_bone ];
						if ( animation_player->sample_position( object_player_index, sampled_position ) )
						{
							bone->_local_space_transform.position = vector64x3_c( ops::interpolate_linear( vector32x3_c( bone->_local_space_transform.position ), sampled_position, animation_player->_blend_weight ) );
						}
						if ( animation_player->sample_rotation( object_player_index, sampled_orientation ) )
						{
							bone->_local_space_transform.rotation = ops::interpolate_spherical_linear( bone->_local_space_transform.rotation, sampled_orientation, animation_player->_blend_weight );
						}
						if ( animation_player->sample_scale( object_player_index, sampled_scale ) )
						{
							bone->_local_space_transform.scale = ops::interpolate_linear( bone->_local_space_transform.scale, sampled_scale, animation_player->_blend_weight );
						}
					}
				}
			}
			else if ( animation_player->_blend_type == animation_player_c::blend_type_e_add )
			{
				for ( sint32_c object_player_index = 0; object_player_index < animation_player->_object_player_list.get_length(); object_player_index++ )
				{
					animation_player_c::object_player_c & object_player = animation_player->_object_player_list[ object_player_index ];
					if ( object_player.object_index_for_bone >= 0 )
					{
						bone_c * bone = &_bone_list[ object_player.object_index_for_bone ];
						if ( animation_player->sample_position( object_player_index, sampled_position ) )
						{
							bone->_local_space_transform.position = bone->_local_space_transform.position + sampled_position * animation_player->_blend_weight;
						}
						if ( animation_player->sample_rotation( object_player_index, sampled_orientation ) )
						{
							bone->_local_space_transform.rotation = bone->_local_space_transform.rotation * ops::interpolate_spherical_linear( quaternion32_c(), sampled_orientation, animation_player->_blend_weight );
						}
						if ( animation_player->sample_scale( object_player_index, sampled_scale ) )
						{
							bone->_local_space_transform.scale = bone->_local_space_transform.scale * ops::interpolate_linear( 1.0f, sampled_scale.a, animation_player->_blend_weight );
						}
					}
				}
			}
		}

		// update world space transforms of bones, attachments, and lights.
		_update_world_transforms();

		// update bone logics.
		for ( sint32_c i = 0; i < _bone_logic_list.get_length(); i++ )
		{
			bone_logic_c * bone_logic = _bone_logic_list[ i ];
			bone_logic->update( time_step );
		}

		// update daughter models.
		for ( sint32_c i = 0; i < _daughter_model_list.get_length(); i++ )
		{
			_daughter_model_list[ i ]->update_animations( time_step );
		}

		// update local space bounding box.
		if ( _bone_list.get_length() > 0 )
		{
			_local_space_obb = box32x3_c( _model_resource->get_data().mesh_box );
			boolean_c first_bone = true;
			for ( sint32_c i = 0; i < _bone_list.get_length(); i++ )
			{
				bone_c * bone = &_bone_list[ i ];
				if ( bone->_source_bone->flags & resource_file_model_c::bone_c::flags_e_rebound )
				{
					vector32x3_c head_point = ops::rotate_scale_and_translate_vector32x3( vector32x3_c( bone->_source_bone->head_position ), bone->_object_space_skin_matrix );
					vector32x3_c tail_point = ops::rotate_scale_and_translate_vector32x3( vector32x3_c( bone->_source_bone->tail_position ), bone->_object_space_skin_matrix );
					float32_c head_radius = bone->_source_bone->head_radius * bone->_local_space_transform.scale.get_largest_element();
					float32_c tail_radius = bone->_source_bone->tail_radius * bone->_local_space_transform.scale.get_largest_element();
					if ( !first_bone )
					{
						_local_space_obb.accumulate_bounds( head_point, head_radius );
						_local_space_obb.accumulate_bounds( tail_point, tail_radius );
					}
					else
					{
						first_bone = false;
						_local_space_obb.set_bounds( head_point, head_radius );
						_local_space_obb.accumulate_bounds( tail_point, tail_radius );
					}
				}
			}
		}
	}

	void_c scene_component_model_c::invalidate_light_probes()
	{
		_scene_object->get_scene()->invalidate_light_probes_with_box( box64x3_c( _local_space_obb ), _scene_object->get_world_space_transform() );
	}

	void_c scene_component_model_c::reseat()
	{
		assert( _scene_object );

		if ( _mother_model )
		{
			assert( _scene_object == _mother_model->_scene_object );
			//_object->_position = vector32x3_c();
			//_object->_rotation = quaternion64_c();
			//_object->_scale = vector32x3_c( 1.0f, 1.0f, 1.0f );
			//_object->_world_transform = _mother_model_component->_object->_world_transform;
			//_scene_transform_inverted = make_inverted( _object->_world_transform );
		}

		for ( sint32_c i = 0; i < _bone_logic_list.get_length(); i++ )
		{
			_bone_logic_list[ i ]->start();
		}

		for ( sint32_c i = 0; i < _daughter_model_list.get_length(); i++ )
		{
			_daughter_model_list[ i ]->reseat();
		}
	}

	//void_c scene_component_model_c::apply_mesh_shape_keys()
	//{
	//	if ( get_model_resource_is_bound() )
	//	{
	//		if ( _model_resource->get_data().mesh_shape_key_list.get_length() > 0 )
	//		{
	//			for ( sint32_c i = 0; i < _mesh_list.get_length(); i++ )
	//			{
	//				mesh_c & mesh = _mesh_list[ i ];
	//				for ( sint32_c j = 0; j < mesh._shape_key_list.get_length(); j++ )
	//				{
	//					mesh_c::shape_key_c const & shape_key = mesh._shape_key_list[ j ];
	//				}
	//			}
	//		}
	//	}
	//}

}
