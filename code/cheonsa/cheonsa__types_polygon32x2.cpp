#include "cheonsa__types_polygon32x2.h"
#include "cheonsa__ops_geometry.h"
#include <cassert>

namespace cheonsa
{

	polygon32x2_c::polygon32x2_c()
		: points_count( 0 )
		, points{}
	{
	}

	vector32x2_c polygon32x2_c::get_point( sint32_c point_index ) const
	{
		assert( point_index >= 0 && point_index < points_count );
		return points[ point_index ];
	}

	vector32x2_c polygon32x2_c::get_edge_vector( sint32_c edge_index ) const
	{
		assert( edge_index >= 0 && edge_index < points_count );
		return get_point( ( edge_index + 1 ) % points_count ) - get_point( edge_index );
	}

	line32_c polygon32x2_c::get_edge_line( sint32_c edge_index ) const
	{
		assert( edge_index >= 0 && edge_index < points_count );
		return ops::make_line32_from_segment( get_point( ( edge_index + 1 ) % points_count ), get_point( edge_index ) ); 
	}

}
