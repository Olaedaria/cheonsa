#include "cheonsa_physics_constraint.h"
#include "cheonsa_physics_body.h"
#include "btBulletDynamicsCommon.h"
#include <cassert>

namespace cheonsa
{

	physics_constraint_c::physics_constraint_c()
		: _type( physics_constraint_type_e_none )
		, _scene( nullptr )
		, _object_a( nullptr )
		, _object_b( nullptr )
		, _bullet_constraint( nullptr )
	{
	}

	physics_constraint_c::~physics_constraint_c()
	{
		release();
	}

	physics_scene_c * physics_constraint_c::get_scene()
	{
		return _scene;
	}

	physics_body_c * physics_constraint_c::get_object_a()
	{
		return _object_a;
	}

	physics_body_c * physics_constraint_c::get_object_b()
	{
		return _object_b;
	}

	physics_constraint_type_e physics_constraint_c::get_type() const
	{
		return _type;
	}

	void_c physics_constraint_c::release()
	{
		if ( _bullet_constraint )
		{
			delete _bullet_constraint;
			_bullet_constraint = nullptr;
			_object_a = nullptr;
			_object_b = nullptr;
		}
		_type = physics_constraint_type_e_none;
	}

	btVector3 make_btVector3_frame( space_transform_c const & frame_in_world_space, space_transform_c & frame_in_object_local_space )
	{
		btVector3 result;
		frame_in_object_local_space.invert();
		frame_in_object_local_space = frame_in_world_space * frame_in_object_local_space;
		vector64x3_c const & frame_in_a_local_space_position = frame_in_object_local_space.position;
		result.setX( frame_in_a_local_space_position.a );
		result.setY( frame_in_a_local_space_position.b );
		result.setZ( frame_in_a_local_space_position.c );
		return result;
	}

	btTransform make_btTransform_frame( space_transform_c const & frame_in_world_space, space_transform_c & frame_in_object_local_space )
	{
		btTransform result;
		frame_in_object_local_space.invert();
		frame_in_object_local_space = frame_in_world_space * frame_in_object_local_space;
		physics_make_btTransform_from_space_transform( frame_in_object_local_space, result.getBasis(), result.getOrigin() );
		return result;
	}

	void_c physics_constraint_c::initialize_fixed( physics_body_c * a, physics_body_c * b, space_transform_c const & frame_in_world_space )
	{
		assert( _type == physics_constraint_type_e_none );
		assert( a != nullptr );
		assert( b != nullptr );

		space_transform_c frame_in_a_local_space;
		a->get_world_space_transform( frame_in_a_local_space );
		btTransform frame_in_a_local_space_btTransform = make_btTransform_frame( frame_in_world_space, frame_in_a_local_space );

		space_transform_c frame_in_b_local_space;
		b->get_world_space_transform( frame_in_b_local_space );
		btTransform frame_in_b_local_space_btTransform = make_btTransform_frame( frame_in_world_space, frame_in_b_local_space );

		_type = physics_constraint_type_e_fixed;
		_object_a = a;
		_object_b = b;
		_bullet_constraint = new btFixedConstraint(
			*a->_bullet_rigid_body,
			*b->_bullet_rigid_body,
			frame_in_a_local_space_btTransform,
			frame_in_b_local_space_btTransform );
	}

	
	void_c physics_constraint_c::initialize_point( physics_body_c * a, physics_body_c * b, space_transform_c const & frame_in_world_space )
	{
		assert( _type == physics_constraint_type_e_none );
		assert( a != nullptr );
		assert( b != nullptr );

		space_transform_c frame_in_a_local_space;
		a->get_world_space_transform( frame_in_a_local_space );
		btVector3 frame_in_a_local_space_btVector3 = make_btVector3_frame( frame_in_world_space, frame_in_a_local_space );

		space_transform_c frame_in_b_local_space;
		b->get_world_space_transform( frame_in_b_local_space );
		btVector3 frame_in_b_local_space_btVector3 = make_btVector3_frame( frame_in_world_space, frame_in_b_local_space );

		_type = physics_constraint_type_e_point;
		_object_a = a;
		_object_b = b;
		_bullet_constraint = new btPoint2PointConstraint(
			*a->_bullet_rigid_body,
			*b->_bullet_rigid_body,
			frame_in_a_local_space_btVector3,
			frame_in_b_local_space_btVector3);
	}

