#include "cheonsa__ops_color.h"
#include "cheonsa__ops_math.h"
#include "cheonsa__ops_vector.h"

namespace cheonsa
{
	namespace ops
	{

		void_c convert_rgba_to_rgba( vector32x4_c const & value, uint32_c & result )
		{
			result = static_cast< uint32_c >( math_saturate( value.a ) * 255.0f ) << 24 |
					 static_cast< uint32_c >( math_saturate( value.b ) * 255.0f ) << 16 |
					 static_cast< uint32_c >( math_saturate( value.c ) * 255.0f ) << 8 |
					 static_cast< uint32_c >( math_saturate( value.d ) * 255.0f );
		}

		void_c convert_rgba_to_rgba( uint32_c const & value, vector32x4_c & result )
		{
			result.a = static_cast< float32_c >( ( value >> 24 ) & 0xFF ) / 255.0f;
			result.b = static_cast< float32_c >( ( value >> 16 ) & 0xFF ) / 255.0f;
			result.c = static_cast< float32_c >( ( value >> 8 ) & 0xFF ) / 255.0f;
			result.d = static_cast< float32_c >( ( value ) & 0xFF ) / 255.0f;
		}

		boolean_c convert_rgb_to_hsv( vector64x3_c const & rgb, vector64x3_c & hsv )
		{
			// taken from an answer on:
			// https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
			float64_c min = rgb.a < rgb.b ? rgb.a : rgb.b; min = min < rgb.c ? min : rgb.c;
			float64_c max = rgb.a > rgb.b ? rgb.a : rgb.b; max = max > rgb.c ? max : rgb.c;
			hsv.c = max;
			float64_c delta = max - min;
			if ( delta < 0.00001 )
			{
				// can't solve hue, preserve its existing value for the color picker's sake.
				hsv.b = 0;
				return true;
			}
			if ( max > 0.0 )
			{
				hsv.b = ( delta / max );
			}
			else
			{
				// can't solve hue, preserve its existing value for the color picker's sake.
				hsv.b = 0.0;
				return true;
			}
			if ( rgb.a >= max )
			{
				hsv.a = ( rgb.b - rgb.c ) / delta; // between yellow and magenta.
			}
			else
			{
				if ( rgb.b >= max )
				{
					hsv.a = 2.0 + ( rgb.c - rgb.a ) / delta; // between cyan and yellow.
				}
				else
				{
					hsv.a = 4.0 + ( rgb.a - rgb.b ) / delta; // between magenta and cyan.
				}
			}
			hsv.a *= 60.0; // scale to degrees.
			if ( hsv.a < 0.0 )
			{
				hsv.a += 360.0;
			}
			return true;
		}

		boolean_c convert_hsv_to_rgb( vector64x3_c const & hsv, vector64x3_c & rgb )
		{
			// taken from an answer on:
			// https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both.
			float64_c hh = hsv.a;
			if ( hh >= 360.0 )
			{
				hh = 0.0;
			}
			hh /= 60.0;
			sint32_c i = static_cast< sint32_c >( hh );
			float64_c ff = hh - i;
			float64_c p = hsv.c * ( 1.0 - hsv.b );
			float64_c q = hsv.c * ( 1.0 - ( hsv.b * ff ) );
			float64_c t = hsv.c * ( 1.0 - ( hsv.b * ( 1.0 - ff ) ) );
			switch ( i )
			{
			case 0:
				rgb.a = hsv.c;
				rgb.b = t;
				rgb.c = p;
				break;
			case 1:
				rgb.a = q;
				rgb.b = hsv.c;
				rgb.c = p;
				break;
			case 2:
				rgb.a = p;
				rgb.b = hsv.c;
				rgb.c = t;
				break;
			case 3:
				rgb.a = p;
				rgb.b = q;
				rgb.c = hsv.c;
				break;
			case 4:
				rgb.a = t;
				rgb.b = p;
				rgb.c = hsv.c;
				break;
			case 5:
			default:
				rgb.a = hsv.c;
				rgb.b = p;
				rgb.c = q;
				break;
			}
			return true;
		}

