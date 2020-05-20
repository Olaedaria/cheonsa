#include "cheonsa__ops_geometry.h"
#include "cheonsa__ops_vector.h"
#include "cheonsa__ops_math.h"
#include <cassert>

namespace cheonsa
{
	namespace ops
	{

		line32_c make_line32_from_normal_and_distance( vector32x2_c const & normal, float32_c distance )
		{
			return make_line32_normalized( line32_c( normal.a, normal.b, distance ) );
		}

		line32_c make_line32_from_normal_and_point( vector32x2_c const & normal, vector32x2_c const & point )
		{
			return make_line32_normalized( line32_c( normal.a, normal.b, -dot_product_float32( normal, point ) ) );
		}

		line32_c make_line32_from_segment( vector32x2_c const & point_a, vector32x2_c const & point_b )
		{
			vector32x2_c normal = normal_vector32x2( orthogonalized_vector32x2( point_b - point_a ) );
			return make_line32_from_normal_and_point( normal, point_a );
		}

		line32_c make_line32_normalized( line32_c const & line )
		{
			float32_c line_normal_length = length_float32( line.get_normal() );
			if ( line_normal_length > constants< float32_c >::division_near_zero() )
			{
				return line32_c( line.a / line_normal_length, line.b / line_normal_length, line.c / line_normal_length );
			}
			return line;
		}

		/*
		vector32x2_c get_box_point( box32x2_c const & box, matrix32x2x2_c const & box_basis, vector32x2_c const & box_origin, sint32_c box_point_index )
		{
			vector32x2_c result;
			switch ( box_point_index )
			{
			case 0:
				// top left.
				result.a = box.minimum.a;
				result.b = box.minimum.b;
				break;
			case 1:
				// top right.
				result.a = box.maximum.a;
				result.b = box.minimum.b;
				break;
			case 2:
				// bottom left.
				result.a = box.minimum.a;
				result.b = box.maximum.b;
				break;
			case 3:
				// bottom right.
				result.a = box.maximum.b;
				result.b = box.maximum.b;
				break;
			}
			result = rotate_and_scale_vector32x2( result, box_origin, box_basis );
			return result;
		}

		line32_c get_box_line( box32x2_c const & box, matrix32x2x2_c const & box_basis, vector32x2_c const & box_origin, sint32_c box_edge_index )
		{
			line32_c result;
			vector32x2_c segment_point_a; // origin of vector to produce tangent.
			vector32x2_c segment_point_b; // extrusion of vector to produce tangent.
			switch ( box_edge_index )
			{
			case 0:
				// left edge, top to bottom, so that right hand tangent/normal points left towards outside.
				segment_point_a.a = box.minimum.a;
				segment_point_a.b = box.minimum.b;
				segment_point_b.a = box.minimum.a;
				segment_point_b.b = box.maximum.b;
				break;
			case 1:
				// top edge, right to left, so that right hand tangent/normal points up towards outside.
				segment_point_a.a = box.maximum.a;
				segment_point_a.b = box.minimum.b;
				segment_point_b.a = box.minimum.a;
				segment_point_b.b = box.minimum.b;
				break;
			case 2:
				// right edge, bottom to top, so that right hand tangent/normal points right towards outside.
				segment_point_a.a = box.maximum.a;
				segment_point_a.b = box.maximum.b;
				segment_point_b.a = box.maximum.a;
				segment_point_b.b = box.minimum.b;
				break;
			case 3:
				// bottom edge, left to right, so that right hand tangent/normal points down towards outside.
				segment_point_a.a = box.minimum.a;
				segment_point_a.b = box.maximum.b;
				segment_point_b.a = box.maximum.a;
				segment_point_b.b = box.maximum.b;
				break;
			}
			segment_point_a = rotate_and_scale_vector32x2( segment_point_a, box_basis, box_origin );
			segment_point_b = rotate_and_scale_vector32x2( segment_point_b, box_basis, box_origin );
			result = make_line32_from_segment( segment_point_a, segment_point_b );
			return result;
		}
		*/


		//
		//
		// plane operations.
		//
		//

		plane32_c make_plane32_from_normal_and_point( vector32x3_c const & normal, vector32x3_c const & point )
		{
			return plane32_c( normal.a, normal.b, normal.c, -dot_product_float32( normal, point ) );
		}

		plane32_c make_plane32_from_triangle( vector32x3_c const & point_a, vector32x3_c const & point_b, vector32x3_c const & point_c )
		{
			vector32x3_c normal = normal_vector32x3( cross_product_vector32x3( point_b - point_a, point_c - point_a ) );
			return make_plane32_from_normal_and_point( normal, point_a );
		}

		plane64_c make_plane64_from_normal_and_point( vector64x3_c const & normal, vector64x3_c const & point )
		{
			//return make_plane64_normalized( plane64_c( normal.a, normal.b, normal.c, -dot_product_float64( normal, point ) ) );
			return plane64_c( normal.a, normal.b, normal.c, -dot_product_float64( normal, point ) );
		}

		plane64_c make_plane64_from_triangle( vector64x3_c const & point_a, vector64x3_c const & point_b, vector64x3_c const & point_c )
		{
			vector64x3_c normal = normal_vector64x3( cross_product_vector64x3( point_b - point_a, point_c - point_a ) );
			return make_plane64_from_normal_and_point( normal, point_a );
		}

		plane64_c make_plane64_normalized( plane64_c const & plane )
		{
			float64_c plane_normal_length = length_float64( plane.get_normal() );
			if ( plane_normal_length > constants< float64_c >::division_near_zero() )
			{
				return plane64_c( plane.a / plane_normal_length, plane.b / plane_normal_length, plane.c / plane_normal_length, plane.d / plane_normal_length );
			}
			return plane;
		}


		//
		//
		// frustum operations.
		//
		//

