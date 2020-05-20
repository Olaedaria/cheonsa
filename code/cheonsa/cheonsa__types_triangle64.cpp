#include "cheonsa__types_triangle64.h"
#include "cheonsa__ops_geometry.h"
#include "cheonsa__ops_vector.h"

namespace cheonsa
{

	void_c triangle64_c::make_planes_from_points()
	{
		face_plane = ops::make_plane64_from_triangle( point_a, point_b, point_c );
		edge_plane_ab = ops::make_plane64_from_normal_and_point( ops::normal_vector64x3( ops::cross_product_vector64x3( point_a - point_b, face_plane.get_normal() ) ), point_a );
		edge_plane_bc = ops::make_plane64_from_normal_and_point( ops::normal_vector64x3( ops::cross_product_vector64x3( point_b - point_c, face_plane.get_normal() ) ), point_b );
		edge_plane_ca = ops::make_plane64_from_normal_and_point( ops::normal_vector64x3( ops::cross_product_vector64x3( point_c - point_a, face_plane.get_normal() ) ), point_c );
	}

}
