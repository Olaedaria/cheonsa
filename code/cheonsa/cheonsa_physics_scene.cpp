#include "cheonsa_physics_scene.h"
#include "cheonsa_physics_shape.h"
#include "cheonsa_physics_ridgid_body.h"
#include "cheonsa_physics_constraint.h"
#include "cheonsa_physics_bullet_debug_drawer.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"
#include "btBulletDynamicsCommon.h"

namespace cheonsa
{

	//float32_c physics_scene_c::get_wind_displacement( vector32x3_c& position, float32_c WindPushInfluence, float32_c WindTurbulenceInfluence, float32_c WindTurbulenceFrequency )
	//{
	//	float32_c WindIntensity = GetWindIntensity( position );
	//	position += _WindDirection * ( WindPushInfluence * WindIntensity );
	//	float32_c Turbulence = sine( _WindOffset * WindTurbulenceFrequency ) * WindTurbulenceInfluence;
	//	position += vector32x3_c( Turbulence, Turbulence, Turbulence );
	//	return WindIntensity;
	//}

	physics_scene_c::physics_scene_c()
		: _gravity( 0.0f, 0.0f, -9.81f )
		, _wind_direction( 0.0f, -1.0f, 0.0f )
		, _wind_speed( 1.0f )
		, _wind_offset( 0.0f )
		, _wind_period( 6.0f )
		, _wind_period_weight_minimum( 0.5f )
		, _bullet_dynamic_world( nullptr )
		, _bullet_dynamic_configuration( nullptr )
		, _bullet_dynamic_solver( nullptr )
		, _bullet_collision_broad_phase( nullptr )
		, _bullet_collision_dispatcher( nullptr )
		, _bullet_debug_drawer( nullptr )
		, _reference_count( 0 )
	{
		_bullet_collision_broad_phase = new btDbvtBroadphase(); //_bullet_collision_broad_phase->getOverlappingPairCache()->setInternalGhostPairCallback( new btGhostPairCallback() ); // i don't remember what this does
		_bullet_dynamic_configuration = new btDefaultCollisionConfiguration();
		_bullet_collision_dispatcher = new btCollisionDispatcher( _bullet_dynamic_configuration );
		_bullet_dynamic_solver = new btSequentialImpulseConstraintSolver();
		_bullet_dynamic_world = new btDiscreteDynamicsWorld( _bullet_collision_dispatcher, _bullet_collision_broad_phase, _bullet_dynamic_solver, _bullet_dynamic_configuration );
		_bullet_debug_drawer = new physics_bullet_debug_drawer_c();
		_bullet_debug_drawer->setDebugMode( btIDebugDraw::DBG_DrawWireframe );
		_bullet_dynamic_world->setDebugDrawer( _bullet_debug_drawer );
	}

	physics_scene_c::~physics_scene_c()
	{
		assert( _reference_count == 0 );
		_bullet_dynamic_world->setDebugDrawer( nullptr );
		delete _bullet_dynamic_world;
		delete _bullet_dynamic_solver;
		delete _bullet_collision_dispatcher;
		delete _bullet_dynamic_configuration;
		delete _bullet_collision_broad_phase;
		delete _bullet_debug_drawer;
	}

	physics_scene_c * physics_scene_c::make_new_instance()
	{
		return new physics_scene_c();
	}

	void_c physics_scene_c::add_reference()
	{
		_reference_count++;
	}

	void_c physics_scene_c::remove_reference()
	{
		assert( _reference_count > 0 );
		_reference_count--;
		if ( _reference_count == 0 )
		{
			delete this;
		}
	}

	void_c physics_scene_c::copy_properties_from( physics_scene_c * other )
	{
		_gravity = other->_gravity;
		_wind_direction = other->_wind_direction;
		_wind_speed = other->_wind_speed;
		_wind_offset = other->_wind_offset;
		_wind_period = other->_wind_period;
		_wind_period_weight_minimum = other->_wind_period_weight_minimum;
	}

	void_c physics_scene_c::update( float32_c update_time_step )
	{
		//// update wind
		//_WindDirection.x = cosine( _WindHeading );
		//_WindDirection.y = sine( _WindHeading );
		//_WindDirection.Z = 0.0f;
		//_WindOffset += _WindWaveSpeed * time_step;

		_wind_offset += _wind_speed * update_time_step;
		_bullet_dynamic_world->setGravity( btVector3( _gravity.a, _gravity.b, _gravity.c ) );
		_bullet_dynamic_world->stepSimulation( update_time_step, 0 ); // this stepSimulation makes bullet update the physics simulation by time_step amount. this is a fixed time step update, as indicated by passing 0 for maxSubSteps.
	}

	//void_c physics_scene_c::pre_render( float32_c render_time_step )
	//{
	//	// this stepSimulation makes bullet interpolate motion states and then copy those states to our scene (right?).
	//	_bullet_dynamic_world->stepSimulation( render_time_step );
	//}