	void_c physics_constraint_c::initialize_hinge( physics_body_c * a, physics_body_c * b, space_transform_c const & frame_in_world_space )
	{
		assert( _type == physics_constraint_type_e_none );
		assert( a != nullptr );
		assert( b != nullptr );

		space_transform_c frame_in_a_local_space;
		a->get_world_space_transform( frame_in_a_local_space );
		btTransform frame_in_a_local_space_btTransform = make_btTransform_frame( frame_in_world_space, frame_in_a_local_space );

		space_transform_c frame_in_b_local_space;
		b->get_world_space_transform( frame_in_b_local_space );
		btTransform frame_in_b_local_space_btTransform = make_btTransform_frame( frame_in_world_space, frame_in_b_local_space );

		_type = physics_constraint_type_e_hinge;
		_object_a = a;
		_object_b = b;
		_bullet_constraint = new btHingeConstraint(
			*a->_bullet_rigid_body,
			*b->_bullet_rigid_body,
			frame_in_a_local_space_btTransform,
			frame_in_b_local_space_btTransform );
	}

	void_c physics_constraint_c::get_hinge_limits( float64_c & angular_lower_a, float64_c & angular_upper_a )
	{
		assert( _type == physics_constraint_type_e_hinge );
		angular_lower_a = reinterpret_cast< btHingeConstraint * >( _bullet_constraint )->getLowerLimit();
		angular_upper_a = reinterpret_cast< btHingeConstraint * >( _bullet_constraint )->getUpperLimit();
	}

	void_c physics_constraint_c::set_hinge_limits( float64_c angular_lower_a, float64_c angular_upper_a )
	{
		assert( _type == physics_constraint_type_e_hinge );
		reinterpret_cast< btHingeConstraint * >( _bullet_constraint )->setLimit( angular_lower_a, angular_upper_a );
	}

	void_c physics_constraint_c::initialize_cone( physics_body_c * a, physics_body_c * b, space_transform_c const & frame_in_world_space )
	{
		assert( _type == physics_constraint_type_e_none );
		assert( a != nullptr );
		assert( b != nullptr );

		space_transform_c frame_in_a_local_space;
		a->get_world_space_transform( frame_in_a_local_space );
		btTransform frame_in_a_local_space_btTransform = make_btTransform_frame( frame_in_world_space, frame_in_a_local_space );

		space_transform_c frame_in_b_local_space;
		b->get_world_space_transform( frame_in_b_local_space );
		btTransform frame_in_b_local_space_btTransform = make_btTransform_frame( frame_in_world_space, frame_in_b_local_space );

		_type = physics_constraint_type_e_cone;
		_object_a = a;
		_object_b = b;
		_bullet_constraint = new btConeTwistConstraint(
			*a->_bullet_rigid_body,
			*b->_bullet_rigid_body,
			frame_in_a_local_space_btTransform,
			frame_in_b_local_space_btTransform );
	}

	void_c physics_constraint_c::get_cone_limits( float64_c & angular_twist_span_a, float64_c & angular_swing_span_b, float64_c & angular_swing_span_c )
	{
		assert( _type == physics_constraint_type_e_cone );
		angular_twist_span_a = reinterpret_cast< btConeTwistConstraint * >( _bullet_constraint )->getTwistSpan();
		angular_swing_span_b = reinterpret_cast< btConeTwistConstraint * >( _bullet_constraint )->getSwingSpan1();
		angular_swing_span_c = reinterpret_cast< btConeTwistConstraint * >( _bullet_constraint )->getSwingSpan2();
	}

