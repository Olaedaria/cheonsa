#pragma once

#include "cheonsa__types_vector64x3.h"
#include "cheonsa__types_plane64.h"

namespace cheonsa
{

	class triangle64_c
	{
	public:
		vector64x3_c point_a;
		vector64x3_c point_b;
		vector64x3_c point_c;
		plane64_c face_plane;
		plane64_c edge_plane_ab; // plane, tangent with face_plane, but along edge ab, with normal point outward (?).
		plane64_c edge_plane_bc; // plane, tangent with face_plane, but along edge bc, with normal point outward (?).
		plane64_c edge_plane_ca; // plane, tangent with face_plane, but along edge ca, with normal point outward (?).

	public:
		inline triangle64_c() : point_a(), point_b(), point_c(), face_plane(), edge_plane_ab(), edge_plane_bc(), edge_plane_ca() {}
		inline triangle64_c( vector64x3_c const & point_a, vector64x3_c const & point_b, vector64x3_c const & point_c ) : point_a( point_a ), point_b( point_b ), point_c( point_c ), face_plane(), edge_plane_ab(), edge_plane_bc(), edge_plane_ca() { make_planes_from_points(); }

		void_c make_planes_from_points(); // calculates the plane members of this triangle from the points.

	};

}