	void_c physics_scene_c::render()
	{
		_bullet_dynamic_world->debugDrawWorld();
	}

	//void_c physics_scene_c::translate_all_objects( vector32x3_c const & amount )
	//{
	//	btCollisionObjectArray & bt_collision_object_array = _bullet_dynamic_world->getCollisionObjectArray();
	//	for ( sint32_c i = 0; i < bt_collision_object_array.size(); i++ )
	//	{
	//		btCollisionObject * bt_collision_object = bt_collision_object_array.at( i );
	//		btTransform bt_transform = bt_collision_object->getWorldTransform();
	//		bt_transform.setOrigin( bt_transform.getOrigin() + btVector3( amount.a, amount.b, amount.c ) );
	//		bt_collision_object->setWorldTransform( bt_transform );
	//	}
	//	update_aabbs();
	//}

	void_c physics_scene_c::update_aabbs()
	{
		_bullet_dynamic_world->updateAabbs();
	}

	void_c physics_scene_c::add_rigid_body( physics_rigid_body_c * rigid_body )
	{
		assert( rigid_body );
		assert( rigid_body->_bullet_rigid_body );
		assert( rigid_body->_bullet_rigid_body->getWorldArrayIndex() == -1 );
		rigid_body->add_reference();
		_bullet_dynamic_world->addRigidBody( rigid_body->_bullet_rigid_body, (int)rigid_body->_layer, (int)rigid_body->_layer_mask );
	}

	void_c physics_scene_c::remove_rigid_body( physics_rigid_body_c * rigid_body )
	{
		assert( rigid_body );
		assert( rigid_body->_bullet_rigid_body );
		assert( rigid_body->_bullet_rigid_body->getWorldArrayIndex() >= 0 );
		rigid_body->remove_reference();
		_bullet_dynamic_world->removeRigidBody( rigid_body->_bullet_rigid_body );
	}

	void_c physics_scene_c::add_constraint( physics_constraint_c * constraint )
	{
		assert( constraint );
		assert( constraint->_bullet_constraint );
		constraint->add_reference();
		_bullet_dynamic_world->addConstraint( constraint->_bullet_constraint );
	}

	void_c physics_scene_c::remove_constraint( physics_constraint_c * constraint )
	{
		assert( constraint );
		assert( constraint->_bullet_constraint );
		constraint->remove_reference();
		_bullet_dynamic_world->removeConstraint( constraint->_bullet_constraint );
	}

	void_c physics_scene_c::find_all_hits( physics_sweep_test_result_c & result, uint16_c collision_layer, uint16_c collision_layer_mask, vector64x3_c const & sweep_start, vector64x3_c const & sweep_end )
	{
		btCollisionWorld::AllHitsRayResultCallback bullet_ray_result( btVector3( sweep_start.a, sweep_start.b, sweep_start.c ), btVector3( sweep_end.a, sweep_end.b, sweep_end.c ) );
		bullet_ray_result.m_collisionFilterGroup = collision_layer;
		bullet_ray_result.m_collisionFilterMask = collision_layer_mask;
		_bullet_dynamic_world->rayTest( btVector3( sweep_start.a, sweep_start.b, sweep_start.c ), btVector3( sweep_end.a, sweep_end.b, sweep_end.c ), bullet_ray_result );

		result.hit_list.remove_all();
		int size = bullet_ray_result.m_hitFractions.size();
		for ( int i = 0; i < size; i++ )
		{
			physics_sweep_test_result_c::hit_c hit;
			hit.fraction = bullet_ray_result.m_hitFractions[i];
			btVector3 BulletVector3 = bullet_ray_result.m_hitPointWorld[i];
			hit.position = vector64x3_c( BulletVector3.x(), BulletVector3.y(), BulletVector3.z() );
			BulletVector3 = bullet_ray_result.m_hitNormalWorld[i];
			hit.normal = vector64x3_c( BulletVector3.x(), BulletVector3.y(), BulletVector3.z() );
			result.hit_list.insert( -1, hit );
		}
	}