		frustum64_c make_frustum64_perspective3( vector64x3_c const & position, vector64x3_c const & forward, vector64x3_c const & up, float64_c const fov_x, float64_c const fov_y, float64_c const clip_near, float64_c const clip_far )
		{
			frustum64_c result;

			assert( length_float64( forward ) > constants< float64_c >::division_near_zero() );
			assert( length_float64( up ) > constants< float64_c >::division_near_zero() );
			vector64x3_c forward_normal = normal_vector64x3( forward );
			vector64x3_c up_normal = normal_vector64x3( up );
			vector64x3_c right_normal = cross_product_vector64x3( forward_normal, up_normal ); // used to find down that is orthogonal to forward, because up may not be orthogonal to forward.
			vector64x3_c down_normal = cross_product_vector64x3( forward_normal, right_normal );

			vector64x2_c near_size_half = vector64x2_c( math_tangent( fov_x / 2.0 ) * clip_near, math_tangent( fov_y / 2.0 ) * clip_near );

			vector64x3_c near_center = position + ( forward_normal * clip_near ) ; // vector64x3_c( transform.get_position() ) + ( vector64x3_c( transform.get_basis_b() ) * near_clip );
			result.points[ frustum_point_index_e_near_top_left ] = near_center + ( down_normal * -near_size_half.b ) + ( right_normal * -near_size_half.a );
			result.points[ frustum_point_index_e_near_top_right ] = near_center + ( down_normal * -near_size_half.b ) + ( right_normal * near_size_half.a );
			result.points[ frustum_point_index_e_near_bottom_left ] = near_center + ( down_normal * near_size_half.b ) + ( right_normal * -near_size_half.a );
			result.points[ frustum_point_index_e_near_bottom_right ] = near_center + ( down_normal * near_size_half.b ) + ( right_normal * near_size_half.a );

			vector64x2_c far_size_half = vector64x2_c( math_tangent( fov_x / 2.0 ) * clip_far, math_tangent( fov_y / 2.0 ) * clip_far );

			vector64x3_c far_center = position + ( forward_normal * clip_far );

			result.points[ frustum_point_index_e_far_top_left ] = far_center + ( down_normal * -far_size_half.b ) + ( right_normal * -far_size_half.a );
			result.points[ frustum_point_index_e_far_top_right ] = far_center + ( down_normal * -far_size_half.b ) + ( right_normal * far_size_half.a );
			result.points[ frustum_point_index_e_far_bottom_left ] = far_center + ( down_normal * far_size_half.b ) + ( right_normal * -far_size_half.a );
			result.points[ frustum_point_index_e_far_bottom_right ] = far_center + ( down_normal * far_size_half.b ) + ( right_normal * far_size_half.a );

			result.make_planes_from_points();

			return result;
		}

		frustum64_c make_frustum64_orthographic3( vector64x3_c const & position, vector64x3_c const & forward, vector64x3_c const & up, float64_c const fov_x, float64_c const fov_y, float64_c const clip_near, float64_c const clip_far )
		{
			frustum64_c result;

			assert( length_float64( forward ) > constants< float64_c >::division_near_zero() );
			assert( length_float64( up ) > constants< float64_c >::division_near_zero() );
			vector64x3_c forward_normal = normal_vector64x3( forward );
			vector64x3_c up_normal = normal_vector64x3( up );
			vector64x3_c right_normal = cross_product_vector64x3( forward_normal, up_normal ); // used to find down that is orthogonal to forward, because up may not be orthogonal to forward.
			vector64x3_c down_normal = cross_product_vector64x3( forward_normal, right_normal );

			vector64x2_c half_size = vector64x2_c( fov_x * 0.5, fov_y * 0.5 );

			vector64x3_c near_center = position + ( forward_normal * clip_near );

			result.points[ frustum_point_index_e_near_top_left ] = near_center + ( down_normal * -half_size.b ) + ( right_normal * -half_size.a );
			result.points[ frustum_point_index_e_near_top_right ] = near_center + ( down_normal * -half_size.b ) + ( right_normal * half_size.a );
			result.points[ frustum_point_index_e_near_bottom_left ] = near_center + ( down_normal * half_size.b ) + ( right_normal * -half_size.a );
			result.points[ frustum_point_index_e_near_bottom_right ] = near_center + ( down_normal * half_size.b ) + ( right_normal * half_size.a );

			vector64x3_c far_center = position + ( down_normal * clip_far );

			result.points[ frustum_point_index_e_far_top_left ] = far_center + ( down_normal * -half_size.b ) + ( right_normal * -half_size.a );
			result.points[ frustum_point_index_e_far_top_right ] = far_center + ( down_normal * -half_size.b ) + ( right_normal * half_size.a );
			result.points[ frustum_point_index_e_far_bottom_left ] = far_center + ( down_normal * half_size.b ) + ( right_normal * -half_size.a );
			result.points[ frustum_point_index_e_far_bottom_right ] = far_center + ( down_normal * half_size.b ) + ( right_normal * half_size.a );

			result.make_planes_from_points();

			return result;
		}

		frustum64_c make_frustum64_from_view_projection( matrix32x4x4_c const & view_projection )
		{
			frustum64_c result;

			// left plane.
			result.planes[ 0 ].a = view_projection.a.d + view_projection.a.a;
			result.planes[ 0 ].b = view_projection.b.d + view_projection.b.a;
			result.planes[ 0 ].c = view_projection.c.d + view_projection.c.a;
			result.planes[ 0 ].d = view_projection.d.d + view_projection.d.a;
			result.planes[ 0 ] = make_plane64_normalized( result.planes[ 0 ] );

			// right plane.
			result.planes[ 1 ].a = view_projection.a.d - view_projection.a.a;
			result.planes[ 1 ].b = view_projection.b.d - view_projection.b.a;
			result.planes[ 1 ].c = view_projection.c.d - view_projection.c.a;
			result.planes[ 1 ].d = view_projection.d.d - view_projection.d.a;
			result.planes[ 1 ] = make_plane64_normalized( result.planes[ 1 ] );

			// top plane.
			result.planes[ 2 ].a = view_projection.a.d - view_projection.a.b;
			result.planes[ 2 ].b = view_projection.b.d - view_projection.b.b;
			result.planes[ 2 ].c = view_projection.c.d - view_projection.c.b;
			result.planes[ 2 ].d = view_projection.d.d - view_projection.d.b;
			result.planes[ 2 ] = make_plane64_normalized( result.planes[ 2 ] );

			// bottom plane.
			result.planes[ 3 ].a = view_projection.a.d + view_projection.a.b;
			result.planes[ 3 ].b = view_projection.b.d + view_projection.b.b;
			result.planes[ 3 ].c = view_projection.c.d + view_projection.c.b;
			result.planes[ 3 ].d = view_projection.d.d + view_projection.d.b;
			result.planes[ 3 ] = make_plane64_normalized( result.planes[ 3 ] );

			// near plane.
			result.planes[ 4 ].a = view_projection.a.c;
			result.planes[ 4 ].b = view_projection.b.c;
			result.planes[ 4 ].c = view_projection.c.c;
			result.planes[ 4 ].d = view_projection.d.c;
			result.planes[ 4 ] = make_plane64_normalized( result.planes[ 4 ] );

			// far plane.
			result.planes[ 5 ].a = view_projection.a.d - view_projection.a.c;
			result.planes[ 5 ].b = view_projection.b.d - view_projection.b.c;
			result.planes[ 5 ].c = view_projection.c.d - view_projection.c.c;
			result.planes[ 5 ].d = view_projection.d.d - view_projection.d.c;
			result.planes[ 5 ] = make_plane64_normalized( result.planes[ 5 ] );

			result.make_points_from_planes();

			return result;
		}

