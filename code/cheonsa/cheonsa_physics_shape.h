#pragma once

#include "cheonsa_physics_types.h"

namespace cheonsa
{

	// defines a shape, which can then be assigned to a rigid body.
	// call any of the initialize_*() functions to initialize the shape.
	// call uninitialize to release the shape so that it can be initialized again with different parameters.
	// frame defines the non-compoun shape's transform within the compound shape. the frame's scale defines the local scaling of the shape.
	// does some very simple reference counting, so you don't have to .
	class physics_shape_c
	{
		friend class physics_rigid_body_c;
		friend class physics_collision_object_c;
		friend class physics_scene_c;

	private:
		physics_shape_type_e _type;
		btCollisionShape * _bullet_shape;
		btTriangleIndexVertexArray * _bullet_triangle_vertex_array; // used by btBvhTriangleMeshShape.
		transform3d_c _frame; // position and rotation defines this non-compound shape's transform within the compound shape. scale is factored in to this shape's local scale setting.
		core_list_c< physics_shape_c * > _sub_shape_list; // if this is a compound shape, then this list will hold references to those sub shapes.
		sint32_c _reference_count; // since shapes can be shared between multiple rigid bodies.

	public:
		physics_shape_c();
		~physics_shape_c();

		void_c add_reference(); // adds a reference count.
		void_c remove_reference(); // removes a reference count, and if it reaches 0 then deletes this instance.

		physics_shape_type_e get_type() const;

		vector64x3_c get_local_scaling() const; // the physics body can't be scaled, but the shape can be.
		void_c set_local_scaling( vector64x3_c const & value ); // the physics body can't be scaled, but the shape can be.

		void_c initialize_compound( core_list_c< physics_shape_c * > const & physics_shape_list ); // the shapes that you provide will not be owned by this shape, so you will still be responsible for managing their allocation in memory.

		void_c initialize_sphere( transform3d_c const & frame, float64_c radius, float64_c margin );
		float64_c get_sphere_radius() const;

		void_c initialize_box( transform3d_c const & frame, vector64x3_c const & dimensions, float64_c margin );
		vector64x3_c get_box_half_extents() const;

		void_c initialize_capsule( transform3d_c const & frame, float64_c radius, float64_c segment_length, float64_c margin );
		float64_c get_capsule_radius() const;
		float64_c get_capsule_segment_length() const;

		void_c initialize_cylinder( transform3d_c const & frame, float64_c radius, float64_c height, float64_c margin );
		float64_c get_cylinder_radius() const;
		float64_c get_cylinder_height() const;

		void_c initialize_cone( transform3d_c const & frame, float64_c radius, float64_c height, float64_c margin );
		float64_c get_cone_radius() const;
		float64_c get_cone_height() const;

		void_c initialize_convex_hull( transform3d_c const & frame, uint32_c vertex_count, vector64x3_c const * vertex_list, float64_c margin );

		void_c initialize_triangle_mesh( transform3d_c const & frame, uint32_c vertex_count, vector64x3_c const * vertex_list, uint32_c index_count, uint16_c const * index_list, float64_c margin );

		void_c uninitialize();

		boolean_c get_is_initialized() const;

		transform3d_c const & get_frame() const;

	};

}
