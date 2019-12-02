#include "cheonsa_glyph_manager.h"
#include "cheonsa_data_stream_memory.h"
#include "cheonsa_data_scribe_binary.h"
#include "cheonsa_resource_file_font.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_SIZES_H

//#include <functional>

// all signed distance field generating portions of this file:
//   make_float64_cross_product_analog
//   solve_quadratic
//   solve_cubic_normed
//   solve_cubic
//   signed_distance_c
//   edge_color_e
//   edge_segment_c
//   edge_segment_cubic_c
//   contour_c
//   shape_c
//   and everything in and around #pragma omp parallel
//
// is code that is adapted/refactored from:

	/*
	 * MULTI-CHANNEL SIGNED DISTANCE FIELD GENERATOR v1.5 (2017-07-23)
	 * ---------------------------------------------------------------
	 * A utility by Viktor Chlumsky, (c) 2014 - 2017
	 *
	 * The technique used to generate multi-channel distance fields in this code
	 * has been developed by Viktor Chlumsky in 2014 for his master's thesis,
	 * "Shape Decomposition for Multi-Channel Distance Fields". It provides improved
	 * quality of sharp corners in glyphs and other 2D shapes in comparison to monochrome
	 * distance fields. To reconstruct an image of the shape, apply the median of three
	 * operation on the triplet of sampled distance field values.
	 *
	 */

#include "cheonsa_menu_element_text.h"

namespace cheonsa
{

	float64_c make_float64_cross_product_analog( vector64x2_c const & vector_a, vector64x2_c const & vector_b )
	{
		return ( vector_a.a * vector_b.b ) - ( vector_a.b * vector_b.a );
	}

	sint32_c solve_quadratic( float64_c x[ 2 ], float64_c a, float64_c b, float64_c c )
	{
		if ( ops::math_absolute_value( a ) < 1e-14 )
		{
			if ( ops::math_absolute_value( b ) < 1e-14 )
			{
				if ( c == 0 )
				{
					return -1;
				}
				return 0;
			}
			x[ 0 ] = -c / b;
			return 1;
		}
		float64_c dscr = b * b - 4 * a * c;
		if ( dscr > 0.0 )
		{
			dscr = ops::math_square_root( dscr );
			x[ 0 ] = ( -b + dscr ) / ( 2 * a );
			x[ 1 ] = ( -b - dscr ) / ( 2 * a );
			return 2;
		}
		else if ( dscr == 0 )
		{
			x[ 0 ] = -b / ( 2 * a );
			return 1;
		}
		else
		{
			return 0;
		}
	}

	sint32_c solve_cubic_normed( double * x, double a, double b, double c )
	{
		float64_c a2 = a * a;
		float64_c q  = ( a2 - 3.0 * b) / 9.0; 
		float64_c r  = ( a * ( 2.0 * a2 - 9.0 * b ) + 27.0 * c ) / 54.0;
		float64_c r2 = r * r;
		float64_c q3 = q * q * q;
		float64_c A;
		float64_c B;
		if ( r2 < q3 )
		{
			float64_c t = r / ops::math_square_root( q3 );
			if ( t < -1.0 )
			{
				t = -1.0;
			}
			else if ( t > 1.0 )
			{
				t = 1.0;
			}
			t = ops::math_arc_cosine( t );
			a /= 3.0;
			q = -2.0 * ops::math_square_root( q );
			x[ 0 ] = q * ops::math_cosine( t / 3.0 ) - a;
			x[ 1 ] = q * ops::math_cosine( ( t + 2.0 * constants< float64_c >::pi() ) / 3.0 ) - a;
			x[ 2 ] = q * ops::math_cosine( ( t - 2.0 * constants< float64_c >::pi() ) / 3.0 ) - a;
			return 3;
		}
		else
		{
			A = -ops::math_power( ops::math_absolute_value( r ) + ops::math_square_root( r2 - q3 ), 1.0 / 3.0 );
			if ( r < 0.0 )
			{
				A = -A;
			}
			B = A == 0 ? 0 : q / A;
			a /= 3;
			x[ 0 ] = ( A + B ) - a;
			x[ 1 ] = -0.5 * ( A + B ) - a;
			x[ 2 ] = 0.5 * ops::math_square_root( 3.0 ) * ( A - B );
			if ( ops::math_absolute_value( x[ 2 ] ) < 1e-14 )
			{
				return 2;
			}
			return 1;
		}
	}

	sint32_c solve_cubic( float64_c x[ 3 ], float64_c a, float64_c b, float64_c c, float64_c d )
	{
		if ( ops::math_absolute_value( a ) < 1e-14 )
		{
			return solve_quadratic( x, b, c, d );
		}
		return solve_cubic_normed( x, b / a, c / a, d / a );
	}

	class signed_distance_c
	{
	public:
		static signed_distance_c infinite;

	public:
		float64_c distance;
		float64_c dot;

	public:
		signed_distance_c()
			: distance( 0.0 )
			, dot( 0.0 )
		{
		}

		signed_distance_c( float64_c distance, float64_c dot )
			: distance( distance )
			, dot( dot )
		{
		}

	};

	signed_distance_c signed_distance_c::infinite( -1e240, 1 );

	boolean_c operator < ( signed_distance_c const & a, signed_distance_c const & b )
	{
		return ( ops::math_absolute_value( a.distance ) < ops::math_absolute_value( b.distance ) ) || ( ops::math_absolute_value( a.distance ) == ops::math_absolute_value( b.distance ) && a.dot < b.dot );
	}

	boolean_c operator > ( signed_distance_c const & a, signed_distance_c const & b )
	{
		return ( ops::math_absolute_value( a. distance ) > ops::math_absolute_value( b.distance ) ) || ( ops::math_absolute_value( a.distance ) == ops::math_absolute_value( b.distance ) && a.dot > b.dot );
	}

	boolean_c operator <= ( signed_distance_c const & a, signed_distance_c const & b )
	{
		return ( ops::math_absolute_value( a.distance ) < ops::math_absolute_value( b.distance ) ) || ( ops::math_absolute_value( a.distance ) == ops::math_absolute_value( b.distance ) && a.dot <= b.dot );
	}

	boolean_c operator >= ( signed_distance_c const & a, signed_distance_c const & b )
	{
		return ( ops::math_absolute_value( a.distance ) > ops::math_absolute_value( b.distance ) ) || ( ops::math_absolute_value( a.distance ) == ops::math_absolute_value( b.distance ) && a.dot >= b.dot );
	}

	enum edge_color_e
	{
		edge_color_e_black = 0,
		edge_color_e_red = 1,
		edge_color_e_green = 2,
		edge_color_e_yellow = 3,
		edge_color_e_blue = 4,
		edge_color_e_magenta = 5,
		edge_color_e_cyan = 6,
		edge_color_e_white = 7
	};

	class edge_segment_c
	{
	public:
		edge_color_e edge_color;

	public:
		edge_segment_c( edge_color_e edge_color )
			: edge_color( edge_color )
		{
		}

		virtual ~edge_segment_c()
		{
		}

		virtual edge_segment_c * clone() const = 0;

		// finds the point on the edge specified by t between 0 and 1.
		virtual vector64x2_c find_point( float64_c t ) const = 0;

		// finds the normalized direction of the edge specified by t between 0 and 1.
		virtual vector64x2_c find_direction( float64_c t ) const = 0;

		// finds the distance between origin point and the point on the edge specified by t between 0 and 1.
		virtual signed_distance_c find_singed_distance( vector64x2_c const & origin, float64_c & t ) const = 0;

		// converts a previously retrieved signed distance from origin to pseudo-distance.
		void_c convert_signed_distance_to_signed_psuedo_distance( signed_distance_c & signed_distance, vector64x2_c origin, float64_c t ) const
		{
			if ( t < 0.0 )
			{
				vector64x2_c direction = find_direction( 0.0 );
				vector64x2_c aq = origin - find_point( 0.0 );
				float64_c ts = ops::make_float64_dot_product( aq, direction );
				if ( ts < 0.0 )
				{
					float64_c pseudo_distance = make_float64_cross_product_analog( aq, direction );
					if ( ops::math_absolute_value( pseudo_distance ) < ops::math_absolute_value( signed_distance.distance ) )
					{
						signed_distance.distance = pseudo_distance;
						signed_distance.dot = 0.0;
					}
				}
			}
			else if ( t > 1.0 )
			{
				vector64x2_c direction = find_direction( 1.0 );
				vector64x2_c bq = origin - find_point( 1.0 );
				float64_c ts = ops::make_float64_dot_product( bq, direction );
				if ( ts > 0.0 )
				{
					float64_c pseudo_distance = make_float64_cross_product_analog( bq, direction );
					if ( ops::math_absolute_value( pseudo_distance ) <= ops::math_absolute_value( signed_distance.distance ) )
					{
						signed_distance.distance = pseudo_distance;
						signed_distance.dot = 0.0;
					}
				}
			}
		}

