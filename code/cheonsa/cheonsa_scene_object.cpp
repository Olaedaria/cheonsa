#include "cheonsa_scene_object.h"
#include "cheonsa_scene_component.h"
#include "cheonsa_scene.h"

namespace cheonsa
{

	void_c scene_object_c::_handle_after_added_to_user_interface()
	{
		core_linked_list_c< scene_component_c * >::node_c const * scene_component_list_node = _scene_component_list.get_first();
		while ( scene_component_list_node )
		{
			scene_component_list_node->get_value()->_handle_after_added_to_user_interface();
			scene_component_list_node = scene_component_list_node->get_next();
		}
	}

	void_c scene_object_c::_handle_before_removed_from_user_interface()
	{
		core_linked_list_c< scene_component_c * >::node_c const * scene_component_list_node = _scene_component_list.get_first();
		while ( scene_component_list_node )
		{
			scene_component_list_node->get_value()->_handle_before_removed_from_user_interface();
			scene_component_list_node = scene_component_list_node->get_next();
		}
	}

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

	void_c scene_object_c::_handle_world_space_transform_modified( transform3d_c const & old_world_space_transform, scene_component_c * initiator )
	{
		core_linked_list_c< scene_component_c * >::node_c const * scene_component_list_node = _scene_component_list.get_first();
		while ( scene_component_list_node )
		{
			scene_component_list_node->get_value()->_handle_on_world_space_transform_modified( old_world_space_transform, initiator );
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
		, _reference_count( 0 )
	{
	}

	scene_object_c::~scene_object_c()
	{
		core_linked_list_c< scene_component_c * >::node_c * scene_component_list_node = _scene_component_list.get_first();
		while ( scene_component_list_node )
		{
			core_linked_list_c< scene_component_c * >::node_c * scene_component_list_node_next = scene_component_list_node->get_next(); // get next now, because current node might be deleted.
			scene_component_list_node->get_value()->_handle_before_removed_from_scene();
			_scene_component_list.remove( scene_component_list_node );
			scene_component_list_node->get_value()->_scene_object = nullptr;
			scene_component_list_node->get_value()->remove_reference(); // this may delete the component if its reference count reaches 0.
			scene_component_list_node = scene_component_list_node_next;
		}
	}

	void_c scene_object_c::add_reference()
	{
		_reference_count++;
	}

	void_c scene_object_c::remove_reference()
	{
		assert( _reference_count > 0 );
		_reference_count--;
		if ( _reference_count == 0 )
		{
			delete this;
		}
	}

	scene_c * scene_object_c::get_scene() const
	{
		return _scene;
	}

	transform3d_c const & scene_object_c::get_world_space_transform() const
	{
		return _world_space_transform;
	}

	void_c scene_object_c::set_world_space_transform( transform3d_c const & value, scene_component_c * who )
	{
		transform3d_c old_world_space_transform = _world_space_transform;
		_world_space_transform = value;
		_handle_world_space_transform_modified( old_world_space_transform, who );
	}

	void_c scene_object_c::set_world_space_position( vector64x3_c const & value, scene_component_c * who )
	{
		transform3d_c old_world_space_transform = _world_space_transform;
		_world_space_transform.position = value;
		_handle_world_space_transform_modified( old_world_space_transform, who );
	}

	void_c scene_object_c::set_world_space_rotation( quaternion32_c const & value, scene_component_c * who )
	{
		transform3d_c old_world_space_transform = _world_space_transform;
		_world_space_transform.rotation = value;
		_handle_world_space_transform_modified( old_world_space_transform, who );
	}

	void_c scene_object_c::set_world_space_scale( vector32x3_c const & value, scene_component_c * who )
	{
		transform3d_c old_world_space_transform = _world_space_transform;
		_world_space_transform.scale = value;
		_handle_world_space_transform_modified( old_world_space_transform, who );
	}

	void_c scene_object_c::add_component( scene_component_c * component )
	{
		assert( component );
		assert( component->_scene_object == nullptr );
		component->add_reference();
		component->_scene_object = this;
		_scene_component_list.insert_at_end( &component->_scene_component_list_node );
		if ( _scene )
		{
			component->_handle_after_added_to_scene();
		}
	}

	void_c scene_object_c::remove_component( scene_component_c * component )
	{
		assert( component );
		assert( component->_scene_object == this );
		if ( _scene )
		{
			component->_handle_before_removed_from_scene();
		}
		_scene_component_list.remove( &component->_scene_component_list_node );
		component->_scene_object = nullptr;
		component->remove_reference();
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

	core_linked_list_c< scene_component_c * > const & scene_object_c::get_scene_component_list() const
	{
		return _scene_component_list;
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