	void_c physics_scene_c::find_nearest_hit( physics_sweep_test_result_c & result, uint16_c collision_layer, uint16_c collision_layer_mask, vector64x3_c const & sweep_start, vector64x3_c const & sweep_end )
	{
		btCollisionWorld::ClosestRayResultCallback bullet_ray_result( btVector3( sweep_start.a, sweep_start.b, sweep_start.c ), btVector3( sweep_end.a, sweep_end.b, sweep_end.c ) );
		bullet_ray_result.m_collisionFilterGroup = collision_layer;
		bullet_ray_result.m_collisionFilterMask = collision_layer_mask;
		_bullet_dynamic_world->rayTest( btVector3( sweep_start.a, sweep_start.b, sweep_start.c ), btVector3( sweep_end.a, sweep_end.b, sweep_end.c ), bullet_ray_result );

		result.hit_list.remove_all();
		if ( bullet_ray_result.hasHit() )
		{
			physics_sweep_test_result_c::hit_c hit;
			hit.object = reinterpret_cast< physics_rigid_body_c * >( bullet_ray_result.m_collisionObject->getUserPointer() );
			hit.fraction = bullet_ray_result.m_closestHitFraction;
			hit.position = vector64x3_c( bullet_ray_result.m_hitPointWorld.x(), bullet_ray_result.m_hitPointWorld.y(), bullet_ray_result.m_hitPointWorld.z() );
			hit.normal = vector64x3_c( bullet_ray_result.m_hitNormalWorld.x(), bullet_ray_result.m_hitNormalWorld.y(), bullet_ray_result.m_hitNormalWorld.z() );
			result.hit_list.insert( -1, hit );
		}
	}

	void_c physics_scene_c::find_nearest_hit( physics_sweep_test_result_c & result, uint16_c collision_layer, uint16_c collision_layer_mask, vector64x3_c const & sweep_start, vector64x3_c const & sweep_end, physics_shape_c * shape )
	{
		if ( !shape->_bullet_shape->isConvex() )
		{
			return;
		}

		btCollisionWorld::ClosestConvexResultCallback bullet_convex_result( btVector3( sweep_start.a, sweep_start.b, sweep_start.c ), btVector3( sweep_end.a, sweep_end.b, sweep_end.c ) );
		bullet_convex_result.m_collisionFilterGroup = collision_layer;
		bullet_convex_result.m_collisionFilterMask = collision_layer_mask;
		btTransform bullet_from( btQuaternion(), btVector3( sweep_start.a, sweep_start.b, sweep_start.c ) );
		btTransform bullet_to( btQuaternion(), btVector3( sweep_end.a, sweep_end.b, sweep_end.c ) );
		_bullet_dynamic_world->convexSweepTest( dynamic_cast< btConvexShape * >( shape->_bullet_shape ), bullet_from, bullet_to, bullet_convex_result );

		result.hit_list.remove_all();
		if ( bullet_convex_result.hasHit() )
		{
			physics_sweep_test_result_c::hit_c hit;
			hit.object = reinterpret_cast<physics_rigid_body_c *>( bullet_convex_result.m_hitCollisionObject->getUserPointer() );
			hit.fraction = bullet_convex_result.m_closestHitFraction;
			hit.position = vector64x3_c( bullet_convex_result.m_hitPointWorld.x(), bullet_convex_result.m_hitPointWorld.y(), bullet_convex_result.m_hitPointWorld.z() );
			hit.normal = vector64x3_c( bullet_convex_result.m_hitNormalWorld.x(), bullet_convex_result.m_hitNormalWorld.y(), bullet_convex_result.m_hitNormalWorld.z() );
			result.hit_list.insert( -1, hit );
		}
	}

	//float32_c physics_scene_c::GetWindIntensity( vector32x3_c& position )
	//{
	//	plane32_c WindPlane( _WindDirection, _WindOffset );
	//	float32_c WaveInput = WindPlane.FindDistanceAbove( position ) / _WindWavePeriod;
	//	float32_c Wave = sine( WaveInput ) * 0.5f + 0.5f;
	//	return Wave * ( _WindSpeedMaximum - _WindSpeedMinimum ) + _WindSpeedMinimum;
	//}

	vector64x3_c physics_scene_c::sample_wind_displacement( vector64x3_c const & point, float64_c wave_frequency, float64_c wave_weight )
	{
		float64_c rolling_distance = ops::dot_product_float64( _wind_direction, point ) + static_cast< float32_c >( _wind_offset ); // planar distance equation
		float64_c rolling_weight = ( ( ( ops::math_sine( rolling_distance / _wind_period * constants< float64_c >::pi() * 2.0 ) * 0.5 + 0.5 ) * ( 1.0 - _wind_period_weight_minimum ) ) + _wind_period_weight_minimum ) * ops::math_saturate( _wind_speed ) * wave_weight;
		float64_c d = rolling_distance * wave_frequency;
		return ( vector64x3_c( ops::math_sine( d ), ops::math_cosine( d ), ops::math_sine( d * 0.25 ) ) * rolling_weight ) + ( _wind_direction * rolling_weight ); // push and wiggle.
	}

	vector64x3_c physics_scene_c::sample_wind_force( vector64x3_c const & point )
	{
		return _wind_direction * _wind_speed;
	}

	vector64x3_c physics_scene_c::sample_gravity_force( vector64x3_c const & point )
	{
		return _gravity;
	}

	vector64x3_c physics_scene_c::sample_up( vector64x3_c const & point )
	{
		// at some point we can a planet or planets to do the gravity calculation.
		// for now though the world is flat.
		return ops::normal_vector64x3( -_gravity );
	}

}
