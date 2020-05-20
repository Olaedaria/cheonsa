#include "cheonsa_scene_component_ridgid_body.h"
#include "cheonsa_scene_object.h"
#include "cheonsa_scene.h"
#include "cheonsa__ops.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

namespace cheonsa
{

	void_c scene_component_ridgid_body_c::_physics_copy_world_space_transform_from_game_to_physics( void_c * instance, vector64x3_c & world_space_position, matrix32x3x3_c & world_space_basis )
	{
		assert( instance );
		scene_component_ridgid_body_c * resolved_instance = reinterpret_cast< scene_component_ridgid_body_c * >( instance );
		world_space_position = ops::rotate_scale_and_translate_vector64x3( resolved_instance->get_scene_object()->get_world_space_transform().position, resolved_instance->_frame );
		world_space_basis = ops::basis_matrix32x3x3_from_rotation_quaternion32( resolved_instance->get_scene_object()->get_world_space_transform().rotation * resolved_instance->_frame.rotation );
	}

	void_c scene_component_ridgid_body_c::_physics_copy_world_space_transform_from_physics_to_game( void_c * instance, vector64x3_c const & world_space_position, matrix32x3x3_c const & world_space_basis )
	{
		assert( instance );
		scene_component_ridgid_body_c * resolved_instance = reinterpret_cast< scene_component_ridgid_body_c * >( instance );
		transform3d_c world_space_transform;
		world_space_transform.position = ops::rotate_scale_and_translate_vector64x3( world_space_position, resolved_instance->_frame_inverse );
		world_space_transform.rotation = ops::rotation_quaternion32_from_basis_matrix32x3x3( world_space_basis ) * resolved_instance->_frame_inverse.rotation;
		world_space_transform.scale = resolved_instance->get_scene_object()->get_world_space_transform().scale; // preserve current scale, physics simulation can't and doesn't change it.
		resolved_instance->get_scene_object()->set_world_space_transform( world_space_transform, resolved_instance );
	}

	void_c scene_component_ridgid_body_c::_handle_before_removed_from_scene()
	{
		scene_component_c::_handle_before_removed_from_scene();
		if ( _ridgid_body->get_is_initialized() )
		{
			_scene_object->get_scene()->get_physics_scene()->remove_rigid_body( _ridgid_body );
		}
	}

	void_c scene_component_ridgid_body_c::_handle_after_added_to_scene()
	{
		scene_component_c::_handle_after_added_to_scene();
		if ( _ridgid_body->get_is_initialized() )
		{
			_scene_object->get_scene()->get_physics_scene()->add_rigid_body( _ridgid_body );
		}
	}

	void_c scene_component_ridgid_body_c::_handle_on_world_space_transform_modified( transform3d_c const & old_world_space_transform, scene_component_c * initiator )
	{
		if ( initiator != this )
		{
			_ridgid_body->set_world_space_transform( _scene_object->get_world_space_transform() * _frame );
		}
	}

	scene_component_ridgid_body_c::scene_component_ridgid_body_c()
		: scene_component_c()
		, _shape_list()
		, _master_shape( nullptr )
		, _ridgid_body( nullptr )
		, _frame()
		, _frame_inverse()
	{
		_ridgid_body = physics_rigid_body_c::make_new_instance();
		_ridgid_body->add_reference();
	}

	scene_component_ridgid_body_c::~scene_component_ridgid_body_c()
	{
		_ridgid_body->remove_reference();
		_ridgid_body = nullptr;

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

	scene_component_ridgid_body_c * scene_component_ridgid_body_c::make_new_instance()
	{
		return new scene_component_ridgid_body_c();
	}

	void_c scene_component_ridgid_body_c::remove_all_shapes()
	{
		assert( _ridgid_body->get_is_initialized() == false );
		for ( sint32_c i = 0; i < _shape_list.get_length(); i++ )
		{
			_shape_list[ i ]->remove_reference();
		}
		_shape_list.remove_all();
	}

	void_c scene_component_ridgid_body_c::add_shape( physics_shape_c * shape )
	{
		assert( _ridgid_body->get_is_initialized() == false );
		assert( shape );
		assert( shape->get_type() >= physics_shape_type_e_sphere && shape->get_type() <= physics_shape_type_e_triangle_mesh );
		assert( _shape_list.find_index_of( shape ) == -1 );
		shape->add_reference();
		_shape_list.insert( -1, shape );
	}

	physics_shape_c const * scene_component_ridgid_body_c::get_shape_at_index( sint32_c index ) const
	{
		return _shape_list[ index ];
	}

	physics_shape_c * scene_component_ridgid_body_c::get_shape_at_index( sint32_c index )
	{
		assert( _ridgid_body->get_is_initialized() == false );
		return _shape_list[ index ];
	}

	void_c scene_component_ridgid_body_c::initialize( transform3d_c const & frame, float64_c mass, uint32_c layer, uint32_c layer_mask, boolean_c kinematic )
	{
		assert( _ridgid_body->get_is_initialized() == false );
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
			_master_shape = physics_shape_c::make_new_instance();
			_master_shape->add_reference();
			_master_shape->initialize_compound( _shape_list );
		}

		// initialize ridgid body.
		_ridgid_body->initialize(
			this,
			&scene_component_ridgid_body_c::_physics_copy_world_space_transform_from_game_to_physics,
			&scene_component_ridgid_body_c::_physics_copy_world_space_transform_from_physics_to_game,
			_master_shape,
			mass,
			kinematic,
			layer,
			layer_mask );

		// initialize frame.
		_frame = frame;
		_frame_inverse = frame.get_inverted();

		// add ridgid body to physics simulation scene.
		if ( _scene_object && _scene_object->get_scene() )
		{
			_scene_object->get_scene()->get_physics_scene()->add_rigid_body( _ridgid_body );
		}
	}

	void_c scene_component_ridgid_body_c::uninitialize()
	{
		assert( _ridgid_body->get_is_initialized() );

		// remove ridgid body from physics simulation scene.
		if ( _scene_object && _scene_object->get_scene() )
		{
			_scene_object->get_scene()->get_physics_scene()->remove_rigid_body( _ridgid_body );
		}

		// uninitialize ridgid body.
		_ridgid_body->uninitialize();

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

	physics_rigid_body_c * scene_component_ridgid_body_c::get_ridgid_body() const
	{
		return _ridgid_body;
	}

}