		static void_c find_bounds_for_point( box64x2_c & result, vector64x2_c const & point )
		{
			if ( point.a < result.minimum.a )
			{
				result.minimum.a = point.a;
			}
			if ( point.a > result.maximum.a )
			{
				result.maximum.a = point.a;
			}
			if ( point.b < result.minimum.b )
			{
				result.minimum.b = point.b;
			}
			if ( point.b > result.maximum.b )
			{
				result.maximum.b = point.b;
			}
		}

		virtual void_c find_bounds( box64x2_c & result ) const = 0;

		// moves start point of edge segment.
		virtual void_c move_start_point( vector64x2_c const & to ) = 0;
		// moves end point of edge segment.
		virtual void_c move_end_point( vector64x2_c const & to ) = 0;
		// splits this segment edge segment into three more segments, which basically increases the resolution of the shape. allocates new instances of edge segments on the heap and appends them to the output, so be sure to delete them when you're done with them.
		virtual void_c subdivide_into_thirds( edge_segment_c * * output ) const = 0;

	};

	class edge_segment_linear_c
		: public edge_segment_c
	{
	public:
		vector64x2_c points[ 2 ];

	public:
		edge_segment_linear_c( vector64x2_c const & point_0, vector64x2_c const & point_1, edge_color_e edge_color )
			: edge_segment_c( edge_color )
			, points()
		{
			points[ 0 ] = point_0;
			points[ 1 ] = point_1;
		}

		virtual edge_segment_linear_c * clone() const override
		{
			return new edge_segment_linear_c( points[ 0 ], points[ 1 ], edge_color );
		}

		virtual vector64x2_c find_point( float64_c t ) const override
		{
			return ops::interpolate_linear( points[ 0 ], points[ 1 ], t );
		}

		virtual vector64x2_c find_direction( float64_c t ) const override
		{
			return ops::make_vector64x2_normalized( points[ 1 ] - points[ 0 ] );
		}

		virtual signed_distance_c find_singed_distance( vector64x2_c const & origin, float64_c & t ) const override
		{
			vector64x2_c aq = origin - points[ 0 ];
			vector64x2_c ab = points[ 1 ] - points[ 0 ];
			t = ops::make_float64_dot_product( aq, ab ) / ops::make_float64_dot_product( ab, ab );
			vector64x2_c eq = points[ t > 0.5 ] - origin;
			float64_c end_point_distance = ops::make_float64_length( eq );
			if ( t > 0.0 && t < 1.0 )
			{
				float64_c ortho_distance = ops::make_float64_dot_product( ops::make_vector64x2_orthonormal( ab, false, false ), aq );
				if ( ops::math_absolute_value( ortho_distance ) < end_point_distance )
				{
					return signed_distance_c( ortho_distance, 0.0 );
				}
			}
			return signed_distance_c( ops::math_non_zero_sign( make_float64_cross_product_analog( aq, ab ) ) * end_point_distance, ops::math_absolute_value( ops::make_float64_dot_product( ops::make_vector64x2_normalized( ab ), ops::make_vector64x2_normalized( eq ) ) ) );
		}

		virtual void_c find_bounds( box64x2_c & result ) const override
		{
			find_bounds_for_point( result, points[ 0 ] );
			find_bounds_for_point( result, points[ 1 ] );
		}

		virtual void_c move_start_point( vector64x2_c const & to ) override
		{
			points[ 0 ] = to;
		}

		virtual void_c move_end_point( vector64x2_c const & to ) override
		{
			points[ 1 ] = to;
		}

		virtual void_c subdivide_into_thirds( edge_segment_c * * output ) const override
		{
			vector64x2_c point_1_3 = find_point( 1.0 / 3.0 );
			vector64x2_c point_2_3 = find_point( 2.0 / 3.0 );

			output[ 0 ] = new edge_segment_linear_c( points[ 0 ], point_1_3, edge_color );
			output[ 1 ] = new edge_segment_linear_c( point_1_3, point_2_3, edge_color );
			output[ 2 ] = new edge_segment_linear_c( point_2_3, points[ 1 ], edge_color );
		}

	};

	class edge_segment_quadratic_c
		: public edge_segment_c
	{
	public:
		vector64x2_c points[ 3 ];

	public:
		edge_segment_quadratic_c( vector64x2_c const & point_0, vector64x2_c const & point_1, vector64x2_c const & point_2, edge_color_e edge_color )
			: edge_segment_c( edge_color )
			, points()
		{
			points[ 0 ] = point_0;
			points[ 1 ] = point_1;
			points[ 2 ] = point_2;
		}

		virtual edge_segment_c * clone() const override
		{
			return new edge_segment_quadratic_c( points[ 0 ], points[ 1 ], points[ 2 ], edge_color );
		}

		virtual vector64x2_c find_point( float64_c t ) const override
		{
			return ops::interpolate_linear( ops::interpolate_linear( points[ 0 ], points[ 1 ], t ), ops::interpolate_linear( points[ 1 ], points[ 2 ], t ), t );
		}

		virtual vector64x2_c find_direction( float64_c t ) const override
		{
			return ops::interpolate_linear( points[ 1 ] - points[ 0 ], points[ 2 ] - points[ 1 ], t );
		}

		virtual signed_distance_c find_singed_distance( vector64x2_c const & origin, float64_c & t ) const override
		{
			vector64x2_c qa = points[ 0 ] - origin;
			vector64x2_c ab = points[ 1 ] - points[ 0 ];
			vector64x2_c br = points[ 0 ] + points[ 2 ] - points[ 1 ] - points[ 1 ];
			float64_c a = ops::make_float64_dot_product( br, br );
			float64_c b = 3 * ops::make_float64_dot_product( ab, br );
			float64_c c = 2 * ops::make_float64_dot_product( ab, ab ) + ops::make_float64_dot_product( qa, br );
			float64_c d = ops::make_float64_dot_product( qa, ab );
			float64_c x[ 3 ];
			sint32_c solutions = solve_cubic( x, a, b, c, d );

			float64_c minimum_distance = ops::math_non_zero_sign( make_float64_cross_product_analog( ab, qa ) ) * ops::make_float64_length( qa ); // distance from A
			t = -ops::make_float64_dot_product( qa, ab ) / ops::make_float64_dot_product( ab, ab );
			{
				float64_c distance = ops::math_non_zero_sign( make_float64_cross_product_analog( points[ 2 ] - points[ 1 ], points[ 2 ] - origin ) ) * ops::make_float64_length( points[ 2 ] - origin ); // distance from B
				if ( ops::math_absolute_value( distance ) < ops::math_absolute_value( minimum_distance ) )
				{
					minimum_distance = distance;
					t = ops::make_float64_dot_product( origin - points[ 1 ], points[ 2 ] - points[ 1 ]) / ops::make_float64_dot_product( points[ 2 ] - points[ 1 ], points[ 2 ] - points[ 1 ] );
				}
			}
			for ( sint32_c i = 0; i < solutions; i++ )
			{
				if ( x[ i ] > 0.0 && x[ i ] < 1.0 )
				{
					vector64x2_c end_point = points[ 0 ] + ( ab * ( x[ i ] * 2.0 ) ) + ( br * ( x[ i ] * x[ i ] ) );
					float64_c distance = ops::math_non_zero_sign( make_float64_cross_product_analog( points[ 2 ] - points[ 0 ], end_point - origin ) ) * ops::make_float64_length( end_point - origin );
					if ( ops::math_absolute_value( distance ) <= ops::math_absolute_value( minimum_distance ) )
					{
						minimum_distance = distance;
						t = x[ i ];
					}
				}
			}

			if ( t >= 0.0 && t <= 1.0 )
			{
				return signed_distance_c( minimum_distance, 0.0 );
			}
			if ( t < 0.5 )
			{
				return signed_distance_c( minimum_distance, ops::math_absolute_value( ops::make_float64_dot_product( ops::make_vector64x2_normalized( ab ), ops::make_vector64x2_normalized( qa ) ) ) );
			}
			else
			{
				return signed_distance_c( minimum_distance, ops::math_absolute_value( ops::make_float64_dot_product( ops::make_vector64x2_normalized( points[ 2 ] - points[ 1 ] ), ops::make_vector64x2_normalized( points[ 2 ] - origin) ) ) );
			}
		}

		virtual void_c find_bounds( box64x2_c & result ) const override
		{
			find_bounds_for_point( result, points[ 0 ] );
			find_bounds_for_point( result, points[ 2 ] );
			vector64x2_c bot = ( points[ 1 ] - points[ 0 ] ) - ( points[ 2 ] - points[ 1 ] );
			if ( bot.a )
			{
				float64_c t = ( points[ 1 ].a - points[ 0 ].a ) / bot.a;
				if ( t > 0.0 && t < 1.0 )
				{
					find_bounds_for_point( result, find_point( t ) );
				}
			}
			if ( bot.b )
			{
				float64_c t = ( points[ 1 ].b - points[ 0 ].b ) / bot.b;
				if ( t > 0.0 && t < 1.0 )
				{
					find_bounds_for_point( result, find_point( t ) );
				}
			}
		}

		virtual void_c move_start_point( vector64x2_c const & to ) override
		{
			points[ 0 ] = to;
		}

		virtual void_c move_end_point( vector64x2_c const & to ) override
		{
			points[ 1 ] = to;
		}

		virtual void_c subdivide_into_thirds( edge_segment_c * * output ) const override
		{
			vector64x2_c point_1_3 = find_point( 1.0 / 3.0 );
			vector64x2_c point_2_3 = find_point( 2.0 / 3.0 );

			output[ 0 ] = new edge_segment_quadratic_c( points[ 0 ], ops::interpolate_linear( points[ 0 ], points[ 1 ], 1.0 / 3.0 ), point_1_3, edge_color );
			output[ 1 ] = new edge_segment_quadratic_c( point_1_3, ops::interpolate_linear( ops::interpolate_linear( points[ 0 ], points[ 1 ], 5.0 / 9.0 ), ops::interpolate_linear( points[ 1 ], points[ 2 ], 4.0 / 9.0 ), 0.5 ), point_2_3, edge_color);
			output[ 2 ] = new edge_segment_quadratic_c( point_2_3, ops::interpolate_linear( points[ 1 ], points[ 2 ], 2.0 / 3.0 ), points[ 2 ], edge_color );
		}

	};

