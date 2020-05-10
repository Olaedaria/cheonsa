#include "cheonsa_physics_ridgid_body.h"
#include "cheonsa_physics_scene.h"
#include "btBulletDynamicsCommon.h"
#include <cassert>

namespace cheonsa
{

	class physics_bullet_motion_state_c : public btMotionState
	{
	public:
		void_c * _object;
		void_c ( *_copy_world_space_transform_from_game_to_physics )( void_c * object, vector64x3_c & world_space_position, matrix32x3x3_c & world_space_basis ); // gets the world space transform of the physics body in the physics simulation.
		void_c ( *_copy_world_space_transform_from_physics_to_game )( void_c * object, vector64x3_c const & world_space_position, matrix32x3x3_c const & world_space_basis ); // sets the world space transform of the physics body in the physics simulation.
		physics_bullet_motion_state_c(
			void_c * object,
			void_c ( *copy_world_space_transform_from_game_to_physics )( void_c * object, vector64x3_c & world_space_position, matrix32x3x3_c & world_space_basis ),
			void_c ( *copy_world_space_transform_from_physics_to_game )( void_c * object, vector64x3_c const & world_space_position, matrix32x3x3_c const & world_space_basis ) )
			: btMotionState()
			, _object( object )
			, _copy_world_space_transform_from_game_to_physics( copy_world_space_transform_from_game_to_physics )
			, _copy_world_space_transform_from_physics_to_game( copy_world_space_transform_from_physics_to_game )
		{
		}

		// bullet calls this to read transform of our object in our scene and copy it to the bullet scene.
		// it calls it for the first time when the body is created.
		// bullet grabs the initial position of the body from the motionstate when the body enters the simulation.
		// it also calls it once each update tick (right?) for kinematic bodies.
		virtual void_c getWorldTransform( btTransform & worldTrans ) const override
		{
			//transform3d_c world_space_transform;
			vector64x3_c world_space_position;
			matrix32x3x3_c world_space_basis;
			(*_copy_world_space_transform_from_game_to_physics)( _object, world_space_position, world_space_basis );
			worldTrans.setBasis( btMatrix3x3( world_space_basis.a.a, world_space_basis.a.b, world_space_basis.a.c, world_space_basis.b.a, world_space_basis.b.b, world_space_basis.b.c, world_space_basis.c.a, world_space_basis.c.b, world_space_basis.c.c ) );
			worldTrans.setOrigin( btVector3( world_space_position.a, world_space_position.b, world_space_position.c ) );
		}

		// bullet calls this to set the transform of our object in our scene.
		// bullet calls this for each update tick after the first.
		virtual void_c setWorldTransform( const btTransform & worldTrans ) override
		{
			btVector3 const & bullet_origin = worldTrans.getOrigin();
			vector64x3_c world_space_position;
			world_space_position.a = bullet_origin[ 0 ];
			world_space_position.b = bullet_origin.y();
			world_space_position.c = bullet_origin.getZ();

			btMatrix3x3 const & bullet_basis = worldTrans.getBasis();
			matrix32x3x3_c world_space_basis;
			world_space_basis.a.a = static_cast< float32_c >( bullet_basis[ 0 ][ 0 ] );
			world_space_basis.a.b = static_cast< float32_c >( bullet_basis[ 0 ][ 1 ] );
			world_space_basis.a.c = static_cast< float32_c >( bullet_basis[ 0 ][ 2 ] );
			world_space_basis.b.a = static_cast< float32_c >( bullet_basis[ 1 ][ 0 ] );
			world_space_basis.b.b = static_cast< float32_c >( bullet_basis[ 1 ][ 1 ] );
			world_space_basis.b.c = static_cast< float32_c >( bullet_basis[ 1 ][ 2 ] );
			world_space_basis.c.a = static_cast< float32_c >( bullet_basis[ 2 ][ 0 ] );
			world_space_basis.c.b = static_cast< float32_c >( bullet_basis[ 2 ][ 1 ] );
			world_space_basis.c.c = static_cast< float32_c >( bullet_basis[ 2 ][ 2 ] );

			// so many choices... no direct access...
			// bullet_basis.getRow( 0 ); // gets by const reference.
			// bullet_basis[ 0 ]; // gets by reference, at least this operator is defined.

			vector64x3_c position( worldTrans.getOrigin().getX(), worldTrans.getOrigin().getY(), worldTrans.getOrigin().getZ() );
			btQuaternion bt_quaternion = worldTrans.getRotation();
			quaternion32_c rotation( static_cast< float32_c >( bt_quaternion.getX() ), static_cast< float32_c >( bt_quaternion.getY() ), static_cast< float32_c >( bt_quaternion.getZ() ), static_cast< float32_c >( bt_quaternion.getW() ) );
			(*_copy_world_space_transform_from_physics_to_game)( _object, world_space_position, world_space_basis );
		}

	};

