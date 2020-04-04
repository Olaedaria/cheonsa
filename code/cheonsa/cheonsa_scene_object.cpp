#include "cheonsa_scene_object.h"
#include "cheonsa_scene_component.h"
#include "cheonsa_scene.h"

namespace cheonsa
{

	void_c scene_object_c::_handle_after_added_to_scene()
	{
		core_linked_list_c< scene_component_c * >::node_c const * scene_component_list_node = _scene_component_list.get_first();
		while ( scene_component_list_node )
		{
			scene_component_list_node->get_value()->_handle_after_added_to_scene();
			scene_component_list_node = scene_component_list_node->get_next();
		}
	}
	
	void_c scene_object_c::_handle_before_removed_from_scene()
	{
		core_linked_list_c< scene_component_c * >::node_c const * scene_component_list_node = _scene_component_list.get_first();
		while ( scene_component_list_node )
		{
			scene_component_list_node->get_value()->_handle_before_removed_from_scene();
			scene_component_list_node = scene_component_list_node->get_next();
		}
	}

	void_c scene_object_c::_handle_before_property_modified( scene_object_property_e property )
	{
		core_linked_list_c< scene_component_c * >::node_c const * scene_component_list_node = _scene_component_list.get_first();
		while ( scene_component_list_node )
		{
			scene_component_list_node->get_value()->_handle_before_property_modified( property );
			scene_component_list_node = scene_component_list_node->get_next();
		}
	}

	void_c scene_object_c::_handle_after_property_modified( scene_object_property_e property )
	{
		core_linked_list_c< scene_component_c * >::node_c const * scene_component_list_node = _scene_component_list.get_first();
		while ( scene_component_list_node )
		{
			scene_component_list_node->get_value()->_handle_after_property_modified( property );
			scene_component_list_node = scene_component_list_node->get_next();
		}
	}

	scene_object_c::scene_object_c()
		: _scene_object_list_node( this )
		, _scene( nullptr )
		, _world_space_transform()
		, _outline_color_index( 0 )
		, _scene_component_list()
		, _user_pointer( nullptr )
	{
	}

	scene_object_c::~scene_object_c()
	{
		// these components should all be on the heap.
		core_linked_list_c< scene_component_c * >::node_c const * scene_component_list_node = _scene_component_list.get_first();
		while ( scene_component_list_node )
		{
			delete scene_component_list_node->get_value();
			scene_component_list_node = scene_component_list_node->get_next();
		}
		_scene_component_list.remove_all_quick_and_dirty();
	}

	scene_c * scene_object_c::get_scene() const
	{
		return _scene;
	}

	space_transform_c const & scene_object_c::get_world_space_transform() const
	{
		return _world_space_transform;
	}

	void_c scene_object_c::set_world_space_transform( space_transform_c const & value )
	{
		_handle_before_property_modified( static_cast< scene_object_property_e >( scene_object_property_e_position | scene_object_property_e_rotation | scene_object_property_e_scale ) );
		_world_space_transform = value;
		_handle_after_property_modified( static_cast< scene_object_property_e >( scene_object_property_e_position | scene_object_property_e_rotation | scene_object_property_e_scale ) );
	}

	vector64x3_c const & scene_object_c::get_world_space_position() const
	{
		return _world_space_transform.position;
	}

	void_c scene_object_c::set_world_space_position( vector64x3_c const & value )
	{
		_handle_before_property_modified( scene_object_property_e_position );
		_world_space_transform.position = value;
		_handle_after_property_modified( scene_object_property_e_position );
	}

	quaternion32_c const & scene_object_c::get_world_space_rotation() const
	{
		return _world_space_transform.rotation;
	}

	void_c scene_object_c::set_world_space_rotation( quaternion32_c const & value )
	{
		_handle_before_property_modified( scene_object_property_e_rotation );
		_world_space_transform.rotation = value;
		_handle_after_property_modified( scene_object_property_e_rotation );
	}

	vector32x3_c const & scene_object_c::get_world_space_scale() const
	{
		return _world_space_transform.scale;
	}

	void_c scene_object_c::set_world_space_scale( vector32x3_c const & value )
	{
		_handle_before_property_modified( scene_object_property_e_scale );
		_world_space_transform.scale = value;
		_handle_after_property_modified( scene_object_property_e_scale );
	}

	void_c scene_object_c::set_world_space_position_rotation( vector64x3_c const & position, quaternion32_c const & rotation )
	{
		_handle_before_property_modified( static_cast< scene_object_property_e >( scene_object_property_e_position | scene_object_property_e_rotation ) );
		_world_space_transform.position = position;
		_world_space_transform.rotation = rotation;
		_handle_after_property_modified( static_cast< scene_object_property_e >( scene_object_property_e_position | scene_object_property_e_rotation ) );
	}

	void_c scene_object_c::set_world_space_position_scale( vector64x3_c const & position, vector32x3_c const & scale )
	{
		_handle_before_property_modified( static_cast< scene_object_property_e >( scene_object_property_e_position | scene_object_property_e_scale ) );
		_world_space_transform.position = position;
		_world_space_transform.scale = scale;
		_handle_after_property_modified( static_cast< scene_object_property_e >( scene_object_property_e_position | scene_object_property_e_scale ) );
	}

	void_c scene_object_c::set_world_space_rotation_scale( quaternion32_c const & rotation, vector32x3_c const & scale )
	{
		_handle_before_property_modified( static_cast< scene_object_property_e >( scene_object_property_e_rotation | scene_object_property_e_scale ) );
		_world_space_transform.rotation = rotation;
		_world_space_transform.scale = scale;
		_handle_after_property_modified( static_cast< scene_object_property_e >( scene_object_property_e_rotation | scene_object_property_e_scale ) );
	}

	void_c scene_object_c::add_component( scene_component_c * component )
	{
		assert( component != nullptr );
		assert( component->_scene_object == nullptr );
		_scene_component_list.insert_at_end( &component->_component_list_node );
		component->_scene_object = this;
		if ( _scene )
		{
			component->_handle_after_added_to_scene();
		}
	}

	void_c scene_object_c::remove_component( scene_component_c * component )
	{
		assert( component != nullptr );
		assert( component->_scene_object == this );
		if ( _scene )
		{
			component->_handle_before_removed_from_scene();
		}
		_scene_component_list.remove( &component->_component_list_node );
		component->_scene_object = nullptr;
	}

	scene_component_c * scene_object_c::find_component( uint8_c type_code )
	{
		core_linked_list_c< scene_component_c * >::node_c const * scene_component_list_node = _scene_component_list.get_first();
		while ( scene_component_list_node )
		{
			if ( scene_component_list_node->get_value()->get_type_code() == type_code )
			{
				return scene_component_list_node->get_value();
			}
			scene_component_list_node = scene_component_list_node->get_next();
		}
		return nullptr;
	}

	core_linked_list_c< scene_component_c * > const * scene_object_c::get_scene_component_list() const
	{
		return &_scene_component_list;
	}

	uint8_c scene_object_c::get_outline_color_index() const
	{
		return _outline_color_index;
	}

	void_c scene_object_c::set_outline_color_index( uint8_c value )
	{
		_outline_color_index = value;
	}

	void_c * scene_object_c::get_user_pointer() const
	{
		return _user_pointer;
	}

	void_c scene_object_c::set_user_pointer( void_c * value )
	{
		_user_pointer = value;
	}

}