		polygon32x2_c make_polygon32x2_from_box32x2( box32x2_c const & box_in )
		{
			polygon32x2_c result;
			result.points_count = 4;
			result.points[ 0 ].a = box_in.minimum.a;
			result.points[ 0 ].b = box_in.minimum.b;
			result.points[ 1 ].a = box_in.minimum.a;
			result.points[ 1 ].b = box_in.maximum.b;
			result.points[ 2 ].a = box_in.maximum.a;
			result.points[ 2 ].b = box_in.maximum.b;
			result.points[ 3 ].a = box_in.maximum.a;
			result.points[ 3 ].b = box_in.minimum.b;
			return result;
		}

		polygon32x2_c make_polygon32x2_transformed( polygon32x2_c const & polygon_in, matrix32x2x2_c transform_basis, vector32x2_c transform_origin )
		{
			polygon32x2_c result;
			result.points_count = polygon_in.points_count;
			for ( sint32_c i = 0; i < polygon_in.points_count; i++ )
			{
				result.points[ i ] = rotate_and_scale_vector32x2( polygon_in.points[ i ], transform_basis ) + transform_origin;
			}
			return result;
		}

		vector64x3_c make_vector64x3_normal_from_triangle( vector64x3_c a, vector64x3_c b, vector64x3_c c )
		{
			return normal_vector64x3( cross_product_vector64x3( a - b, c - b ) );
		}

		box64x3_c make_aabb_from_obb( box64x3_c const & obb, transform3d_c const & obb_transform )
		{
			matrix32x3x3_c obb_matrix = obb_transform.get_scaled_basis();
			if ( obb_matrix.a.a < 0.0f ) { obb_matrix.a.a = -obb_matrix.a.a; }
			if ( obb_matrix.a.b < 0.0f ) { obb_matrix.a.b = -obb_matrix.a.b; }
			if ( obb_matrix.a.c < 0.0f ) { obb_matrix.a.c = -obb_matrix.a.c; }
			if ( obb_matrix.b.a < 0.0f ) { obb_matrix.b.a = -obb_matrix.b.a; }
			if ( obb_matrix.b.b < 0.0f ) { obb_matrix.b.b = -obb_matrix.b.b; }
			if ( obb_matrix.b.c < 0.0f ) { obb_matrix.b.c = -obb_matrix.b.c; }
			if ( obb_matrix.c.a < 0.0f ) { obb_matrix.c.a = -obb_matrix.c.a; }
			if ( obb_matrix.c.b < 0.0f ) { obb_matrix.c.b = -obb_matrix.c.b; }
			if ( obb_matrix.c.c < 0.0f ) { obb_matrix.c.c = -obb_matrix.c.c; }
			vector64x3_c center = ( obb.minimum + obb.maximum ) * 0.5;
			vector64x3_c extent = rotate_and_scale_vector64x3( obb.maximum - center, obb_matrix );
			return box64x3_c( center - extent + obb_transform.position, center + extent + obb_transform.position );
		}

		box32x2_c make_aabb_from_obb( box32x2_c const & obb, vector32x2_c const & obb_position, matrix32x2x2_c obb_basis )
		{
			if ( obb_basis.a.a < 0.0f ) { obb_basis.a.a = -obb_basis.a.a; }
			if ( obb_basis.a.b < 0.0f ) { obb_basis.a.b = -obb_basis.a.b; }
			if ( obb_basis.b.a < 0.0f ) { obb_basis.b.a = -obb_basis.b.a; }
			if ( obb_basis.b.b < 0.0f ) { obb_basis.b.b = -obb_basis.b.b; }
			vector32x2_c center = ( obb.minimum + obb.maximum ) * 0.5f;
			vector32x2_c extent = rotate_and_scale_vector32x2( obb.minimum - center, obb_basis );
			return box32x2_c( center - extent + obb_position, center + extent + obb_position);
		}

		float32_c distance_between_point_and_line( vector32x2_c const & point, line32_c const & line )
		{
			return ( point.a * line.a ) + ( point.b * line.b ) + ( line.c );
		}

		float64_c distance_between_point_and_plane( vector64x3_c const & point, plane64_c const & plane )
		{
			return ( point.a * plane.a ) + ( point.b * plane.b ) + ( point.c * plane.c ) + ( plane.d );
		}

		vector64x3_c nearest_point_on_box( vector64x3_c const & point, box64x3_c const & box )
		{
			return vector64x3_c( math_clamp( point.a, box.minimum.a, box.maximum.a ),
				math_clamp( point.b, box.minimum.b, box.maximum.b ),
				math_clamp( point.c, box.minimum.c, box.maximum.c ) );
		}

		vector64x3_c nearest_point_on_box( vector64x3_c const & point, box64x3_c const & box, transform3d_c const & box_transform )
		{
			vector64x3_c box_center = box.get_center();
			vector64x3_c box_center_transformed = rotate_vector64x3( box_center, box_transform.rotation ) * box_transform.scale + box_transform.position;
			vector64x3_c box_half_distances = ( box.maximum - box_center ) * box_transform.scale;
			vector64x3_c box_center_to_point = point - box_center_transformed;
			matrix32x3x3_c box_transform_basis = box_transform.get_scaled_basis();
			return box_center_transformed + ( box_transform_basis.a * math_clamp( dot_product_float64( vector64x3_c( box_transform_basis.a ), box_center_to_point ), -box_half_distances.a, box_half_distances.a ) ) + ( box_transform_basis.b * math_clamp( dot_product_float64( vector64x3_c( box_transform_basis.b ), box_center_to_point ), -box_half_distances.b, box_half_distances.b ) ) + ( box_transform_basis.c * math_clamp( dot_product_float64( vector64x3_c( box_transform_basis.c ), box_center_to_point ), -box_half_distances.c, box_half_distances.c ) );
		}

		vector64x3_c nearest_point_on_plane( vector64x3_c const & point, plane64_c const & plane )
		{
			return point - ( plane.get_normal() * distance_between_point_and_plane( point, plane ) );
		}