		float64_c const husl_bounds_m[ 3 ][ 3 ] =
		{
			{  3.240969941904521, -1.537383177570093, -0.498610760293    },
			{ -0.96924363628087,   1.87596750150772,   0.041555057407175 },
			{  0.055630079696993, -0.20397695888897,   1.056971514242878 },
		};

		float64_c const husl_bounds_m_inv[ 3 ][ 3 ] =
		{
			{ 0.41239079926595,  0.35758433938387, 0.18048078840183  },
			{ 0.21263900587151,  0.71516867876775, 0.072192315360733 },
			{ 0.019330818715591, 0.11919477979462, 0.95053215224966  },
		};

		float64_c const husl_ref_x = 0.95045592705167;
		float64_c const husl_ref_y = 1.0;
		float64_c const husl_ref_z = 1.089057750759878;

		float64_c const husl_ref_u = 0.19783000664283;
		float64_c const husl_ref_v = 0.46831999493879;

		float64_c const husl_kappa = 903.2962962;
		float64_c const husl_epsilon = 0.0088564516;

		struct husl_point_c
		{
			float64_c x;
			float64_c y;
		};

		class husl_converter_c
		{
		public:
			static void_c get_bounds( float64_c L, husl_point_c * result )
			{
				float64_c sub1 = math_power( L + 16.0, 3.0 ) / 1560896.0;
				float64_c sub2 = sub1 > husl_epsilon ? sub1 : L / husl_kappa;
				sint32_c i = 0;
				for ( sint32_c c = 0; c < 3; c++ )
				{
					float64_c m1 = husl_bounds_m[ c ][ 0 ];
					float64_c m2 = husl_bounds_m[ c ][ 1 ];
					float64_c m3 = husl_bounds_m[ c ][ 2 ];
					for ( sint32_c t = 0; t < 2; t++ )
					{
						float64_c top1 = ( 284517.0 * m1 - 94839.0 * m3 ) * sub2;
						float64_c top2 = ( 838422.0 * m3 + 769860.0 * m2 + 731718.0 * m1 ) * L * sub2 - 769860.0 * t * L;
						float64_c bottom = ( 632260.0 * m3 - 126452.0 * m2 ) * sub2 + 126452.0 * t;
						result[ i ].x = top1 / bottom;
						result[ i ].y = top2 / bottom;
						i++;
					}
				}
			}

			static float64_c intersect_line_line( husl_point_c * line_point_a, husl_point_c * line_point_b )
			{
				return ( line_point_a->y - line_point_b->y ) / ( line_point_b->x - line_point_a->x );
			}

			static float64_c distance_from_pole( husl_point_c * point )
			{
				return math_square_root( point->x * point->x + point->y * point->y );
			}

			static boolean_c length_of_ray_until_intersect( float64_c theta, husl_point_c * ray, float64_c & length )
			{
				length = ray->y / ( math_sine( theta ) - ray->x * math_cosine( theta ) );
				return length >= 0.0;
			}

			static float64_c max_safe_chroma_for_l( float64_c L )
			{
				husl_point_c bounds[ 6 ];
				get_bounds( L, bounds );

				float64_c min = std::numeric_limits< float64_c >::max();

				for ( sint32_c i = 0; i < 2; i++ )
				{
					float64_c m1 = bounds[ i ].x;
					float64_c b1 = bounds[ i ].y;
					husl_point_c line = { m1, b1 };
					husl_point_c line_point_b = { -1.0 / m1, 0.0 };
					float64_c x = intersect_line_line( & line, & line_point_b );
					husl_point_c pole = { x, b1 + x * m1 };
					float64_c length = distance_from_pole( & pole );
					min = math_minimum( min, length );
				}

				return min;
			}