	physics_rigid_body_c::physics_rigid_body_c()
		: _bullet_motion_state( nullptr )
		, _bullet_rigid_body( nullptr )
		, _shape( nullptr )
		, _layer( 0 )
		, _layer_mask( 0 )
		, _reference_count( 0 )
	{
	}

	physics_rigid_body_c::~physics_rigid_body_c()
	{
		assert( _reference_count == 0 );
		uninitialize();
	}

	physics_rigid_body_c * physics_rigid_body_c::make_new_instance()
	{
		return new physics_rigid_body_c();
	}

	void_c physics_rigid_body_c::add_reference()
	{
		_reference_count++;
	}

	void_c physics_rigid_body_c::remove_reference()
	{
		assert( _reference_count > 0 );
		_reference_count--;
		if ( _reference_count == 0 )
		{
			delete this;
		}
	}

	void_c physics_rigid_body_c::initialize(
		void_c * object,
		void_c ( *copy_world_space_transform_from_game_to_physics )( void_c * object, vector64x3_c & world_space_position, matrix32x3x3_c & world_space_basis ),
		void_c ( *copy_world_space_transform_from_physics_to_game )( void_c * object, vector64x3_c const & world_space_position, matrix32x3x3_c const & world_space_basis ),
		physics_shape_c * shape,
		float64_c mass,
		boolean_c kinematic,
		uint32_c layer,
		uint32_c layer_mask )
	{
		assert( _bullet_rigid_body == nullptr );
		assert( _bullet_motion_state == nullptr );
		assert( object );
		assert( copy_world_space_transform_from_game_to_physics );
		assert( copy_world_space_transform_from_physics_to_game );
		assert( shape );

		shape->add_reference();
		_shape = shape;
		_layer = layer;
		_layer_mask = layer_mask;

		btVector3 bullet_local_inertia;
		_shape->_bullet_shape->calculateLocalInertia( mass, bullet_local_inertia );

		int bullet_flags = 0;
		if ( mass <= 0.0f )
		{
			mass = 0.0f;
			bullet_flags |= btRigidBody::CF_STATIC_OBJECT;
		}
		if ( kinematic )
		{
			bullet_flags |= btRigidBody::CF_KINEMATIC_OBJECT;
		}

		_bullet_motion_state = new physics_bullet_motion_state_c( object, copy_world_space_transform_from_game_to_physics, copy_world_space_transform_from_physics_to_game );
		_bullet_rigid_body = new btRigidBody( mass, _bullet_motion_state, _shape->_bullet_shape, bullet_local_inertia );
		_bullet_rigid_body->setUserPointer( this );
		_bullet_rigid_body->setCollisionFlags( bullet_flags );
	}

	void_c physics_rigid_body_c::uninitialize()
	{
		if ( _bullet_rigid_body )
		{
			assert( _bullet_rigid_body->getWorldArrayIndex() < 0 ); // shouldn't be in a dynamics world at this point.

			delete _bullet_rigid_body;
			_bullet_rigid_body = nullptr;

			delete _bullet_motion_state;
			_bullet_motion_state = nullptr;

			if ( _shape )
			{
				_shape->remove_reference();
				_shape = nullptr;
			}
		}
	}

	boolean_c physics_rigid_body_c::get_is_initialized() const
	{
		return _bullet_rigid_body != nullptr;
	}

