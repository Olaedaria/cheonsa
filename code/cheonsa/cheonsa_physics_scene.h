#pragma once

#include "cheonsa_core_list.h"
#include "cheonsa_physics_types.h"

namespace cheonsa
{

	// physics scene.
	// collision detection and physics simulation manager.
	// wrapper interface for a proper physics engine, currently bullet3 2.88.
	// currently models wind with a very simple and fake function of time and space model.
	// but in the future it would be nice to model weather volumetrically with cells (cubes), each with its own pressure, temperature, humidity, etc.
	class physics_scene_c
	{
	private:
		// bullet internals.
		btDiscreteDynamicsWorld * _bullet_dynamic_world;
		btDefaultCollisionConfiguration * _bullet_dynamic_configuration;
		btConstraintSolver * _bullet_dynamic_solver;
		btBroadphaseInterface * _bullet_collision_broad_phase;
		btCollisionDispatcher * _bullet_collision_dispatcher;
		btIDebugDraw * _bullet_debug_drawer;
		boolean_c _bullet_aabbs_are_dirty;
		vector32x3_c _up; // negative of gravity, normalized.

		// wind model.
		vector64x3_c _wind_direction;
		float64_c _wind_speed;
		float64_c _wind_offset;
		float64_c _wind_period;
		float64_c _wind_period_weight_minimum;

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

		void_c add_collision_object( physics_collision_object_c * collision_object ); // also marks aabbs as dirty, so that they can be recalculated on the next call to update().
		void_c remove_collision_object( physics_collision_object_c * collision_object );

		void_c add_rigid_body( physics_rigid_body_c * rigid_body );
		void_c remove_rigid_body( physics_rigid_body_c * rigid_body );

		void_c add_constraint( physics_constraint_c * constraint );
		void_c remove_constraint( physics_constraint_c * constraint );

		void_c update_aabbs(); // this needs to be called any collision objects are added, or after changing the transform of a collision object that is already in the simulation scene. this will prevent collision detection from breaking. this gets called automatically by update(), but if you need to update before update then this is provided here for convenience.

		void_c find_all_hits( physics_sweep_test_result_c & result, uint16_c collision_layer, uint16_c collision_layer_mask, vector64x3_c const & sweep_start, vector64x3_c const & sweep_end );
		void_c find_nearest_hit( physics_sweep_test_result_c & result, uint16_c collision_layer, uint16_c collision_layer_mask, vector64x3_c const & sweep_start, vector64x3_c const & sweep_end );
		void_c find_nearest_hit( physics_sweep_test_result_c & result, uint16_c collision_layer, uint16_c collision_layer_mask, vector64x3_c const & sweep_start, vector64x3_c const & sweep_end, physics_shape_c * shape );

		vector64x3_c sample_wind_displacement( vector64x3_c const & point, float64_c wave_frequency, float64_c wave_weight );
		vector64x3_c sample_wind_force( vector64x3_c const & point );

		vector64x3_c get_gravity() const;
		void_c set_gravity( vector64x3_c const & value );

		vector32x3_c get_up( vector64x3_c const & point ); // negative of gravity force, normalized.

	};

}
