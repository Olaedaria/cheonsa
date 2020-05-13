#pragma once

#include "cheonsa_physics_shape.h"

namespace cheonsa
{

	// a rigid body in the physics simulation simulation.
	// has a simple reference count system.
	class physics_rigid_body_c
	{
		friend class physics_constraint_c;
		friend class physics_scene_c;

	private:
		btMotionState * _bullet_motion_state;
		btRigidBody * _bullet_rigid_body;
		physics_shape_c * _shape;
		uint32_c _layer;
		uint32_c _layer_mask;
		sint32_c _reference_count;

		physics_rigid_body_c();

	public:
		~physics_rigid_body_c();

		static physics_rigid_body_c * make_new_instance(); // creates a new instance with a reference count of 0.

		void_c add_reference(); // adds a reference count.
		void_c remove_reference(); // removes a reference count, and if it reaches 0 then deletes this instance.

		void_c initialize(
			// a pointer to the game object instance that we want to link with this physics rigid body.
			// this can be any type of object with spatial properties.
			void_c * object,
			// the physics simulation engine calls this function when it wants to take the world space transform from the linked game object instance and assign it to its linked physics rigid body.
			// this function should copy the requested world_space_position and world_space_basis properties from the game object instance.
			void_c ( *copy_world_space_transform_from_game_to_physics )( void_c * object, vector64x3_c & world_space_position, matrix32x3x3_c & world_space_basis ),
			// the physics simulation engine calls this function when it wants update the world space transform of the game object to match the world space transform of the linked physics rigid body.
			// this function should copy the provided world_space_position and world_space_basis properties to the game object instance.
			void_c ( *copy_world_space_transform_from_physics_to_game )( void_c * object, vector64x3_c const & world_space_position, matrix32x3x3_c const & world_space_basis ),
			// a shape to link with this physics body.
			// this shape will not be owned by the physics body.
			// it will be the user's responsibility to delete the shape instance if needed when it's no longer in use so as to not leak memory.
			physics_shape_c * shape,
			float64_c mass,
			boolean_c kinematic,
			uint32_c layer,
			uint32_c layer_mask );

		void_c uninitialize();

		boolean_c get_is_initialized() const;

		void_c get_world_space_transform( transform3d_c & world_space_transform ) const; // gets world space position and rotation of physics body, and scale of physics shape.
		void_c set_world_space_transform( transform3d_c const & world_space_transform ); // sets world space position and rotation of physics body, and scale of physics shape.
		void_c set_world_space_position( vector64x3_c const & world_position ); // sets position of body.
		void_c clear_forces_and_velocities();

		uint32_c get_layer() const;

		uint32_c get_layer_mask() const;

		float64_c get_friction() const;
		void_c set_friction( float64_c value );

		float64_c get_restitution() const;
		void_c set_restitution( float64_c value );

		float64_c get_mass() const;
		void_c set_mass( float64_c value );

		float64_c get_mass_inverse() const; // kind of faster than get_mass because bullet internally already stores mass as inverse.

		vector64x3_c get_local_momentum( const vector64x3_c & local_point ) const;

		vector64x3_c get_local_velocity( const vector64x3_c & local_point ) const;

		vector64x3_c get_gravity() const;
		void_c set_gravity( const vector64x3_c & value );

		vector64x3_c get_linear_factor() const;
		void_c set_linear_factor( const vector64x3_c & value );

		vector64x3_c get_angular_factor() const;
		void_c set_angular_factor( const vector64x3_c & value );

		float64_c get_linear_damping() const;
		void_c set_linear_damping( float64_c value );

		float64_c get_angular_damping() const;
		void_c set_angular_damping( float64_c value );

		vector64x3_c get_linear_momentum() const;
		void_c set_linear_momentum( const vector64x3_c & value );

		vector64x3_c get_angular_momentum() const;
		void_c set_angular_momentum( const vector64x3_c & value );

		vector64x3_c get_linear_velocity() const;
		void_c set_linear_velocity( const vector64x3_c & value );

		vector64x3_c get_angular_velocity() const;
		void_c set_angular_velocity( const vector64x3_c & value );

		void_c apply_local_linear_impulse( const vector64x3_c & linear_impulse, const vector64x3_c & local_point );
		void_c apply_linear_impulse( const vector64x3_c & linear_impulse );
		void_c apply_angular_impulse( const vector64x3_c & angular_impulse );

		boolean_c get_kinematic() const;
		void_c set_kinematic( boolean_c value );

		boolean_c get_collision_response() const;
		void_c set_collision_response( boolean_c value ); // if set to false, the physics object will remain in the physics scene but it will be like a ghost.

		//boolean_c get_sleeping();
		//void_c set_sleeping( boolean_c value );

		//vector32x3_c get_basis_a();
		//vector32x3_c get_basis_b();
		//vector32x3_c get_basis_c();

	};

}