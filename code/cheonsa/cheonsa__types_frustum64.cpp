#include "cheonsa__types_frustum64.h"
#include "cheonsa__ops_geometry.h"
#include "cheonsa__ops_vector.h"

namespace cheonsa
{

	frustum64_c::frustum64_c()
		: planes{}
		, points{}
	{
	}

	void_c frustum64_c::make_planes_from_points()
	{
		planes[ frustum_plane_index_e_top ] = ops::make_plane64_from_triangle( points[ frustum_point_index_e_near_top_right ], points[ frustum_point_index_e_near_top_left ], points[ frustum_point_index_e_far_top_right ] );
		planes[ frustum_plane_index_e_bottom ] = ops::make_plane64_from_triangle( points[ frustum_point_index_e_near_bottom_left ], points[ frustum_point_index_e_near_bottom_right ], points[ frustum_point_index_e_far_bottom_left ] );
		planes[ frustum_plane_index_e_left ] = ops::make_plane64_from_triangle( points[ frustum_point_index_e_near_top_left ], points[ frustum_point_index_e_near_bottom_left ], points[ frustum_point_index_e_far_top_left ] );
		planes[ frustum_plane_index_e_right ] = ops::make_plane64_from_triangle( points[ frustum_point_index_e_near_bottom_right ], points[ frustum_point_index_e_near_top_right ], points[ frustum_point_index_e_far_bottom_right ] );
		planes[ frustum_plane_index_e_near ] = ops::make_plane64_from_triangle( points[ frustum_point_index_e_near_top_left ], points[ frustum_point_index_e_near_top_right ], points[ frustum_point_index_e_near_bottom_left ] );
		planes[ frustum_plane_index_e_far ] = ops::make_plane64_from_triangle( points[ frustum_point_index_e_far_top_right ], points[ frustum_point_index_e_far_top_left ], points[ frustum_point_index_e_far_bottom_right ] );
	}

	void_c frustum64_c::make_points_from_planes()
	{
		ops::intersect_plane_vs_plane_vs_plane( planes[ frustum_plane_index_e_near ], planes[ frustum_plane_index_e_top ], planes[ frustum_plane_index_e_left ], &points[ frustum_point_index_e_near_top_left ] );
		ops::intersect_plane_vs_plane_vs_plane( planes[ frustum_plane_index_e_near ], planes[ frustum_plane_index_e_top ], planes[ frustum_plane_index_e_right ], &points[ frustum_point_index_e_near_top_right ] );
		ops::intersect_plane_vs_plane_vs_plane( planes[ frustum_plane_index_e_near ], planes[ frustum_plane_index_e_bottom ], planes[ frustum_plane_index_e_left ], &points[ frustum_point_index_e_near_bottom_left ] );
		ops::intersect_plane_vs_plane_vs_plane( planes[ frustum_plane_index_e_near ], planes[ frustum_plane_index_e_bottom ], planes[ frustum_plane_index_e_right ], &points[ frustum_point_index_e_near_bottom_right ] );
		ops::intersect_plane_vs_plane_vs_plane( planes[ frustum_plane_index_e_far ], planes[ frustum_plane_index_e_top ], planes[ frustum_plane_index_e_left ], &points[ frustum_point_index_e_far_top_left ] );
		ops::intersect_plane_vs_plane_vs_plane( planes[ frustum_plane_index_e_far ], planes[ frustum_plane_index_e_top ], planes[ frustum_plane_index_e_right ], &points[ frustum_point_index_e_far_top_right ] );
		ops::intersect_plane_vs_plane_vs_plane( planes[ frustum_plane_index_e_far ], planes[ frustum_plane_index_e_bottom ], planes[ frustum_plane_index_e_left ], &points[ frustum_point_index_e_far_bottom_left ] );
		ops::intersect_plane_vs_plane_vs_plane( planes[ frustum_plane_index_e_far ], planes[ frustum_plane_index_e_bottom ], planes[ frustum_plane_index_e_right ], &points[ frustum_point_index_e_far_bottom_right ] );
	}

	void_c frustum64_c::translate( vector64x3_c const & offset )
	{
		for ( sint32_c i = 0; i < 8; i++ )
		{
			planes[ i ].d += ops::dot_product_float64( planes[ i ].get_normal(), offset );
		}
		make_points_from_planes();
	}

}
