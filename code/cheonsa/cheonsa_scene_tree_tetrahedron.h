#pragma once

#include "cheonsa__types.h"

namespace cheonsa
{

	class scene_component_c;

	// search may return:
	// 0 values, tree is empty.
	// 1 values, tree contains only one value.
	// 2 values, query point lied on line of 2.
	// 3 values, query point lied on plane of 3, or lies outside of volume and is projected onto surface.
	// 4 values, query point lied in volume of 4.
	// given an arbitrary cloud of points, this tree constructs a volume "mesh", similar to a convex hull, but with internal "cells" (tetrahedrons).
	// each tetrahedron is made up of 4 points.
	// tetrahedrons do not intersect each other.
	// 3d barycentric coordinates.
	class scene_tree_tetrahedron_c
	{
	public:
		class search_result_c
		{
		public:
			sint32_c count; // between 0 and 4.
			scene_component_c * values[ 4 ];
			float32_c weights[ 4 ];

		};


	};

}