			static float64_c max_chroma_for_lh( float64_c L, float64_c H )
			{
				double hrad = H / 360.0 * constants< float64_c >::pi() * 2.0;

				husl_point_c bounds[ 6 ];
				get_bounds( L, bounds );
				float64_c min = std::numeric_limits< float64_c >::max();

				for ( sint32_c i = 0; i < 6; i++ )
				{
					float64_c length;
					if ( length_of_ray_until_intersect( hrad, & bounds[ i ], length ) )
					{
						min = math_minimum( min, length );
					}
				}

				return min;
			}

			static float64_c from_linear( float64_c c )
			{
				if ( c <= 0.0031308 )
				{
					return 12.92 * c;
				}
				else
				{
					return 1.055 * math_power( c, 1.0 / 2.4 ) - 0.055;
				}
			}

			static float64_c to_linear( float64_c c )
			{
				if ( c > 0.04045 )
				{
					return math_power( ( c + 0.055 ) / ( 1.0 + 0.055 ), 2.4 );
				}
				else
				{
					return c / 12.92;
				}
			}

			static float64_c dp3( float64_c const * a, float64_c const * b )
			{
				return a[ 0 ] * b[ 0 ] + a[ 1 ] * b[ 1 ] + a[ 2 ] * b[ 2 ];
			}

			static void_c xyz_to_rgb( float64_c const * xyz, float64_c * rgb )
			{
				rgb[ 0 ] = from_linear( dp3( husl_bounds_m[ 0 ], xyz ) );
				rgb[ 1 ] = from_linear( dp3( husl_bounds_m[ 0 ], xyz ) );
				rgb[ 2 ] = from_linear( dp3( husl_bounds_m[ 0 ], xyz ) );
			}

			static void_c rgb_to_xyz( float64_c const * rgb, float64_c * xyz )
			{
				float64_c rgbl[ 3 ];
				rgbl[ 0 ] = to_linear( xyz[ 0 ] );
				rgbl[ 1 ] = to_linear( xyz[ 1 ] );
				rgbl[ 2 ] = to_linear( xyz[ 2 ] );
				xyz[ 0 ] = dp3( husl_bounds_m_inv[ 0 ], rgbl );
				xyz[ 1 ] = dp3( husl_bounds_m_inv[ 1 ], rgbl );
				xyz[ 2 ] = dp3( husl_bounds_m_inv[ 2 ], rgbl );
			}

			static float64_c y_to_l( float64_c Y )
			{
				if ( Y <= husl_epsilon )
				{
					return ( Y / husl_ref_y ) * husl_kappa;
				}
				else
				{
					return 116.0 * math_power( Y / husl_ref_y, 1.0 / 3.0 ) - 16.0;
				}
			}

			static float64_c l_to_y( float64_c L )
			{
				if ( L <= 8.0 )
				{
					return husl_ref_y * L / husl_kappa;
				}
				else
				{
					return husl_ref_y * math_power( ( L + 16.0 ) / 116.0, 3.0 );
				}
			}

			static void_c xyz_to_luv( float64_c const * xyz, float64_c * luv )
			{
				float64_c varU = ( 4.0 * xyz[ 0 ] ) / ( xyz[ 0 ] + ( 15.0 * xyz[ 1 ] ) + ( 3.0 * xyz[ 2 ] ) );
				float64_c varV = ( 9.0 * xyz[ 1 ] ) / ( xyz[ 0 ] + ( 15.0 * xyz[ 1 ] ) + ( 3.0 * xyz[ 2 ] ) );
				luv[ 0 ] = y_to_l( xyz[ 1 ] );
				if ( luv[ 0 ] == 0.0 )
				{
					luv[ 1 ] = 0.0;
					luv[ 2 ] = 0.0;
					return;
				}
				luv[ 1 ] = 13.0 * luv[ 0 ] * ( varU - husl_ref_u );
				luv[ 2 ] = 13.0 * luv[ 0 ] * ( varV - husl_ref_v );
			}