	class edge_segment_cubic_c
		: public edge_segment_c
	{
	public:
		vector64x2_c points[ 4 ];

	public:
		edge_segment_cubic_c( vector64x2_c const & point_0, vector64x2_c const & point_1, vector64x2_c const & point_2, vector64x2_c const & point_3, edge_color_e edge_color )
			: edge_segment_c( edge_color )
			, points()
		{
			points[ 0 ] = point_0;
			points[ 1 ] = point_1;
			points[ 2 ] = point_2;
			points[ 3 ] = point_3;
		}

		virtual edge_segment_c * clone() const override
		{
			return new edge_segment_cubic_c( points[ 0 ], points[ 1 ], points[ 2 ], points[ 3 ], edge_color );
		}

		virtual vector64x2_c find_point( float64_c t ) const override
		{
			vector64x2_c point_1_2 = ops::interpolate_linear( points[ 1 ], points[ 2 ], t );
			return ops::interpolate_linear( ops::interpolate_linear( ops::interpolate_linear( points[ 0 ], points[ 1 ], t ), point_1_2, t ), ops::interpolate_linear( point_1_2, ops::interpolate_linear( points[ 2 ], points[ 3 ], t ), t ), t );
		}

		virtual vector64x2_c find_direction( float64_c t ) const override
		{
			vector64x2_c tangent = ops::interpolate_linear( ops::interpolate_linear( points[ 1 ] - points[ 0 ], points[ 2 ] - points[ 1 ], t ), ops::interpolate_linear( points[ 2 ] - points[ 1 ], points[ 3 ] - points[ 2 ], t ), t);
			if ( tangent.a == 0.0 && tangent.b == 0.0 )
			{
				if ( t == 0.0 )
				{
					return points[ 2 ] - points[ 0 ];
				}
				else if ( t == 1.0 )
				{
					return points[ 3 ] - points[ 1 ];
				}
			}
			return tangent;
		}

		virtual signed_distance_c find_singed_distance( vector64x2_c const & origin, float64_c & t ) const override
		{
			vector64x2_c qa = points[ 0 ] - origin;
			vector64x2_c ab = points[ 1 ] - points[ 0 ];
			vector64x2_c br = points[ 2 ] - points[ 1 ] - ab;
			vector64x2_c as = ( points[ 3 ] - points[ 2 ] ) - ( points[ 2 ] - points[ 1 ] ) - br;

			vector64x2_c ep_direction = find_direction( 0.0 );
			float64_c minimum_distance = ops::math_non_zero_sign( make_float64_cross_product_analog( ep_direction, qa ) ) * ops::make_float64_length( qa ); // distance from A
			t = -ops::make_float64_dot_product( qa, ep_direction ) / ops::make_float64_dot_product( ep_direction, ep_direction );
			{
				ep_direction = find_direction( 1.0 );
				float64_c distance = ops::math_non_zero_sign( make_float64_cross_product_analog( ep_direction, points[ 3 ] - origin ) ) * ops::make_float64_length( points[ 3 ] - origin ); // distance from B
				if ( ops::math_absolute_value( distance ) < ops::math_absolute_value( minimum_distance ) )
				{
					minimum_distance = distance;
					t = ops::make_float64_dot_product( origin + ep_direction - points[ 3 ], ep_direction ) / ops::make_float64_dot_product( ep_direction, ep_direction );
				}
			}

			// iterative minimum distance search.
			sint32_c cubic_search_starts = 4;
			sint32_c cubic_search_steps = 4;
			for ( sint32_c i = 0; i <= cubic_search_starts; i++ )
			{
				float64_c t2 = static_cast< float64_c >( i / cubic_search_starts );
				for ( sint32_c step = 0; ; step++ )
				{
					vector64x2_c qpt = find_point( t2 ) - origin;
					double distance = ops::math_non_zero_sign( make_float64_cross_product_analog( find_direction( t2 ), qpt ) ) * ops::make_float64_length( qpt );
					if ( ops::math_absolute_value( distance ) < ops::math_absolute_value( minimum_distance ) )
					{
						minimum_distance = distance;
						t = t2;
					}
					if ( step == cubic_search_steps )
					{
						break;
					}
					// improve t2.
					vector64x2_c d1 = ( as * ( 3.0 * t2 * t2 ) ) + ( br * ( 6.0 * t2 ) ) + ( ab * 3.0 );
					vector64x2_c d2 = ( as * ( 6.0 * t2 ) ) + ( br * 6.0 );
					t2 -= ops::make_float64_dot_product( qpt, d1 ) / ( ops::make_float64_dot_product( d1, d1 ) + ops::make_float64_dot_product( qpt, d2 ) );
					if ( t2 < 0.0 || t2 > 1.0 )
					{
						break;
					}
				}
			}

			if ( t >= 0.0 && t <= 1.0 )
			{
				return signed_distance_c( minimum_distance, 0.0 );
			}
			if ( t < 0.5 )
			{
				return signed_distance_c( minimum_distance, ops::math_absolute_value( ops::make_float64_dot_product( find_direction( 0.0 ), ops::make_vector64x2_normalized( qa ) ) ) );
			}
			else
			{
				return signed_distance_c( minimum_distance, ops::math_absolute_value( ops::make_float64_dot_product( find_direction( 1.0 ), ops::make_vector64x2_normalized( points[ 3 ] - origin ) ) ) );
			}
		}

		virtual void_c find_bounds( box64x2_c & result ) const override
		{
			find_bounds_for_point( result, points[ 0 ] );
			find_bounds_for_point( result, points[ 3 ] );
			vector64x2_c a0 = points[ 1 ] - points[ 0 ];
			vector64x2_c a1 = ( points[ 2 ] - points[ 1 ] - a0 ) * 2.0;
			vector64x2_c a2 = points[ 3 ] - ( points[ 2 ] * 3.0 ) + ( points[ 1 ] * 3.0 ) - points[ 0 ];
			float64_c x[ 2 ];
			sint32_c solutions;
			solutions = solve_quadratic( x, a2.a, a1.a, a0.a );
			for ( sint32_c i = 0; i < solutions; i++ )
			{
				if ( x[ i ] > 0.0 && x[ i ] < 1.0 )
				{
					find_bounds_for_point( result, find_point( x[ i ] ) );
				}
			}
			solutions = solve_quadratic( x, a2.b, a1.b, a0.b );
			for ( sint32_c i = 0; i < solutions; i++ )
			{
				if ( x[ i ] > 0.0 && x[ i ] < 1.0 )
				{
					find_bounds_for_point( result, find_point( x[ i ] ) );
				}
			}
		}

		virtual void_c move_start_point( vector64x2_c const & to ) override
		{
			vector64x2_c original_start_direction = points[ 0 ] - points[ 1 ];
			vector64x2_c original_point_1 = points[ 1 ];
			points[ 1 ] += ( points[ 2 ] - points[ 1 ] ) * ( make_float64_cross_product_analog( points[ 0 ] - points[ 1 ], to - points[ 0 ] ) / make_float64_cross_product_analog( points[ 0 ] - points[ 1 ], points[ 2 ] - points[ 1 ] ) );
			points[ 0 ] = to;
			if ( ops::make_float64_dot_product( original_start_direction, points[ 0 ] - points[ 1 ] ) < 0.0 )
			{
				points[ 1 ] = original_point_1;
			}
		}

		virtual void_c move_end_point( vector64x2_c const & to ) override
		{
			points[ 2 ] += to - points[ 3 ];
			points[ 3 ] = to;
		}

		virtual void_c subdivide_into_thirds( edge_segment_c * * output ) const override
		{
			output[ 0 ] = new edge_segment_cubic_c( points[ 0 ], points[ 0 ] == points[ 1 ] ? points[ 0 ] : ops::interpolate_linear( points[ 0 ], points[ 1 ], 1.0 / 3.0 ), ops::interpolate_linear( ops::interpolate_linear( points[ 0 ], points[ 1 ], 1.0 / 3.0 ), ops::interpolate_linear( points[ 1 ], points[ 2 ], 1.0 / 3.0 ), 1.0 / 3.0 ), find_point( 1.0 / 3.0 ), edge_color );
			output[ 1 ] = new edge_segment_cubic_c( find_point( 1.0 / 3.0 ), ops::interpolate_linear( ops::interpolate_linear( ops::interpolate_linear( points[ 0 ], points[ 1 ], 1.0 / 3.0 ), ops::interpolate_linear( points[ 1 ], points[ 2 ], 1.0 / 3.0 ), 1.0 / 3.0 ), ops::interpolate_linear( ops::interpolate_linear( points[ 1 ], points[ 2 ], 1.0 / 3.0 ), ops::interpolate_linear( points[ 2 ], points[ 3 ], 1.0 / 3.0 ), 1.0 / 3.0 ), 2.0 / 3.0 ), ops::interpolate_linear( ops::interpolate_linear( ops::interpolate_linear( points[ 0 ], points[ 1 ], 2.0 / 3.0 ), ops::interpolate_linear( points[ 1 ], points[ 2 ], 2.0 / 3.0 ), 2.0 / 3.0 ), ops::interpolate_linear( ops::interpolate_linear( points[ 1 ], points[ 2 ], 2.0 / 3.0 ), ops::interpolate_linear( points[ 2 ], points[ 3 ], 2.0 / 3.0 ), 2.0 / 3.0 ), 1.0 / 3.0 ), find_point( 2.0 / 3.0 ), edge_color );
			output[ 2 ] = new edge_segment_cubic_c( find_point( 2.0 / 3.0 ), ops::interpolate_linear( ops::interpolate_linear( points[ 1 ], points[ 2 ], 2.0 / 3.0 ), ops::interpolate_linear( points[ 2 ], points[ 3 ], 2.0 / 3.0 ), 2.0 / 3.0 ), points[ 2 ] == points[ 3 ] ? points[ 3 ] : ops::interpolate_linear( points[ 2 ], points[ 3 ], 2.0 / 3.0 ), points[ 3 ], edge_color );
		}

	};

