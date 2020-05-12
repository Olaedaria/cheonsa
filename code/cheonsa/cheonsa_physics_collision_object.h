#pragma once

#include "cheonsa_physics_shape.h"

namespace cheonsa
{

	// a static collision object in the physics simulation scene.
	// ridgid bodies can collide with it, but it does not react.
	class physics_collision_object_c
	{
		friend class physics_scene_c;

	private:
		btCollisionObject * _bullet_collision_object;
		physics_shape_c * _shape;
		uint32_c _layer;
		uint32_c _layer_mask;
		sint32_c _reference_count;

		physics_collision_object_c();

	public:
		~physics_collision_object_c();

		static physics_collision_object_c * make_new_instance();

		void_c add_reference();
		void_c remove_reference();

		void_c initialize(
			// a pointer to the game object instance that we want to link with this physics rigid body.
			// this can be any type of object with spatial properties.
			void_c * object,
			physics_shape_c * shape,
			uint32_c layer,
			uint32_c layer_mask );

		void_c uninitialize();

		boolean_c get_is_initialized() const;

		void_c get_world_space_transform( transform3d_c & world_space_transform ) const; // gets world space position and rotation of physics body, and scale of physics shape.
		void_c set_world_space_transform( transform3d_c const & world_space_transform ); // sets world space position and rotation of physics body, and scale of physics shape.

		uint32_c get_layer() const;

		uint32_c get_layer_mask() const;

		float64_c get_friction() const;
		void_c set_friction( float64_c value );

		float64_c get_restitution() const;
		void_c set_restitution( float64_c value );

	};

}
