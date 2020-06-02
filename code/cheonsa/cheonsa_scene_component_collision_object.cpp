#include "cheonsa_scene_component_collision_object.h"
#include "cheonsa_scene_object.h"
#include "cheonsa_scene.h"

namespace cheonsa
{

	void_c scene_component_collision_object_c::_handle_before_removed_from_scene()
	{
		scene_component_c::_handle_before_removed_from_scene();
		if ( _collision_object->get_is_initialized() )
		{
			_scene_object->get_scene()->get_physics_scene()->remove_collision_object( _collision_object );
		}
	}

	void_c scene_component_collision_object_c::_handle_after_added_to_scene()
	{
		scene_component_c::_handle_after_added_to_scene();
		if ( _collision_object->get_is_initialized() )
		{
			_scene_object->get_scene()->get_physics_scene()->add_collision_object( _collision_object );
		}
	}

	void_c scene_component_collision_object_c::_handle_on_world_space_transform_modified( transform3d_c const & old_world_space_transform, scene_component_c * initiator )
	{
		if ( initiator != this )
		{
			_collision_object->set_world_space_transform( _scene_object->get_world_space_transform() );
			if ( _scene_object->get_scene() )
			{
				_scene_object->get_scene()->get_physics_scene()->make_aabbs_dirty();
			}
		}
	}

	scene_component_collision_object_c::scene_component_collision_object_c()
		: scene_component_c()
		, _shape_list()
		, _master_shape( nullptr )
		, _collision_object( nullptr )
		, _frame()
		, _frame_inverse()
	{
		_collision_object = physics_collision_object_c::make_new_instance();
		_collision_object->add_reference();
	}

	scene_component_collision_object_c::~scene_component_collision_object_c()
	{
		_collision_object->remove_reference();
		_collision_object = nullptr;

		if ( _master_shape )
		{
			_master_shape->remove_reference();
			_master_shape = nullptr;
		}

		for ( sint32_c i = 0; i < _shape_list.get_length(); i++ )
		{
			_shape_list[ i ]->remove_reference();
			_shape_list[ i ] = nullptr;
		}
		_shape_list.remove_all();
	}

	void_c scene_component_collision_object_c::remove_all_shapes()
	{
		assert( _collision_object->get_is_initialized() == false );
		for ( sint32_c i = 0; i < _shape_list.get_length(); i++ )
		{
			_shape_list[ i ]->remove_reference();
		}
		_shape_list.remove_all();
	}

	void_c scene_component_collision_object_c::add_shape( physics_shape_c * shape )
	{
		assert( _collision_object->get_is_initialized() == false );
	}

	physics_shape_c const * scene_component_collision_object_c::get_shape_at_index( sint32_c index ) const
	{
		return _shape_list[ index ];
	}

	physics_shape_c * scene_component_collision_object_c::get_shape_at_index( sint32_c index )
	{
		assert( _collision_object->get_is_initialized() == false );
		return _shape_list[ index ];
	}

	void_c scene_component_collision_object_c::initialize( transform3d_c const & frame, float64_c mass, uint32_c layer, uint32_c layer_mask, boolean_c kinematic )
	{
		assert( _collision_object->get_is_initialized() == false );
		assert( _shape_list.get_length() != 0 );
		assert( frame.scale == vector32x3_c( 1.0f, 1.0f, 1.0f ) );

		// initialize master shape.
		if ( _shape_list.get_length() == 1 && _shape_list[ 0 ]->get_frame() == transform3d_c() )
		{
			_master_shape = _shape_list[ 0 ];
			_master_shape->add_reference();
		}
		else
		{
			_master_shape = new physics_shape_c();
			_master_shape->add_reference();
			_master_shape->initialize_compound( _shape_list );
		}

		// initialize rigid body.
		_collision_object->initialize(
			this,
			_master_shape,
			layer,
			layer_mask );

		// initialize frame.
		_frame = frame;
		_frame_inverse = frame.get_inverted();

		// add collision object to physics simulation scene.
		if ( _scene_object && _scene_object->get_scene() )
		{
			_scene_object->get_scene()->get_physics_scene()->add_collision_object( _collision_object );
		}
	}

	void_c scene_component_collision_object_c::uninitialize()
	{
		assert( _collision_object->get_is_initialized() );

		// remove collision object from physics simulation scene.
		if ( _scene_object && _scene_object->get_scene() )
		{
			_scene_object->get_scene()->get_physics_scene()->remove_collision_object( _collision_object );
		}

		// uninitialize rigid body.
		_collision_object->uninitialize();

		// uninitialize frame.
		_frame = transform3d_c();
		_frame_inverse = transform3d_c();

		// release master shape.
		if ( _shape_list.get_length() == 1 && _shape_list[ 0 ]->get_frame() == transform3d_c() )
		{
			_master_shape->uninitialize();
		}
		_master_shape->remove_reference();
		_master_shape = nullptr;
	}

	physics_collision_object_c * scene_component_collision_object_c::get_collision_object() const
	{
		return _collision_object;
	}

}