	class contour_c
	{
	public:
		core_list_c< edge_segment_c * > edge_segments;

	public:
		contour_c()
			: edge_segments()
		{
		}

		~contour_c()
		{
			for ( sint32_c i = 0; i < edge_segments.get_length(); i++ )
			{
				delete edge_segments[ i ];
			}
		}

		// adds an edge holder.
		void_c add_edge_segment( edge_segment_c * edge_segment )
		{
			edge_segments.insert_at_end( edge_segment );
		}

		//// adds a blank edge holder and returns its reference.
		//edge_segment_c * & add_edge_segment()
		//{
		//	return *edge_segments.emplace_at_end();
		//}

		// calculates the bounds of the contour.
		void_c find_bounds( box64x2_c & result ) const
		{
			for ( sint32_c i = 0; i < edge_segments.get_length(); i++ )
			{
				edge_segments[ i ]->find_bounds( result );
			}
		}

		static float64_c shoelace( vector64x2_c const & point_a, vector64x2_c const & point_b )
		{
			return ( point_b.a - point_a.a ) * ( point_a.b + point_b.b );
		}

		// calculates the winding of the contour, returns 1 if positive, -1 if negative.
		sint32_c find_winding() const
		{
			if ( edge_segments.get_length() == 0 )
			{
				return 0;
			}

			float64_c total = 0.0;
			if ( edge_segments.get_length() == 1 )
			{
				vector64x2_c a = edge_segments[ 0 ]->find_point( 0.0 );
				vector64x2_c b = edge_segments[ 0 ]->find_point( 1.0 / 3.0 );
				vector64x2_c c = edge_segments[ 0 ]->find_point( 2.0 / 3.0 );
				total += shoelace( a, b );
				total += shoelace( b, c );
				total += shoelace( c, a );
			}
			else if ( edge_segments.get_length() == 2 )
			{
				vector64x2_c a = edge_segments[ 0 ]->find_point( 0.0 );
				vector64x2_c b = edge_segments[ 0 ]->find_point( 0.5 );
				vector64x2_c c = edge_segments[ 1 ]->find_point( 0.0 );
				vector64x2_c d = edge_segments[ 1 ]->find_point( 0.5 );
				total += shoelace( a, b );
				total += shoelace( b, c );
				total += shoelace( c, d );
				total += shoelace( d, a );
			}
			else
			{
				vector64x2_c previous_point = edge_segments[ edge_segments.get_length() - 1 ]->find_point( 0.0 );
				for ( sint32_c i = 0; i < edge_segments.get_length(); i++ )
				{
					vector64x2_c current_point = edge_segments[ i ]->find_point( 0.0 );
					total += shoelace( previous_point, current_point );
					previous_point = current_point;
				}
			}

			return ( 0.0 < total ) - ( total < 0.0 );
		}
	};

	class shape_c
	{
	public:
		core_list_c< contour_c * > contours;

	public:
		shape_c()
			: contours()
		{
		}

		~shape_c()
		{
			for ( sint32_c i = 0; i < contours.get_length(); i++ )
			{
				delete contours[ i ];
			}
		}

		// increasses resolution of contours by 3, i don't know what this accomplishes, maybe it reduces errors in multi-channel signed distance field generation... which we don't plan to use so maybe we can skip using this.
		void_c subdivide_into_thirds()
		{
			for ( sint32_c i = 0; i < contours.get_length(); i++ )
			{
				contour_c * contour = contours[ i ];
				if ( contour->edge_segments.get_length() == 1 )
				{
					edge_segment_c * original_edge_segment = contour->edge_segments[ 0 ];
					contour->edge_segments.construct_mode_dynamic( 3, 3 );
					original_edge_segment->subdivide_into_thirds( contour->edge_segments.get_internal_array() );
					delete original_edge_segment;
				}
			}
		}

		// performs basic checks to determine if the object represents a valid shape.
		boolean_c validate() const
		{
			for ( sint32_c i = 0; i < contours.get_length(); i++ )
			{
				contour_c * contour = contours[ i ];
				if ( contour->edge_segments.get_length() > 0 )
				{
					vector64x2_c corner = contour->edge_segments[ contour->edge_segments.get_length() - 1 ]->find_point( 1.0 );
					for ( sint32_c j = 0; j < contour->edge_segments.get_length(); i++ )
					{
						edge_segment_c * edge_segment = contour->edge_segments[ j ];
						if ( edge_segment == nullptr )
						{
							return false;
						}
						if ( edge_segment->find_point( 0.0 ) != corner )
						{
							return false;
						}
						corner = edge_segment->find_point( 1.0 );
					}
				}
			}
			return true;
		}

		// calculates the shape's bounding box.
		void_c find_bounds( box64x2_c & result ) const
		{
			if ( contours.get_length() )
			{
				result.minimum.a = 1000000000.0;
				result.minimum.b = 1000000000.0;
				result.maximum.a = 1000000000.0;
				result.maximum.b = 1000000000.0;
				for ( sint32_c i = 0; i < contours.get_length(); i++ )
				{
					contours[ i ]->find_bounds( result );
				}
			}
			else
			{
				result.minimum.a = 0.0;
				result.minimum.b = 0.0;
				result.maximum.a = 0.0;
				result.maximum.b = 0.0;
			}
		}

	};

	glyph_manager_c::glyph_atlas_c::row_c::row_c()
		: quantized_size( 0 )
		, top( 0 )
		, width( 0 )
		, height( 0 )
	{
	}

	glyph_manager_c::glyph_atlas_c::glyph_atlas_c()
		: _needs_upload( false )
		, _texture_data( nullptr )
		, _texture_data_size( 0 )
		, _row_list()
		, _was_modified_during_run_time( false )
	{
		_texture_data_size = glyph_manager_c::glyph_atlas_width * glyph_manager_c::glyph_atlas_height;
		_texture_data = new uint8_c[ _texture_data_size ];
		ops::memory_zero( _texture_data, _texture_data_size );
	}

	glyph_manager_c::glyph_atlas_c::~glyph_atlas_c()
	{
		delete[] _texture_data;
	}

	void_c glyph_manager_c::glyph_atlas_c::reset()
	{
		ops::memory_zero( _texture_data, _texture_data_size );
		_row_list.remove_all();
		_needs_upload = true;
		_was_modified_during_run_time = true;
	}