		vector64x3_c nearest_point_on_ray( vector64x3_c const & point, ray64_c const & ray )
		{
			float64_c t = dot_product_float64( point - ray.position, ray.normal );
			return ray.position + ( ray.normal * t );
		}

		vector64x3_c nearest_point_on_segment( vector64x3_c const & point, segment64_c const & segment )
		{
			vector64x3_c segment_ab = segment.point_b - segment.point_a;
			float64_c t = dot_product_float64( point - segment.point_a, segment_ab ) / length_squared_float64( segment_ab );
			if ( t <= 0.0 )
			{
				return segment.point_a;
			}
			else if ( t >= 1.0 )
			{
				return segment.point_b;
			}
			else
			{
				return segment.point_a + ( segment.point_b - segment.point_a ) * t;
			}
		}

		vector64x2_c nearest_point_on_box( vector64x2_c const & point, box64x2_c const & box )
		{
			return vector64x2_c( math_clamp( point.a, box.minimum.a, box.maximum.a ), math_clamp( point.b, box.minimum.b, box.maximum.b ) );
		}

		boolean_c intersect_frustum_vs_point( frustum64_c const & frustum, vector64x3_c const & point )
		{
			for ( sint32_c i = 0; i < 6; i++ )
			{
				if ( distance_between_point_and_plane( point, frustum.planes[ i ] ) < 0.0 )
				{
					return false;
				}
			}
			return true;
		}

		boolean_c intersect_frustum_vs_sphere( frustum64_c const & frustum, sphere64_c const & sphere )
		{
			for ( sint32_c i = 0; i < 6; i++ )
			{
				if ( distance_between_point_and_plane( sphere.position, frustum.planes[ i ] ) < -sphere.radius )
				{
					return false;
				}
			}
			return true;
		}

		boolean_c intersect_frustum_vs_box( frustum64_c const & frustum, box64x3_c const & box )
		{
			vector64x3_c c = box.get_center();
			vector64x3_c d = box.maximum - c;
			for ( sint32_c i = 0; i < 6; i++ )
			{
				plane64_c const & p = frustum.planes[ i ];
				float64_c m = distance_between_point_and_plane( c, p );
				float64_c n = ( d.a * math_absolute_value( p.a ) ) + ( d.b * math_absolute_value( p.b ) ) + ( d.c * math_absolute_value( p.c ) );
				if ( m + n < 0 )
				{
					return false;
				}
			}
			return true;
		}

		boolean_c intersect_frustum_vs_box( frustum64_c const & frustum, box64x3_c const & box, transform3d_c const & box_transform )
		{
			// this code taken from:
			// http://www.gamedev.net/topic/539116-optimized-obb-frustum64_c/
			matrix32x3x3_c box_transform_basis = box_transform.get_scaled_basis();
			vector64x3_c box_center = rotate_scale_and_translate_vector64x3( box.get_center(), box_transform );
			vector64x3_c box_half_lengths = ( box.maximum - box.minimum ) * 0.5;
			vector64x3_c to_inside; // temp, will store the corner point of the box that is closest to the inside of the plane we are testing against.
			for ( sint32_c i = 0; i < 6; i++ ) // test the box against each plane in the frustum.
			{
				plane64_c const & p = frustum.planes[ i ];
				// find the corner on the box that is most likely to be above the plane that we are testing against.
				to_inside.a = dot_product_float64( vector64x3_c( box_transform_basis.a ), p.get_normal() ) > 0.0 ? box_half_lengths.a : -box_half_lengths.a;
				to_inside.b = dot_product_float64( vector64x3_c( box_transform_basis.b ), p.get_normal() ) > 0.0 ? box_half_lengths.b : -box_half_lengths.b;
				to_inside.c = dot_product_float64( vector64x3_c( box_transform_basis.c ), p.get_normal() ) > 0.0 ? box_half_lengths.c : -box_half_lengths.c;
				to_inside = box_center + ( box_transform_basis.a * to_inside.a ) + ( box_transform_basis.b * to_inside.b ) + ( box_transform_basis.c * to_inside.c );
				// test if the point is inside (above) our outside (below) the plane we are testing against.
				if ( distance_between_point_and_plane( to_inside, p ) < 0.0 )
				{
					return false;
				}
			}
			return true;
		}

		boolean_c intersect_sphere_vs_point( sphere64_c const & sphere, vector64x3_c const & point )
		{
			return length_squared_float64( sphere.position - point ) <= ( sphere.radius * sphere.radius );
		}

		boolean_c intersect_sphere_vs_sphere( sphere64_c const & sphere_a, sphere64_c const & sphere_b )
		{
			return length_squared_float64( sphere_a.position - sphere_b.position ) <= ( ( sphere_a.radius + sphere_b.radius ) * ( sphere_a.radius + sphere_b.radius ) );
		}

		boolean_c intersect_box_vs_point( box64x3_c const & box, vector64x3_c const & point )
		{
			return ( point.a >= box.minimum.a && point.a <= box.maximum.a ) && ( point.b >= box.minimum.b && point.b <= box.maximum.b ) && ( point.c >= box.minimum.c && point.c <= box.maximum.c );
		}

		boolean_c intersect_box_vs_box( box64x3_c const & box_a, box64x3_c const & box_b )
		{
			return ( box_a.maximum.a >= box_b.minimum.b && box_a.maximum.b >= box_b.minimum.b && box_a.maximum.c >= box_b.minimum.c );
		}