			static void_c luv_to_xyz( float64_c const * luv, float64_c * xyz )
			{
				if ( luv[ 0 ] == 0.0 )
				{
					xyz[ 0 ] = 0.0;
					xyz[ 1 ] = 0.0;
					xyz[ 2 ] = 0.0;
					return;
				}
				float64_c varU = luv[ 1 ] / ( 13.0 * luv[ 0 ] ) + husl_ref_u;
				float64_c varV = luv[ 2 ] / ( 13.0 * luv[ 0 ] ) + husl_ref_v;
				xyz[ 1 ] = l_to_y( luv[ 0 ] );
				xyz[ 0 ] = 0.0 - ( 9.0 * xyz[ 1 ] * varU ) / ( ( varU - 4.0 ) * varV - varU * varV );
				xyz[ 2 ] = ( 9.0 * xyz[ 1 ] - ( 15.0 * varV * xyz[ 1 ] ) - ( varV * xyz[ 0 ] ) ) / ( 3.0 * varV );
			}

			static void_c luv_to_lch( float64_c const * luv, float64_c * lch )
			{
				lch[ 0 ] = luv[ 0 ];
				lch[ 1 ] = math_power( luv[ 1 ] * luv[ 1 ] + luv[ 2 ] * luv[ 2 ], 0.5 );
				float64_c Hrad = math_arc_tangent( luv[ 2 ], luv[ 1 ] );
				lch[ 2 ] = Hrad * 180.0 / constants< float64_c >::pi();
				if ( lch[ 2 ] < 0.0 )
				{
					lch[ 2 ] += 360.0;
				}
			}

			static void_c lch_to_luv( float64_c const * lch, float64_c * luv )
			{
				luv[ 0 ] = lch[ 0 ];
				float64_c Hrad = lch[ 2 ] / 360.0 * 2.0 * constants< float64_c >::pi();
				luv[ 1 ] = math_cosine( Hrad ) * lch[ 1 ];
				luv[ 2 ] = math_sine( Hrad ) * lch[ 1 ];
			}

			static void_c husl_to_lch( float64_c const * husl, float64_c * lch )
			{
				if ( husl[ 2 ] > 99.9999999 )
				{
					lch[ 0 ] = 100.0;
					lch[ 1 ] = 0.0;
					lch[ 2 ] = husl[ 0 ];
					return;
				}

				if ( husl[ 2 ] < 0.00000001 )
				{
					lch[ 0 ] = 0.0;
					lch[ 1 ] = 0.0;
					lch[ 2 ] = husl[ 0 ];
					return;
				}

				lch[ 0 ] = husl[ 2 ];
				float64_c max = max_chroma_for_lh( husl[ 2 ], husl[ 0 ] );
				lch[ 1 ] = max / 100 * husl[ 1 ];
				lch[ 2 ] = husl[ 0 ];
			}

			static void_c lch_to_husl( float64_c const * lch, float64_c * husl )
			{
				if ( lch[ 0 ] > 99.9999999 )
				{
					husl[ 0 ] = lch[ 2 ];
					husl[ 1 ] = 0.0;
					husl[ 2 ] = 100.0;
					return;
				}

				if ( lch[ 0 ] < 0.00000001 )
				{
					husl[ 0 ] = lch[ 2 ];
					husl[ 1 ] = 0.0;
					husl[ 2 ] = 0.0;
					return;
				}

				float64_c max = max_chroma_for_lh( lch[ 0 ], lch[ 2 ] );

				husl[ 0 ] = lch[ 2 ];
				husl[ 1 ] = lch[ 1 ] / max * 100.0;
				husl[ 2 ] = lch[ 0 ];
			}

			static void_c huslp_to_lch( float64_c const * huslp, float64_c * lch )
			{
				if ( huslp[ 2 ] > 99.9999999 )
				{
					lch[ 0 ] = 100.0;
					lch[ 1 ] = 0.0;
					lch[ 2 ] = huslp[ 0 ];
					return;
				}

				if ( huslp[ 2 ] < 0.00000001 )
				{
					lch[ 0 ] = 0.0;
					lch[ 1 ] = 0.0;
					lch[ 2 ] = huslp[ 0 ];
					return;
				}

				float64_c max = max_safe_chroma_for_l( huslp[ 2 ] );
				lch[ 0 ] = huslp[ 2 ];
				lch[ 1 ] = max / 100 * huslp[ 1 ];
				lch[ 2 ] = huslp[ 0 ];
			}