	void_c physics_rigid_body_c::get_world_space_transform( transform3d_c & world_space_transform ) const
	{
		assert( _bullet_rigid_body );
		physics_make_space_transform_from_btTransform( _bullet_rigid_body->getWorldTransform().getBasis(), _bullet_rigid_body->getWorldTransform().getOrigin(), world_space_transform );
		world_space_transform.scale.a = static_cast< float32_c >( _shape->_bullet_shape->getLocalScaling().x() );
		world_space_transform.scale.b = static_cast< float32_c >( _shape->_bullet_shape->getLocalScaling()[ 1 ] );
		world_space_transform.scale.c = static_cast< float32_c >( _shape->_bullet_shape->getLocalScaling().getZ() );
	}

	void_c physics_rigid_body_c::set_world_space_transform( transform3d_c const & world_space_transform )
	{
		assert( _bullet_rigid_body );
		physics_make_btTransform_from_space_transform( world_space_transform, _bullet_rigid_body->getWorldTransform().getBasis(), _bullet_rigid_body->getWorldTransform().getOrigin() );
		_shape->_bullet_shape->setLocalScaling( btVector3( world_space_transform.scale.a, world_space_transform.scale.b, world_space_transform.scale.c ) );
	}

	void_c physics_rigid_body_c::set_world_space_position( vector64x3_c const & world_position )
	{
		btTransform bullet_transform;
		_bullet_motion_state->getWorldTransform( bullet_transform );
		bullet_transform.getOrigin().setX( world_position.a );
		bullet_transform.getOrigin().setY( world_position.b );
		bullet_transform.getOrigin().setZ( world_position.c );
		_bullet_motion_state->setWorldTransform( bullet_transform );
	}

	void_c physics_rigid_body_c::clear_forces_and_velocities()
	{
		_bullet_rigid_body->clearForces();
		_bullet_rigid_body->setLinearVelocity( btVector3( 0.0f, 0.0f, 0.0 ) );
		_bullet_rigid_body->setAngularFactor( btVector3( 0.0f, 0.0f, 0.0 ) );
	}

	uint32_c physics_rigid_body_c::get_layer() const
	{
		return _layer;
	}

	uint32_c physics_rigid_body_c::get_layer_mask() const
	{
		return _layer_mask;
	}

	float64_c physics_rigid_body_c::get_friction() const
	{
		assert( _bullet_rigid_body );
		return _bullet_rigid_body->getFriction();
	}

	void_c physics_rigid_body_c::set_friction( float64_c value )
	{
		assert( _bullet_rigid_body );
		_bullet_rigid_body->setFriction( value );
	}

	float64_c physics_rigid_body_c::get_restitution() const
	{
		assert( _bullet_rigid_body );
		return _bullet_rigid_body->getRestitution();
	}

	void_c physics_rigid_body_c::set_restitution( float64_c value )
	{
		assert( _bullet_rigid_body );
		_bullet_rigid_body->setRestitution( value );
	}

	float64_c physics_rigid_body_c::get_mass() const
	{
		assert( _bullet_rigid_body );
		return 1.0 / _bullet_rigid_body->getInvMass();
	}

	void_c physics_rigid_body_c::set_mass( float64_c mass )
	{
		assert( _bullet_rigid_body );
		assert( _shape );
		btVector3 local_inertia;
		_shape->_bullet_shape->calculateLocalInertia( mass, local_inertia );
		_bullet_rigid_body->setMassProps( mass, local_inertia );
		_bullet_rigid_body->setCollisionFlags( mass != 0.0f ? btRigidBody::CF_KINEMATIC_OBJECT : btRigidBody::CF_STATIC_OBJECT );
	}

	float64_c physics_rigid_body_c::get_mass_inverse() const
	{
		assert( _bullet_rigid_body );
		return _bullet_rigid_body->getInvMass();
	}

	vector64x3_c physics_rigid_body_c::get_local_momentum( const vector64x3_c & local_point ) const
	{
		return get_local_velocity( local_point ) * get_mass();
	}

	vector64x3_c physics_rigid_body_c::get_local_velocity( const vector64x3_c & local_point ) const
	{
		assert( _bullet_rigid_body );
		btVector3 bullet_value = _bullet_rigid_body->getVelocityInLocalPoint( btVector3( local_point.a, local_point.b, local_point.c ) );
		return vector64x3_c( bullet_value.x(), bullet_value.y(), bullet_value.z() );
	}

