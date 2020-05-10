#pragma once

#include "cheonsa_core_list.h"
#include "cheonsa_physics_types.h"

namespace cheonsa
{

	// physics scene.
	// collision detection and physics simulation manager.
	// wrapper interface for a proper physics engine, currently bullet3 2.88.
	// handles wind, and later on probably water also, and fluid/atmosphere currents and density, pressure, temperature, weather, etc.
	// can be used to drive dynamic effects like hair, clothes, flags, fire, water, rain, snow, etc.
	class physics_scene_c
	{
	private:
		// gravity force.
		vector64x3_c _gravity; // global gravity force to apply to all objects in the physics scene.

		// wind force.
		vector64x3_c _wind_direction;
		float64_c _wind_speed;
		float64_c _wind_offset;
		float64_c _wind_period;
		float64_c _wind_period_weight_minimum;

		// bullet internals.
		btDiscreteDynamicsWorld * _bullet_dynamic_world;
		btDefaultCollisionConfiguration * _bullet_dynamic_configuration;
		btConstraintSolver * _bullet_dynamic_solver;
		btBroadphaseInterface * _bullet_collision_broad_phase;
		btCollisionDispatcher * _bullet_collision_dispatcher;
		btIDebugDraw * _bullet_debug_drawer;

		sint32_c _reference_count;

		physics_scene_c();

	public:
		~physics_scene_c();

		static physics_scene_c * make_new_instance(); // creates a new instance with a reference count of 0.

		void_c add_reference();
		void_c remove_reference();

		void_c copy_properties_from( physics_scene_c * other );

		void_c update( float32_c update_time_step ); // cheonsa provides a fixed time step, but we can provide a variable time step to the physics engine, but bullet will derive a number of fixed time steps from it.
		void_c render(); // renders debug if it is enabled.

		void_c add_rigid_body( physics_rigid_body_c * rigid_body );
		void_c remove_rigid_body( physics_rigid_body_c * rigid_body );
		void_c update_aabbs(); // call this after calling add_rigid_body one or more times in series, or after changing the scene transform of a body that is already in the scene. this will prevent collision detection from breaking.

		void_c add_constraint( physics_constraint_c * constraint );
		void_c remove_constraint( physics_constraint_c * constraint );

		void_c find_all_hits( physics_sweep_test_result_c & result, uint16_c collision_layer, uint16_c collision_layer_mask, vector64x3_c const & sweep_start, vector64x3_c const & sweep_end );
		void_c find_nearest_hit( physics_sweep_test_result_c & result, uint16_c collision_layer, uint16_c collision_layer_mask, vector64x3_c const & sweep_start, vector64x3_c const & sweep_end );
		void_c find_nearest_hit( physics_sweep_test_result_c & result, uint16_c collision_layer, uint16_c collision_layer_mask, vector64x3_c const & sweep_start, vector64x3_c const & sweep_end, physics_shape_c * shape );

		vector64x3_c sample_wind_displacement( vector64x3_c const & point, float64_c wave_frequency, float64_c wave_weight );
		vector64x3_c sample_wind_force( vector64x3_c const & point );

		vector64x3_c sample_gravity_force( vector64x3_c const & point ); // samples cumulative gravity force affecting the given point from the global gravity force and all gravity generators in the scene.
		vector64x3_c sample_up( vector64x3_c const & point ); // negative of gravity force, normalized.

	};

}