		boolean_c intersect_box_vs_box( box64x3_c const & box_a, transform3d_c const & box_a_transform, box64x3_c const & box_b, transform3d_c const & box_b_transform )
		{
			// this code taken from:
			// http://www.3dkingdoms.com/weekly/bbox.cpp

			vector64x3_c size_a = ( box_a.maximum - box_a.minimum ) * 0.5;
			vector64x3_c size_b = ( box_b.maximum - box_b.minimum ) * 0.5;

			matrix32x3x3_c rotation; // rotation from a to b.
			matrix32x3x3_c rotation_absolute; // rotation from a to b, absolutley valued.

			matrix32x3x3_c box_a_transform_basis = box_a_transform.get_scaled_basis();
			matrix32x3x3_c box_b_transform_basis = box_b_transform.get_scaled_basis();

			for ( sint32_c i = 0; i < 3; i++ )
			{
				for ( sint32_c k = 0; k < 3; k++ )
				{
					rotation.get_element_at_index( i ).get_element_at_index( k ) = dot_product_float32( box_a_transform_basis.get_element_at_index( i ), box_b_transform_basis.get_element_at_index( k ) );
					rotation_absolute.get_element_at_index( i ).get_element_at_index( k ) = math_absolute_value( rotation.get_element_at_index( i ).get_element_at_index( k ) );
				}
			}

			vector64x3_c center_a = ( box_a.get_center() + box_a_transform.position );
			vector64x3_c center_b = ( box_b.get_center() + box_b_transform.position );
			vector64x3_c center_delta = ( box_a.get_center() + box_a_transform.position ) - ( box_b.get_center() + box_b_transform.position ); // center delta in world space.
			vector64x3_c center_delta_local = vector64x3_c( dot_product_float64( vector64x3_c( rotation.get_element_at_index( 0 ) ), center_delta ), dot_product_float64( vector64x3_c( rotation.get_element_at_index( 1 ) ), center_delta ), dot_product_float64( vector64x3_c( rotation.get_element_at_index( 2 ) ), center_delta ) ); // center delta in a space.

			float64_c extent_a;
			float64_c extent_b;
			float64_c separation;

			// test if any of box_a's basis vectors separate the box.
			for ( sint32_c i = 0; i < 3; i++ )
			{
				extent_a = size_a.get_element_at_index( i );
				extent_b = dot_product_float64( size_b, vector64x3_c( rotation_absolute.get_element_at_index( i ).get_element_at_index( 0 ), rotation_absolute.get_element_at_index( i ).get_element_at_index( 1 ), rotation_absolute.get_element_at_index( i ).get_element_at_index( 2 ) ) );
				separation = math_absolute_value( center_delta_local.get_element_at_index( i ) );

				if ( separation > ( extent_a + extent_b ) )
				{
					return false;
				}
			}

			// test if any of box_b's basis vectors separate the box.
			for ( sint32_c k = 0; k < 3; k++ )
			{
				extent_a = dot_product_float64( size_a, vector64x3_c( rotation_absolute.get_element_at_index( 0 ).get_element_at_index( k ), rotation_absolute.get_element_at_index( 1 ).get_element_at_index( k ), rotation_absolute.get_element_at_index( 2 ).get_element_at_index( k ) ) );
				extent_b = size_b.get_element_at_index( k );
				separation = math_absolute_value( dot_product_float64( center_delta_local, vector64x3_c( rotation.get_element_at_index( 0 ).get_element_at_index( k ), rotation.get_element_at_index( 1 ).get_element_at_index( k ), rotation.get_element_at_index( 2 ).get_element_at_index( k ) ) ) );

				if ( separation > ( extent_a + extent_b ) )
				{
					return false;
				}
			}

			// now test cross products of each basis vector combination ( a[i], b[k] ).
			for ( sint32_c i = 0; i < 3; i++ )
			{
				for ( sint32_c k = 0; k < 3; k++ )
				{
					sint32_c i1 = ( i + 1 ) % 3;
					sint32_c i2 = ( i + 2 ) % 3;
					sint32_c k1 = ( k + 1 ) % 3;
					sint32_c k2 = ( k + 2 ) % 3;
					extent_a = ( size_a.get_element_at_index( i1 ) * rotation_absolute.get_element_at_index( i2 ).get_element_at_index( k ) ) + ( size_a.get_element_at_index( i2 ) * rotation_absolute.get_element_at_index( i1 ).get_element_at_index( k ) );
					extent_b = ( size_b.get_element_at_index( k1 ) * rotation_absolute.get_element_at_index( i ).get_element_at_index( k2 ) ) + ( size_b.get_element_at_index( k2 ) * rotation_absolute.get_element_at_index( i ).get_element_at_index( k1 ) );
					separation = math_absolute_value( ( center_delta_local.get_element_at_index( i2 ) * rotation.get_element_at_index( i1 ).get_element_at_index( k ) ) - ( center_delta_local.get_element_at_index( i1 ) * rotation.get_element_at_index( i2 ).get_element_at_index( k ) ) );
					if ( separation > extent_a + extent_b )
					{
						return false;
					}
				}
			}

			// no separating axis found, the boxes overlap.
			return true;
		}

		boolean_c intersect_box_vs_sphere( box64x3_c const & box, sphere64_c const & sphere )
		{
			vector64x3_c closest_point_on_box = nearest_point_on_box( sphere.position, box );
			return length_squared_float64( sphere.position - closest_point_on_box ) <= ( sphere.radius * sphere.radius );
		}

		boolean_c intersect_box_vs_sphere( box64x3_c const & box, transform3d_c const & box_transform, sphere64_c const & sphere )
		{
			vector64x3_c closest_point_on_box = nearest_point_on_box( sphere.position, box, box_transform );
			return length_squared_float64( sphere.position - closest_point_on_box ) <= ( sphere.radius * sphere.radius );
		}

		boolean_c intersect_box_vs_point( box32x2_c const & box, vector32x2_c const & point )
		{
			return ( point.a >= box.minimum.a && point.a <= box.maximum.a ) && ( point.b >= box.minimum.b && point.b <= box.maximum.b );
		}

		boolean_c intersect_box_vs_point( box64x2_c const & box, vector64x2_c const & point )
		{
			return ( point.a >= box.minimum.a && point.a <= box.maximum.a ) && ( point.b >= box.minimum.b && point.b <= box.maximum.b );
		}

		boolean_c intersect_box_vs_box( box32x2_c const & box_a, box32x2_c const & box_b )
		{
			return ( box_a.maximum.a >= box_b.minimum.a && box_a.minimum.a <= box_b.maximum.a ) && ( box_a.maximum.b >= box_b.minimum.b && box_a.minimum.b <= box_b.maximum.b );
		}

		boolean_c intersect_box_vs_box( box64x2_c const & box_a, box64x2_c const & box_b )
		{
			return ( box_a.maximum.a >= box_b.minimum.a && box_a.minimum.a <= box_b.maximum.a ) && ( box_a.maximum.b >= box_b.minimum.b && box_a.minimum.b <= box_b.maximum.b );
		}

		boolean_c intersect_box_vs_circle( box64x2_c const & box, circle64_c const & circle )
		{
			vector64x2_c nearest_point_on_or_in_box = nearest_point_on_box( circle.position, box );
			return length_squared_float64( circle.position - nearest_point_on_or_in_box ) <= ( circle.radius * circle.radius );
		}

