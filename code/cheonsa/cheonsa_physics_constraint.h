#pragma once

#include "cheonsa_physics_types.h"

namespace cheonsa
{

	// defines how to constrain the relative range of motion between two different rigid bodies.
	class physics_constraint_c
	{
		friend class physics_scene_c;

	private:
		physics_constraint_type_e _type;
		physics_rigid_body_c * _rigid_body_a;
		physics_rigid_body_c * _rigid_body_b;
		btTypedConstraint * _bullet_constraint;
		sint32_c _reference_count;

	public:
		physics_constraint_c();
		~physics_constraint_c();

		void_c add_reference(); // adds a reference count.
		void_c remove_reference(); // removes a reference count, and if it reaches 0 then deletes this instance.

		physics_rigid_body_c * get_object_a();
		physics_rigid_body_c * get_object_b();

		physics_constraint_type_e get_type() const;

		void_c initialize_fixed( physics_rigid_body_c * a, physics_rigid_body_c * b, transform3d_c const & frame_in_world_space ); // constrains objects a and b together at the position and rotation defined by world_space_frame.

		void_c initialize_point( physics_rigid_body_c * a, physics_rigid_body_c * b, transform3d_c const & frame_in_world_space ); // constrains objects a and b together at the position and rotation defined by world_space_frame.

		void_c initialize_hinge( physics_rigid_body_c * a, physics_rigid_body_c * b, transform3d_c const & frame_in_world_space ); // constrains objects a and b together at the position and rotation defined by world_space_frame. the world space x axis of frame becomes the hinge constraint's axis of rotation.
		void_c get_hinge_limits( float64_c & angular_lower_a, float64_c & angular_upper_a );
		void_c set_hinge_limits( float64_c angular_lower_a, float64_c angular_upper_a );

		void_c initialize_cone( physics_rigid_body_c * a, physics_rigid_body_c * b, transform3d_c const & frame_in_world_space ); // constrains objects a and b together at the transform defined by frame_in_world. the x axis of the frame becomes the constraint's twist axis, and the y and z axes of the frame are used to define the cone's shape.
		void_c get_cone_limits( float64_c & angular_twist_span_a, float64_c & angular_swing_span_b, float64_c & angular_swing_span_c );
		void_c set_cone_limits( float64_c angular_twist_span_a, float64_c angular_swing_span_b, float64_c angular_swing_span_c );

		void_c initialize_generic( physics_rigid_body_c * a, physics_rigid_body_c * b, transform3d_c const & world_space_frame );
		void_c get_generic_limits( vector64x3_c & linear_lower, vector64x3_c & linear_upper, vector64x3_c & angular_lower, vector64x3_c & angular_upper );
		void_c set_generic_limits( vector64x3_c const & linear_lower, vector64x3_c const & linear_upper, vector64x3_c const & angular_lower, vector64x3_c const & angular_upper );

		void_c uninitialize();

	};

}