	boolean_c glyph_manager_c::glyph_atlas_c::find_space( sint32_c width, sint32_c height, sint32_c & x, sint32_c & y, uint8_c quantized_size )
	{
		// if there are no rows then we need to add the first row with the required hight.
		if ( _row_list.get_length() == 0 )
		{
			row_c * new_row = _row_list.emplace_at_end();
			new_row->quantized_size = quantized_size;
			new_row->top = 0;
			new_row->width = 0;
			new_row->height = height;
		}
		
		// find space in any of the rows, growing the last row vertically if able, or inserting a new last row if able.
		for ( sint32_c row_index = 0; row_index < _row_list.get_length(); row_index++ )
		{
			row_c & current_row = _row_list[ row_index ];

			// only select rows that match our quantized size, so that we don't waste space storing tiny glyphs in tall rows.
			// this may not be a great idea if different fonts implement different sizes with disparity.
			if ( current_row.quantized_size == quantized_size )
			{
				// find out how much space is available in the current row.
				sint32_c available_width = glyph_manager_c::glyph_atlas_width - current_row.width;
				sint32_c available_height = row_index < _row_list.get_length() - 1 ? current_row.height : glyph_manager_c::glyph_atlas_height - current_row.top;

				// check if the glyph can fit in the current row.
				if ( width <= available_width && height <= available_height )
				{
					// the glyph can fit in the current row.
					x = current_row.width;
					y = current_row.top;
					assert( x >= 0 && x + width <= glyph_manager_c::glyph_atlas_width );
					assert( y >= 0 && y + height <= glyph_manager_c::glyph_atlas_height );
					current_row.width += width;
					if ( height > current_row.height )
					{
						sint32_c extra_height = available_width > width ? height / 4 : 0; // extra height is to increase likelyhood that glyphs of similar size can still be packed into this row in the future, because once a row is added beneath this row then this row will not be able to grow any more.
						current_row.height = ops::math_minimum( height + extra_height, available_height );
					}
					return true;
				}
			}

			// add another row if needed.
			if ( row_index == _row_list.get_length() - 1 )
			{
				// create a new row that can contain the glyph in the next iteration.
				sint32_c new_row_top = current_row.top + current_row.height; // save because current_row reference could be invalidated after call to emplace_at_end().
				if ( new_row_top + height > glyph_manager_c::glyph_atlas_height )
				{
					// there's no room left in this atlas texture array slice (page).
					return false;
				}
				row_c * new_row = _row_list.emplace_at_end();
				new_row->quantized_size = quantized_size;
				new_row->top = new_row_top;
				new_row->width = 0;
				new_row->height = 0;
			}
		}

		// there were no rows of the 
		return false;
	}

	void_c glyph_manager_c::glyph_atlas_c::set_pixel( sint32_c x, sint32_c y, uint8_c value )
	{
		assert( x >= 0 && x < glyph_manager_c::glyph_atlas_width && y >= 0 && y < glyph_manager_c::glyph_atlas_height );
		_texture_data[ ( y * glyph_manager_c::glyph_atlas_width ) + x ] = value;
	}

	void_c glyph_manager_c::glyph_atlas_c::copy_sub_image( glyph_atlas_c * from, sint32_c from_left, sint32_c from_top, glyph_atlas_c * to, sint32_c to_left, sint32_c to_top, sint32_c width, sint32_c height )
	{
		assert( from != nullptr && to != nullptr );
		//assert( from->_texture_width == to->_texture_width && from->_texture_height == to->_texture_height );
		assert( from_left >= 0 && from_left + width <= glyph_manager_c::glyph_atlas_width && from_top >= 0 && from_top + height <= glyph_manager_c::glyph_atlas_height );
		assert( to_left >= 0 && to_left + width <= glyph_manager_c::glyph_atlas_width && to_top >= 0 && to_top + height <= glyph_manager_c::glyph_atlas_height );

		sint32_c to_x_delta = to_left - from_left;
		sint32_c to_y_delta = to_top - from_top;

		sint32_c from_y = from_top;
		sint32_c from_bottom = from_top + height;
		sint32_c from_right = from_left + width;
		for ( ; from_y < from_bottom; from_y++ )
		{
			sint32_c from_x = from_left;
			for ( ; from_x < from_right; from_x++ )
			{
				sint32_c from_i = ( from_y * glyph_manager_c::glyph_atlas_width ) + from_x;
				sint32_c to_i = ( ( from_y + to_y_delta ) * glyph_manager_c::glyph_atlas_width ) + ( from_x + to_x_delta );
				from->_texture_data[ from_i ] = to->_texture_data[ to_i ];
			}
		}
	}

	class free_type_context_c
	{
	public:
		vector64x2_c position;
		shape_c * shape;
		contour_c * contour;

	public:
		free_type_context_c()
			: position()
			, shape( nullptr )
			, contour( nullptr )
		{
		}

	};

	vector64x2_c ftPoint2( FT_Vector const & vector )
	{
		return vector64x2_c( vector.x / 64.0, vector.y / 64.0 );
	}

	int ftMoveTo( FT_Vector const * to, void_c * user )
	{
		free_type_context_c * context = reinterpret_cast< free_type_context_c * >( user );
		context->contour = new contour_c();
		context->shape->contours.insert_at_end( context->contour );
		context->position = ftPoint2( *to );
		return 0;
	}

	int ftLineTo( FT_Vector const * to, void_c * user )
	{
		free_type_context_c * context = reinterpret_cast< free_type_context_c * >( user );
		context->contour->add_edge_segment( new edge_segment_linear_c( context->position, ftPoint2( *to ), edge_color_e_white ) );
		context->position = ftPoint2( *to );
		return 0;
	}

	int ftConicTo( FT_Vector const * control, FT_Vector const * to, void_c * user )
	{
		free_type_context_c * context = reinterpret_cast< free_type_context_c * >( user );
		context->contour->add_edge_segment( new edge_segment_quadratic_c( context->position, ftPoint2( *control ), ftPoint2( *to ), edge_color_e_white ) );
		context->position = ftPoint2( *to );
		return 0;
	}

	int ftCubicTo( FT_Vector const * control1, FT_Vector const * control2, FT_Vector const * to, void_c * user)
	{
		free_type_context_c * context = reinterpret_cast< free_type_context_c * >( user );
		context->contour->add_edge_segment( new edge_segment_cubic_c( context->position, ftPoint2( *control1 ), ftPoint2( *control2 ), ftPoint2( *to ), edge_color_e_white ) );
		context->position = ftPoint2( *to );
		return 0;
	}

	glyph_manager_c::glyph_manager_c()
		: _glyph_pool( sizeof( glyph_c ), 0xFFFF ) // 0xFFFF glyphs should be more than enough... it's how many 32x32 glyphs will fit in a 4096x4096 texture.
		, _glyph_dictionary()
		, _glyph_atlas_array()
		, _glyph_atlas_texture( nullptr )
		, _glyph_atlas_staging_texture( nullptr )
		, _free_type_library_handle( nullptr )
	{
	}

	glyph_manager_c::~glyph_manager_c()
	{
		FT_Done_FreeType( reinterpret_cast< FT_Library >( _free_type_library_handle ) );
		_free_type_library_handle = nullptr;
		
		delete[] _glyph_atlas_array;
		_glyph_atlas_array = nullptr;

		delete _glyph_atlas_texture;
		_glyph_atlas_texture = nullptr;

		delete _glyph_atlas_staging_texture;
		_glyph_atlas_staging_texture = nullptr;
	}

	boolean_c glyph_manager_c::start()
	{
		_glyph_atlas_array = new glyph_atlas_c[ glyph_manager_c::glyph_atlas_array_slice_count ];
		FT_Error error = FT_Init_FreeType( reinterpret_cast< FT_Library * >( &_free_type_library_handle ) );
		if ( error != 0 )
		{
			debug_annoy( L"error", L"FT_Init_FreeType() failed." );
			return false;
		}

		_glyph_atlas_texture = engine_c::get_instance()->get_video_interface()->create_texture( video_texture_format_e_r8_unorm, glyph_manager_c::glyph_atlas_width, glyph_manager_c::glyph_atlas_height, 1, glyph_manager_c::glyph_atlas_array_slice_count, nullptr, 0, false, false, false, false );
		if ( _glyph_atlas_texture == nullptr )
		{
			debug_annoy( L"error", L"glyph atlas texture creation failed." );
			return false;
		}

		_glyph_atlas_staging_texture = engine_c::get_instance()->get_video_interface()->create_texture( video_texture_format_e_r8_unorm, glyph_manager_c::glyph_atlas_width, glyph_manager_c::glyph_atlas_height, 1, 1, nullptr, 0, true, false, false, false );
		if ( _glyph_atlas_staging_texture == nullptr )
		{
			debug_annoy( L"error", L"glyph atlas staging texture creation failed." );
			return false;
		}
		
		return true;
	}