		boolean_c intersect_plane_vs_plane_vs_plane( plane64_c const & plane_a, plane64_c const & plane_b, plane64_c const & plane_c, vector64x3_c * intersection_point )
		{
			if ( plane_a.get_normal() != plane_b.get_normal() || plane_a.get_normal() != plane_c.get_normal() || plane_b.get_normal() != plane_c.get_normal() )
			{
				// http://www.cgafaq.info/wiki/Intersection_of_three_planes
				vector64x3_c numerator = ( cross_product_vector64x3( plane_b.get_normal(), plane_c.get_normal() ) * plane_a.get_distance() ) + ( cross_product_vector64x3( plane_c.get_normal(), plane_a.get_normal() ) * plane_b.get_distance() ) + ( cross_product_vector64x3( plane_a.get_normal(), plane_b.get_normal() ) * plane_c.get_distance() );
				vector64x3_c denominator = cross_product_vector64x3( plane_b.get_normal(), plane_c.get_normal() ) * plane_a.get_normal();
				if ( intersection_point )
				{
					* intersection_point = numerator / denominator;
				}
				return true;
			}
			return false;
		}

		boolean_c intersect_segment_vs_box( segment64_c const & segment, box64x3_c const & box )
		{
			// taken from "realtime collision detection"

			// calculate some things.
			vector64x3_c box_center = box.get_center();
			vector64x3_c box_half_extent = box.maximum - box_center;
			vector64x3_c segment_center = ( segment.point_a + segment.point_b ) * 0.5;
			vector64x3_c segment_half_extent = segment.point_b - segment_center;
			vector64x3_c segment_vs_box_delta = segment_center - box_center;

			// try world coordinate axes as separating axes.
			float64_c adx = math_absolute_value( segment_half_extent.a );
			if ( math_absolute_value( segment_center.a ) > box_half_extent.a + adx )
			{
				return false;
			}
			float64_c ady = math_absolute_value( segment_half_extent.b );
			if ( math_absolute_value( segment_center.b ) > box_half_extent.b + ady )
			{
				return false;
			}
			float64_c adz = math_absolute_value( segment_half_extent.c );
			if ( math_absolute_value( segment_center.c ) > box_half_extent.c + adz )
			{
				return false;
			}

			// add an epsilon term to counteract arithetic errors when segment is near parallel to a coordinate axis.
			adx += constants< float32_c >::epsilon();
			ady += constants< float32_c >::epsilon();
			adz += constants< float32_c >::epsilon();

			// try cross products of segment direction vector with coordinate axes.
			if ( math_absolute_value( segment_center.b * segment_half_extent.c - segment_center.c * segment_half_extent.b ) > box_half_extent.b * adz + box_half_extent.c * ady )
			{
				return false;
			}
			if ( math_absolute_value( segment_center.c * segment_half_extent.a - segment_center.a * segment_half_extent.c ) > box_half_extent.a * adz + box_half_extent.c * adx )
			{
				return false;
			}
			if ( math_absolute_value( segment_center.a * segment_half_extent.b - segment_center.b * segment_half_extent.a ) > box_half_extent.a * ady + box_half_extent.b * adx )
			{
				return false;
			}

			return true;
		}

		void_c project_polygon( polygon32x2_c const & polygon, vector32x2_c const & axis, float32_c & result_minimum, float32_c & result_maximum )
		{
			assert( polygon.points_count > 0 && polygon.points_count <= 8 );
			float32_c d = dot_product_float32( axis, polygon.points[ 0 ] );
			result_minimum = d;
			result_maximum = d;
			for ( sint32_c i = 1; i < polygon.points_count; i++ )
			{
				d = dot_product_float32( axis, polygon.points[ i ] );
				if ( d < result_minimum )
				{
					result_minimum = d;
				}
				else if ( d > result_maximum )
				{
					result_maximum = d;
				}
			}
		}

		float32_c interval_distance( float32_c minimum_a, float32_c maximum_a, float32_c minimum_b, float32_c maximum_b )
		{
			if ( minimum_a < minimum_b )
			{
				return minimum_b - maximum_a;
			}
			else
			{
				return minimum_a - maximum_b;
			}
		}

		boolean_c intersect_point_vs_polygon( vector32x2_c const & point, polygon32x2_c const & polygon )
		{
			for ( sint32_c i = 0; i < polygon.points_count; i++ )
			{
				line32_c edge_line = polygon.get_edge_line( i );
				if ( distance_between_point_and_line( point, edge_line ) > 0.0f )
				{
					return false;
				}
			}
			return true;
		}

		sint32_c intersect_polygon_vs_polygon( polygon32x2_c const & polygon_a, polygon32x2_c const & polygon_b )
		{
			// check for full separation of polygon_b with polygon_a.
			for ( sint32_c i = 0; i < polygon_a.points_count + polygon_b.points_count; i++ )
			{
				vector32x2_c edge_vector;
				if ( i < polygon_a.points_count )
				{
					edge_vector = polygon_a.get_edge_vector( i );
				}
				else
				{
					edge_vector = polygon_b.get_edge_vector( i - polygon_a.points_count );
				}
				vector32x2_c axis = normal_vector32x2( vector32x2_c( -edge_vector.b, edge_vector.a ) ); // normal of edge, tangent to edge.
				float32_c minimum_a;
				float32_c maximum_a;
				float32_c minimum_b;
				float32_c maximum_b;
				project_polygon( polygon_a, axis, minimum_a, maximum_a );
				project_polygon( polygon_b, axis, minimum_b, maximum_b );
				if ( interval_distance( minimum_a, maximum_a, minimum_b, maximum_b ) > 0.0f )
				{
					return 0;
				}
			}

			// at this point we know that there is for sure intersection.
			// classify if it's partial or full.
			for ( sint32_c i = 0; i < polygon_b.points_count; i++ )
			{
				if ( !intersect_point_vs_polygon( polygon_b.points[ i ], polygon_a ) )
				{
					// containment of polygon_b within polygon_a is partial.
					return 1;
				}
			}

			// containment of polygon_b within polygon_a is full.
			return 2;
		}

		boolean_c sweep_ray_vs_plane( ray64_c const & ray, plane64_c const & plane, float64_c & t )
		{
			float64_c cosine = dot_product_float64( ray.normal, plane.get_normal() );
			if ( cosine < 0.0 )
			{
				float64_c distance_above = distance_between_point_and_plane( ray.position, plane );
				if ( distance_above > 0.0 )
				{
					t = distance_above / -cosine;
					return true;
				}
			}
			return false;
		}

