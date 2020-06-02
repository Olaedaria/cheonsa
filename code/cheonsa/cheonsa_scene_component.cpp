#include "cheonsa_scene_component.h"
#include "cheonsa_scene_object.h"
#include "cheonsa_scene.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

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

	void_c scene_component_c::_handle_after_added_to_user_interface()
	{
	}

	void_c scene_component_c::_handle_before_removed_from_user_interface()
	{
	}

	void_c scene_component_c::_handle_after_added_to_scene()
	{
		assert( _scene_object );
		assert( _scene_object->get_scene() );
		_insert_or_update_component_tree();
	}

	void_c scene_component_c::_handle_before_removed_from_scene()
	{
		assert( _scene_object );
		assert( _scene_object->get_scene() );
		_remove_from_component_tree();
	}

	void_c scene_component_c::_handle_on_world_space_transform_modified( transform3d_c const & old_world_space_transform, scene_component_c * initiator )
	{
		assert( _scene_object );
	}

	scene_component_c::scene_component_c()
		: _scene_object( nullptr )
		, _scene_component_list_node( this )
		, _scene_tree_leaf_node()
		, _scene_tree_list_node( this )
		, _local_space_obb()
		, _world_space_aabb()
		, _reference_count( 0 )
	{
		_reset_local_space_obb();
	}

	scene_component_c::~scene_component_c()
	{
	}

	sint32_c scene_component_c::add_reference()
	{
		_reference_count++;
		return _reference_count;
	}

	sint32_c scene_component_c::remove_reference()
	{
		assert( _reference_count > 0 );
		_reference_count--;
		if ( _reference_count == 0 )
		{
			delete this;
			return 0;
		}
		return _reference_count;
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

	transform3d_c const & scene_component_c::get_world_space_transform() const
	{
		assert( _scene_object );
		return _scene_object->get_world_space_transform();
	}

}