	void_c physics_constraint_c::set_cone_limits( float64_c angular_twist_span_a, float64_c angular_swing_span_b, float64_c angular_swing_span_c )
	{
		assert( _type == physics_constraint_type_e_cone );
		reinterpret_cast< btConeTwistConstraint * >( _bullet_constraint )->setLimit( angular_twist_span_a, angular_swing_span_b, angular_swing_span_c );
	}

	void_c physics_constraint_c::initialize_generic( physics_body_c * a, physics_body_c * b, space_transform_c const & frame_in_world_space )
	{
		assert( _type == physics_constraint_type_e_none );
		assert( a != nullptr );
		assert( b != nullptr );
		
		space_transform_c frame_in_a_local_space;
		a->get_world_space_transform( frame_in_a_local_space );
		btTransform frame_in_a_local_space_btTransform = make_btTransform_frame( frame_in_world_space, frame_in_a_local_space );

		space_transform_c frame_in_b_local_space;
		b->get_world_space_transform( frame_in_b_local_space );
		btTransform frame_in_b_local_space_btTransform = make_btTransform_frame( frame_in_world_space, frame_in_b_local_space );

		_type = physics_constraint_type_e_cone;
		_object_a = a;
		_object_b = b;
		_bullet_constraint = new btGeneric6DofConstraint(
			*a->_bullet_rigid_body,
			*b->_bullet_rigid_body,
			frame_in_a_local_space_btTransform,
			frame_in_b_local_space_btTransform, 
			false );
	}

	void_c physics_constraint_c::get_generic_limits( vector64x3_c & linear_lower, vector64x3_c & linear_upper, vector64x3_c & angular_lower, vector64x3_c & angular_upper )
	{
		assert( _type == physics_constraint_type_e_generic );

		btVector3 bt_temp;

		reinterpret_cast< btGeneric6DofConstraint * >( _bullet_constraint )->getLinearLowerLimit( bt_temp );
		linear_lower.a = bt_temp.x();
		linear_lower.b = bt_temp.y();
		linear_lower.c = bt_temp.z();

		reinterpret_cast< btGeneric6DofConstraint * >( _bullet_constraint )->getLinearUpperLimit( bt_temp );
		linear_upper.a = bt_temp.x();
		linear_upper.b = bt_temp.y();
		linear_upper.c = bt_temp.z();

		reinterpret_cast< btGeneric6DofConstraint * >( _bullet_constraint )->getAngularLowerLimit( bt_temp );
		angular_lower.a = bt_temp.x();
		angular_lower.b = bt_temp.y();
		angular_lower.c = bt_temp.z();

		reinterpret_cast< btGeneric6DofConstraint * >( _bullet_constraint )->getAngularUpperLimit( bt_temp );
		angular_upper.a = bt_temp.x();
		angular_upper.b = bt_temp.y();
		angular_upper.c = bt_temp.z();
	}

	void_c physics_constraint_c::set_generic_limits( vector64x3_c const & linear_lower, vector64x3_c const & linear_upper, vector64x3_c const & angular_lower, vector64x3_c const & angular_upper )
	{
		assert( _type == physics_constraint_type_e_generic );

		btVector3 bt_temp;

		bt_temp = btVector3( linear_lower.a, linear_lower.b, linear_lower.c );
		reinterpret_cast< btGeneric6DofConstraint * >( _bullet_constraint )->setLinearLowerLimit( bt_temp );

		bt_temp = btVector3( linear_upper.a, linear_upper.b, linear_upper.c );
		reinterpret_cast< btGeneric6DofConstraint * >( _bullet_constraint )->setLinearUpperLimit( bt_temp );

		bt_temp = btVector3( angular_lower.a, angular_lower.b, angular_lower.c );
		reinterpret_cast< btGeneric6DofConstraint * >( _bullet_constraint )->setAngularLowerLimit( bt_temp );

		bt_temp = btVector3( angular_upper.a, angular_upper.b, angular_upper.c );
		reinterpret_cast< btGeneric6DofConstraint * >( _bullet_constraint )->setAngularUpperLimit( bt_temp );
	}

}
