#pragma once

#include "cheonsa_physics_types.h"

namespace cheonsa
{

	// call any of the initialize_*() functions to initialize the shape.
	// call release() to release the shape, then you can call any of the initialize_*() functions again.
	// local_space_transform defines the local scale of the shape. it also defines the position and rotation of the shape if it is going to be added to a compound shape (otherwise position and rotation are ignored).
	class physics_shape_c
	{
	private:
		friend class physics_body_c;
		friend class physics_scene_c;
		friend class scene_component_body_c;

		space_transform_c _local_space_transform;
		physics_shape_type_e _shape_type;
		btCollisionShape * _bullet_shape;
		btTriangleIndexVertexArray * _bullet_triangle_vertex_array; // used by btBvhTriangleMeshShape.

		void_c _initialize_compound( vector64x3_c const & local_scaling ); // initializes this shape as a compound shape, then you can use _add_compound_shape() and _remove_compound_shape() to add and remove shapes.
		void_c _add_compound_shape( physics_shape_c * shape );
		void_c _remove_compound_shape( physics_shape_c * shape );

	public:
		physics_shape_c();
		~physics_shape_c();

		physics_shape_type_e get_shape_type() const;

		vector64x3_c get_local_scaling() const;

		void_c initialize_sphere( space_transform_c const & local_space_transform, float64_c radius, float64_c margin );
		float64_c get_sphere_radius();

		void_c initialize_box( space_transform_c const & local_space_transform, vector64x3_c const & dimensions, float64_c margin );
		vector64x3_c get_box_half_extents();

		void_c initialize_capsule( space_transform_c const & local_space_transform, float64_c radius, float64_c segment_length, float64_c margin );
		float64_c get_capsule_radius();
		float64_c get_capsule_segment_length();

		void_c initialize_cylinder( space_transform_c const & local_space_transform, float64_c radius, float64_c height, float64_c margin );
		float64_c get_cylinder_radius();
		float64_c get_cylinder_height();

		void_c initialize_cone( space_transform_c const & local_space_transform, float64_c radius, float64_c height, float64_c margin );
		float64_c get_cone_radius();
		float64_c get_cone_height();

		void_c initialize_convex_hull( space_transform_c const & local_space_transform, uint32_c vertex_count, vector64x3_c const * vertex_list, float64_c margin );

		void_c initialize_triangle_mesh( space_transform_c const & local_space_transform, uint32_c vertex_count, vector64x3_c const * vertex_list, uint32_c index_count, uint16_c const * index_list, float64_c margin );

		void_c uninitialize();

	};

}
