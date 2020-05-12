#include "cheonsa_scene_component_light_probe.h"
#include "cheonsa_scene_component_model.h"
#include "cheonsa_scene_object.h"
#include "cheonsa_scene.h"

namespace cheonsa
{

	void_c scene_component_light_probe_c::_handle_before_removed_from_scene()
	{
		scene_component_c::_handle_before_removed_from_scene();
		_scene_object->get_scene()->_light_probe_list.insert_at_end( &_scene_light_probe_list_node );
		_is_up_to_date = false;
	}

	void_c scene_component_light_probe_c::_handle_after_added_to_scene()
	{
		scene_component_c::_handle_after_added_to_scene();
		_scene_object->get_scene()->_light_probe_list.remove( &_scene_light_probe_list_node );
		if ( _scene_object->get_scene()->_last_baked_light_probe == this )
		{
			_scene_object->get_scene()->_last_baked_light_probe = nullptr;
		}
	}

	void_c scene_component_light_probe_c::_handle_on_world_space_transform_modified( transform3d_c const & old_world_space_transform, scene_component_c * initiator )
	{
		scene_component_c::_handle_on_world_space_transform_modified( old_world_space_transform, initiator );
		if ( old_world_space_transform.position != _scene_object->get_world_space_transform().position )
		{
			_is_up_to_date = false;
		}
	}

	scene_component_light_probe_c::scene_component_light_probe_c()
		: scene_component_c()
		, _scene_light_probe_list_node( this )
		, _cube_color( nullptr )
		, _cube_depth_stencil( nullptr )
		, _is_up_to_date( false )
		, _local_model( nullptr )
	{
	}

	scene_component_light_probe_c::~scene_component_light_probe_c()
	{
		if ( _cube_color )
		{
			delete _cube_color;
			_cube_color = nullptr;
		}
		if ( _cube_depth_stencil )
		{
			delete _cube_depth_stencil;
			_cube_depth_stencil = nullptr;
		}
	}

	scene_component_light_probe_c * scene_component_light_probe_c::make_new_instance()
	{
		return new scene_component_light_probe_c();
	}

	scene_component_model_c * scene_component_light_probe_c::get_local_model() const
	{
		return _local_model;
	}

	void_c scene_component_light_probe_c::set_local_model( scene_component_model_c * value )
	{
		if ( _local_model != value )
		{
			if ( _local_model )
			{
				_local_model->remove_reference();
			}
			_local_model = value;
			if ( _local_model )
			{
				_local_model->add_reference();
			}
		}
	}

}