	vector64x3_c physics_rigid_body_c::get_gravity() const
	{
		assert( _bullet_rigid_body );
		btVector3 bullet_value = _bullet_rigid_body->getGravity();
		return vector64x3_c( bullet_value.x(), bullet_value.y(), bullet_value.z() );
	}

	void_c physics_rigid_body_c::set_gravity( const vector64x3_c & value )
	{
		assert( _bullet_rigid_body );
		_bullet_rigid_body->setGravity( btVector3( value.a, value.b, value.c ) );
	}

	vector64x3_c physics_rigid_body_c::get_linear_factor() const
	{
		assert( _bullet_rigid_body );
		btVector3 bullet_value = _bullet_rigid_body->getLinearFactor();
		return vector64x3_c( bullet_value.x(), bullet_value.y(), bullet_value.z() );
	}

	void_c physics_rigid_body_c::set_linear_factor( const vector64x3_c & value )
	{
		assert( _bullet_rigid_body );
		_bullet_rigid_body->setLinearFactor( btVector3( value.a, value.b, value.c ) );
	}

	vector64x3_c physics_rigid_body_c::get_angular_factor() const
	{
		assert( _bullet_rigid_body );
		btVector3 bullet_value = _bullet_rigid_body->getAngularFactor();
		return vector64x3_c( bullet_value.x(), bullet_value.y(), bullet_value.z() );
	}

	void_c physics_rigid_body_c::set_angular_factor( const vector64x3_c & FactorAngular )
	{
		assert( _bullet_rigid_body );
		_bullet_rigid_body->setAngularFactor( btVector3( FactorAngular.a, FactorAngular.b, FactorAngular.c ) );
	}

	float64_c physics_rigid_body_c::get_linear_damping() const
	{
		assert( _bullet_rigid_body );
		return _bullet_rigid_body->getLinearDamping();
	}

	void_c physics_rigid_body_c::set_linear_damping( float64_c DampingLinear )
	{
		assert( _bullet_rigid_body );
		_bullet_rigid_body->setDamping( DampingLinear, _bullet_rigid_body->getAngularDamping() );
	}

	float64_c physics_rigid_body_c::get_angular_damping() const
	{
		assert( _bullet_rigid_body );
		return _bullet_rigid_body->getAngularDamping();
	}

	void_c physics_rigid_body_c::set_angular_damping( float64_c DampingAngular )
	{
		assert( _bullet_rigid_body );
		_bullet_rigid_body->setDamping( _bullet_rigid_body->getLinearDamping(), DampingAngular );
	}

	vector64x3_c physics_rigid_body_c::get_linear_momentum() const
	{
		return get_linear_velocity() * get_mass();
	}

	void_c physics_rigid_body_c::set_linear_momentum( vector64x3_c const & value )
	{
		assert( _bullet_rigid_body );
		set_linear_velocity( value * _bullet_rigid_body->getInvMass() );
	}

	vector64x3_c physics_rigid_body_c::get_angular_momentum() const
	{
		return get_angular_velocity() * get_mass();
	}

	void_c physics_rigid_body_c::set_angular_momentum( vector64x3_c const & value )
	{
		assert( _bullet_rigid_body );
		set_angular_velocity( value * _bullet_rigid_body->getInvMass() );
	}

	vector64x3_c physics_rigid_body_c::get_linear_velocity() const
	{
		assert( _bullet_rigid_body );
		btVector3 bullet_value = _bullet_rigid_body->getLinearVelocity();
		return vector64x3_c( bullet_value.x(), bullet_value.y(), bullet_value.z() );
	}

	void_c physics_rigid_body_c::set_linear_velocity( vector64x3_c const & VelocityLinear )
	{
		assert( _bullet_rigid_body );
		_bullet_rigid_body->activate( true );
		_bullet_rigid_body->setLinearVelocity( btVector3( VelocityLinear.a, VelocityLinear.b, VelocityLinear.c ) );
	}

	vector64x3_c physics_rigid_body_c::get_angular_velocity() const
	{
		assert( _bullet_rigid_body );
		btVector3 bullet_value = _bullet_rigid_body->getAngularVelocity();
		return vector64x3_c( bullet_value.x(), bullet_value.y(), bullet_value.z() );
	}