			static void_c lch_to_huslp( float64_c const * lch, float64_c * huslp )
			{
				if ( lch[ 0 ] > 99.9999999 )
				{
					huslp[ 0 ] = lch[ 2 ];
					huslp[ 1 ] = 0.0;
					huslp[ 2 ] = 100.0;
					return;
				}

				if ( lch[ 0 ] < 0.00000001 )
				{
					huslp[ 0 ] = lch[ 2 ];
					huslp[ 1 ] = 0.0;
					huslp[ 2 ] = 0.0;
					return;
				}

				double max = max_safe_chroma_for_l( lch[ 0 ] );
				huslp[ 0 ] = lch[ 2 ];
				huslp[ 1 ] = lch[ 1 ] / max * 100.0;
				huslp[ 2 ] = lch[ 0 ];
			}

			static void_c lch_to_rgb( float64_c const * lch, float64_c * rgb )
			{
				float64_c a[ 3 ];
				float64_c b[ 3 ];
				lch_to_luv( lch, a );
				luv_to_xyz( a, b );
				xyz_to_rgb( b, rgb );
			}

			static void_c rgb_to_lch( float64_c const * rgb, float64_c * lch )
			{
				float64_c a[ 3 ];
				float64_c b[ 3 ];
				rgb_to_xyz( rgb, a );
				xyz_to_luv( a, b );
				luv_to_lch( b, lch );
			}

			static void_c husl_to_rgb( float64_c const * husl, float64_c * rgb )
			{
				float64_c a[ 3 ];
				husl_to_lch( husl, a );
				lch_to_rgb( a, rgb );
			}

			static void_c rgb_to_husl( float64_c const * rgb, float64_c * husl )
			{
				float64_c a[ 3 ];
				rgb_to_lch( rgb, a );
				lch_to_husl( a, husl );
			}

			static void_c huslp_to_rgb( float64_c const * huslp, float64_c * rgb )
			{
				float64_c a[ 3 ];
				huslp_to_lch( huslp, a );
				lch_to_rgb( a, rgb );
			}

			static void_c rgb_to_huslp( float64_c const * rgb, float64_c * huslp )
			{
				float64_c a[ 3 ];
				rgb_to_lch( rgb, a );
				lch_to_huslp( a, huslp );
			}

		};

		boolean_c convert_rgb_to_husl( float64_c const * rgb, float64_c * husl )
		{
			husl_converter_c::rgb_to_husl( rgb, husl );
			return true;
		}

		boolean_c convert_husl_to_rgb( float64_c const * husl, float64_c * rgb )
		{
			husl_converter_c::husl_to_rgb( husl, rgb );
			return true;
		}

		vector32x4_c adjust_color_brightness( vector32x4_c const & color, float32_c amount )
		{
			vector32x4_c result;
			result.a = math_saturate( color.a + amount );
			result.b = math_saturate( color.b + amount );
			result.c = math_saturate( color.c + amount );
			result.d = color.d;
			return result;
		}

		vector32x4_c adjust_color_contrast( vector32x4_c const & color, float32_c amount )
		{
			amount = 1.0f + amount;
			vector32x4_c result;
			result.a = math_saturate( color.a * amount );
			result.b = math_saturate( color.b * amount );
			result.c = math_saturate( color.c * amount );
			result.d = color.d;
			return result;
		}

		vector32x4_c adjust_color_saturation( vector32x4_c const & color, float32_c amount )
		{
			amount = math_saturate( amount );
			vector32x4_c result;
			float32_c p = math_square_root( dot_product_float32( vector32x3_c( color.a, color.b, color.c ), vector32x3_c( 0.299f, 0.587f, 0.114f ) ) );
			result.a = ( ( color.a - p ) * amount ) + p;
			result.b = ( ( color.b - p ) * amount ) + p;
			result.c = ( ( color.c - p ) * amount ) + p;
			result.d = color.d;
			return result;
		}

	}
}