		boolean_c sweep_ray_vs_box( ray64_c const & ray, box64x3_c const & box, float64_c & t_minimum, float64_c & t_maximum )
		{
			t_minimum = 0.0;
			t_maximum = constants< float64_c >::maximum();
			for ( sint32_c i = 0; i < 3; i++ )
			{
				if ( math_absolute_value( ray.normal.get_element_at_index( i ) ) < constants< float64_c >::epsilon() )
				{
					if ( ray.position.get_element_at_index( i ) < box.minimum.get_element_at_index( i ) || ray.position.get_element_at_index( i ) > box.maximum.get_element_at_index( i ) )
					{
						return false;
					}
				}
				else
				{
					float64_c t1 = ( box.minimum.get_element_at_index( i ) - ray.position.get_element_at_index( i ) ) / ray.normal.get_element_at_index( i );
					float64_c t2 = ( box.maximum.get_element_at_index( i ) - ray.position.get_element_at_index( i ) ) / ray.normal.get_element_at_index( i );
					if ( t1 > t2 )
					{
						float64_c temp = t1;
						t1 = t2;
						t2 = temp;
					}
					t_minimum = math_maximum( t_minimum, t1 );
					t_maximum = math_minimum( t_maximum, t2 );
					if ( t_minimum > t_maximum )
					{
						return false;
					}
				}
			}
			return true;
		}

		boolean_c sweep_point_vs_plane( segment64_c const & point_path, plane64_c const & plane, float64_c & t )
		{
			vector64x3_c ab = point_path.point_b - point_path.point_a;
			t = ( plane.d - dot_product_float64( plane.get_normal(), point_path.point_a ) ) / dot_product_float64( plane.get_normal(), ab );
			if ( t >= 0.0 && t <= 1.0 )
			{
				return true;
			}
			return false;
		}

		boolean_c sweep_point_vs_sphere( segment64_c const & point_path, sphere64_c const & sphere, float64_c & t )
		{
			vector64x3_c m = point_path.point_a - sphere.position;
			float64_c b = dot_product_float64( m, point_path.point_b - point_path.point_a );
			float64_c c = dot_product_float64( m, m ) - sphere.radius * sphere.radius;

			// exit if r's origin is outside s (c > 0) and r pointing away from s (b > 0)
			if ( c > 0.0 && b > 0.0 )
			{
				return false;
			}

			// a negative discriminant corresponds to ray64_c missing sphere64_c
			float64_c discriminant = b * b - c;
			if ( discriminant < 0.0 )
			{
				return false;
			}

			// ray64_c now found to intersect sphere64_c, compute smallest t value of intersection
			t = -b - math_square_root( discriminant );

			// if t is negative then ray64_c started inside of sphere64_c so clamp to zero.
			if ( t < 0.0 )
			{
				t = 0.0;
			}

			return true;
		}

		boolean_c sweep_point_vs_capsule( segment64_c const & point_path, capsule64_c const & capsule, float64_c & t )
		{
			vector64x3_c sphere_path_vector = capsule.point_b - capsule.point_a;
			vector64x3_c point_to_sphere = point_path.point_a - capsule.point_a;
			vector64x3_c point_path_vector = point_path.point_b - point_path.point_a;
			float64_c a = dot_product_float64( point_to_sphere, sphere_path_vector );
			float64_c b = dot_product_float64( point_path_vector, sphere_path_vector );
			float64_c c = dot_product_float64( sphere_path_vector, sphere_path_vector );

			// test if swept point (segment64_c) is fully outside either end swept sphere64_c (capsule)
			if ( a < 0.0 && a + b < 0.0 )
			{
				// swept point (segment64_c) lies past end a of swept sphere64_c (capsule)
				return sweep_point_vs_sphere( point_path, sphere64_c( capsule.point_a, capsule.radius_a ), t );
			}
			else if ( a > c && a + b > c )
			{
				// swept point (segment64_c) lies past end b of swept sphere64_c (capsule)
				return sweep_point_vs_sphere( point_path, sphere64_c( capsule.point_b, capsule.radius_a ), t );
			}

			float64_c d = dot_product_float64( point_path_vector, point_path_vector );
			float64_c e = dot_product_float64( point_to_sphere, point_path_vector );
			float64_c f = c * d - b * b;
			float64_c g = dot_product_float64( point_to_sphere, point_to_sphere ) - capsule.radius_a * capsule.radius_a;
			float64_c h = c * g - a * a;
			if ( math_absolute_value( f ) < constants< float64_c >::value_near_zero() )
			{
				// swept point runs parallel to swept sphere64_c
				if ( h > 0.0 )
				{
					return false;
				}
				else if ( a < 0.0 )
				{
					t = -e / d;
				}
				else if ( a > c )
				{
					t = ( b - a ) / e;
				}
				else
				{
					t = 0.0; // point_path.a lies inside of cylinder.
				}
				return true;
			}

			float64_c i = c * e - f * h;
			float64_c discriminant = i * i - f * h;
			if ( discriminant < 0.0 )
			{
				// no real roots, no intersection
				return false;
			}

			t = ( -i - math_square_root( discriminant ) ) / f;
			if ( a + t * b < 0.0 )
			{
				return sweep_point_vs_sphere( point_path, sphere64_c( capsule.point_a, capsule.radius_a ), t );
			}
			else if ( a + t * b > c )
			{
				return sweep_point_vs_sphere( point_path, sphere64_c( capsule.point_b, capsule.radius_a ), t );
			}

			return ( t >= 0.0 ) && ( t <= 1.0 );
		}