	glyph_c const * glyph_manager_c::load_quantized_glyph( resource_file_font_c * font, float32_c font_size, char16_c code_point )
	{
		assert( font );
		assert( font->get_is_loaded() );
		assert( font->get_file_hash() != 0 );
		assert( font_size > 1.0f && font_size < 128.0f );
		assert( !ops::char16_is_space( code_point ) );

		glyph_key_c glyph_key( font->get_file_hash(), font_size, code_point );

	karas_house:
		glyph_c const * glyph = _glyph_dictionary.find_value_else_nullptr( glyph_key );
		if ( glyph )
		{
			assert( glyph->key.code_point == code_point || glyph->key.code_point == '?' );
			return glyph;
		}

		resource_file_font_c::size_metrics_c const * font_size_metrics = font->get_quantized_size_metrics( glyph_key.quantized_size );
		FT_Activate_Size( reinterpret_cast< FT_Size >( font_size_metrics->free_type_size_handle ) );
		FT_Face free_type_face_handle = reinterpret_cast< FT_Face >( font->get_free_type_face_handle() );
		FT_Select_Charmap( free_type_face_handle, FT_ENCODING_UNICODE );
		if ( free_type_face_handle->num_charmaps > 1 )
		{
			FT_CharMap t = free_type_face_handle->charmaps[ 1 ];
		}
		FT_UInt char_index = FT_Get_Char_Index( free_type_face_handle, glyph_key.code_point );
		if ( char_index == 0 )
		{
			if ( glyph_key.code_point == '?' )
			{
				return nullptr;
			}
			glyph_key.code_point = '?';
			goto karas_house;
		}
		FT_Error free_type_error = FT_Load_Glyph( free_type_face_handle, char_index, FT_LOAD_DEFAULT );
		if ( free_type_error )
		{
			return nullptr;
		}

		sint32_c sdf_range = glyph_manager_c::get_sdf_range( glyph_key.quantized_size );
		glyph_c * new_glyph = reinterpret_cast< glyph_c * >( _glyph_pool.allocate() );
		new_glyph->key = glyph_key;
		new_glyph->box.minimum.a = ( static_cast< float32_c >( free_type_face_handle->glyph->metrics.horiBearingX ) / 64.0f ) - sdf_range;
		new_glyph->box.minimum.b = ( static_cast< float32_c >( -free_type_face_handle->glyph->metrics.horiBearingY ) / 64.0f ) - sdf_range;
		new_glyph->box.maximum.a = ( static_cast< float32_c >( free_type_face_handle->glyph->metrics.horiBearingX + free_type_face_handle->glyph->metrics.width ) / 64.0f ) + sdf_range;
		new_glyph->box.maximum.b = ( static_cast< float32_c >( -free_type_face_handle->glyph->metrics.horiBearingY + free_type_face_handle->glyph->metrics.height ) / 64.0f ) + sdf_range;
		new_glyph->horizontal_advance = static_cast< float32_c >( free_type_face_handle->glyph->metrics.horiAdvance ) / 64.0f;

		free_type_context_c context;
		shape_c shape;
		context.shape = &shape;
		FT_Outline_Funcs ftFunctions;
		ftFunctions.move_to = &ftMoveTo;
		ftFunctions.line_to = &ftLineTo;
		ftFunctions.conic_to = &ftConicTo;
		ftFunctions.cubic_to = &ftCubicTo;
		ftFunctions.shift = 0;
		ftFunctions.delta = 0;
		free_type_error = FT_Outline_Decompose( &free_type_face_handle->glyph->outline, &ftFunctions, &context );
		if ( free_type_error )
		{
			return nullptr;
		}

		// figure out where to put the glyph image in the atlas texture.
		sint32_c integer_map_width = static_cast< sint32_c >( new_glyph->box.maximum.a - new_glyph->box.minimum.a + 0.5f );
		sint32_c integer_map_height = static_cast< sint32_c >( new_glyph->box.maximum.b - new_glyph->box.minimum.b + 0.5f );
		sint32_c integer_map_position_x;
		sint32_c integer_map_position_y;

		assert( integer_map_width < 512 && integer_map_height < 512 ); // this should be pretty generous.

		glyph_atlas_c * glyph_atlas = nullptr;
		for ( sint32_c i = 0; i < glyph_manager_c::glyph_atlas_array_slice_count; i++ )
		{
			glyph_atlas = &_glyph_atlas_array[ i ];
			if ( glyph_atlas->find_space( integer_map_width, integer_map_height, integer_map_position_x, integer_map_position_y, glyph_key.quantized_size ) )
			{
				new_glyph->atlas_index = static_cast< uint8_c >( i );
				break;
			}
			else if ( i == glyph_manager_c::glyph_atlas_array_slice_count - 1 )
			{
				// there's no more room to add another glyph atlas.
				needs_reset = true;
				return nullptr;
			}
		}

		new_glyph->map.minimum.a = static_cast< float32_c >( integer_map_position_x ) / static_cast< float32_c >( glyph_manager_c::glyph_atlas_width );
		new_glyph->map.minimum.b = static_cast< float32_c >( integer_map_position_y ) / static_cast< float32_c >( glyph_manager_c::glyph_atlas_height );
		new_glyph->map.maximum.a = static_cast< float32_c >( integer_map_position_x + integer_map_width ) / static_cast< float32_c >( glyph_manager_c::glyph_atlas_width );
		new_glyph->map.maximum.b = static_cast< float32_c >( integer_map_position_y + integer_map_height ) / static_cast< float32_c >( glyph_manager_c::glyph_atlas_height );

		_glyph_dictionary.insert( glyph_key, new_glyph );

		core_list_c< sint32_c > windings;
		sint32_c contour_count = shape.contours.get_length();
		windings.construct_mode_dynamic( contour_count, contour_count );
		for ( sint32_c i = 0; i < shape.contours.get_length(); i++ )
		{
			windings[ i ] = shape.contours[ i ]->find_winding();
		}

		#pragma omp parallel
		{
			// i don't know why but i need to translate the rendered glyph a bit to get it to align correctly when it prints.
			float64_c tx = static_cast< float64_c >( free_type_face_handle->glyph->metrics.horiBearingX ) / 64.0;
			float64_c ty = static_cast< float64_c >( free_type_face_handle->glyph->metrics.height - free_type_face_handle->glyph->metrics.horiBearingY ) / 64.0;
			core_list_c< float64_c > contour_signed_distance;
			contour_signed_distance.construct_mode_dynamic( contour_count, contour_count );
			#pragma omp for
			for ( sint32_c y = 0; y < integer_map_height ; y++ )
			{
				for ( sint32_c x = 0; x < integer_map_width; x++ )
				{
					float64_c dummy = 0.0;
					vector64x2_c query_point = vector64x2_c( x - sdf_range + tx + 0.5f, y - sdf_range - ty + 0.5f ); // we want to find the signed distance for this point.
					float64_c negative_distance = -signed_distance_c::infinite.distance;
					float64_c positive_distance =  signed_distance_c::infinite.distance;
					sint32_c winding = 0;

					for ( sint32_c contour_index = 0; contour_index < contour_count; contour_index++ )
					{
						contour_c * contour = shape.contours[ contour_index ];
						signed_distance_c minimum_distance = signed_distance_c::infinite;
						sint32_c edge_segments_count = contour->edge_segments.get_length();
						for ( sint32_c edge_segment_index = 0; edge_segment_index < edge_segments_count; edge_segment_index++ )
						{
							edge_segment_c * edge_segment = contour->edge_segments[ edge_segment_index ];
							signed_distance_c distance = edge_segment->find_singed_distance( query_point, dummy );
							if ( distance < minimum_distance )
							{
								minimum_distance = distance;
							}
						}
						contour_signed_distance[ contour_index ] = minimum_distance.distance;
						if ( windings[ contour_index ] > 0 && minimum_distance.distance >= 0 && ops::math_absolute_value( minimum_distance.distance ) < ops::math_absolute_value( positive_distance ) )
						{
							positive_distance = minimum_distance.distance;
						}
						if ( windings[ contour_index ] < 0 && minimum_distance.distance <= 0 && ops::math_absolute_value( minimum_distance.distance ) < ops::math_absolute_value( negative_distance ) )
						{
							negative_distance = minimum_distance.distance;
						}
					}

					float64_c signed_distance = constants< float64_c >::maximum();
					if ( positive_distance >= 0 && ops::math_absolute_value( positive_distance ) <= ops::math_absolute_value( negative_distance ) )
					{
						signed_distance = positive_distance;
						winding = 1;
						for ( sint32_c contour_index = 0; contour_index < contour_count; contour_index++ )
						{
							if ( windings[ contour_index ] > 0 && contour_signed_distance[ contour_index ] > signed_distance && ops::math_absolute_value( contour_signed_distance[ contour_index ] ) < ops::math_absolute_value( negative_distance ) )
							{
								signed_distance = contour_signed_distance[ contour_index ];
							}
						}
					}
					else if ( negative_distance <= 0 && ops::math_absolute_value( negative_distance ) <= ops::math_absolute_value( positive_distance ) )
					{
						signed_distance = negative_distance;
						winding = -1;
						for ( sint32_c contour_index = 0; contour_index < contour_count; contour_index++ )
						{
							if ( windings[ contour_index ] < 0 && contour_signed_distance[ contour_index ] < signed_distance && ops::math_absolute_value( contour_signed_distance[ contour_index ] ) < ops::math_absolute_value( positive_distance ) )
							{
								signed_distance = contour_signed_distance[ contour_index ];
							}
						}
					}

					for ( sint32_c contour_index = 0; contour_index < contour_count; contour_index++ )
					{
						if ( windings[ contour_index ] != winding && ops::math_absolute_value( contour_signed_distance[ contour_index ] ) < ops::math_absolute_value( signed_distance ) )
						{
							signed_distance = contour_signed_distance[ contour_index ];
						}
					}

					uint8_c signed_distance_uint8 = static_cast< uint8_c >( ops::math_saturate( signed_distance / static_cast< float32_c >( glyph_key.quantized_size ) + 0.5 ) * 255.0 );

					glyph_atlas->set_pixel( integer_map_position_x + x, integer_map_position_y + ( integer_map_height - y - 1 ), signed_distance_uint8 );
				}
			}
		}

		glyph_atlas->_needs_upload = true;
		glyph_atlas->_was_modified_during_run_time = true;

		assert( new_glyph->key.code_point != 0 );
		return new_glyph;
	}

