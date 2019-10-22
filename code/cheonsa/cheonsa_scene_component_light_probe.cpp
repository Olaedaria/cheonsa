#include "cheonsa_scene_component_light_probe.h"
#include "cheonsa_scene_object.h"
#include "cheonsa_scene.h"

namespace cheonsa
{

	void_c scene_component_light_probe_c::_handle_after_added_to_scene()
	{
		scene_component_c::_handle_after_added_to_scene();
		_scene_object->get_scene()->_light_probe_list.insert_at_end( &_scene_light_probe_list_node );
		_is_up_to_date = false;
	}

	void_c scene_component_light_probe_c::_handle_before_removed_from_scene()
	{
		scene_component_c::_handle_before_removed_from_scene();
		_scene_object->get_scene()->_light_probe_list.remove( &_scene_light_probe_list_node );
		if ( _scene_object->get_scene()->_last_baked_light_probe == this )
		{
			_scene_object->get_scene()->_last_baked_light_probe = nullptr;
		}
		_is_up_to_date = false;
	}

	void_c scene_component_light_probe_c::_handle_after_property_modified( scene_object_property_e property )
	{
		if ( property & scene_object_property_e_position || property & scene_object_property_e_rotation || property & scene_object_property_e_scale )
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

}
