#pragma once

#include "cheonsa__types__fundamental.h"
#include "cheonsa__types_plane64.h"
#include "cheonsa__types_vector64x3.h"

namespace cheonsa
{

	enum frustum_plane_index_e
	{
		frustum_plane_index_e_top,
		frustum_plane_index_e_bottom,
		frustum_plane_index_e_left,
		frustum_plane_index_e_right,
		frustum_plane_index_e_near,
		frustum_plane_index_e_far,
	};

	enum frustum_point_index_e
	{
		frustum_point_index_e_near_top_left,
		frustum_point_index_e_near_top_right,
		frustum_point_index_e_near_bottom_left,
		frustum_point_index_e_near_bottom_right,
		frustum_point_index_e_far_top_left,
		frustum_point_index_e_far_top_right,
		frustum_point_index_e_far_bottom_left,
		frustum_point_index_e_far_bottom_right,
	};

	// double precision frustum, for gathering of renderables.
	class frustum64_c
	{
	public:
		plane64_c planes[ 6 ]; // each of the volume faces. the normal (top side) of each plane points towards the inside of the frustum volume.
		vector64x3_c points[ 8 ]; // each of the corner points.

	public:
		frustum64_c(); // initializes everything as 0, so it's in an unusable state.

		void_c make_planes_from_points();
		void_c make_points_from_planes();

		void_c translate( vector64x3_c const & offset ); // can be used to translate the frustum without reorienting it.

	};

}