	void_c glyph_manager_c::reset()
	{
		//if ( _glyph_dictionary.get_length() > 0 )
		{
			_glyph_dictionary.remove_all();
			_glyph_pool.reset();
			for ( sint32_c i = 0; i < glyph_manager_c::glyph_atlas_array_slice_count; i++ )
			{
				_glyph_atlas_array[ i ].reset();
			}
			needs_reset = false;
			menu_element_text_c::invalidate_glyph_layout_of_all_instances(); // this will have the effect of re-building the glyph cache. hopefully the text elements aren't needing too much, as this could over flow the glyph cache and create a kind of dead lock loop.
		}
	}

	void_c glyph_manager_c::update_glyph_map_texture()
	{
		for ( sint32_c i = 0; i < glyph_manager_c::glyph_atlas_array_slice_count; i++ )
		{
			glyph_atlas_c * glyph_atlas = &_glyph_atlas_array[ i ];
			if ( glyph_atlas->_needs_upload )
			{
				_glyph_atlas_staging_texture->set_data( glyph_atlas->_texture_data, glyph_atlas->_texture_data_size );
				engine_c::get_instance()->get_video_interface()->copy_sub_resource( _glyph_atlas_texture, i, _glyph_atlas_staging_texture, 0 );
				glyph_atlas->_needs_upload = false;
			}
		}
	}

	char8_c const * const glyph_list_signature = "glYa";
	char8_c const * const row_list_signature = "glYb";
	char8_c const * const font_cache_folder = "_common/fonts/cache/";

	boolean_c glyph_manager_c::save_to_disk()
	{
		string16_c folder_path = engine_c::get_instance()->get_content_manager()->get_engine_data_folder_path();
		folder_path += font_cache_folder;

		if ( !ops::data_does_folder_exist( folder_path ) )
		{
			ops::data_create_folder( folder_path );
		}

		// save glyph atlas texture array slices that were modified during run time.
		boolean_c was_modified_during_run_time = false;
		string16_c file_path;
		data_stream_file_c file_stream;
		core_list_c< uint8_c > file_data;
		core_list_c< image_png_chunk_c > chunk_list;
		data_stream_memory_c glyph_stream;
		data_scribe_binary_c glyph_scribe;
		data_stream_memory_c row_stream;
		data_scribe_binary_c row_scribe;
		image_png_chunk_c * chunk = nullptr;
		for ( sint32_c i = 0; i < glyph_manager_c::glyph_atlas_array_slice_count; i++ )
		{
			glyph_atlas_c const * glyph_atlas = &_glyph_atlas_array[ i ];

			if ( glyph_atlas->_row_list.get_length() == 0 && i > 0 )
			{
				break;
			}

			if ( glyph_atlas->_was_modified_during_run_time == false )
			{
				continue;
			}

			was_modified_during_run_time = true;

			// buld glyph list data.
			if ( i == 0 )
			{
				glyph_stream.open();
				glyph_scribe.set_stream( &glyph_stream );
				if ( !glyph_scribe.save_uint32( glyph_manager_c::glyph_atlas_width ) )
				{
					return false;
				}
				if ( !glyph_scribe.save_uint32( glyph_manager_c::glyph_atlas_height ) )
				{
					return false;
				}
				if ( !glyph_scribe.save_uint32( _glyph_dictionary.get_length() ) )
				{
					return false;
				}
				sint32_c glyphs_saved = 0;
				core_dictionary_c< glyph_key_c, glyph_c const * >::iterator_c iterator = _glyph_dictionary.get_iterator();
				while ( iterator.next() )
				{
					glyph_c const * glyph = iterator.get_value();
					if ( !glyph_scribe.save_uint32( glyph->key.font_file_hash ) )
					{
						return false;
					}
					if ( !glyph_scribe.save_uint16( glyph->key.code_point ) )
					{
						return false;
					}
					if ( !glyph_scribe.save_uint8( glyph->key.quantized_size ) )
					{
						return false;
					}
					if ( !glyph_scribe.save_sint16( static_cast< sint16_c >( glyph->box.minimum.a ) ) )
					{
						return false;
					}
					if ( !glyph_scribe.save_sint16( static_cast< sint16_c >( glyph->box.minimum.b ) ) )
					{
						return false;
					}
					if ( !glyph_scribe.save_sint16( static_cast< sint16_c >( glyph->box.maximum.a ) ) )
					{
						return false;
					}
					if ( !glyph_scribe.save_sint16( static_cast< sint16_c >( glyph->box.maximum.b ) ) )
					{
						return false;
					}
					if ( !glyph_scribe.save_sint16( static_cast< sint16_c >( glyph->map.minimum.a * glyph_manager_c::glyph_atlas_width ) ) )
					{
						return false;
					}
					if ( !glyph_scribe.save_sint16( static_cast< sint16_c >( glyph->map.minimum.b * glyph_manager_c::glyph_atlas_height ) ) )
					{
						return false;
					}
					if ( !glyph_scribe.save_sint16( static_cast< sint16_c >( glyph->map.maximum.a * glyph_manager_c::glyph_atlas_width ) ) )
					{
						return false;
					}
					if ( !glyph_scribe.save_sint16( static_cast< sint16_c >( glyph->map.maximum.b * glyph_manager_c::glyph_atlas_height ) ) )
					{
						return false;
					}
					if ( !glyph_scribe.save_float32( glyph->horizontal_advance ) )
					{
						return false;
					}
					if ( !glyph_scribe.save_uint8( glyph->atlas_index ) )
					{
						return false;
					}
					glyphs_saved++;
				}
				assert( glyphs_saved == _glyph_dictionary.get_length() );
				chunk = chunk_list.emplace_at_end();
				chunk->data_is_ours = false;
				chunk->data = glyph_stream.get_internal_buffer().get_internal_array();
				chunk->data_size = glyph_stream.get_size();
				ops::memory_copy( glyph_list_signature, chunk->type, 4 );
				chunk = nullptr;
			}

			// build row list data.
			row_stream.open();
			row_scribe.set_stream( &row_stream );
			row_scribe.save_sint32( glyph_atlas->_row_list.get_length() );
			for ( sint32_c j = 0; j < glyph_atlas->_row_list.get_length(); j++ )
			{
				glyph_atlas_c::row_c const & row = glyph_atlas->_row_list[ j ];
				row_scribe.save_uint8( row.quantized_size );
				row_scribe.save_sint32( row.top );
				row_scribe.save_sint32( row.width );
				row_scribe.save_sint32( row.height );
			}
			chunk = chunk_list.emplace_at_end();
			chunk->data_is_ours = false;
			chunk->data = row_stream.get_internal_buffer().get_internal_array();
			chunk->data_size = row_stream.get_size();
			ops::memory_copy( row_list_signature, chunk->type, 4 );
			chunk = nullptr;

			// create png file data.
			image_c image;
			image.width = glyph_manager_c::glyph_atlas_width;
			image.height = glyph_manager_c::glyph_atlas_height;
			image.channel_count = 1;
			image.channel_bit_depth = 8;
			image.data.construct_mode_static_from_array( glyph_atlas->_texture_data, glyph_atlas->_texture_data_size );
			image_save_to_png( image, &chunk_list, file_data );

			// close glyph and row scribes streams.
			if ( i == 0 )
			{
				glyph_scribe.set_stream( nullptr );
				glyph_stream.close();
			}
			row_scribe.set_stream( nullptr );
			row_stream.close();
			chunk_list.remove_all();

			// save png file to disk.
			string8_c index_string;
			ops::convert_sint32_to_string8( i, index_string );
			file_path = folder_path;
			file_path += "page";
			file_path += index_string;
			file_path += ".png";
			if ( !file_stream.open( file_path, data_stream_mode_e_write ) )
			{
				return false;
			}
			file_stream.save( file_data.get_internal_array(), file_data.get_internal_array_size_used() );
			file_stream.close();
			file_data.remove_all();
		}

		return true;
	}