		boolean_c sweep_point_vs_box( segment64_c const & point_path, box64x3_c const & box, float64_c & t )
		{
			// Smits' method

			// taken from:
			// An Efficient and Robust ray64_c-CBox3 Intersection Algorithm
			// Amy Williams
			// Steve Barrus
			// R. Keith Morley
			// Peter Shirley
			// University of Utah

			vector64x3_c point_path_vector = point_path.point_b - point_path.point_a;

			float64_c t_minimum;	// entrance percent.
			float64_c t_maximum;	// exit percent.

			float64_c t_minimum_y;
			float64_c t_maximum_y;

			float64_c t_minimum_z;
			float64_c t_maximum_z;

			if ( point_path_vector.a > constants< float64_c >::division_near_zero() )
			{
				t_minimum = ( box.minimum.a - point_path.point_a.a ) / point_path_vector.a;
				t_maximum = ( box.maximum.a - point_path.point_a.a ) / point_path_vector.a;
			}
			else
			{
				t_minimum = ( box.maximum.a - point_path.point_a.a ) / point_path_vector.a;
				t_maximum = ( box.minimum.a - point_path.point_a.a ) / point_path_vector.a;
			}

			if ( point_path_vector.b > constants< float64_c >::division_near_zero() )
			{
				t_minimum_y = ( box.minimum.b - point_path.point_a.b ) / point_path_vector.b;
				t_maximum_y = ( box.maximum.b - point_path.point_a.b ) / point_path_vector.b;
			}
			else
			{
				t_minimum_y = ( box.maximum.b - point_path.point_a.b ) / point_path_vector.b;
				t_maximum_y = ( box.minimum.b - point_path.point_a.b ) / point_path_vector.b;
			}

			if ( ( t_minimum > t_maximum_y ) || ( t_minimum_y > t_maximum ) )
			{
				return false;
			}

			if ( t_minimum_y > t_minimum )
			{
				t_minimum = t_minimum_y;
			}

			if ( t_maximum_y < t_maximum )
			{
				t_maximum = t_maximum_y;
			}

			if ( point_path_vector.c > constants< float64_c >::division_near_zero() )
			{
				t_minimum_z = ( box.minimum.c - point_path.point_a.c ) / point_path_vector.c;
				t_maximum_z = ( box.maximum.c - point_path.point_a.c ) / point_path_vector.c;
			}
			else
			{
				t_minimum_z = ( box.maximum.c - point_path.point_a.c ) / point_path_vector.c;
				t_maximum_z = ( box.minimum.c - point_path.point_a.c ) / point_path_vector.c;
			}

			if ( ( t_minimum > t_maximum_z ) || ( t_minimum_z > t_maximum ) )
			{
				return false;
			}

			if ( t_minimum_z > t_minimum )
			{
				t_minimum = t_minimum_z;
			}

			if ( t_maximum_z < t_maximum )
			{
				t_maximum = t_maximum_z;
			}

			t = t_minimum;

			return ( ( t_minimum <= 1.0 ) && ( t_maximum >= 0.0 ) );
		}

		boolean_c sweep_point_vs_box( segment64_c const & point_path, box64x3_c const & box, transform3d_c const & box_transform, float64_c & t )
		{
			transform3d_c box_transform_inverted = box_transform.get_inverted();
			segment64_c point_path_local = segment64_c( rotate_scale_and_translate_vector64x3( point_path.point_a, box_transform_inverted ), rotate_scale_and_translate_vector64x3( point_path.point_b, box_transform_inverted ) );
			return sweep_point_vs_box( point_path_local, box, t );
		}

		boolean_c sweep_sphere_vs_box( segment64_c const & sphere_path, float64_c const sphere_radius, box64x3_c const & box, transform3d_c const & box_transform, float64_c & t )
		{
			box64x3_c box_with_margin = box64x3_c( box.minimum.a - sphere_radius, box.minimum.b - sphere_radius, box.minimum.c - sphere_radius, box.maximum.a + sphere_radius, box.maximum.b + sphere_radius, box.maximum.c + sphere_radius );
			transform3d_c box_transform_inverted = box_transform.get_inverted();
			segment64_c sphere_path_local = segment64_c( rotate_scale_and_translate_vector64x3( sphere_path.point_a, box_transform_inverted ), rotate_scale_and_translate_vector64x3( sphere_path.point_b, box_transform_inverted ) );

			if ( !sweep_point_vs_box( sphere_path_local, box_with_margin, t ) || t > 1.0 )
			{
				return false;
			}

			vector64x3_c point = sphere_path_local.point_a + ( ( sphere_path_local.point_b - sphere_path_local.point_a ) * t );
			sint32_c u = 0;
			sint32_c v = 0;
			if ( point.a < box.minimum.a )
			{
				u |= 1;
			}
			else if ( point.a > box.maximum.a )
			{
				v |= 1;
			}
			if ( point.b < box.minimum.b )
			{
				u |= 2;
			}
			else if ( point.b > box.maximum.b )
			{
				v |= 2;
			}
			if ( point.c < box.minimum.c )
			{
				u |= 4;
			}
			else if ( point.c > box.maximum.c )
			{
				v |= 4;
			}
			sint32_c m = u + v;

			if ( m == 7 )
			{
				float64_c t_current = 0.0;
				float64_c t_minimum = constants< float64_c >::maximum();
				if ( sweep_point_vs_capsule( sphere_path_local, capsule64_c( box.get_point( v ), sphere_radius, box.get_point( v ^ 1 ), sphere_radius ), t_current ) )
				{
					t_minimum = math_minimum( t, t_minimum );
				}
				if ( sweep_point_vs_capsule( sphere_path_local, capsule64_c( box.get_point( v ), sphere_radius, box.get_point( v ^ 2 ), sphere_radius ), t_current ) )
				{
					t_minimum = math_minimum( t, t_minimum );
				}
				if ( sweep_point_vs_capsule( sphere_path_local, capsule64_c( box.get_point( v ), sphere_radius, box.get_point( v ^ 4 ), sphere_radius ), t_current ) )
				{
					t_minimum = math_minimum( t, t_minimum );
					if ( t_minimum == constants< float64_c >::maximum() )
					{
						return false;
					}
				}
				t = t_minimum;
				return true;
			}

			// if only one bit is set tin m then p is in a face region.
			if ( ( m && ( m - 1 ) ) == 0 )
			{
				return true;
			}

			// p is in an edge region, intersect against capsule at edge.
			return sweep_point_vs_capsule( sphere_path_local, capsule64_c( box.get_point( u ^ 7 ), sphere_radius, box.get_point( v ), sphere_radius ), t );
		}

		boolean_c sweep_point_vs_triangle( segment64_c const & point_path, triangle64_c const & triangle, float64_c & t )
		{
			float64_c distance_a = distance_between_point_and_plane( point_path.point_a, triangle.face_plane );
			if ( distance_a < 0.0f )
			{
				return false;
			}

			float64_c distance_b = distance_between_point_and_plane( point_path.point_b, triangle.face_plane );
			if ( distance_b >= 0.0f )
			{
				return false;
			}

			t = distance_a / ( distance_a - distance_b );

			vector64x3_c intersection_point = point_path.point_a + ( point_path.point_b - point_path.point_a ) * t;

			float64_c u = dot_product_float64( intersection_point, triangle.edge_plane_bc.get_normal() ) - triangle.edge_plane_bc.get_distance();
			if ( u < 0.0 || u > 1.0 )
			{
				return false;
			}

			float64_c v = dot_product_float64( intersection_point, triangle.edge_plane_ca.get_normal() ) - triangle.edge_plane_ca.get_distance();
			if ( v < 0.0 )
			{
				return false;
			}

			float64_c w = 1.0 - u - v;
			if ( w < 0.0 )
			{
				return false;
			}

			return true;
		}

	}
}
