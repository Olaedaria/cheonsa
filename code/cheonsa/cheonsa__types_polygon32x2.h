#pragma once

#include "cheonsa__types_vector32x2.h"
#include "cheonsa__types_line32.h"

namespace cheonsa
{

	// 2d polygon.
	// winding of points can be clock wise or counter clock wise.
	// geometry can be convex or concave.
	// cheonsa's polygon operation functions will construct polygons with counter clock wise wind order.
	// cheonsa's polygon intersection functions will probably assume convex geometry.
	class polygon32x2_c
	{
	public:
		//vector32x2_c origin;
		//matrix32x2x2_c basis;
		sint32_c points_count;
		vector32x2_c points[ 8 ]; // points in local space, without transform applied.

	public:
		polygon32x2_c();

		// gets point in polygon.
		vector32x2_c get_point( sint32_c point_index ) const;
		
		// gets edge vector in polygon, which is edge point b minus edge point a.
		vector32x2_c get_edge_vector( sint32_c edge_index ) const;

		// gets edge line in polygon.
		// if wind order of polygon points is clock wise then normal of line will point towards interior of polygon.
		// if wind order of polygon points is counter clock wise then normal of line will point towards exterior of polygon.
		line32_c get_edge_line( sint32_c edge_index ) const;

	};

}