	boolean_c glyph_manager_c::load_from_disk()
	{
		assert( _glyph_dictionary.get_length() == 0 );

		string16_c folder_path = engine_c::get_instance()->get_content_manager()->get_engine_data_folder_path();
		folder_path += font_cache_folder;

		if ( !ops::data_does_folder_exist( folder_path ) )
		{
			return false;
		}

		// load png files, which contain the textures, glyphs, and rows
		string16_c file_path;
		data_stream_file_c file_stream;
		data_stream_memory_c chunk_stream;
		data_scribe_binary_c chunk_scribe;
		core_list_c< image_png_chunk_c > chunk_list;
		image_c image;
		core_list_c< uint8_c > file_data;
		for ( sint32_c i = 0; i < glyph_manager_c::glyph_atlas_array_slice_count; i++ )
		{
			glyph_atlas_c * glyph_atlas = &_glyph_atlas_array[ i ];

			glyph_atlas->_needs_upload = true;
			glyph_atlas->_row_list.remove_all();
			glyph_atlas->_was_modified_during_run_time = false;

			string8_c index_string;
			ops::convert_sint32_to_string8( i, index_string );

			// open file, load file data, close file.
			file_path = folder_path;
			file_path += "page";
			file_path += index_string;
			file_path += ".png";
			if ( !ops::data_does_file_exist( file_path ) )
			{
				if ( i == 0 )
				{
					return false; // we need at least the first png.
				}
				return true; // reached end of saved atlas list (probably).
			}
			if ( !file_stream.open( file_path, data_stream_mode_e_read ) )
			{
				goto cancel;
			}
			file_data.construct_mode_dynamic( file_stream.get_size() );
			if ( !file_stream.load( file_data.get_internal_array(), file_data.get_length() ) )
			{
				goto cancel;
			}
			file_stream.close();

			// decode png file data and copy to our glyph atlas.
			image.width = 0;
			image.height = 0;
			image.channel_count = 0;
			image.channel_bit_depth = 0;
			image.data.remove_all();
			if ( !image_load_from_png( file_data, image, &chunk_list ) )
			{
				goto cancel;
			}
			if ( image.width != glyph_manager_c::glyph_atlas_width || image.height != glyph_manager_c::glyph_atlas_height || image.channel_count != 1 || image.channel_bit_depth != 8 || image.data.get_length() != glyph_atlas->_texture_data_size )
			{
				goto cancel;
			}
			ops::memory_copy( image.data.get_internal_array(), glyph_atlas->_texture_data, image.data.get_length() );

			// process chunks.
			image_png_chunk_c * glyph_list_chunk = nullptr;
			image_png_chunk_c * row_list_chunk = nullptr;
			for ( sint32_c j = 0; j < chunk_list.get_length(); j++ )
			{
				image_png_chunk_c * chunk = &chunk_list[ j ];
				if ( ops::memory_compare( chunk->type, glyph_list_signature, 4 ) )
				{
					if ( i != 0 || glyph_list_chunk != nullptr )
					{
						goto cancel;
					}

					glyph_list_chunk = chunk;
					chunk_stream.open_static( chunk->data, chunk->data_size );
					chunk_scribe.set_stream( &chunk_stream );
					uint32_c texture_width = 0;
					if ( !chunk_scribe.load_uint32( texture_width ) )
					{
						goto cancel;;
					}
					uint32_c texture_height = 0;
					if ( !chunk_scribe.load_uint32( texture_height ) )
					{
						goto cancel;
					}
					if ( texture_width != glyph_manager_c::glyph_atlas_width || texture_height != glyph_manager_c::glyph_atlas_height )
					{
						goto cancel;
					}
					uint32_c glyph_count = 0;
					if ( !chunk_scribe.load_uint32( glyph_count ) )
					{
						goto cancel;
					}
					for ( uint32_c i = 0; i < glyph_count; i++ )
					{
						glyph_c * glyph = reinterpret_cast< glyph_c * >( _glyph_pool.allocate() );
						if ( !chunk_scribe.load_uint32( glyph->key.font_file_hash ) )
						{
							return false;
						}
						if ( !chunk_scribe.load_char16( glyph->key.code_point ) )
						{
							return false;
						}
						if ( !chunk_scribe.load_uint8( glyph->key.quantized_size ) )
						{
							return false;
						}
						sint16_c temp_sint16;
						if ( !chunk_scribe.load_sint16( temp_sint16 ) )
						{
							return false;
						}
						glyph->box.minimum.a = static_cast< float32_c >( temp_sint16 );
						if ( !chunk_scribe.load_sint16( temp_sint16 ) )
						{
							return false;
						}
						glyph->box.minimum.b = static_cast< float32_c >( temp_sint16 );
						if ( !chunk_scribe.load_sint16( temp_sint16 ) )
						{
							return false;
						}
						glyph->box.maximum.a = static_cast< float32_c >( temp_sint16 );
						if ( !chunk_scribe.load_sint16( temp_sint16 ) )
						{
							return false;
						}
						glyph->box.maximum.b = static_cast< float32_c >( temp_sint16 );
						if ( !chunk_scribe.load_sint16( temp_sint16 ) )
						{
							return false;
						}
						glyph->map.minimum.a = static_cast< float32_c >( temp_sint16 ) / static_cast< float32_c >( glyph_manager_c::glyph_atlas_width );
						if ( !chunk_scribe.load_sint16( temp_sint16 ) )
						{
							return false;
						}
						glyph->map.minimum.b = static_cast< float32_c >( temp_sint16 ) / static_cast< float32_c >( glyph_manager_c::glyph_atlas_height );
						if ( !chunk_scribe.load_sint16( temp_sint16 ) )
						{
							return false;
						}
						glyph->map.maximum.a = static_cast< float32_c >( temp_sint16 ) / static_cast< float32_c >( glyph_manager_c::glyph_atlas_width );
						if ( !chunk_scribe.load_sint16( temp_sint16 ) )
						{
							return false;
						}
						glyph->map.maximum.b = static_cast< float32_c >( temp_sint16 ) / static_cast< float32_c >( glyph_manager_c::glyph_atlas_height );
						if ( !chunk_scribe.load_float32( glyph->horizontal_advance ) )
						{
							return false;
						}
						if ( !chunk_scribe.load_uint8( glyph->atlas_index ) )
						{
							return false;
						}
						_glyph_dictionary.insert( glyph->key, glyph );
					}
					chunk_scribe.set_stream( nullptr );
					chunk_stream.close();
				}
				else if ( ops::memory_compare( chunk->type, row_list_signature, 4 ) )
				{
					if ( row_list_chunk != nullptr )
					{
						goto cancel;
					}

					row_list_chunk = chunk;
					chunk_stream.open_static( row_list_chunk->data, row_list_chunk->data_size );
					chunk_scribe.set_stream( &chunk_stream );
					sint32_c row_count = 0;
					if ( !chunk_scribe.load_sint32( row_count ) )
					{
						return false;
					}
					for ( sint32_c j = 0; j < row_count; j++ )
					{
						glyph_atlas_c::row_c * row = glyph_atlas->_row_list.emplace_at_end();
						if ( !chunk_scribe.load_uint8( row->quantized_size ) )
						{
							return false;
						}
						if ( !chunk_scribe.load_sint32( row->top ) )
						{
							return false;
						}
						if ( !chunk_scribe.load_sint32( row->width ) )
						{
							return false;
						}
						if ( !chunk_scribe.load_sint32( row->height ) )
						{
							return false;
						}
					}
					chunk_scribe.set_stream( nullptr );
					chunk_stream.close();
				}
			}
			chunk_list.remove_all();
		}

		assert( false ); // this should be unreachable.

	cancel:
		reset();
		return false;
	}

	uint8_c const glyph_manager_c::quantized_sizes[ 4 ] = { 12, 24, 32, 48 };

	sint32_c glyph_manager_c::get_quantized_index( float32_c size )
	{
		for ( sint32_c i = 0; i < glyph_manager_c::quantized_count - 1; i++ )
		{
			if ( size <= glyph_manager_c::quantized_sizes[ i ] )
			{
				return i;
			}
		}
		return glyph_manager_c::quantized_count - 1;
	}

	uint8_c glyph_manager_c::get_quantized_size( float32_c size )
	{
		for ( sint32_c i = 0; i < glyph_manager_c::quantized_count - 1; i++ )
		{
			if ( size <= glyph_manager_c::quantized_sizes[ i ] )
			{
				return glyph_manager_c::quantized_sizes[ i ];
			}
		}
		return glyph_manager_c::quantized_sizes[ glyph_manager_c::quantized_count - 1 ];
	}

	uint8_c glyph_manager_c::get_sdf_range( uint8_c quantized_size )
	{
		return quantized_size / 2;
	}

}