	void_c physics_rigid_body_c::set_angular_velocity( vector64x3_c const & VelocityAngular )
	{
		assert( _bullet_rigid_body );
		_bullet_rigid_body->setAngularVelocity( btVector3( VelocityAngular.a, VelocityAngular.b, VelocityAngular.c ) );
	}

	void_c physics_rigid_body_c::apply_local_linear_impulse( vector64x3_c const & linear_impulse, vector64x3_c const & local_point )
	{
		assert( _bullet_rigid_body );
		_bullet_rigid_body->applyImpulse( btVector3( linear_impulse.a, linear_impulse.b, linear_impulse.c ), btVector3( local_point.a, local_point.b, local_point.c ) );
	}

	void_c physics_rigid_body_c::apply_linear_impulse( vector64x3_c const & linear_impulse )
	{
		assert( _bullet_rigid_body );
		_bullet_rigid_body->applyCentralImpulse( btVector3( linear_impulse.a, linear_impulse.b, linear_impulse.c ) );
	}

	void_c physics_rigid_body_c::apply_angular_impulse( vector64x3_c const & angular_impulse )
	{
		assert( _bullet_rigid_body );
		_bullet_rigid_body->applyTorqueImpulse( btVector3( angular_impulse.a, angular_impulse.b, angular_impulse.c ) );
	}

	boolean_c physics_rigid_body_c::get_kinematic() const
	{
		assert( _bullet_rigid_body );
		return ( _bullet_rigid_body->getCollisionFlags() & btRigidBody::CF_KINEMATIC_OBJECT ) != 0;
	}

	void_c physics_rigid_body_c::set_kinematic( boolean_c value )
	{
		assert( _bullet_rigid_body );
		_bullet_rigid_body->setCollisionFlags( ( _bullet_rigid_body->getCollisionFlags() & ~btRigidBody::CF_KINEMATIC_OBJECT ) | ( value ? btRigidBody::CF_KINEMATIC_OBJECT : 0 ) );
	}

	boolean_c physics_rigid_body_c::get_collision_response() const
	{
		assert( _bullet_rigid_body );
		return ( _bullet_rigid_body->getCollisionFlags() & btRigidBody::CF_NO_CONTACT_RESPONSE ) == 0;
	}

	void_c physics_rigid_body_c::set_collision_response( boolean_c value )
	{
		assert( _bullet_rigid_body );
		_bullet_rigid_body->setCollisionFlags( ( _bullet_rigid_body->getCollisionFlags() & ~btRigidBody::CF_NO_CONTACT_RESPONSE ) | ( value ? btRigidBody::CF_NO_CONTACT_RESPONSE : 0 ) );
	}

	//boolean_c physics_rigid_body_c::get_sleeping()
	//{
	//	return ( _bullet_rigid_body->getActivationState() == ISLAND_SLEEPING );
	//}

	//void_c physics_rigid_body_c::set_sleeping( boolean_c value )
	//{
	//	_bullet_rigid_body->setActivationState( value ? WANTS_DEACTIVATION : ACTIVE_TAG );
	//}

	//vector32x3_c physics_rigid_body_c::get_basis_a()
	//{
	//	btMatrix3x3 bullet_value = _bullet_rigid_body->getWorldTransform().getBasis();
	//	return vector32x3_c( bullet_value.getRow( 0 ).x(), bullet_value.getRow( 1 ).x(), bullet_value.getRow( 2 ).x() );
	//}

	//vector32x3_c physics_rigid_body_c::get_basis_b()
	//{
	//	btMatrix3x3 bullet_value = _bullet_rigid_body->getWorldTransform().getBasis();
	//	return vector32x3_c( bullet_value.getRow( 0 ).y(), bullet_value.getRow( 1 ).y(), bullet_value.getRow( 2 ).y() );
	//}

	//vector32x3_c physics_rigid_body_c::get_basis_c()
	//{
	//	btMatrix3x3 bullet_value = _bullet_rigid_body->getWorldTransform().getBasis();
	//	return vector32x3_c( bullet_value.getRow( 0 ).z(), bullet_value.getRow( 1 ).z(), bullet_value.getRow( 2 ).z() );
	//}

}
