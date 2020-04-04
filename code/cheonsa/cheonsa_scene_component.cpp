#include "cheonsa_scene_component.h"
#include "cheonsa_scene_component_light.h"
#include "cheonsa_scene_component_light_probe.h"
#include "cheonsa_scene_component_menu_control.h"
#include "cheonsa_scene_component_model.h"
#include "cheonsa_scene_component_sound.h"
#include "cheonsa_scene_component_sprite.h"
#include "cheonsa_scene_object.h"
#include "cheonsa_scene.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	scene_component_c * scene_component_c::make_new_instance( uint8_c type_code )
	{
		if ( type_code == scene_component_light_c::get_type_code_static() )
		{
			return new scene_component_light_c();
		}
		else if ( type_code == scene_component_light_probe_c::get_type_code_static() )
		{
			return new scene_component_light_probe_c();
		}
		else if ( type_code == scene_component_menu_control_c::get_type_code_static() )
		{
			return new scene_component_menu_control_c();
		}
		else if ( type_code == scene_component_model_c::get_type_code_static() )
		{
			return new scene_component_model_c();
		}
		else if ( type_code == scene_component_sound_c::get_type_code_static() )
		{
			return new scene_component_sound_c();
		}
		else if ( type_code == scene_component_sprite_c::get_type_code_static() )
		{
			return new scene_component_sprite_c();
		}
		return nullptr;
	}

	void_c scene_component_c::_reset_local_space_obb()
	{
		_local_space_obb.minimum.a = -0.025f;
		_local_space_obb.minimum.b = -0.025f;
		_local_space_obb.minimum.c = -0.025f;
		_local_space_obb.maximum.a = 0.025f;
		_local_space_obb.maximum.b = 0.025f;
		_local_space_obb.maximum.c = 0.025f;
	}

	void_c scene_component_c::_update_world_space_abb_from_local_space_obb()
	{
		assert( _scene_object );
		_world_space_aabb = ops::make_aabb_from_obb( box64x3_c( _local_space_obb ), _scene_object->get_world_space_transform() );
	}

	void_c scene_component_c::_handle_after_added_to_scene()
	{
		_insert_or_update_component_tree();
	}

	void_c scene_component_c::_handle_before_removed_from_scene()
	{
		_remove_from_component_tree();
	}

	void_c scene_component_c::_handle_before_property_modified( scene_object_property_e property )
	{
	}

	void_c scene_component_c::_handle_after_property_modified( scene_object_property_e property )
	{
		if ( property & scene_object_property_e_position || property & scene_object_property_e_scale || property & scene_object_property_e_scale )
		{
			_insert_or_update_component_tree();
		}
	}

	void_c scene_component_c::_insert_or_update_component_tree()
	{
		if ( _scene_object && _scene_object->_scene )
		{
			_scene_object->_scene->_scene_component_tree.insert_or_update_item( this );
		}
	}

	void_c scene_component_c::_remove_from_component_tree()
	{
		if ( _scene_object && _scene_object->_scene )
		{
			_scene_object->_scene->_scene_component_tree.remove_item( this );
		}
	}

	scene_component_c::scene_component_c()
		: _scene_object( nullptr )
		, _component_list_node( this )
		, _scene_tree_leaf_node()
		, _scene_tree_list_node( this )
		, _local_space_obb()
		, _world_space_aabb()
	{
		_reset_local_space_obb();
	}

	scene_component_c::~scene_component_c()
	{
		_remove_from_component_tree();
	}

	scene_object_c * scene_component_c::get_scene_object() const
	{
		return _scene_object;
	}

	box32x3_c const & scene_component_c::get_local_space_obb() const
	{
		return _local_space_obb;
	}

	box64x3_c const & scene_component_c::get_world_space_aabb() const
	{
		return _world_space_aabb;
	}

	space_transform_c const & scene_component_c::get_world_space_transform() const
	{
		assert( _scene_object );
		return _scene_object->get_world_space_transform();
	}

}
