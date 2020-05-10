#include "cheonsa__ops.h"
#include "third_party/strnatcmp.h"
#include "third_party/xxhash.h"
#include <cstdlib>
#include <chrono> // for random number generation.
#include <cassert>
#if defined( cheonsa_platform_windows )
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX // silly windows, leave us alone.
#include <Windows.h>
#include <Shlobj.h> // needed for SHGetKnownFolderPath, used to build shortcut list to common locations on windows.
#endif

namespace cheonsa
{
	namespace ops
	{


		//
		//
		// basic numeric operations.
		//
		//

		boolean_c math_is_real( float32_c a ) // is it?
		{
			return ( a == a ) && ( a >= -constants< float32_c >::maximum() && a <= constants< float32_c >::maximum() );
		}

		boolean_c math_is_real( float64_c a ) // proove it.
		{
			return ( a == a ) && ( a >= -constants< float64_c >::maximum() && a <= constants< float64_c >::maximum() );
		}

		boolean_c math_compare_close( float32_c a, float32_c b, float32_c margin )
		{
			return math_absolute_value( a - b ) <= margin;
		}

		boolean_c math_compare_close( float64_c a, float64_c b, float64_c margin )
		{
			return math_absolute_value( a - b ) <= margin;
		}

		sint32_c math_minimum( sint32_c a, sint32_c b )
		{
			return a < b ? a : b;
		}

		float32_c math_minimum( float32_c a, float32_c b )
		{
			return a < b ? a : b;
		}

		float64_c math_minimum( float64_c a, float64_c b )
		{
			return a < b ? a : b;
		}

		sint32_c math_maximum( sint32_c a, sint32_c b )
		{
			return a > b ? a : b;
		}

		float32_c math_maximum( float32_c a, float32_c b )
		{
			return a > b ? a : b;
		}

		float64_c math_maximum( float64_c a, float64_c b )
		{
			return a > b ? a : b;
		}

		sint32_c math_absolute_value( sint32_c a )
		{
			return abs( a );
		}

		float32_c math_absolute_value( float32_c a )
		{
			return abs( a );
		}

		float64_c math_absolute_value( float64_c a )
		{
			return abs( a );
		}

		sint16_c math_clamp( sint16_c value, sint16_c minimum, sint16_c maximum )
		{
			if ( value < minimum )
			{
				return minimum;
			}
			else if ( value > maximum )
			{
				return maximum;
			}
			else
			{
				return value;
			}
		}

		sint32_c math_clamp( sint32_c value, sint32_c minimum, sint32_c maximum )
		{
			if ( value < minimum )
			{
				return minimum;
			}
			else if ( value > maximum )
			{
				return maximum;
			}
			else
			{
				return value;
			}
		}

		float32_c math_clamp( float32_c value, float32_c minimum, float32_c maximum )
		{
			if ( value < minimum )
			{
				return minimum;
			}
			else if ( value > maximum )
			{
				return maximum;
			}
			else
			{
				return value;
			}
		}

		float64_c math_clamp( float64_c value, float64_c minimum, float64_c maximum )
		{
			if ( value < minimum )
			{
				return minimum;
			}
			else if ( value > maximum )
			{
				return maximum;
			}
			else
			{
				return value;
			}
		}

		sint32_c math_next_power_of_two( sint32_c a )
		{
			sint32_c result = 2;
			while ( result < a )
			{
				result *= 2;
			}
			return result;
		}

		uint32_c math_next_power_of_two( uint32_c a )
		{
			uint32_c result = 2;
			while ( result < a )
			{
				result *= 2;
			}
			return result;
		}

		float32_c math_saturate( float32_c value )
		{
			if ( value < 0.0f )
			{
				return 0.0f;
			}
			else if ( value > 1.0f )
			{
				return 1.0f;
			}
			else
			{
				return value;
			}
		}

		float64_c math_saturate( float64_c value )
		{
			if ( value < 0.0 )
			{
				return 0.0;
			}
			else if ( value > 1.0 )
			{
				return 1.0;
			}
			else
			{
				return value;
			}
		}

		float32_c math_nearest_multiple( float32_c value, float32_c multiple )
		{
			if ( value == 0.0 )
			{
				return 0.0;
			}
			float32_c half_multiple = multiple * 0.5f;
			float32_c count = math_round_down( ( value + ( value > 0.0f ? half_multiple : -half_multiple ) ) / multiple );
			return multiple * count;
		}

		float64_c math_nearest_multiple( float64_c value, float64_c multiple )
		{
			if ( value == 0.0 )
			{
				return 0.0;
			}
			float64_c half_multiple = multiple * 0.5;
			float64_c count = math_round_down( ( value + ( value > 0.0 ? half_multiple : -half_multiple ) ) / multiple );
			return multiple * count;
		}

		sint32_c math_next_nearest_multiple( sint32_c value, sint32_c multiple )
		{
			assert( multiple > 0 );
			sint32_c modulo = value % multiple;
			if ( modulo > 0 )
			{
				return value + ( multiple - modulo );
			}
			return value;
		}

		float32_c math_round( float32_c value )
		{
			return floor( value + 0.5f );
		}

		float64_c math_round( float64_c value )
		{
			return floor( value + 0.5 );
		}

		float32_c math_round_down( float32_c value )
		{
			return floor( value );
		}

		float64_c math_round_down( float64_c value )
		{
			return floor( value );
		}

		float32_c math_round_up( float32_c value )
		{
			return ceil( value );
		}

		float64_c math_round_up( float64_c value )
		{
			return ceil( value );
		}

		float32_c math_round_to_decimal_place( float32_c value, sint32_c places )
		{
			float64_c n = pow( 10, places );
			return static_cast< float32_c >( round( static_cast< float64_c >( value ) * n ) / n );
		}

		float64_c math_round_to_decimal_place( float64_c value, sint32_c places )
		{
			float64_c n = pow( 10, places );
			return round( value * n ) / n;
		}

		float32_c math_modulo( float32_c value, float32_c multiple )
		{
			return fmod( value, multiple );
		}

		float64_c math_modulo( float64_c value, float64_c multiple )
		{
			return fmod( value, multiple );
		}

		float32_c math_natural_logarithm( float32_c value )
		{
			return log( value );
		}

		float32_c math_square( float32_c value )
		{
			return value * value;
		}

		float64_c math_square( float64_c value )
		{
			return value * value;
		}

		float32_c math_cube( float32_c value )
		{
			return value * value * value;
		}

		float64_c math_cube( float64_c value )
		{
			return value * value * value;
		}

		sint32_c math_power( sint32_c value, sint32_c exponent )
		{
			sint32_c result = value;
			while ( exponent > 1 )
			{
				result *= result;
				exponent--;
			}
			return result;
		}

		float32_c math_power( float32_c value, float32_c exponent )
		{
			return pow( value, exponent );
		}

		float64_c math_power( float64_c value, float64_c exponent )
		{
			return pow( value, exponent );
		}

		float32_c math_root( float32_c value, float32_c exponent )
		{
			return pow( value, 1.0f / exponent );
		}

		float64_c math_root( float64_c value, float64_c exponent )
		{
			return pow( value, 1.0 / exponent );
		}

		sint32_c math_square_root( sint32_c value )
		{
			sint32_c result = 0;
			sint32_c bit = 1 << 30; // the second-to-top bit is set.

			// "bit" starts at the highest power of four <= the argument.
			while ( bit > value )
			{
				bit >>= 2;
			}

			while ( bit != 0 )
			{
				if ( value >= result + bit)
				{
					value -= result + bit;
					result += bit << 1;
				}
				result >>= 1;
				bit >>= 2;
			}

			return result;
		}

		float32_c math_square_root( float32_c value )
		{
			return sqrt( value );
		}

		float64_c math_square_root( float64_c value )
		{
			return sqrt( value );
		}

		float32_c math_sine( float32_c angle )
		{
			return sin( angle );
		}

		float64_c math_sine( float64_c angle )
		{
			return sin( angle );
		}

		float32_c math_arc_sine( float32_c dot )
		{
			return asin( dot );
		}

		float64_c math_arc_sine( float64_c dot )
		{
			return asin( dot );
		}

		float32_c math_cosine( float32_c angle )
		{
			return cos( angle );
		}

		float64_c math_cosine( float64_c angle )
		{
			return cos( angle );
		}

		float32_c math_arc_cosine( float32_c dot )
		{
			return acos( dot );
		}

		float64_c math_arc_cosine( float64_c dot )
		{
			return acos( dot );
		}

		float32_c math_tangent( float32_c angle )
		{
			return tan( angle );
		}

		float64_c math_tangent( float64_c angle )
		{
			return tan( angle );
		}

		float32_c math_arc_tangent( float32_c dot )
		{
			return atan( dot );
		}

		float64_c math_arc_tangent( float64_c dot )
		{
			return atan( dot );
		}

		float32_c math_arc_tangent( float32_c b, float32_c a )
		{
			return atan2( b, a );
		}

		float64_c math_arc_tangent( float64_c b, float64_c a )
		{
			return atan2( b, a );
		}

		float32_c math_wrap( float32_c value, float32_c range )
		{
			value = fmod( value, range );
			if ( value < 0.0f )
			{
				value += range;
			}
			return value;
		}

		float64_c math_wrap( float64_c value, float64_c range )
		{
			value = fmod( value, range );
			if ( value < 0.0 )
			{
				value += range;
			}
			return value;
		}

		float32_c math_find_shortest_angle_difference( float32_c angle_a, float32_c angle_b )
		{
			angle_a = math_wrap( angle_a, constants< float32_c >::pi() * 2.0f );
			angle_b = math_wrap( angle_b, constants< float32_c >::pi() * 2.0f );
			float32_c difference = angle_b - angle_a;
			if ( difference > constants< float32_c >::pi() )
			{
				difference -= constants< float32_c >::pi() * 2.0f;
			}
			else if ( difference < -constants< float32_c >::pi() )
			{
				difference += constants< float32_c >::pi() * 2.0f;
			}
			return difference;
		}

		float64_c math_find_shortest_angle_difference( float64_c angle_a, float64_c angle_b )
		{
			angle_a = math_wrap( angle_a, constants< float64_c >::pi() * 2.0 );
			angle_b = math_wrap( angle_b, constants< float64_c >::pi() * 2.0 );
			float64_c difference = angle_b - angle_a;
			if ( difference > constants< float64_c >::pi() )
			{
				difference -= constants< float64_c >::pi() * 2.0f;
			}
			else if ( difference < -constants< float64_c >::pi() )
			{
				difference += constants< float64_c >::pi() * 2.0f;
			}
			return difference;
		}

		float32_c math_find_angle_around_axis( vector32x3_c const & tangent_to_axis_that_defines_angle_zero, vector32x3_c const & tangent_to_axis_that_defines_angle_quarter, vector32x3_c const & tangent_to_find_angle_of )
		{
			// this function returns the number of radians that tangent_to_find_angle_of is away from.
			float32_c dot_forward = dot_product_float32( tangent_to_find_angle_of, tangent_to_axis_that_defines_angle_zero ); // we will use this value to determine the base foreward angle.
			float32_c dot_right = dot_product_float32( tangent_to_find_angle_of, tangent_to_axis_that_defines_angle_quarter ); // we will use this value to determine if the result needs to be pushed past pi radians (180 degrees).
			return math_arc_cosine( dot_forward ) * dot_right > 0.0f ? -1.0f : 1.0f; // clock-wise : counter-clock-wise.
		}

		float64_c math_find_angle_around_axis( vector64x3_c const & tangent_to_axis_that_defines_angle_zero, vector64x3_c const & tangent_to_axis_that_defines_angle_quarter, vector64x3_c const & tangent_to_find_angle_of )
		{
			// this function returns the number of radians that tangent_to_find_angle_of is away from.
			float64_c dot_forward = dot_product_float64( tangent_to_find_angle_of, tangent_to_axis_that_defines_angle_zero ); // we will use this value to determine the base foreward angle.
			float64_c dot_right = dot_product_float64( tangent_to_find_angle_of, tangent_to_axis_that_defines_angle_quarter ); // we will use this value to determine if the result needs to be pushed past pi radians (180 degrees).
			return math_arc_cosine( dot_forward ) * dot_right > 0.0 ? -1.0 : 1.0; // clock-wise : counter-clock-wise.
		}

		float64_c math_wobble( float64_c time, float64_c frequency )
		{
			time *= constants< float64_c >::pi() * 2.0 * frequency;
			return ( math_sine( time ) + math_sine( time * 0.666 ) + math_sine( time * 1.333 ) ) * 0.333;
		}

		sint32_c math_sign( float32_c value )
		{
			return ( 0.0f < value ) - ( value < 0.0f );
		}

		sint32_c math_sign( float64_c value )
		{
			return ( 0.0 < value ) - ( value < 0.0 );
		}

		sint32_c math_non_zero_sign( float32_c value )
		{
			return 2 * ( value > 0.0f ) - 1;
		}

		sint32_c math_non_zero_sign( float64_c value )
		{
			return 2 * ( value > 0.0 ) - 1;
		}




		//
		//
		// vector operations.
		//
		//

		float32_c length_squared_float32( vector32x2_c const & vector )
		{
			return ( vector.a * vector.a ) + ( vector.b * vector.b );
		}

		float64_c length_squared_float64( vector64x2_c const & vector )
		{
			return ( vector.a * vector.a ) + ( vector.b * vector.b );
		}

		float32_c length_squared_float32( vector32x3_c const & vector )
		{
			return ( vector.a * vector.a ) + ( vector.b * vector.b ) + ( vector.c * vector.c );
		}

		float64_c length_squared_float64( vector64x3_c const & vector )
		{
			return ( vector.a * vector.a ) + ( vector.b * vector.b ) + ( vector.c * vector.c );
		}

		float32_c length_squared_float32( vector32x4_c const & vector )
		{
			return ( vector.a * vector.a ) + ( vector.b * vector.b ) + ( vector.c * vector.c ) + ( vector.d * vector.d );
		}

		float64_c length_squared_float64( vector64x4_c const & vector )
		{
			return ( vector.a * vector.a ) + ( vector.b * vector.b ) + ( vector.c * vector.c ) + ( vector.d * vector.d );
		}

		float32_c length_float32( vector32x2_c const & vector )
		{
			return math_square_root( ( vector.a * vector.a ) + ( vector.b * vector.b ) );
		}

		float64_c length_float64( vector64x2_c const & vector )
		{
			return math_square_root( ( vector.a * vector.a ) + ( vector.b * vector.b ) );
		}

		float32_c length_float32( vector32x3_c const & vector )
		{
			return math_square_root( ( vector.a * vector.a ) + ( vector.b * vector.b ) + ( vector.c * vector.c ) );
		}

		float64_c length_float64( vector64x3_c const & vector )
		{
			return math_square_root( ( vector.a * vector.a ) + ( vector.b * vector.b ) + ( vector.c * vector.c ) );
		}

		float32_c length_float32( vector32x4_c const & vector )
		{
			return math_square_root( ( vector.a * vector.a ) + ( vector.b * vector.b ) + ( vector.c * vector.c ) + ( vector.d * vector.d ) );
		}

		float64_c length_float64( vector64x4_c const & vector )
		{
			return math_square_root( ( vector.a * vector.a ) + ( vector.b * vector.b ) + ( vector.c * vector.c ) + ( vector.d * vector.d ) );
		}

		vector32x2_c normal_vector32x2( vector32x2_c const & vector )
		{
			vector32x2_c result;
			float32_c a_vector_length = length_float32( vector );
			if ( a_vector_length > constants< float32_c >::division_near_zero() )
			{
				result.a = vector.a / a_vector_length;
				result.b = vector.b / a_vector_length;
			}
			return result;
		}

		vector64x2_c normal_vector64x2( vector64x2_c const & vector )
		{
			vector64x2_c result;
			float64_c a_vector_length = length_float64( vector );
			if ( a_vector_length > constants< float64_c >::division_near_zero() )
			{
				result.a = vector.a / a_vector_length;
				result.b = vector.b / a_vector_length;
			}
			return result;
		}

		vector32x3_c normal_vector32x3( vector32x3_c const & vector )
		{
			vector32x3_c result;
			float32_c a_vector_length = length_float32( vector );
			if ( a_vector_length > constants< float32_c >::division_near_zero() )
			{
				result.a = vector.a / a_vector_length;
				result.b = vector.b / a_vector_length;
				result.c = vector.c / a_vector_length;
			}
			return result;
		}

		vector64x3_c normal_vector64x3( vector64x3_c const & vector )
		{
			vector64x3_c result;
			float64_c a_vector_length = length_float64( vector );
			if ( a_vector_length > constants< float64_c >::division_near_zero() )
			{
				result.a = vector.a / a_vector_length;
				result.b = vector.b / a_vector_length;
				result.c = vector.c / a_vector_length;
			}
			return result;
		}

		vector32x4_c normal_vector32x4( vector32x4_c const & vector )
		{
			vector32x4_c result;
			float32_c a_vector_length = length_float32( vector );
			if ( a_vector_length > constants< float32_c >::division_near_zero() )
			{
				result.a = vector.a / a_vector_length;
				result.b = vector.b / a_vector_length;
				result.c = vector.c / a_vector_length;
				result.d = vector.d / a_vector_length;
			}
			return result;
		}

		vector64x4_c normal_vector64x4( vector64x4_c const & vector )
		{
			vector64x4_c result;
			float64_c a_vector_length = length_float64( vector );
			if ( a_vector_length > constants< float64_c >::division_near_zero() )
			{
				result.a = vector.a / a_vector_length;
				result.b = vector.b / a_vector_length;
				result.c = vector.c / a_vector_length;
				result.d = vector.d / a_vector_length;
			}
			return result;
		}

		quaternion32_c normal_quaternion32( quaternion32_c const & quaternion )
		{
			quaternion32_c result;
			float32_c length_inverse = 1.0f / math_square_root( ( quaternion.a * quaternion.a ) + ( quaternion.b * quaternion.b ) + ( quaternion.c * quaternion.c ) + ( quaternion.d * quaternion.d ) );
			result.a = quaternion.a * length_inverse;
			result.b = quaternion.b * length_inverse;
			result.c = quaternion.c * length_inverse;
			result.d = quaternion.d * length_inverse;
			return result;
		}

		void_c normalize_quaternion32( quaternion32_c & quaternion )
		{
			float32_c length_inverse = 1.0f / math_square_root( ( quaternion.a * quaternion.a ) + ( quaternion.b * quaternion.b ) + ( quaternion.c * quaternion.c ) + ( quaternion.d * quaternion.d ) );
			quaternion.a /= length_inverse;
			quaternion.b /= length_inverse;
			quaternion.c /= length_inverse;
			quaternion.d /= length_inverse;
		}

		float32_c dot_product_float32( vector32x2_c const & vector_a, vector32x2_c const & vector_b )
		{
			return ( vector_a.a * vector_b.a ) + ( vector_a.b * vector_b.b );
		}

		float64_c dot_product_float64( vector64x2_c const & vector_a, vector64x2_c const & vector_b )
		{
			return ( vector_a.a * vector_b.a ) + ( vector_a.b * vector_b.b );
		}

		float32_c dot_product_float32( vector32x3_c const & vector_a, vector32x3_c const & vector_b )
		{
			return ( vector_a.a * vector_b.a ) + ( vector_a.b * vector_b.b ) + ( vector_a.c * vector_b.c );
		}

		float64_c dot_product_float64( vector64x3_c const & vector_a, vector64x3_c const & vector_b )
		{
			return ( vector_a.a * vector_b.a ) + ( vector_a.b * vector_b.b ) + ( vector_a.c * vector_b.c );
		}

		float32_c dot_product_float32( vector32x4_c const & vector_a, vector32x4_c const & vector_b )
		{
			return ( vector_a.a * vector_b.a ) + ( vector_a.b * vector_b.b ) + ( vector_a.c * vector_b.c ) + ( vector_a.d * vector_b.d );
		}

		float64_c dot_product_float64( vector64x4_c const & vector_a, vector64x4_c const & vector_b )
		{
			return ( vector_a.a * vector_b.a ) + ( vector_a.b * vector_b.b ) + ( vector_a.c * vector_b.c ) + ( vector_a.d * vector_b.d );
		}

		float32_c dot_product_float32( quaternion32_c const & quaternion_a, quaternion32_c const & quaternion_b )
		{
			return ( quaternion_a.a * quaternion_b.a ) + ( quaternion_a.b * quaternion_b.b ) + ( quaternion_a.c * quaternion_b.c ) + ( quaternion_a.d * quaternion_b.d );
		}

		vector32x3_c cross_product_vector32x3( vector32x3_c const & vector_a, vector32x3_c const & vector_b )
		{
			return vector32x3_c( ( vector_a.b * vector_b.c ) - ( vector_a.c * vector_b.b ), ( vector_a.c * vector_b.a ) - ( vector_a.a * vector_b.c ), ( vector_a.a * vector_b.b ) - ( vector_a.b * vector_b.a ) );
		}

		vector64x3_c cross_product_vector64x3( vector64x3_c const & vector_a, vector64x3_c const & vector_b )
		{
			return vector64x3_c( ( vector_a.b * vector_b.c ) - ( vector_a.c * vector_b.b ), ( vector_a.c * vector_b.a ) - ( vector_a.a * vector_b.c ), ( vector_a.a * vector_b.b ) - ( vector_a.b * vector_b.a ) );
		}

		vector32x2_c orthogonalized_vector32x2( vector32x2_c const & vector )
		{
			return vector32x2_c( vector.b, -vector.a );
		}

		vector64x2_c orthogonalized_vector64x2( vector64x2_c const & vector )
		{
			return vector64x2_c( vector.b, -vector.a );
		}

		vector32x2_c orthonormalized_vector32x2( vector32x2_c const & vector, boolean_c polarity, boolean_c allow_zero )
		{
			float32_c length = length_float32( vector );
			if ( length == 0.0f )
			{
				return polarity ? vector32x2_c( 0.0f, !allow_zero ? 1.0f : 0.0f ) : vector32x2_c( 0.0f, !allow_zero ? -1.0f : 0.0f );
			}
			return polarity ? vector32x2_c( -vector.b / length, vector.a / length ) : vector32x2_c( vector.b / length, -vector.a / length );
		}

		vector64x2_c orthonormalized_vector64x2( vector64x2_c const & vector, boolean_c polarity, boolean_c allow_zero )
		{
			float64_c length = length_float64( vector );
			if ( length == 0.0 )
			{
				return polarity ? vector64x2_c( 0.0, !allow_zero ) : vector64x2_c( 0.0, -!allow_zero );
			}
			return polarity ? vector64x2_c( -vector.b / length, vector.a / length ) : vector64x2_c( vector.b / length, -vector.a / length );
		}

		vector32x3_c rotate_vector32x3( vector32x3_c const & vector, quaternion32_c const & rotation )
		{
			// nVidia SDK implementation
			vector32x3_c qvec( rotation.a, rotation.b, rotation.c );
			vector32x3_c uv = cross_product_vector32x3( qvec, vector );
			vector32x3_c uuv = cross_product_vector32x3( qvec, uv ) * 2.0f;
			uv *= 2.0f * rotation.d;
			return vector + uv + uuv;
		}

		vector64x3_c rotate_vector64x3( vector64x3_c const & vector, quaternion32_c const & rotation )
		{
			// 16 multiplications, 6 additions, 6 subtractions.
			// nVidia SDK implementation
			vector64x3_c qvec( rotation.a, rotation.b, rotation.c );
			vector64x3_c uv = cross_product_vector64x3( qvec, vector ); // 6 multiplications, 3 subtractions.
			vector64x3_c uuv = cross_product_vector64x3( qvec, uv ) * 2.0f; // 6 multiplications, 3 subtractions.
			uv *= 2.0 * rotation.d; // 4 multiplications.
			return vector + uv + uuv; // 6 additions.
		}

		vector32x2_c rotate_vector32x2( vector32x2_c const & point, float32_c const angle )
		{
			float32_c s = math_sine( angle );
			float32_c c = math_cosine( angle );
			return vector32x2_c( point.a * c - point.b * s, point.a * s + point.b * c );
		}

		vector64x2_c rotate_vector64x2( vector64x2_c const & point, float64_c const angle )
		{
			float64_c s = math_sine( angle );
			float64_c c = math_cosine( angle );
			return vector64x2_c( point.a * c - point.b * s, point.a * s + point.b * c );
		}

		vector32x3_c rotate_vector32x3( vector32x3_c const & point, vector32x3_c const & axis, float32_c const angle )
		{
			float32_c s = math_sine( angle );
			float32_c c = math_cosine( angle );
			return ( point * c ) + ( axis * dot_product_float32( axis, point ) * ( 1.0f - c ) ) + ( cross_product_vector32x3( point, axis ) * s );
		}

		vector64x3_c rotate_vector64x3( vector64x3_c const & point, vector64x3_c const & axis, float64_c const angle )
		{
			float64_c s = math_sine( angle );
			float64_c c = math_cosine( angle );
			return ( point * c ) + ( axis * dot_product_float64( axis, point ) * ( 1.0f - c ) ) + ( cross_product_vector64x3( point, axis ) * s );
		}

		vector32x2_c rotate_and_scale_vector32x2( vector32x2_c const & point, matrix32x2x2_c const & transform )
		{
			return vector32x2_c( ( point.a * transform.a.a ) + ( point.b * transform.b.a ), ( point.a * transform.a.b ) + ( point.b * transform.b.b ) );
		}

		vector32x3_c rotate_and_scale_vector32x3( vector32x3_c const & point, matrix32x3x3_c const & transform )
		{
			// 9 multiplications, 6 additions.
			vector32x3_c result;
			result.a = ( transform.a.a * point.a ) + ( transform.b.a * point.b ) + ( transform.c.a * point.c );
			result.b = ( transform.a.b * point.a ) + ( transform.b.b * point.b ) + ( transform.c.b * point.c );
			result.c = ( transform.a.c * point.a ) + ( transform.b.c * point.b ) + ( transform.c.c * point.c );
			return result;
		}

		vector64x3_c rotate_and_scale_vector64x3( vector64x3_c const & point, matrix32x3x3_c const & transform )
		{
			// 9 multiplications, 6 additions.
			vector64x3_c result;
			result.a = ( transform.a.a * point.a ) + ( transform.b.a * point.b ) + ( transform.c.a * point.c );
			result.b = ( transform.a.b * point.a ) + ( transform.b.b * point.b ) + ( transform.c.b * point.c );
			result.c = ( transform.a.c * point.a ) + ( transform.b.c * point.b ) + ( transform.c.c * point.c );
			return result;
		}

		vector32x3_c rotate_and_scale_vector32x3( vector32x3_c const & vector, matrix32x4x4_c const & transform )
		{
			// 9 multiplications, 6 additions.
			vector32x3_c result;
			result.a = ( transform.a.a * vector.a ) + ( transform.b.a * vector.b ) + ( transform.c.a * vector.c );
			result.b = ( transform.a.b * vector.a ) + ( transform.b.b * vector.b ) + ( transform.c.b * vector.c );
			result.c = ( transform.a.c * vector.a ) + ( transform.b.c * vector.b ) + ( transform.c.c * vector.c );
			return result;
		}

		vector64x3_c rotate_and_scale_vector64x3( vector64x3_c const & vector, matrix32x4x4_c const & transform )
		{
			// 9 multiplications, 9 additions.
			vector64x3_c result;
			result.a = ( transform.a.a * vector.a ) + ( transform.b.a * vector.b ) + ( transform.c.a * vector.c );
			result.b = ( transform.a.b * vector.a ) + ( transform.b.b * vector.b ) + ( transform.c.b * vector.c );
			result.c = ( transform.a.c * vector.a ) + ( transform.b.c * vector.b ) + ( transform.c.c * vector.c );
			return result;
		}

		vector32x3_c rotate_scale_and_translate_vector32x3( vector32x3_c const & point, matrix32x4x4_c const & transform )
		{
			// 9 multiplications, 9 additions.
			vector32x3_c result;
			result.a = ( transform.a.a * point.a ) + ( transform.b.a * point.b ) + ( transform.c.a * point.c ) + ( transform.d.a );
			result.b = ( transform.a.b * point.a ) + ( transform.b.b * point.b ) + ( transform.c.b * point.c ) + ( transform.d.b );
			result.c = ( transform.a.c * point.a ) + ( transform.b.c * point.b ) + ( transform.c.c * point.c ) + ( transform.d.c );
			return result;
		}

		vector64x3_c rotate_scale_and_translate_vector64x3( vector64x3_c const & point, matrix32x4x4_c const & transform )
		{
			// 9 multiplications, 9 additions.
			vector64x3_c result;
			result.a = ( transform.a.a * point.a ) + ( transform.b.a * point.b ) + ( transform.c.a * point.c ) + ( transform.d.a );
			result.b = ( transform.a.b * point.a ) + ( transform.b.b * point.b ) + ( transform.c.b * point.c ) + ( transform.d.b );
			result.c = ( transform.a.c * point.a ) + ( transform.b.c * point.b ) + ( transform.c.c * point.c ) + ( transform.d.c );
			return result;
		}

		vector32x4_c rotate_scale_and_translate_vector32x4( vector32x3_c const & point, matrix32x4x4_c const & transform )
		{
			// 12 multiplications, 12 additions.
			vector32x4_c result;
			result.a = ( transform.a.a * point.a ) + ( transform.b.a * point.b ) + ( transform.c.a * point.c ) + ( transform.d.a );
			result.b = ( transform.a.b * point.a ) + ( transform.b.b * point.b ) + ( transform.c.b * point.c ) + ( transform.d.b );
			result.c = ( transform.a.c * point.a ) + ( transform.b.c * point.b ) + ( transform.c.c * point.c ) + ( transform.d.c );
			result.d = ( transform.a.d * point.a ) + ( transform.b.d * point.b ) + ( transform.c.d * point.c ) + ( transform.d.d );
			return result;
		}

		vector64x3_c rotate_scale_and_translate_vector64x3( vector64x3_c const & point, transform3d_c const & transform )
		{
			vector64x3_c result;
			result = rotate_vector64x3( point, transform.rotation ) * transform.scale + transform.position;
			return result;
		}

		vector32x3_c constrained_normal_vector32x3( vector32x3_c const & normal, vector32x3_c const & normal_target, float32_c const maximum_angle )
		{
			float32_c angle = math_arc_cosine( math_clamp( dot_product_float32( normal, normal_target ), -1.0f, 1.0f ) );
			if ( angle > maximum_angle )
			{
				return rotate_vector32x3( normal_target, cross_product_vector32x3( normal, normal_target ), maximum_angle );
			}
			return normal;
		}

		vector64x3_c constrained_normal_vector64x3( vector64x3_c const & normal, vector64x3_c const & normal_target, float64_c const maximum_angle )
		{
			float64_c angle = math_arc_cosine( math_clamp( dot_product_float64( normal, normal_target ), -1.0, 1.0 ) );
			if ( angle > maximum_angle )
			{
				return rotate_vector64x3( normal_target, cross_product_vector64x3( normal, normal_target ), maximum_angle );
			}
			return normal;
		}

		vector64x3_c reflected_vector64x3( vector64x3_c const & direction_vector, vector64x3_c const & normal_vector )
		{
			return direction_vector - ( normal_vector * ( dot_product_float64( direction_vector, normal_vector ) * 2.0 ) );
		}

		vector64x3_c reflected_vector64x3( vector64x3_c const & point, plane64_c const & plane )
		{
			return point + ( plane.get_normal() * ( distance_between_point_and_plane( point , plane ) * -2.0 ) );
		}

		vector64x3_c parallel_component_vector64x3( vector64x3_c const & normal_vector, vector64x3_c const & other_vector )
		{
			return normal_vector * dot_product_float64( normal_vector, other_vector );
		}

		vector64x3_c perpendicular_component_vector64x3( vector64x3_c const & normal_vector, vector64x3_c const & other_vector )
		{
			return other_vector - parallel_component_vector64x3( normal_vector, other_vector );
		}

		vector32x3_c parallel_component_vector32x3( vector32x3_c const & normal_vector, vector32x3_c const & other_vector )
		{
			return normal_vector * dot_product_float32( normal_vector, other_vector );
		}

		vector32x3_c perpendicular_component_vector32x3( vector32x3_c const & normal_vector, vector32x3_c const & other_vector )
		{
			return other_vector - parallel_component_vector32x3( normal_vector, other_vector );
		}


		//
		//
		// matrix, quaternion, and angle conversions.
		//
		//

		vector32x3_c basis_vector32x3_x_from_rotation_quaternion32( quaternion32_c const & rotation )
		{
			vector32x3_c result;
			result.a = ( 1.0f - 2.0f * rotation.b * rotation.b - 2.0f * rotation.c * rotation.c );
			result.b = (        2.0f * rotation.a * rotation.b + 2.0f * rotation.c * rotation.d );
			result.c = (        2.0f * rotation.a * rotation.c - 2.0f * rotation.b * rotation.d );
			return result;
		}

		vector32x3_c basis_vector32x3_y_from_rotation_quaternion32( quaternion32_c const & rotation )
		{
			vector32x3_c result;
			result.a = (        2.0f * rotation.a * rotation.b - 2.0f * rotation.c * rotation.d );
			result.b = ( 1.0f - 2.0f * rotation.a * rotation.a - 2.0f * rotation.c * rotation.c );
			result.c = (        2.0f * rotation.c * rotation.b + 2.0f * rotation.a * rotation.d );
			return result;
		}

		vector32x3_c basis_vector32x3_z_from_rotation_quaternion32( quaternion32_c const & rotation )
		{
			vector32x3_c result;
			result.a = (        2.0f * rotation.a * rotation.c + 2.0f * rotation.b * rotation.d );
			result.b = (        2.0f * rotation.c * rotation.b - 2.0f * rotation.a * rotation.d );
			result.c = ( 1.0f - 2.0f * rotation.a * rotation.a - 2.0f * rotation.b * rotation.b );
			return result;
		}

		inline matrix32x4x4_c make_matrix32x4x4_basis_from_quaternion32_blender( quaternion32_c const & rotation )
		{
			// taken from blender, math_rotation.c, quat_to_mat4.
			// this isn't used right now, but it's here for reference.
			matrix32x4x4_c result;
			float64_c q0 = constants< float64_c >::square_root_of_2() * static_cast< float64_c >( rotation.d );
			float64_c q1 = constants< float64_c >::square_root_of_2() * static_cast< float64_c >( rotation.a );
			float64_c q2 = constants< float64_c >::square_root_of_2() * static_cast< float64_c >( rotation.b );
			float64_c q3 = constants< float64_c >::square_root_of_2() * static_cast< float64_c >( rotation.c );
			float64_c qda = q0 * q1;
			float64_c qdb = q0 * q2;
			float64_c qdc = q0 * q3;
			float64_c qaa = q1 * q1;
			float64_c qab = q1 * q2;
			float64_c qac = q1 * q3;
			float64_c qbb = q2 * q2;
			float64_c qbc = q2 * q3;
			float64_c qcc = q3 * q3;
			result.a.a = static_cast< float32_c >(  1.0 - qbb - qcc );
			result.a.b = static_cast< float32_c >(  qdc + qab );
			result.a.c = static_cast< float32_c >( -qdb + qac );
			result.b.a = static_cast< float32_c >( -qdc + qab );
			result.b.b = static_cast< float32_c >(  1.0 - qaa - qcc );
			result.b.c = static_cast< float32_c >(  qda + qbc );
			result.c.a = static_cast< float32_c >(  qdb + qac );
			result.c.b = static_cast< float32_c >( -qda + qbc );
			result.c.c = static_cast< float32_c >(  1.0 - qaa - qbb );
			return result;
		}

		inline matrix32x4x4_c make_matrix32x4x4_basis_from_quaternion32_irrlicht( quaternion32_c const & rotation )
		{
			// taken from iirlicht.
			// this isn't used right now, but it's here for reference.
			matrix32x4x4_c result;
			result.a.a = ( 1.0f - 2.0f * rotation.b * rotation.b - 2.0f * rotation.c * rotation.c );
			result.a.b = ( 2.0f * rotation.a * rotation.b + 2.0f * rotation.c * rotation.d );
			result.a.c = ( 2.0f * rotation.a * rotation.c - 2.0f * rotation.b * rotation.d );
			result.b.a = ( 2.0f * rotation.a * rotation.b - 2.0f * rotation.c * rotation.d );
			result.b.b = ( 1.0f - 2.0f * rotation.a * rotation.a - 2.0f * rotation.c * rotation.c );
			result.b.c = ( 2.0f * rotation.c * rotation.b + 2.0f * rotation.a * rotation.d );
			result.c.a = ( 2.0f * rotation.a * rotation.c + 2.0f * rotation.b * rotation.d );
			result.c.b = ( 2.0f * rotation.c * rotation.b - 2.0f * rotation.a * rotation.d );
			result.c.c = ( 1.0f - 2.0f * rotation.a * rotation.a - 2.0f * rotation.b * rotation.b );
			return result;
		}

		matrix32x4x4_c transform_matrix32x4x4_c_from_transform3d( transform3d_c const & transform )\
		{
			matrix32x4x4_c result = basis_matrix32x4x4_from_rotation_quaternion32( transform.rotation );
			result.d.a = static_cast< float32_c >( transform.position.a );
			result.d.b = static_cast< float32_c >( transform.position.b );
			result.d.c = static_cast< float32_c >( transform.position.c );
			return result;
		}

		matrix32x4x4_c transform_matrix32x4x4_c_from_transform3d( transform3d_c const & transform, vector64x3_c const & relative_to_origin )
		{
			matrix32x4x4_c result = basis_matrix32x4x4_from_rotation_quaternion32( transform.rotation );
			result.d.a = static_cast< float32_c >( transform.position.a - relative_to_origin.a );
			result.d.b = static_cast< float32_c >( transform.position.b - relative_to_origin.b );
			result.d.c = static_cast< float32_c >( transform.position.c - relative_to_origin.c );
			return result;
		}

		matrix32x4x4_c basis_matrix32x4x4_from_rotation_quaternion32( quaternion32_c const & rotation )
		{
			return make_matrix32x4x4_basis_from_quaternion32_blender( rotation );
		}

		inline matrix32x3x3_c basis_matrix32x3x3_from_rotation_quaternion32_blender( quaternion32_c const & rotation )
		{
			// taken from blender, math_rotation.c, quat_to_mat4.
			matrix32x3x3_c result;
			float64_c q0 = constants< float64_c >::square_root_of_2() * static_cast< float64_c >( rotation.d );
			float64_c q1 = constants< float64_c >::square_root_of_2() * static_cast< float64_c >( rotation.a );
			float64_c q2 = constants< float64_c >::square_root_of_2() * static_cast< float64_c >( rotation.b );
			float64_c q3 = constants< float64_c >::square_root_of_2() * static_cast< float64_c >( rotation.c );
			float64_c qda = q0 * q1;
			float64_c qdb = q0 * q2;
			float64_c qdc = q0 * q3;
			float64_c qaa = q1 * q1;
			float64_c qab = q1 * q2;
			float64_c qac = q1 * q3;
			float64_c qbb = q2 * q2;
			float64_c qbc = q2 * q3;
			float64_c qcc = q3 * q3;
			result.a.a = static_cast< float32_c >(  1.0 - qbb - qcc );
			result.a.b = static_cast< float32_c >(  qdc + qab );
			result.a.c = static_cast< float32_c >( -qdb + qac );
			result.b.a = static_cast< float32_c >( -qdc + qab );
			result.b.b = static_cast< float32_c >(  1.0 - qaa - qcc );
			result.b.c = static_cast< float32_c >(  qda + qbc );
			result.c.a = static_cast< float32_c >(  qdb + qac );
			result.c.b = static_cast< float32_c >( -qda + qbc );
			result.c.c = static_cast< float32_c >(  1.0 - qaa - qbb );
			return result;
		}

		inline matrix32x3x3_c basis_matrix32x3x3_from_rotation_quaternion32_irrlicht( quaternion32_c const & rotation )
		{
			// quaternion must be normalized already.
			// taken from iirlich, quaternion.h, inline void quaternion::getMatrixFast( matrix4 &dest) const.
			matrix32x3x3_c result;
			result.a.a = ( 1.0f - 2.0f * rotation.b * rotation.b - 2.0f * rotation.c * rotation.c );
			result.a.b = ( 2.0f * rotation.a * rotation.b + 2.0f * rotation.c * rotation.d );
			result.a.c = ( 2.0f * rotation.a * rotation.c - 2.0f * rotation.b * rotation.d );
			result.b.a = ( 2.0f * rotation.a * rotation.b - 2.0f * rotation.c * rotation.d );
			result.b.b = ( 1.0f - 2.0f * rotation.a * rotation.a - 2.0f * rotation.c * rotation.c );
			result.b.c = ( 2.0f * rotation.c * rotation.b + 2.0f * rotation.a * rotation.d );
			result.c.a = ( 2.0f * rotation.a * rotation.c + 2.0f * rotation.b * rotation.d );
			result.c.b = ( 2.0f * rotation.c * rotation.b - 2.0f * rotation.a * rotation.d );
			result.c.c = ( 1.0f - 2.0f * rotation.a * rotation.a - 2.0f * rotation.b * rotation.b );
			return result;
		}

		inline quaternion32_c rotation_quaternion32_from_basis_matrix32x3x3_blender( matrix32x3x3_c const & basis )
		{
			// basis must be normalized already.
			// taken from blender, math_rotation.c, mat3_normalized_to_quat.
			quaternion32_c result;
			float64_c s;
			float64_c tr = 0.25 * static_cast< float64_c >( 1.0f + basis.a.a + basis.b.b + basis.c.c );
			if ( tr > 1e-4 )
			{
				s = math_square_root( tr );
				result.d = static_cast< float32_c >( s );
				s = 1.0 / ( 4.0 * s );
				result.a = static_cast< float32_c >( static_cast< float64_c >( basis.b.c - basis.c.b ) * s );
				result.b = static_cast< float32_c >( static_cast< float64_c >( basis.c.a - basis.a.c ) * s );
				result.c = static_cast< float32_c >( static_cast< float64_c >( basis.a.b - basis.b.a ) * s );
			}
			else
			{
				if ( basis.a.a > basis.b.b && basis.a.a > basis.c.c )
				{
					s = 2.0f * math_square_root( 1.0f + basis.a.a - basis.b.b - basis.c.c );
					result.a = static_cast< float32_c >( 0.25 * s );
					s = 1.0 / s;
					result.d = static_cast< float32_c >( static_cast< float64_c >( basis.b.c - basis.c.b ) * s );
					result.b = static_cast< float32_c >( static_cast< float64_c >( basis.b.a + basis.a.b ) * s );
					result.c = static_cast< float32_c >( static_cast< float64_c >( basis.c.a + basis.a.c ) * s );
				}
				else if ( basis.b.b > basis.c.c )
				{
					s = 2.0f * math_square_root( 1.0f + basis.b.b - basis.a.a - basis.c.c );
					result.b = static_cast< float32_c >( 0.25 * s );
					s = 1.0 / s;
					result.d = static_cast< float32_c >( static_cast< float64_c >( basis.c.a - basis.a.c ) * s );
					result.a = static_cast< float32_c >( static_cast< float64_c >( basis.b.a + basis.a.b ) * s );
					result.c = static_cast< float32_c >( static_cast< float64_c >( basis.c.b + basis.b.c ) * s );
				}
				else
				{
					s = 2.0f * math_square_root( 1.0f + basis.c.c - basis.a.a - basis.b.b );
					result.c = static_cast< float32_c >( 0.25 * s );
					s = 1.0 / s;
					result.d = static_cast< float32_c >( static_cast< float64_c >( basis.a.b - basis.b.a ) * s );
					result.a = static_cast< float32_c >( static_cast< float64_c >( basis.c.a + basis.a.c ) * s );
					result.b = static_cast< float32_c >( static_cast< float64_c >( basis.c.b + basis.b.c ) * s );
				}
			}
			result = normal_quaternion32( result );
			return result;
		}

		inline quaternion32_c rotation_quaternion32_from_basis_matrix32x3x3_irrlicht( matrix32x3x3_c const & basis )
		{
			// basis does not need to be normalized.
			// taken from irrlicht, quaternion.h, inline quaternion& quaternion::operator=(const matrix4& m).
			quaternion32_c result;

			float32_c diagonal = basis.a.a + basis.b.b + basis.c.c + 1.0f;

			if( diagonal > 0.0f )
			{
				float32_c scale = sqrtf( diagonal ) * 2.0f; // get scale from diagonal

				// TODO: speed this up
				result.a = ( basis.b.c - basis.c.b ) / scale;
				result.b = ( basis.c.a - basis.a.c ) / scale;
				result.c = ( basis.a.b - basis.b.a ) / scale;
				result.d = 0.25f * scale;
			}
			else
			{
				if ( basis.a.a > basis.b.b && basis.a.a > basis.c.c )
				{
					// 1st element of diag is greatest value
					// find scale according to 1st element, and double it
					float32_c scale = sqrtf( 1.0f + basis.a.a - basis.b.b - basis.c.c ) * 2.0f;

					// TODO: speed this up
					result.a = 0.25f * scale;
					result.b = ( basis.b.a + basis.a.b ) / scale;
					result.c = ( basis.a.c + basis.c.a ) / scale;
					result.d = ( basis.b.c - basis.c.b ) / scale;
				}
				else if ( basis.b.b > basis.c.c )
				{
					// 2nd element of diag is greatest value
					// find scale according to 2nd element, and double it
					float32_c scale = sqrtf( 1.0f + basis.b.b - basis.a.a - basis.c.c ) * 2.0f;

					// TODO: speed this up
					result.a = ( basis.b.a + basis.a.b ) / scale;
					result.b = 0.25f * scale;
					result.c = ( basis.c.b + basis.b.c ) / scale;
					result.d = ( basis.c.a - basis.a.c ) / scale;
				}
				else
				{
					// 3rd element of diag is greatest value
					// find scale according to 3rd element, and double it
					float32_c scale = sqrtf( 1.0f + basis.c.c - basis.a.a - basis.b.b ) * 2.0f;

					// TODO: speed this up
					result.a = ( basis.c.a + basis.a.c ) / scale;
					result.b = ( basis.c.b + basis.b.c ) / scale;
					result.c = 0.25f * scale;
					result.d = ( basis.a.b - basis.b.a ) / scale;
				}
			}

			ops::normalize_quaternion32( result );

			return result;
		}

		matrix32x3x3_c basis_matrix32x3x3_from_rotation_quaternion32( quaternion32_c const & rotation )
		{
			return basis_matrix32x3x3_from_rotation_quaternion32_blender( rotation );
		}

		quaternion32_c rotation_quaternion32_from_basis_matrix32x3x3( matrix32x3x3_c const & basis )
		{
			return rotation_quaternion32_from_basis_matrix32x3x3_blender( basis );
		}

		matrix32x3x3_c basis_matrix32x3x3_from_axis_angle( vector32x4_c const & axis_angle )
		{
			matrix32x3x3_c basis;
			float32_c v1 = axis_angle.a;
			float32_c v2 = axis_angle.b;
			float32_c v3 = axis_angle.c;
			float32_c v1s = v1 * v1;
			float32_c v2s = v2 * v2;
			float32_c v3s = v3 * v3;
			float32_c c = math_cosine( axis_angle.d );
			float32_c s = math_sine( axis_angle.d );
			basis.a.a = ( v1s + ( ( 1.0f - v1s ) * c ) );
			basis.a.b = ( ( ( v1 * v2 ) * ( 1.0f - c ) ) + ( v3 * s ) );
			basis.a.c = ( ( ( v1 * v3 ) * ( 1.0f - c ) ) - ( v2 * s ) );
			basis.b.a = ( ( ( v1 * v2 ) * ( 1.0f - c ) ) - ( v3 * s ) );
			basis.b.b = ( v2s + ( ( 1.0f - v2s ) * c ) );
			basis.b.c = ( ( ( v2 * v3 ) * ( 1.0f - c ) ) + ( v1 * s ) );
			basis.c.a = ( ( ( v1 * v3 ) * ( 1.0f - c ) ) + ( v2 * s ) );
			basis.c.b = ( ( ( v2 * v3 ) * ( 1.0f - c ) ) - ( v1 * s ) );
			basis.c.c = ( v3s + ( ( 1.0f - v3s ) * c ) );
			return basis;
		}

		matrix32x3x3_c joint_rotate_basis_matrix32x3x3_y( matrix32x3x3_c basis, vector32x3_c const & basis_y_target )
		{
			matrix32x3x3_c result;
			float32_c basis_y_length = length_float32( basis.b );
			vector32x3_c basis_y_normal = basis.b / basis_y_length;
			float32_c d = dot_product_float32( basis_y_normal, basis_y_target );
			float32_c angle = math_arc_cosine( d );
			if ( angle > constants< float32_c >::angle_near_zero() )
			{
				float32_c s = math_sine( angle );
				float32_c c = math_cosine( angle );
				vector32x3_c axis = cross_product_vector32x3( basis_y_target, basis_y_normal );
				result.a = ( ( basis.a * c ) + ( axis * dot_product_float32( axis, basis.a ) * ( 1.0f - c ) ) + ( cross_product_vector32x3( basis.a, axis ) * s ) );
				result.b = basis_y_target * basis_y_length;
				result.b = ( ( basis.c * c ) + ( axis * dot_product_float32( axis, basis.c ) * ( 1.0f - c ) ) + ( cross_product_vector32x3( basis.c, axis ) * s ) );
			}
			return result;
		}

		matrix32x3x3_c joint_rotate_basis_matrix32x3x3_z( matrix32x3x3_c basis, vector32x3_c const & basis_z_target )
		{
			matrix32x3x3_c result;
			float32_c basis_z_length = length_float32( basis.c );
			vector32x3_c basis_z_normal = basis.c / basis_z_length;
			float32_c d = dot_product_float32( basis_z_normal, basis_z_target );
			float32_c angle = math_arc_cosine( d );
			if ( angle > constants< float32_c >::angle_near_zero() )
			{
				float32_c s = math_sine( angle );
				float32_c c = math_cosine( angle );
				vector32x3_c axis = cross_product_vector32x3( basis_z_target, basis_z_normal );
				result.a = ( ( basis.a * c ) + ( axis * dot_product_float32( axis, basis.a ) * ( 1.0f - c ) ) + ( cross_product_vector32x3( basis.a, axis ) * s ) );
				result.b = ( ( basis.b * c ) + ( axis * dot_product_float32( axis, basis.b ) * ( 1.0f - c ) ) + ( cross_product_vector32x3( basis.b, axis ) * s ) );
				result.c = basis_z_target * basis_z_normal;
			}
			return result;
		}

		matrix32x3x3_c transpose_matrix32x3x3( matrix32x3x3_c const & matrix )
		{
			matrix32x3x3_c result;
			result.a.a = matrix.a.a;
			result.a.b = matrix.b.a;
			result.a.c = matrix.c.a;
			result.b.a = matrix.a.b;
			result.b.b = matrix.b.b;
			result.b.c = matrix.c.b;
			result.c.a = matrix.a.c;
			result.c.b = matrix.b.c;
			result.c.c = matrix.c.c;
			return result;
		}

		matrix32x4x4_c view_matrix32x4x4_from_look_at( vector32x3_c const & position, vector32x3_c const & forward, vector32x3_c const & up )
		{
			matrix32x3x3_c basis;
			basis.c = -normal_vector32x3( forward ); // z-axis, backwards, relative to viewer's perspective.
			basis.a = normal_vector32x3( cross_product_vector32x3( up, basis.c ) ); // x-axis, right, relative to viewer's perspective.
			basis.b = cross_product_vector32x3( basis.c, basis.a); // y-axis, down, relative to viewer's perspective.
			matrix32x4x4_c result;
			result.a.a = basis.a.a;
			result.a.b = basis.b.a;
			result.a.c = basis.c.a;
			result.a.d = 0.0f;
			result.b.a = basis.a.b;
			result.b.b = basis.b.b;
			result.b.c = basis.c.b;
			result.b.d = 0.0f;
			result.c.a = basis.a.c;
			result.c.b = basis.b.c;
			result.c.c = basis.c.c;
			result.c.d = 0.0f;
			result.d.a = -dot_product_float32( basis.a, position );
			result.d.b = -dot_product_float32( basis.b, position );
			result.d.c = -dot_product_float32( basis.c, position );
			result.d.d = 1.0f;
			return result;
		}

		matrix32x4x4_c projection_orthographic_matrix32x4x4( float32_c const width, float32_c const height, float32_c const clip_near, float32_c const clip_far )
		{
			matrix32x4x4_c result;
			result.a.a = 2.0f / width;
			result.a.b = 0.0;
			result.a.c = 0.0;
			result.a.d = 0.0;
			result.b.a = 0.0;
			result.b.b = 2.0f / height;
			result.b.c = 0.0;
			result.b.d = 0.0;
			result.c.a = 0.0;
			result.c.b = 0.0;
			result.c.c = 1.0f / ( clip_near - clip_far );
			result.c.d = 0.0;
			result.d.a = 0.0;
			result.d.b = 0.0;
			result.d.c = clip_near / ( clip_near - clip_far );
			result.d.d = 1.0f;
			return result;
		}

		matrix32x4x4_c projection_orthographic_off_center_matrix32x4x4_a( float32_c const left, float32_c const right, float32_c const top, float32_c const bottom, float32_c const clip_near, float32_c const clip_far )
		{
			matrix32x4x4_c result;
			result.a.a = 2.0f / ( right - left );
			result.a.b = 0.0f;
			result.a.c = 0.0f;
			result.a.d = 0.0f;
			result.b.a = 0.0f;
			result.b.b = 2.0f / ( bottom - top ); // ( top - bottom );
			result.b.c = 0.0f;
			result.b.d = 0.0f;
			result.c.a = 0.0f;
			result.c.b = 0.0f;
			result.c.c = 1.0f / ( clip_near - clip_far );
			result.c.d = 0.0f;
			result.d.a = ( left + right ) / ( left - right );
			result.d.b = ( top + bottom ) / ( top - bottom ); // ( bottom + top ) / ( bottom - top );
			result.d.c = clip_near / ( clip_near - clip_far );
			result.d.d = 1.0f;
			return result;
		}

		matrix32x4x4_c projection_orthographic_off_center_matrix32x4x4_b( float32_c const width, float32_c const height, float32_c const center_x, float32_c const center_y, float32_c clip_near, float32_c clip_far )
		{
			float32_c left = ( width * -0.5f ) - ( center_x * width * 0.5f );
			float32_c right = ( width * 0.5f ) - ( center_x * width * 0.5f );
			float32_c top = ( height * -0.5f ) - ( center_y * height * 0.5f );
			float32_c bottom = ( height * 0.5f ) - ( center_y * height * 0.5f );
			return projection_orthographic_off_center_matrix32x4x4_a( left, right, top, bottom, clip_near, clip_far );
		}

		matrix32x4x4_c projection_perspective_matrix32x4x4( float32_c const field_of_view, float32_c const aspect_ratio, float32_c const clip_near, float32_c const clip_far )
		{
			float32_c dn = 0.0f;
			float32_c df = 1.0f;
			float32_c s = 1.0f / math_tangent( field_of_view / 2.0f );
			matrix32x4x4_c result;
			result.a.a = s / aspect_ratio;
			result.a.b = 0.0f;
			result.a.c = 0.0f;
			result.a.d = 0.0f;
			result.b.a = 0.0f;
			result.b.b = s;
			result.b.c = 0.0f;
			result.b.d = 0.0f;
			result.c.a = 0.0f;
			result.c.b = 0.0f;
			result.c.c = ( df * clip_far - dn * clip_near ) / ( clip_near - clip_far );
			result.c.d = -1.0f;
			result.d.a = 0.0f;
			result.d.b = 0.0f;
			result.d.c = ( df - dn ) * ( clip_near * clip_far ) / ( clip_near - clip_far );
			result.d.d = 0.0f;
			return result;
		}

		matrix32x4x4_c projection_perspective_off_center_matrix32x4x4_a( float32_c const left, float32_c const right, float32_c const top, float32_c bottom, float32_c const clip_near, float32_c const clip_far )
		{
			matrix32x4x4_c result;
			result.a.a = ( 2.0f * clip_near ) / ( right - left );
			result.a.b = 0.0f;
			result.a.c = 0.0f;
			result.a.d = 0.0f;
			result.b.a = 0.0f;
			result.b.b = ( 2.0f * clip_near ) / ( bottom - top ); // ( top - bottom );
			result.b.c = 0.0f;
			result.b.d = 0.0f;
			result.c.a = ( left + right ) / ( right - left );
			result.c.b = ( bottom + top ) / ( bottom - top ); // ( top + bottom ) / ( top - bottom );
			result.c.c = clip_far / ( clip_near - clip_far );
			result.c.d = -1.0f;
			result.d.a = 0.0f;
			result.d.b = 0.0f;
			result.d.c = ( clip_near * clip_far ) / ( clip_near - clip_far );
			result.d.d = 0.0f;
			return result;
		}

		matrix32x4x4_c projection_perspective_off_center_matrix32x4x4_b( float32_c const field_of_view, float32_c const aspect_ratio, float32_c center_x, float32_c center_y, float32_c clip_near, float32_c clip_far )
		{
			float32_c s = 1.0f / math_tangent( field_of_view / 2.0f );
			float32_c left = -s / aspect_ratio;
			float32_c right = s / aspect_ratio;
			float32_c top = -s;
			float32_c bottom = s;
			float32_c offset_x = ( right - left ) * center_x;
			float32_c offset_y = ( bottom - top ) * center_y;
			return projection_perspective_off_center_matrix32x4x4_a( left, right, top, bottom, clip_near, clip_far );
		}

		matrix32x4x4_c transpose_matrix32x4x4( matrix32x4x4_c const & matrix )
		{
			matrix32x4x4_c result;
			result.a.a = matrix.a.a;
			result.a.b = matrix.b.a;
			result.a.c = matrix.c.a;
			result.a.d = matrix.d.a;
			result.b.a = matrix.a.b;
			result.b.b = matrix.b.b;
			result.b.c = matrix.c.b;
			result.b.d = matrix.d.b;
			result.c.a = matrix.a.c;
			result.c.b = matrix.b.c;
			result.c.c = matrix.c.c;
			result.c.d = matrix.d.c;
			result.d.a = matrix.a.d;
			result.d.b = matrix.b.d;
			result.d.c = matrix.c.d;
			result.d.d = matrix.d.d;
			return result;
		}

		matrix32x4x4_c invert_matrix32x4x4_fast( matrix32x4x4_c const & matrix )
		{
			matrix32x4x4_c result;
			result.a.a = matrix.a.a;
			result.a.b = matrix.b.a;
			result.a.c = matrix.c.a;
			result.b.a = matrix.a.b;
			result.b.b = matrix.b.b;
			result.b.c = matrix.c.b;
			result.c.a = matrix.a.c;
			result.c.b = matrix.b.c;
			result.c.c = matrix.c.c;
			result.d.a = -matrix.d.a;
			result.d.b = -matrix.d.b;
			result.d.c = -matrix.d.c;
			result.d.d = matrix.d.d;
			return result;
		}

		matrix32x2x2_c invert_matrix32x2x2( matrix32x2x2_c const & matrix )
		{
			float32_c matrix_determinant = ( matrix.a.a * matrix.b.b ) - ( matrix.a.b * matrix.b.a );
			return matrix32x2x2_c( matrix.a.a / matrix_determinant, matrix.a.b / -matrix_determinant, matrix.b.a / -matrix_determinant, matrix.b.b / matrix_determinant );
		}

		matrix32x2x2_c basis_matrix32x2x2_from_angle_scale( float32_c const angle, float32_c const scale )
		{
			float32_c c = math_cosine( angle ) * scale;
			float32_c s = math_sine( angle ) * scale;
			return matrix32x2x2_c( c, s, -s, c );
		}

		quaternion32_c invert_quaternion32( quaternion32_c const & quaternion )
		{
			quaternion32_c result;
			// if quaternion is always supposed to be unit length, then we don't need to divide by length squared because it will be equal to 1.
			//float32_c length_squared = quaternion.a * quaternion.a + quaternion.b * quaternion.b + quaternion.c * quaternion.c + quaternion.d * quaternion.d;
			//result.a = -quaternion.a / length_squared;
			//result.b = -quaternion.b / length_squared;
			//result.c = -quaternion.c / length_squared;
			//result.d =  quaternion.d / length_squared;
			result.a = -quaternion.a;
			result.b = -quaternion.b;
			result.c = -quaternion.c;
			result.d =  quaternion.d;
			return result;
		}

		quaternion32_c rotation_quaternion32_from_axis_angle( vector32x4_c const & axis_angle )
		{
			quaternion32_c rotation;
			float32_c half_angle = axis_angle.d / 2.0f;
			float32_c half_angle_sine = math_sine( half_angle );
			rotation.a = half_angle_sine * axis_angle.a;
			rotation.b = half_angle_sine * axis_angle.b;
			rotation.c = half_angle_sine * axis_angle.c;
			rotation.d = math_cosine( half_angle );
			return rotation;
		}

		quaternion32_c rotation_quaternion32_from_look_at2( vector32x3_c const & forward, vector32x3_c const & up )
		{
			matrix32x3x3_c basis;
			basis.a = normal_vector32x3( cross_product_vector32x3( forward, up ) ); // x-axis right.
			basis.b = normal_vector32x3( forward ); // y-axis forward.
			basis.c = cross_product_vector32x3( basis.a, basis.b ); // z-axis up.
			return rotation_quaternion32_from_basis_matrix32x3x3( basis );
		}

		vector32x4_c axis_angle_from_rotation_quaternion32( quaternion32_c const & rotation )
		{
			vector32x4_c axis_angle;
			axis_angle.a = rotation.a;
			axis_angle.b = rotation.b;
			axis_angle.c = rotation.c;
			axis_angle.d = 2.0f * math_arc_cosine( rotation.d );
			return axis_angle;
		}

		quaternion32_c rotation_quaternion32_from_euler_angles( vector32x3_c const & euler_angles )
		{
			// taken from iirlicht
			// thank you
			quaternion32_c result;
			float64_c angle;
			angle = euler_angles.a * 0.5;
			float64_c sr = math_sine( angle );
			float64_c cr = math_cosine( angle );
			angle = euler_angles.b * 0.5;
			float64_c sp = math_sine( angle );
			float64_c cp = math_cosine( angle );
			angle = euler_angles.c * 0.5;
			float64_c sy = math_sine( angle );
			float64_c cy = math_cosine( angle );
			float64_c cpcy = cp * cy;
			float64_c spcy = sp * cy;
			float64_c cpsy = cp * sy;
			float64_c spsy = sp * sy;
			result.a = static_cast< float32_c >( sr * cpcy - cr * spsy );
			result.b = static_cast< float32_c >( cr * spcy + sr * cpsy );
			result.c = static_cast< float32_c >( cr * cpsy - sr * spcy );
			result.d = static_cast< float32_c >( cr * cpcy + sr * spsy );
			return result;
		}

		vector32x3_c euler_angles_from_rotation_quaternion32( quaternion32_c const & rotation )
		{
			// taken from iirlicht
			// thank you
			vector32x3_c result;
			vector64x4_c rotation64( static_cast< float64_c >( rotation.a ), static_cast< float64_c >( rotation.b ), static_cast< float64_c >( rotation.c ), static_cast< float64_c >( rotation.d ) );
			float64_c sqx = rotation64.a * rotation64.a;
			float64_c sqy = rotation64.b * rotation64.b;
			float64_c sqz = rotation64.c * rotation64.c;
			float64_c sqw = rotation64.d * rotation64.d;
			float64_c test = 2.0 * ( rotation64.b * rotation64.d - rotation64.a * rotation64.c );
			if ( math_compare_close( test, 1.0, 0.000001 ) )
			{
				result.c = static_cast< float32_c >( -2.0 * math_arc_tangent( rotation64.a, rotation64.d ) ); // heading = rotation about z-axis
				result.a =  0.0f; // bank = rotation about x-axis
				result.b = constants< float32_c >::pi() * 0.5f; // attitude = rotation about y-axis
			}
			else if ( math_compare_close( test, -1.0, 0.000001 ) )
			{
				result.c = static_cast< float32_c >( 2.0 * math_arc_tangent( rotation64.a, rotation64.d ) ); // heading = rotation about z-axis
				result.a = 0.0f; // bank = rotation about x-axis
				result.b = constants< float32_c >::pi() * -0.5f; // attitude = rotation about y-axis
			}
			else
			{
				result.c = static_cast< float32_c >( math_arc_tangent( 2.0 * ( rotation64.a * rotation64.b + rotation64.c * rotation64.d ), (  sqx - sqy - sqz + sqw ) ) ); // heading = rotation about z-axis
				result.a = static_cast< float32_c >( math_arc_tangent( 2.0 * ( rotation64.b * rotation64.c + rotation64.a * rotation64.d ), ( -sqx - sqy + sqz + sqw ) ) ); // bank = rotation about x-axis
				result.b = static_cast< float32_c >( math_arc_sine( math_clamp( test, -1.0, 1.0 ) ) ); // attitude = rotation about y-axis
			}
			return result;
		}


		//
		//
		// line operations.
		//
		//
		
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


		//
		//
		// color format conversions.
		//
		//

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

		float32_c interpolate_linear( float32_c const source, float32_c const destination, float32_c const amount )
		{
			return ( source * ( 1.0f - amount ) ) + ( destination * amount );
		}

		vector32x2_c interpolate_linear( vector32x2_c const & source, vector32x2_c const & destination, float32_c const amount )
		{
			return ( source * ( 1.0f - amount ) ) + ( destination * amount );
		}

		vector32x3_c interpolate_linear( vector32x3_c const & source, vector32x3_c const & destination, float32_c const amount )
		{
			return ( source * ( 1.0f - amount ) ) + ( destination * amount );
		}

		vector32x4_c interpolate_linear( vector32x4_c const & source, vector32x4_c const & destination, float32_c const amount )
		{
			return ( source * ( 1.0f - amount ) ) + ( destination * amount );
		}

		float64_c interpolate_linear( float64_c const source, float64_c const destination, float64_c const amount )
		{
			return ( source * ( 1.0 - amount ) ) + ( destination * amount );
		}

		vector64x2_c interpolate_linear( vector64x2_c const & source, vector64x2_c const & destination, float64_c const amount )
		{
			return ( source * ( 1.0 - amount ) ) + ( destination * amount );
		}

		vector64x3_c interpolate_linear( vector64x3_c const & source, vector64x3_c const & destination, float64_c const amount )
		{
			return ( source * ( 1.0f - amount ) ) + ( destination * amount );
		}

		vector64x4_c interpolate_linear( vector64x4_c const & source, vector64x4_c const & destination, float64_c const amount )
		{
			return ( source * ( 1.0f - amount ) ) + ( destination * amount );
		}

		float32_c interpolate_cosine( float32_c const source, float32_c const destination, float32_c const amount )
		{
			float32_c ratio = ( 1.0f - cos( amount * constants< float32_c >::pi() ) ) / 2.0f;
			return ( source * ( 1.0f - ratio ) ) + ( destination * ratio );
		}

		vector32x2_c interpolate_cosine( vector32x2_c const & source, vector32x2_c const & destination, float32_c const amount )
		{
			float32_c ratio = ( 1.0f - cos( amount * constants< float32_c >::pi() ) ) / 2.0f;
			return ( source * ( 1.0f - ratio ) ) + ( destination * ratio );
		}

		vector32x3_c interpolate_cosine( vector32x3_c const & source, vector32x3_c const & destination, float32_c const amount )
		{
			float32_c ratio = ( 1.0f - cos( amount * constants< float32_c >::pi() ) ) / 2.0f;
			return ( source * ( 1.0f - ratio ) ) + ( destination * ratio );
		}

		vector32x4_c interpolate_cosine( vector32x4_c const & source, vector32x4_c const & destination, float32_c const amount )
		{
			float32_c ratio = ( 1.0f - cos( amount * constants< float32_c >::pi() ) ) / 2.0f;
			return ( source * ( 1.0f - ratio ) ) + ( destination * ratio );
		}

		float32_c interpolate_cubic( float32_c const previous_source, float32_c const source, float32_c const destination, float32_c const next_destination, float32_c const amount )
		{
			float32_c amount_squared = amount * amount;
			float32_c a0 = next_destination - destination - previous_source + source;
			float32_c a1 = previous_source - source - a0;
			float32_c a2 = destination - previous_source;
			float32_c a3 = source;
			return ( a0 * amount * amount_squared ) + ( a1 * amount_squared ) + ( a2 * amount ) + ( a3 );
		}

		vector32x2_c interpolate_cubic( vector32x2_c const & previous_source, vector32x2_c const & source, vector32x2_c const & destination, vector32x2_c const & next_destination, float32_c const amount )
		{
			float32_c amount_squared = amount * amount;
			vector32x2_c a0 = next_destination - destination - previous_source + source;
			vector32x2_c a1 = previous_source - source - a0;
			vector32x2_c a2 = destination - previous_source;
			vector32x2_c a3 = source;
			return ( a0 * amount * amount_squared ) + ( a1 * amount_squared ) + ( a2 * amount ) + ( a3 );
		}

		vector32x3_c interpolate_cubic( vector32x3_c const & previous_source, vector32x3_c const & source, vector32x3_c const & destination, vector32x3_c const & next_destination, float32_c const amount )
		{
			float32_c amount_squared = amount * amount;
			vector32x3_c a0 = next_destination - destination - previous_source + source;
			vector32x3_c a1 = previous_source - source - a0;
			vector32x3_c a2 = destination - previous_source;
			vector32x3_c a3 = source;
			return ( a0 * amount * amount_squared ) + ( a1 * amount_squared ) + ( a2 * amount ) + ( a3 );
		}

		vector32x4_c interpolate_cubic( vector32x4_c const & previous_source, vector32x4_c const & source, vector32x4_c const & destination, vector32x4_c const & next_destination, float32_c const amount )
		{
			float32_c amount_squared = amount * amount;
			vector32x4_c a0 = next_destination - destination - previous_source + source;
			vector32x4_c a1 = previous_source - source - a0;
			vector32x4_c a2 = destination - previous_source;
			vector32x4_c a3 = source;
			return ( a0 * amount * amount_squared ) + ( a1 * amount_squared ) + ( a2 * amount ) + ( a3 );
		}

		float32_c interpolate_hermite( float32_c const previous_source, float32_c const source, float32_c const destination, float32_c const next_destination, float32_c const amount, float32_c const tension, float32_c const bias )
		{
			float32_c m0 = ( ( source - previous_source ) * ( 1.0f + bias ) * ( 1.0f - tension ) / 2.0f ) + ( ( destination - source ) * ( 1.0f - bias ) * ( 1.0f - tension ) / 2.0f );
			float32_c m1 = ( ( destination - source ) * ( 1.0f + bias ) * ( 1.0f - tension ) / 2.0f ) + ( ( next_destination - destination ) * ( 1.0f - bias ) * ( 1.0f - tension ) / 2.0f );
			float32_c mu2 = amount * amount;
			float32_c mu3 = mu2 * amount;
			float32_c a0 = ( 2.0f * mu3 ) - ( 3.0f * mu2 ) + 1.0f;
			float32_c a1 = mu3 - ( 2.0f * mu2 ) + amount;
			float32_c a2 = mu3 - mu2;
			float32_c a3 = ( -2.0f * mu3 ) + ( 3.0f * mu2 );
			return ( ( source * a0 ) + ( m0 * a1 ) + ( m1 * a2 ) + ( destination * a3 ) );
		}

		vector32x2_c interpolate_hermite( vector32x2_c const & previous_source, vector32x2_c const & source, vector32x2_c const & destination, vector32x2_c const & next_destination, float32_c const amount, float32_c const tension, float32_c const bias )
		{
			vector32x2_c m0 = ( ( source - previous_source ) * ( 1.0f + bias ) * ( 1.0f - tension ) / 2.0f ) + ( ( destination - source ) * ( 1.0f - bias ) * ( 1.0f - tension ) / 2.0f );
			vector32x2_c m1 = ( ( destination - source ) * ( 1.0f + bias ) * ( 1.0f - tension ) / 2.0f ) + ( ( next_destination - destination ) * ( 1.0f - bias ) * ( 1.0f - tension ) / 2.0f );
			float32_c mu2 = amount * amount;
			float32_c mu3 = mu2 * amount;
			float32_c a0 = ( 2.0f * mu3 ) - ( 3.0f * mu2 ) + 1.0f;
			float32_c a1 = mu3 - ( 2.0f * mu2 ) + amount;
			float32_c a2 = mu3 - mu2;
			float32_c a3 = ( -2.0f * mu3 ) + ( 3.0f * mu2 );
			return ( ( source * a0 ) + ( m0 * a1 ) + ( m1 * a2 ) + ( destination * a3 ) );
		}

		vector32x3_c interpolate_hermite( vector32x3_c const & previous_source, vector32x3_c const & source, vector32x3_c const & destination, vector32x3_c const & next_destination, float32_c const amount, float32_c const tension, float32_c const bias )
		{
			vector32x3_c m0 = ( ( source - previous_source ) * ( 1.0f + bias ) * ( 1.0f - tension ) / 2.0f ) + ( ( destination - source ) * ( 1.0f - bias ) * ( 1.0f - tension ) / 2.0f );
			vector32x3_c m1 = ( ( destination - source ) * ( 1.0f + bias ) * ( 1.0f - tension ) / 2.0f ) + ( ( next_destination - destination ) * ( 1.0f - bias ) * ( 1.0f - tension ) / 2.0f );
			float32_c mu2 = amount * amount;
			float32_c mu3 = mu2 * amount;
			float32_c a0 = ( 2.0f * mu3 ) - ( 3.0f * mu2 ) + 1.0f;
			float32_c a1 = mu3 - ( 2.0f * mu2 ) + amount;
			float32_c a2 = mu3 - mu2;
			float32_c a3 = ( -2.0f * mu3 ) + ( 3.0f * mu2 );
			return ( ( source * a0 ) + ( m0 * a1 ) + ( m1 * a2 ) + ( destination * a3 ) );
		}

		vector32x4_c interpolate_hermite( vector32x4_c const & previous_source, vector32x4_c const & source, vector32x4_c const & destination, vector32x4_c const & next_destination, float32_c const amount, float32_c const tension, float32_c const bias )
		{
			vector32x4_c m0 = ( ( source - previous_source ) * ( 1.0f + bias ) * ( 1.0f - tension ) / 2.0f ) + ( ( destination - source ) * ( 1.0f - bias ) * ( 1.0f - tension ) / 2.0f );
			vector32x4_c m1 = ( ( destination - source ) * ( 1.0f + bias ) * ( 1.0f - tension ) / 2.0f ) + ( ( next_destination - destination ) * ( 1.0f - bias ) * ( 1.0f - tension ) / 2.0f );
			float32_c mu2 = amount * amount;
			float32_c mu3 = mu2 * amount;
			float32_c a0 = ( 2.0f * mu3 ) - ( 3.0f * mu2 ) + 1.0f;
			float32_c a1 = mu3 - ( 2.0f * mu2 ) + amount;
			float32_c a2 = mu3 - mu2;
			float32_c a3 = ( -2.0f * mu3 ) + ( 3.0f * mu2 );
			return ( ( source * a0 ) + ( m0 * a1 ) + ( m1 * a2 ) + ( destination * a3 ) );
		}

		quaternion32_c interpolate_spherical_linear( quaternion32_c const & source, quaternion32_c const & destination, float32_c const amount )
		{
			// this code taken from:
			// http://www.eudclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/index.html

			if ( amount <= 0.0f )
			{
				return source;
			}
			else if ( amount >= 1.0f )
			{
				return destination;
			}

			float32_c cosine_half_theta = dot_product_float32( source, destination );
			if ( math_absolute_value( cosine_half_theta ) >= 1.0f ) // difference is 0 degrees, or 360 degrees, or greater than 360 degrees (if quaternions are not normalized), so source and destination are the same.
			{
				return source;
			}
			float32_c half_theta = math_arc_cosine( cosine_half_theta );
			float32_c sine_half_theta = math_square_root( 1.0f - ( cosine_half_theta * cosine_half_theta ) );
			if ( math_absolute_value( sine_half_theta ) < 0.0f )
			{
				return destination;
			}

			float32_c ratio_source = math_sine( ( 1.0f - amount ) * half_theta ) / sine_half_theta;
			float32_c ratio_destination = math_sine( amount * half_theta ) / sine_half_theta;

			quaternion32_c result(
				( source.a * ratio_source ) + ( destination.a * ratio_destination ),
				( source.b * ratio_source ) + ( destination.b * ratio_destination ),
				( source.c * ratio_source ) + ( destination.c * ratio_destination ),
				( source.d * ratio_source ) + ( destination.d * ratio_destination ) );
			return normal_quaternion32( result );
		}

		quaternion32_c traverse( quaternion32_c const & source, quaternion32_c const & destination, float32_c amount )
		{
			float32_c cosine_half_theta = dot_product_float32( source, destination );
			if ( math_absolute_value( cosine_half_theta ) >= 1.0f ) // difference is 0 degrees, or 360 degrees, or greater than 360 degrees (if quaternions are not normalized), so source and destination are the same.
			{
				return source;
			}
			float32_c half_theta = math_arc_cosine( cosine_half_theta );
			if ( amount > half_theta )
			{
				return destination;
			}
			return interpolate_spherical_linear( source, destination, amount / half_theta );
		}

		float32_c traverse( float32_c const source, float32_c const destination, float32_c const amount )
		{
			float32_c result = destination;
			if ( destination > source )
			{
				result = source + amount;
				if ( result > destination )
				{
					return destination;
				}
			}
			else if ( destination < source )
			{
				result = source - amount;
				if ( result < destination )
				{
					return destination;
				}
			}
			return result;
		}

		float32_c traverse_with_ease( float32_c const source, float32_c const destination, float32_c amount, float32_c const margin )
		{
			float32_c distance = math_absolute_value( source - destination );
			if ( margin > 0.0f && distance < margin ) // ease
			{
				amount *= distance / margin;
			}

			float32_c result = destination;
			if ( destination > source )
			{
				result = source + amount;
				if ( result > destination )
				{
					return destination;
				}
			}
			else if ( destination < source )
			{
				result = source - amount;
				if ( result < destination )
				{
					return destination;
				}
			}
			return result;
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


		//
		//
		// file path operations.
		//
		//

		string16_c path_get_folder_path( string16_c const & a )
		{
			string16_c result;

			// skip the trailing slash if it's present.
			sint32_c a_length = a.get_length();
			char16_c a_character = a.character_list[ a_length - 1 ];
			if ( a_character == '/' || a_character == '\\' )
			{
				a_length--;
			}

			// scan for slashes.
			sint32_c i = a_length - 1;
			while ( i >= 0 )
			{
				a_character = a.character_list[ i ];
				if ( a_character == '/' || a_character == '\\' )
				{
					i++;
					break;
				}
				i--;
			}

			if ( i > 0 )
			{
				result.character_list.construct_mode_dynamic_from_array( a.character_list.get_internal_array(), i );
				result.character_list.insert( -1, 0 );
			}

			return result;
		}

		string16_c path_get_file_or_folder_extension( string16_c const & a )
		{
			string16_c result;

			// file paths must not end with a slash.
			sint32_c a_length = a.get_length();
			assert( a_length > 0 );
			char16_c a_character = a.character_list[ a_length - 1 ];
			if ( a_character == '/' || a_character == '\\' )
			{
				a_length--;
			}

			// walk backwards until we find a slash, but also keep note of the index of the earliest occurrence of a dot in the file name.
			sint32_c i = a_length - 1;
			sint32_c dot = -1;
			while ( i >= 0 )
			{
				char16_c a_character = a.character_list[ i ];
				if ( a_character == '.' )
				{
					dot = i;
				}
				else if ( a_character == '/' || a_character == '\\' )
				{
					break;
				}
				i--;
			}

			// create the result.
			if ( dot >= 0 )
			{
				result.character_list.construct_mode_dynamic_from_array( &a.character_list.get_internal_array()[ dot ], a_length - dot );
			}

			return result;
		}

		string16_c path_get_file_or_folder_name( string16_c const & a )
		{
			string16_c result;

			// folder paths will end in a slash and we want to exclude it from the scan.
			sint32_c a_length = a.get_length();
			assert( a_length > 0 );
			char16_c a_character = a.character_list[ a_length - 1 ];
			if ( a_character == '/' || a_character == '\\' )
			{
				a_length--;
			}

			// walk backwards until we find a slash.
			sint32_c i = a_length - 1;
			while ( i >= 0 )
			{
				char16_c a_character = a.character_list[ i ];
				if ( a_character == '/' || a_character == '\\' )
				{
					break;
				}
				i--;
			}
			i++; // + 1 because we want to exclude the slash from the result, or prevent -1.

			// create the result.
			if ( a_length - i > 0 )
			{
				result.character_list.construct_mode_dynamic_from_array( &a.character_list.get_internal_array()[ i ], a_length - i );
				result.character_list.insert( -1, 0 );
			}

			return result;
		}

		string16_c path_get_file_or_folder_name_without_extension( string16_c const & a )
		{
			string16_c result;

			// folder paths will end in a slash and we want to exclude it from the scan.
			sint32_c a_length = a.get_length();
			assert( a_length > 0 );
			char16_c a_character = a.character_list[ a_length - 1 ];
			if ( a_character == '/' || a_character == '\\' )
			{
				a_length--;
			}

			// walk backwards until we find a slash, but also keep note of the index of the earliest occurrence of a dot in the file name.
			sint32_c i = a_length - 1;
			sint32_c dot = i;
			while ( i >= 0 )
			{
				char16_c a_character = a.character_list[ i ];
				if ( a_character == '.' )
				{
					dot = i;
				}
				else if ( a_character == '/' || a_character == '\\' )
				{
					break;
				}
				i--;
			}
			i++; // + 1 because we want to exclude the slash from the result, or prevent -1.

			if ( dot > i )
			{
				result.character_list.construct_mode_dynamic_from_array( &a.character_list.get_internal_array()[ i ], dot - i );
				result.character_list.insert( -1, 0 );
			}

			return result;
		}


		//
		//
		// file operations.
		//
		//

//		boolean_c file_system_set_file_or_folder_last_write_time( string16_c const & file_path, sint64_c milliseconds_since_epoch )
//		{
//			assert( path_is_formatted_for_cheonsa( file_path, false ) );
//#if defined( cheonsa_platform_windows )
//			string16_c file_path_windows = path_format_for_windows( file_path );
//			boolean_c result = false;
//			HANDLE file_handle = CreateFileW( file_path_windows.character_list.get_internal_array(), FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
//			if ( file_handle != INVALID_HANDLE_VALUE )
//			{
//				ULARGE_INTEGER file_time_large_integer;
//				file_time_large_integer.QuadPart = time_format_for_windows( milliseconds_since_epoch );
//				FILETIME file_time;
//				file_time.dwLowDateTime = file_time_large_integer.LowPart;
//				file_time.dwHighDateTime = file_time_large_integer.HighPart;
//				SetFileTime( file_handle, NULL, NULL, &file_time );
//				CloseHandle( file_handle );
//				result = true;
//			}
//			return result;
//#else
//#error file_system_get_file_or_folder_modified_time is not implemented.
//#endif
//		}

		boolean_c file_system_is_path_formatted_for_cheonsa( string16_c const & path, file_system_path_type_e path_type )
		{
			// all cheonsa paths should be relative.
			// should not start with a drive letter, but we check this indirectly by checking that it does not contain a colon ':'.
			// check that only valid characters are in the path.
			char16_c const * c = path.character_list.get_internal_array();
			char16_c character;
			while ( *c )
			{
				character = *c++;
				if ( !char16_is_printable( character ) || ( character == '<' ) || ( character == '>' ) || ( character == '"' ) || ( character == '|' ) || ( character == '?' ) || ( character == '*' ) || ( character == '\\' ) )
				{
					return false;
				}
			}
			// check for presence or absence of trailing slash.
			if ( path_type == file_system_path_type_e_file && character == '/' )
			{
				return false;
			}
			if ( path_type == file_system_path_type_e_folder && character != '/' )
			{
				return false;
			}
			return true;
		}

		string16_c file_system_fix_path( string16_c const & path )
		{
			string16_c result;
			string16_c t;
			string16_c node;
			sint32_c i = 0;
			WCHAR t2[ MAX_PATH ] = {};
			DWORD t2r = 0;
			//if ( string16_starts_with( path.character_list.get_internal_array(), "\\\\?\\" ) )
			//{
			//	i = 4;
			//}
			// check for drive letter.
			if ( path.get_length() < i + 2 )
			{
				goto done;
			}
			// check for colon.
			if ( path.character_list[ i + 1 ] == ':' )
			{
				if ( !char16_is_latin_letter( path.character_list[ i ] ) )
				{
					goto done;
				}
				t += char16_to_upper( path.character_list[ i ] );
				t += ':';
			}
			// if path is just drive letter and colon, then add the trailing slash and return.
			if ( path.get_length() == 2 )
			{
				t += '\\';
				result = t;
				goto done;
			}
			i = 2;

		process_next_node:
			while ( i < path.get_length() )
			{
				char16_c c = path.character_list[ i ];
				if ( node.get_length() == 0 )
				{
					if ( c == '/' || c == '\\' )
					{
						node += '\\';
						i++;
					}
					else
					{
						goto done;
					}
				}
				else
				{
					if ( c == '/' || c == '\\' )
					{
						break;
					}
					else
					{
						node += c;
						i++;
					}
				}
			}

			if ( node == "\\." )
			{
				// ignore this node.
			}
			else if( node == "\\.." )
			{
				// get mother node.
				t = path_get_folder_path( t );
				if ( t.get_length() == 0 )
				{
					// if collapsed into nothing, return nothing.
					goto done;
				}
			}
			else
			{
				if ( t.character_list[ t.get_length() - 1 ] != '\\' )
				{
					t += node;
				}
			}
			if ( i < path.get_length() )
			{
				node = string16_c();
				goto process_next_node;
			}

			// fix casing.
			t2r = GetLongPathName( t.character_list.get_internal_array(), t2, MAX_PATH );
			if ( t2r == 0 )
			{
				goto done;
			}
			else if ( t2r == 3 )
			{
				// just drive letter.
				result = t2;
				goto done;
			}

			// determine if trailing slash needs to be added or removed.
			DWORD file_attributes = GetFileAttributes( t2 );
			if ( file_attributes == INVALID_FILE_ATTRIBUTES )
			{
				goto done;
			}
			result = t2;
			if ( file_attributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				if ( result.character_list[ result.get_length() - 1 ] != '\\' )
				{
					result += '\\';
				}
			}
			else
			{
				if ( result.character_list[ result.get_length() - 1 ] == '\\' )
				{
					result.character_list.remove( -1, 2 );
					result.character_list.insert( -1, 0 );
				}
			}

		done:
			return result;
		}

		boolean_c file_system_is_path_formatted_for_windows( string16_c const & path, file_system_path_type_e path_type )
		{
			// all windows paths should be absolute.
			// check for max path prefix.
			sint32_c i = 0;
			//if ( string16_starts_with( path.character_list.get_internal_array(), "\\\\?\\" ) )
			//{
			//	i = 4;
			//}
			// check for drive letter.
			if ( path.get_length() < i + 3 )
			{
				return false;
			}
			if ( char16_is_latin_letter( path.character_list[ i ] ) )
			{
				// check for colon.
				if ( path.character_list[ i + 1 ] == ':' )
				{
					// check for back slash.
					if ( path.character_list[ i + 2 ] == '\\' )
					{
						// check that only valid characters are in the path.
						char16_c const * c = &path.character_list.get_internal_array()[ i ];
						char16_c character;
						while ( *c )
						{
							character = *c++;
							if ( !char16_is_printable( character ) || ( character == 127 ) || ( character == '<' ) || ( character == '>' ) || ( character == '"' ) || ( character == '|' ) || ( character == '?' ) || ( character == '*' ) || ( character == '/' ) )
							{
								return false;
							}
						}
						// check for presence or absence of trailing slash.
						if ( path_type == file_system_path_type_e_file && character == '\\' )
						{
							return false;
						}
						if ( path_type == file_system_path_type_e_folder && character != '\\' )
						{
							return false;
						}
						return true;
					}
				}
			}
			return false;
		}

		string16_c file_system_convert_path_format_from_cheonsa_to_windows( string16_c const & path )
		{
			string16_c result;
			result = path;
			char16_c * character_pointer = result.character_list.get_internal_array();
			while ( *character_pointer )
			{
				if ( *character_pointer == '/' )
				{
					*character_pointer = '\\';
				}
				character_pointer++;
			}
			return result;
		}

		string16_c file_system_convert_path_format_from_windows_to_cheonsa( string16_c const & path )
		{
			string16_c result;
			result = path;
			char16_c * c = result.character_list.get_internal_array();
			while ( *c )
			{
				if ( *c == '\\' )
				{
					*c = '/';
				}
				c++;
			}
			return result;
		}

		boolean_c file_system_does_file_exist( string16_c const & file_path )
		{
#if defined( cheonsa_platform_windows )
			//assert( file_system_is_path_formatted_for_windows( file_path, file_system_path_type_e_file ) );
			DWORD file_attributes = GetFileAttributes( file_path.character_list.get_internal_array() );
			return ( ( file_attributes != INVALID_FILE_ATTRIBUTES ) && ( ( file_attributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 ) );
#else
#error ops::file_system_does_file_exist() is not implemented.
#endif
		}

		boolean_c file_system_create_file( string16_c const & file_path )
		{
#if defined( cheonsa_platform_windows )
			assert( file_system_is_path_formatted_for_windows( file_path, file_system_path_type_e_file ) );
			FILE * file = nullptr;
			errno_t asdkgjfhawe = _wfopen_s( &file, file_path.character_list.get_internal_array(), L"wb" );
			if ( asdkgjfhawe != 0 )
			{
				return false;
			}
			if ( file == 0 )
			{
				return false;
			}
			fclose( file );
			return true;
#else
#error ops::file_system_create_file() is not implemented.
#endif
		}

		boolean_c file_system_does_folder_exist( string16_c const & folder_path )
		{
#if defined( cheonsa_platform_windows )
			assert( file_system_is_path_formatted_for_windows( folder_path, file_system_path_type_e_folder ) );
			DWORD file_attributes = GetFileAttributes( folder_path.character_list.get_internal_array() );
			return ( ( file_attributes != INVALID_FILE_ATTRIBUTES ) && ( ( file_attributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 ) );
#else
#error ops::file_system_does_folder_exist() is not implemented.
#endif
		}

		boolean_c file_system_create_folder( string16_c const & folder_path )
		{
#if defined( cheonsa_platform_windows )
			assert( file_system_is_path_formatted_for_windows( folder_path, file_system_path_type_e_folder ) );
			// windows doesn't let us create a set of nested folders in one call, so we have to create folders at each level one by one.
			BOOL result = FALSE;
			sint32_c final_end = folder_path.get_length();
			sint32_c end = 7; // skip the "\\?\C:\".
			while ( end < final_end )
			{
				while ( end < final_end )
				{
					if ( folder_path.character_list[ end ] == '\\' )
					{
						end++;
						break;
					}
					end++;
				}
				string16_c sub_folder_path = ops::string16_sub_string( folder_path, 0, end );
				DWORD file_attributes = GetFileAttributes( sub_folder_path.character_list.get_internal_array() );
				BOOL folder_exists = ( ( file_attributes != INVALID_FILE_ATTRIBUTES ) && ( ( file_attributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 ) );
				if ( folder_exists == false )
				{
					result = CreateDirectoryW( sub_folder_path.character_list.get_internal_array(), NULL );
				}
			}
			return result != 0;
#else
#error ops::file_system_create_folder() is not implemented.
#endif
		}

		boolean_c file_system_move_file_or_folder( string16_c const & from_file_path, string16_c const & to_file_path )
		{
			assert( false );
#if defined( cheonsa_platform_windows )
			assert( file_system_is_path_formatted_for_windows( from_file_path, file_system_path_type_e_dont_care ) );
			assert( file_system_is_path_formatted_for_windows( to_file_path, file_system_path_type_e_dont_care ) );
			BOOL result = MoveFileW( from_file_path.character_list.get_internal_array(), to_file_path.character_list.get_internal_array() );
			return result != 0;
#else
#error ops::file_system_move_file_or_folder() is not implemented.
#endif
		}

		boolean_c file_system_delete_file_or_folder( string16_c const & file_path )
		{
			assert( false );
#if defined( cheonsa_platform_windows )
			assert( file_system_is_path_formatted_for_windows( file_path, file_system_path_type_e_dont_care ) );
			BOOL result = DeleteFileW( file_path.character_list.get_internal_array() );
			return result != 0;
#else
#error ops::file_system_delete_file_or_folder() is not implemented.
#endif
		}

		boolean_c file_system_get_file_or_folder_last_write_time( string16_c const & file_path, sint64_c & last_write_time )
		{
#if defined( cheonsa_platform_windows )
			assert( file_system_is_path_formatted_for_windows( file_path, file_system_path_type_e_dont_care ) );
			boolean_c result = false;
			HANDLE file_handle = CreateFileW( file_path.character_list.get_internal_array(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
			if ( file_handle != INVALID_HANDLE_VALUE )
			{
				FILETIME file_time;
				if ( GetFileTime( file_handle, NULL, NULL, &file_time ) )
				{
					ULARGE_INTEGER file_time_large_integer;
					file_time_large_integer.LowPart = file_time.dwLowDateTime;
					file_time_large_integer.HighPart = file_time.dwHighDateTime;
					last_write_time = time_convert_time_format_from_windows_to_cheonsa( file_time_large_integer.QuadPart );
					result = true;
				}
				CloseHandle( file_handle );
			}
			return result;
#else
#error ops::file_system_get_file_or_folder_last_write_time() is not implemented.
#endif
		}

		void_c file_system_get_file_information_list( core_list_c< file_system_file_information_c > & result, string16_c const & folder_path, boolean_c get_files, boolean_c get_folders, char8_c const * file_extension_filter )
		{
			result.remove_all();
#if defined( cheonsa_platform_windows )
			assert( file_system_is_path_formatted_for_windows( folder_path, file_system_path_type_e_folder ) );

			string16_c folder_path_with_wild_card = folder_path;
			folder_path_with_wild_card += "*";

			HANDLE find_handle = INVALID_HANDLE_VALUE;
			WIN32_FIND_DATAW find_file_data;
			find_handle = FindFirstFileW( folder_path_with_wild_card.character_list.get_internal_array(), &find_file_data );
			if ( find_handle == INVALID_HANDLE_VALUE )
			{
				return;
			}

			core_list_c< string16_c > file_extension_filter_list;
			if ( file_extension_filter != nullptr )
			{
				ops::string16_split_at_delimiter( string16_c( file_extension_filter ), string16_c( core_list_mode_e_static, L"|" ), file_extension_filter_list );
			}

			do
			{
				if ( ( find_file_data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) != 0 || ( find_file_data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM ) != 0 )
				{
					continue;
				}
				if ( ops::string16_compare( find_file_data.cFileName, L"." ) || ops::string16_compare( find_file_data.cFileName, L".." ) )
				{
					continue;
				}

				ULARGE_INTEGER windows_time;
				if ( get_folders && ( find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
				{
					file_system_file_information_c * result_item = result.emplace( -1, 1 );
					result_item->path = folder_path;
					result_item->path += string16_c( core_list_mode_e_static, find_file_data.cFileName );
					result_item->path += L'\\';
					windows_time.LowPart = find_file_data.ftCreationTime.dwLowDateTime;
					windows_time.HighPart = find_file_data.ftCreationTime.dwHighDateTime;
					result_item->creation_time = time_convert_time_format_from_windows_to_cheonsa( windows_time.QuadPart );
					windows_time.LowPart = find_file_data.ftLastWriteTime.dwLowDateTime;
					windows_time.HighPart = find_file_data.ftLastWriteTime.dwHighDateTime;
					result_item->last_write_time = time_convert_time_format_from_windows_to_cheonsa( windows_time.QuadPart );
					result_item->is_folder = true;
				}
				else if ( get_files && ( find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
				{
					boolean_c add = true;
					if ( file_extension_filter_list.get_length() > 0 )
					{
						add = false;
						for ( sint32_c i = 0; i < file_extension_filter_list.get_length(); i++ )
						{
							if ( string16_ends_with( find_file_data.cFileName, file_extension_filter_list[ i ].character_list.get_internal_array() ) )
							{
								add = true;
								break;
							}
						}
					}
					if ( add )
					{
						file_system_file_information_c * result_item = result.emplace( -1, 1 );
						result_item->path = folder_path;
						result_item->path += string16_c( core_list_mode_e_static, find_file_data.cFileName );
						windows_time.LowPart = find_file_data.ftCreationTime.dwLowDateTime;
						windows_time.HighPart = find_file_data.ftCreationTime.dwHighDateTime;
						result_item->creation_time = time_convert_time_format_from_windows_to_cheonsa( windows_time.QuadPart );
						windows_time.LowPart = find_file_data.ftLastWriteTime.dwLowDateTime;
						windows_time.HighPart = find_file_data.ftLastWriteTime.dwHighDateTime;
						result_item->last_write_time = time_convert_time_format_from_windows_to_cheonsa( windows_time.QuadPart );
						result_item->is_folder = false;
					}
				}
			} while ( FindNextFileW( find_handle, &find_file_data ) != 0 );
			FindClose( find_handle );
#else
#error ops::file_system_get_file_information_list() is not implemented.
#endif
		}

		void_c _file_system_get_files_recursive( core_list_c< string16_c >& result, string16_c const & folder_path, boolean_c const search_sub_folders, char8_c const * extension_filter )
		{
#if defined( cheonsa_platform_windows )
			assert( file_system_is_path_formatted_for_windows( folder_path, file_system_path_type_e_folder ) );

			string16_c folder_path_with_wild_card = folder_path;
			folder_path_with_wild_card += "*";

			HANDLE find_handle = INVALID_HANDLE_VALUE;
			WIN32_FIND_DATAW find_file_data;
			find_handle = FindFirstFileW( folder_path_with_wild_card.character_list.get_internal_array(), &find_file_data );
			if ( find_handle == INVALID_HANDLE_VALUE )
			{
				return;
			}

			do
			{
				if ( ( find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
				{
					string16_c sub_file_name = string16_c( core_list_mode_e_static, find_file_data.cFileName );
					if ( ( extension_filter == nullptr ) || ( string16_ends_with( sub_file_name.character_list.get_internal_array(), extension_filter ) ) )
					{
						string16_c sub_file_path;
						sub_file_path += folder_path;
						sub_file_path += sub_file_name;
						result.insert( -1, sub_file_path );
					}
				}
				else if ( search_sub_folders )
				{
					string16_c sub_folder_name = string16_c( core_list_mode_e_static, find_file_data.cFileName );
					string16_c sub_folder_path;
					sub_folder_path += folder_path;
					sub_folder_path += sub_folder_name;
					sub_folder_path += "\\";
					_file_system_get_files_recursive( result, sub_folder_path, search_sub_folders, extension_filter );
				}
			} while ( FindNextFileW( find_handle, &find_file_data ) != 0 );
			FindClose( find_handle );
#else
#error ops::_file_system_get_files_recursive() is not implemented.
#endif
		}

		void_c file_system_get_file_path_list( core_list_c< string16_c > & result, string16_c const & folder_path, boolean_c const search_sub_folders, char8_c const * file_extension_filter )
		{
			_file_system_get_files_recursive( result, folder_path, search_sub_folders, file_extension_filter );
		}

		void_c _file_system_get_folders_recursive( core_list_c< string16_c > & result, string16_c const & folder_path, boolean_c const search_sub_folders )
		{
#if defined( cheonsa_platform_windows )
			assert( file_system_is_path_formatted_for_windows( folder_path, file_system_path_type_e_folder ) );

			string16_c folder_path_with_wild_card = folder_path;
			folder_path_with_wild_card += "*";

			HANDLE find_handle = INVALID_HANDLE_VALUE;
			WIN32_FIND_DATAW find_file_data;
			find_handle = FindFirstFileW( folder_path_with_wild_card.character_list.get_internal_array(), &find_file_data );
			if ( find_handle == INVALID_HANDLE_VALUE )
			{
				return;
			}

			do
			{
				if ( ( find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
				{
					string16_c sub_folder_name = string16_c( core_list_mode_e_static, find_file_data.cFileName );
					if ( sub_folder_name != "." && sub_folder_name != ".." )
					{
						string16_c sub_folder_path;
						sub_folder_path += folder_path;
						sub_folder_path += sub_folder_name;
						sub_folder_path += '\\';
						result.insert( -1, sub_folder_path );
					}
					if ( search_sub_folders )
					{
						string16_c sub_folder_path;
						sub_folder_path += folder_path;
						sub_folder_path += sub_folder_name;
						sub_folder_path += "\\";
						_file_system_get_folders_recursive( result, sub_folder_path, search_sub_folders );
					}
				}
			} while ( FindNextFileW( find_handle, &find_file_data ) != 0 );
			FindClose( find_handle );
#else
#error ops::_file_system_get_folders_recursive() is not implemented.
#endif
		}

		void_c file_system_get_folder_path_list( core_list_c< string16_c > & result, string16_c const & folder_path, boolean_c const search_sub_folders )
		{
			_file_system_get_folders_recursive( result, folder_path, search_sub_folders );
		}

		boolean_c file_system_get_quick_access_folder_path( file_system_quick_access_folder_e folder, string16_c & result_folder_path )
		{
#if defined( cheonsa_platform_windows )
			PWSTR ppszPath = NULL;
			if ( folder == file_system_quick_access_folder_e_desktop )
			{
				if ( S_OK == SHGetKnownFolderPath( FOLDERID_Desktop, 0, NULL, &ppszPath ) )
				{
					result_folder_path = ppszPath;
					CoTaskMemFree( ppszPath );
					return true;
				}
			}
			else if ( folder == file_system_quick_access_folder_e_documents )
			{
				if ( S_OK == SHGetKnownFolderPath( FOLDERID_Documents, 0, NULL, &ppszPath ) )
				{
					result_folder_path  = ppszPath;
					CoTaskMemFree( ppszPath );
					return true;
				}
			}
#else
#error ops::file_system_get_quick_access_folder_path() is not implemented.
#endif
			return false;
		}

		void_c file_system_get_drive_path_list( core_list_c< file_system_file_information_c > & result )
		{
			result.remove_all();
#if defined( cheonsa_platform_windows )
			// scan all 26 letters.
			// what a dumb design in my opinion but it's set in stone i guess.
			DWORD logical_drives = GetLogicalDrives();
			for ( sint32_c i = 0; i < 26; i++ )
			{
				uint32_c bit = static_cast< uint32_c >( 0x0001 ) << i;
				if ( logical_drives & bit )
				{
					string16_c drive_path;
					drive_path += static_cast< char16_c >( 'A' + i );
					drive_path += ":\\";
					UINT drive_type = GetDriveTypeW( drive_path.character_list.get_internal_array() );
					if ( drive_type >= 2 )
					{
						file_system_file_information_c * information = result.emplace( -1, 1 );
						information->label = string16_c();
						information->path = drive_path;
						WCHAR drive_label[ MAX_PATH + 1 ];
						if ( GetVolumeInformationW( drive_path.character_list.get_internal_array(), drive_label, MAX_PATH + 1, NULL, 0, 0, NULL, 0 ) )
						{
							information->label = drive_label;
						}
					}
				}
			}
#else
#error ops::file_system_get_drive_path_list() is not implemented.
#endif
		}


		//
		//
		// time operations.
		//
		//

		sint64_c time_get_high_resolution_timer_frequency()
		{
#if defined( cheonsa_platform_windows )
			static LARGE_INTEGER result = {};
			if ( result.QuadPart == 0 )
			{
				QueryPerformanceFrequency( &result );
			}
			return result.QuadPart;
#else
#error ops::time_get_high_resolution_timer_frequency() is not implemented.
#endif
		}

		sint64_c time_get_high_resolution_timer_count()
		{
#if defined( cheonsa_platform_windows )
			LARGE_INTEGER result = {};
			QueryPerformanceCounter( &result );
			return result.QuadPart;
#else
#error ops::time_get_high_resolution_timer_count() is not implemented.
#endif
		}

		sint64_c time_get_milliseconds_since_epoch()
		{
#if defined( cheonsa_platform_windows )
			FILETIME file_time = {};
			GetSystemTimeAsFileTime( &file_time );
			ULARGE_INTEGER time_large_integer;
			time_large_integer.LowPart = file_time.dwLowDateTime;
			time_large_integer.HighPart = file_time.dwHighDateTime;
			return time_convert_time_format_from_windows_to_cheonsa( time_large_integer.QuadPart );
#else
#error ops::time_get_milliseconds_since_epoch() is not implemented.
#endif
		}

		sint64_c time_get_local_time_zone_offset()
		{
#if defined( cheonsa_platform_windows )
			DYNAMIC_TIME_ZONE_INFORMATION dynamic_time_zone_information = {};
			if ( GetDynamicTimeZoneInformation( &dynamic_time_zone_information ) != 0 )
			{
				// Bias contains the current local timezone offset plus daylight savings offset.
				// Bias units are originally in minutes so we need to convert to milliseconds.
				return ( dynamic_time_zone_information.Bias * 60 * 1000 ); // 60 seconds per minute, 1000 milliseconds per second.
			}
			return 0;
#else
#error ops::time_get_local_time_zone_offset() is not implemented.
#endif
		}

#if defined( cheonsa_platform_windows )

		// windows measures time in 100 nanosecond intervals.
		// cheonsa measures time in 1 millisecond intervals.
		// 1 nanosecond = 1/1000000000 = 0.000000001 second.
		// 100 nanosecond = 0.0000001 second
		// 1 millisecond  = 1/1000 = 0.0001 second
		// 0.0001 / 0.0000001 = 10000
		//   10000 (100 nanosecond intervals per millisecond)
#define cheonsa_windows_interval_to_cheonsa_interval 10000

		// windows' epoch is 1601 January 1 00:00 GMT.
		// cheonsa's epoch is 2000 January 1 12:00 GMT. (Julain years J2000).
		// these are 12591201600 seconds apart.
		// calculated the difference between the two epochs using python:
		//   >>> windows_epoch = datetime.datetime( 1601, 1, 1, 0 )
		//   >>> cheonsa_epoch = datetime.datetime( 2000, 1, 1, 12 )
		//   >>> ( cheonsa_epoch - windows_epoch ).total_seconds()
#define cheonsa_windows_epoch_to_cheonsa_epoch 12591201600

		sint64_c time_convert_time_format_from_windows_to_cheonsa( sint64_c windows_time )
		{
			// times 10000000 to convert from 1 second intervals to 100 nanosecond intervals.
			return ( windows_time - ( cheonsa_windows_epoch_to_cheonsa_epoch * 10000000 ) ) / cheonsa_windows_interval_to_cheonsa_interval;
		}

		sint64_c time_convert_time_format_from_cheonsa_to_windows( sint64_c cheonsa_time )
		{
			// times 1000 to convert from 1 second intervals to 1 millisecond intervals.
			return ( cheonsa_time + ( cheonsa_windows_epoch_to_cheonsa_epoch * 1000 ) ) * cheonsa_windows_interval_to_cheonsa_interval;
		}

#else
#error is not implemented.
#endif

		//
		//
		// random number generation.
		//
		//

		void_c random_start()
		{
			srand( static_cast< unsigned int >( time( nullptr ) ) );
		}

		float32_c random_float32( float32_c minimum, float32_c maximum )
		{
			return static_cast< float32_c >( static_cast< float64_c >( rand() ) / static_cast< float64_c >( RAND_MAX ) * ( maximum - minimum ) + minimum );
		}

		float64_c random_float64( float64_c minimum, float64_c maximum )
		{
			return static_cast< float64_c >( rand() ) / static_cast< float64_c >( RAND_MAX ) * ( maximum - minimum ) + minimum;
		}

		sint32_c random_sint32( sint32_c minimum, sint32_c maximum )
		{
			return static_cast< sint32_c >( static_cast< float64_c >( rand() ) / static_cast< float64_c >( RAND_MAX ) * static_cast< float64_c >( maximum - minimum ) ) + minimum;
		}

		uint32_c random_uint32( uint32_c maximum )
		{
			return static_cast< uint32_c >( static_cast< float64_c >( rand() ) / static_cast< float64_c >( RAND_MAX ) * static_cast< float64_c >( maximum ) );
		}

		vector32x2_c random_point_on_circle()
		{
			float32_c random_angle = random_float32( 0.0f, constants< float32_c >::pi() * 2.0f );
			return vector32x2_c( math_cosine( random_angle ), math_sine( random_angle ) );
		}

		vector32x3_c random_point_on_sphere()
		{
			// found at https://math.stackexchange.com/questions/44689/how-to-find-a-random-axis-or-unit-vector-in-3d
			float32_c random_angle = random_float32( 0.0f, constants< float32_c >::pi() * 2.0f );
			float32_c random_z = random_float32( -1.0f, 1.0f );
			return vector32x3_c(
				math_cosine( math_square_root( 1.0f - ( random_z * random_z ) ) ),
				math_sine( math_square_root( 1.0f - ( random_z * random_z ) ) ),
				random_z );
		}


		//
		//
		// string conversions and oprations.
		//
		//

		char8_c convert_digit_to_char8( sint8_c const digit )
		{
			assert( digit >= 0 && digit <= 15 );
			static char8_c const map[] =
			{
				'0',
				'1',
				'2',
				'3',
				'4',
				'5',
				'6',
				'7',
				'8',
				'9',
				'a',
				'b',
				'c',
				'd',
				'e',
				'f'
			};
			if ( digit < 16 )
			{
				return map[ digit ];
			}
			return 0;
		}

		sint8_c convert_char8_to_digit( char8_c const character )
		{
			if ( character >= '0' && character <= '9' )
			{
				return character - '0';
			}
			else if ( character >= 'a' && character <= 'f' )
			{
				return character - 'a' + 10;
			}
			else if ( character >= 'A' && character <= 'F' )
			{
				return character - 'A' + 10;
			}
			return -1;
		}

		// does result = value * base + digit, but checks if overflow or underflow will occur.
		// if overflow or underflow would occur, caps result at int_type_c's maximum or minimum range limit and returns false.
		// otherwise, performs the operation and returns true.
		template< typename int_type_c >
		boolean_c _accumulate_digit( int_type_c & result, int_type_c value, sint8_c base, sint8_c digit )
		{
			// handle case where multiplying by base would overflow value.
			if ( value > constants< int_type_c >::maximum() / base )
			{
				result = constants< int_type_c >::maximum();
				return false;
			}
			// handle case where multiplying by base would underflow value.
			if ( value < constants< int_type_c >::minimum() / base )
			{
				result = constants< int_type_c >::minimum();
				return false;
			}
			// handle case where adding digit would overflow value.
			if ( digit > 0 && value * base > constants< int_type_c >::maximum() - digit )
			{
				result = constants< int_type_c >::maximum();
				return false;
			}
			// handle case where subtracting digit would underflow value.
			else if ( digit < 0 && value * base < constants< int_type_c >::minimum() - digit )
			{
				result = constants< int_type_c >::minimum();
				return false;
			}
			// handle ordinary case.
			result = value * base + digit;
			return true;
		}

		template< typename string_type_c, typename int_type_c >
		boolean_c _convert_string_to_int( string_type_c const & in, int_type_c & out, sint8_c base )
		{
			assert( base >= 2 && base <= 16 );

			int_type_c result = 0;
			auto const * c = in.character_list.get_internal_array();

			boolean_c negative = false;
			if ( *c == '-' )
			{
				if ( constants< int_type_c >::is_signed() == false )
				{
					out = 0;
					return true;
				}
				negative = true;
				c++;
			}

			while ( *c )
			{
				// parse digit.
				sint8_c digit = convert_char8_to_digit( *c );
				if ( digit < 0 || digit >= base )
				{
					return false;
				}

				if ( !_accumulate_digit< int_type_c >( result, result, base, negative ? -digit : digit ) )
				{
					out = result; // encountered underflow or overflow, no reason to keep going.
					return true;
				}
				c++;
			}

			out = result;

			return true;
		}

		template< typename int_type_c, typename string_type_c >
		void_c _convert_int_to_string( int_type_c in, string_type_c & out, sint8_c base )
		{
			assert( base >= 2 && base <= 16 );
			out.character_list.remove_all();
			if ( in < 0 )
			{
				out.character_list.insert( -1, '-' );
			}
			else if ( in == 0 )
			{
				out.character_list.insert( -1, '0' );
			}
			while ( in > 0 )
			{
				int_type_c digit = in % base;
				in /= base;
				out.character_list.insert( 0, convert_digit_to_char8( static_cast< sint8_c >( digit ) ) );
			}
			out.character_list.insert( -1, 0 );
		}

		template< typename string_type_c, typename int_type_c >
		boolean_c _convert_string_to_intxn( string_type_c const & in, core_list_c< int_type_c > & out )
		{
			if ( out.get_mode_is_dynamic() )
			{
				out.remove_all();
			}
			string_type_c element_string;
			int_type_c element_value;
			sint32_c element_index = 0;
			auto const * c = in.character_list.get_internal_array();
			while ( *c != 0 )
			{
				if ( char16_is_decimal_digit( *c ) || *c == '.' || *c == '-' )
				{
					do
					{
						element_string += *c;
						c++;
					} while ( ( *c != 0 ) && ( char16_is_decimal_digit( *c ) || *c == '.' || *c == '-' ) );
					element_value = 0;
					_convert_string_to_int< string_type_c, int_type_c >( element_string, element_value, 10 );
					element_string = string_type_c();
					if ( out.get_mode_is_dynamic() )
					{
						// append element to dynamic mode list.
						out.insert( -1, element_value );
					}
					else
					{
						if ( element_index < out.get_length() )
						{
							// set element in static mode list.
							out[ element_index ] = element_value;
						}
						else
						{
							// out of bounds of static mode list.
							return false;
						}
					}
					element_index++;
				}
				c++;
			}
			return true;
		}

		template< typename int_type_c, typename string_type_c >
		void_c _convert_intxn_to_string( core_list_c< int_type_c > const & in, string_type_c & out, char8_c const * brackets = nullptr )
		{
			out = string_type_c();
			if ( in.get_length() > 1 && brackets )
			{
				out += brackets[ 0 ];
			}
			for ( sint32_c element_index = 0; element_index < in.get_length(); element_index++ )
			{
				if ( element_index > 0 )
				{
					out += ", ";
				}
				string_type_c element_string;
				_convert_int_to_string< int_type_c, string_type_c >( in[ element_index ], element_string, 10 );
				out += element_string;
			}
			if ( in.get_length() > 1 && brackets )
			{
				out += brackets[ 1 ];
			}
		}

		boolean_c convert_string8_to_sint8( string8_c const & in, sint8_c & out )
		{
			return _convert_string_to_int< string8_c, sint8_c >( in, out, 10 );
		}

		void_c convert_sint8_to_string8( sint8_c in, string8_c & out )
		{
			_convert_int_to_string< sint8_c, string8_c >( in, out, 10 );
		}

		boolean_c convert_string8_to_sint8xn( string8_c const & in, core_list_c< sint8_c > & out )
		{
			return _convert_string_to_intxn< string8_c, sint8_c >( in, out );
		}

		void_c convert_sint8xn_to_string8( core_list_c< sint8_c > const & in, string8_c & out )
		{
			_convert_intxn_to_string< sint8_c, string8_c >( in, out );
		}

		boolean_c convert_string8_to_uint8( string8_c const & in, uint8_c & out )
		{
			return _convert_string_to_int< string8_c, uint8_c >( in, out, 10 );
		}

		void_c convert_uint8_to_string8( uint8_c in, string8_c & out )
		{
			_convert_int_to_string< uint8_c, string8_c >( in, out, 10 );
		}

		boolean_c convert_string8_to_uint8xn( string8_c const & in, core_list_c< uint8_c > & out )
		{
			return _convert_string_to_intxn< string8_c, uint8_c >( in, out );
		}

		void_c convert_uint8xn_to_string8( core_list_c< uint8_c > const & in, string8_c & out )
		{
			_convert_intxn_to_string< uint8_c, string8_c >( in, out );
		}

		boolean_c convert_string8_to_sint16( string8_c const & in, sint16_c & out )
		{
			return _convert_string_to_int< string8_c, sint16_c >( in, out, 10 );
		}

		void_c convert_sint16_to_string8( sint16_c in, string8_c & out )
		{
			_convert_int_to_string< sint16_c, string8_c >( in, out, 10 );
		}

		boolean_c convert_string8_to_sint16xn( string8_c const & in, core_list_c< sint16_c > & out )
		{
			return _convert_string_to_intxn< string8_c, sint16_c >( in, out );
		}

		void_c convert_sint16xn_to_string8( core_list_c< sint16_c > const & in, string8_c & out )
		{
			_convert_intxn_to_string< sint16_c, string8_c >( in, out );
		}

		boolean_c convert_string8_to_uint16( string8_c const & in, uint16_c & out )
		{
			return _convert_string_to_int< string8_c, uint16_c >( in, out, 10 );
		}

		void_c convert_uint16_to_string8( uint16_c in, string8_c & out )
		{
			_convert_int_to_string< uint16_c, string8_c >( in, out, 10 );
		}

		boolean_c convert_string8_to_uint16xn( string8_c const & in, core_list_c< uint16_c > & out )
		{
			return _convert_string_to_intxn< string8_c, uint16_c >( in, out );
		}

		void_c convert_uint16xn_to_string8( core_list_c< uint16_c > const & in, string8_c & out )
		{
			_convert_intxn_to_string< uint16_c, string8_c >( in, out );
		}

		boolean_c convert_string8_to_sint32( string8_c const & in, sint32_c & out )
		{
			return _convert_string_to_int< string8_c, sint32_c >( in, out, 10 );
		}

		void_c convert_sint32_to_string8( sint32_c in, string8_c & out )
		{
			_convert_int_to_string< sint32_c, string8_c >( in, out, 10 );
		}

		boolean_c convert_string8_to_sint32xn( string8_c const & in, core_list_c< sint32_c > & out )
		{
			return _convert_string_to_intxn< string8_c, sint32_c >( in, out );
		}

		void_c convert_sint32xn_to_string8( core_list_c< sint32_c > const & in, string8_c & out )
		{
			_convert_intxn_to_string< sint32_c, string8_c >( in, out );
		}

		boolean_c convert_string8_to_uint32( string8_c const & in, uint32_c & out )
		{
			return _convert_string_to_int< string8_c, uint32_c >( in, out, 10 );
		}

		void_c convert_uint32_to_string8( uint32_c in, string8_c & out )
		{
			_convert_int_to_string< uint32_c, string8_c >( in, out, 10 );
		}

		boolean_c convert_string8_to_uint32xn( string8_c const & in, core_list_c< uint32_c > & out )
		{
			return _convert_string_to_intxn< string8_c, uint32_c >( in, out );
		}

		void_c convert_uint32xn_to_string8( core_list_c< uint32_c > const & in, string8_c & out )
		{
			_convert_intxn_to_string< uint32_c, string8_c >( in, out );
		}

		boolean_c convert_string8_to_sint64( string8_c const & in, sint64_c & out )
		{
			return _convert_string_to_int< string8_c, sint64_c >( in, out, 10 );
		}

		void_c convert_sint64_to_string8( sint64_c in, string8_c & out )
		{
			_convert_int_to_string< sint64_c, string8_c >( in, out, 10 );
		}

		boolean_c convert_string8_to_sint64xn( string8_c const & in, core_list_c< sint64_c > & out )
		{
			return _convert_string_to_intxn< string8_c, sint64_c >( in, out );
		}

		void_c convert_sint64xn_to_string8( core_list_c< sint64_c > const & in, string8_c & out )
		{
			_convert_intxn_to_string< sint64_c, string8_c >( in, out );
		}

		boolean_c convert_string8_to_uint64( string8_c const & in, uint64_c & out )
		{
			return _convert_string_to_int< string8_c, uint64_c >( in, out, 10 );
		}

		void_c convert_uint64_to_string8( uint64_c in, string8_c & out )
		{
			_convert_int_to_string< uint64_c, string8_c >( in, out, 10 );
		}

		boolean_c convert_string8_to_uint64xn( string8_c const & in, core_list_c< uint64_c > & out )
		{
			return _convert_string_to_intxn< string8_c, uint64_c >( in, out );
		}

		void_c convert_uint64xn_to_string8( core_list_c< uint64_c > const & in, string8_c & out )
		{
			_convert_intxn_to_string< uint64_c, string8_c >( in, out );
		}

		boolean_c convert_hexadecimal_string8_to_uint64( string8_c const & in, uint64_c & out )
		{
			if ( in.get_length() > 16 )
			{
				return false;
			}
			uint64_c result = 0;
			sint32_c in_count = in.get_length();
			for ( sint32_c i = 0; i < in_count; i++ )
			{
				uint8_c digit = convert_char8_to_digit( in.character_list.get_internal_array()[ i ] );
				if ( digit == -1 )
				{
					return false;
				}
				if ( i > 0 )
				{
					result <<= 4;
				}
				result |= digit;
			}
			out = result;
			return true;
		}

		void_c convert_uint64_to_hexadecimal_string8( uint64_c in, string8_c & out )
		{
			out.character_list.construct_mode_dynamic( 17 );
			for ( sint32_c i = 0; i < 16; i++ )
			{
				out.character_list[ 16 - i ] = convert_digit_to_char8( static_cast< uint8_c >( in & 0x0F ) );
				in >>= 4;
			}
			out.character_list[ 16 ] = 0;
		}

		boolean_c convert_string8_to_float32( string8_c const & in, float32_c & out )
		{
			out = static_cast< float32_c >( atof( in.character_list.get_internal_array() ) );
			//out = static_cast< float32_c >( wcstod( in.character_list.get_array(), nullptr ) );
			return true;
		}

		void_c convert_float32_to_string8( float32_c in, string8_c & out )
		{
			char8_c buffer[ 32 ];
			if ( snprintf( buffer, 32, "%.4f", static_cast< float64_c >( in ) ) >= 0 )
			{
				out = buffer;
			}
		}

		boolean_c convert_string8_to_float32xn( string8_c const & in, core_list_c< float32_c > & out )
		{
			if ( out.get_mode_is_dynamic() )
			{
				out.remove_all();
			}
			else
			{
				assert( out.get_length() > 0 );
			}
			string8_c element_string;
			float32_c element_value = 0.0;
			sint32_c element_index = 0; // for writing to fixed length out.
			char8_c const * c = in.character_list.get_internal_array();
			while ( ( *c != 0 ) )
			{
				if ( char16_is_decimal_digit( *c ) || *c == '.' || *c == '-' )
				{
					element_string = string8_c();
					while ( char16_is_decimal_digit( *c ) || *c == '.' || *c == '-' )
					{
						element_string += *c;
						c++;
					}
					convert_string8_to_float32( element_string, element_value );
					if ( out.get_mode_is_dynamic() )
					{
						out.insert( -1, element_value );
					}
					else
					{
						if ( element_index < out.get_length() )
						{
							out[ element_index ] = element_value;
							element_index++;
							if ( element_index == out.get_length() )
							{
								return true;
							}
						}
					}
				}
				else
				{
					c++;
				}
			}
			return out.get_mode_is_dynamic();
		}

		void_c convert_float32xn_to_string8( core_list_c< float32_c > const & in, string8_c & out )
		{
			out = in.get_length() > 1 ? "(" : "";
			for ( sint32_c i = 0; i < in.get_length(); i++ )
			{
				if ( i > 0 )
				{
					out += ", ";
				}
				string8_c element_string;
				convert_float32_to_string8( in[ i ], element_string );
				out += element_string;
			}
			if ( in.get_length() > 1 )
			{
				out += ")";
			}
		}

		boolean_c convert_string8_to_float64( string8_c const & in, float64_c & out )
		{
			out = atof( in.character_list.get_internal_array() );
			return true;
		}

		void_c convert_float64_to_string8( float64_c const in, string8_c & out )
		{
			char8_c buffer[ 32 ];
			if ( snprintf( buffer, 32, "%.4f", in ) >= 0 )
			{
				out = buffer;
			}
		}

		boolean_c convert_string8_to_float64xn( string8_c const & in, core_list_c< float64_c > & out )
		{
			if ( out.get_mode_is_dynamic() )
			{
				out.remove_all();
			}
			else
			{
				assert( out.get_length() > 0 );
			}
			string8_c element_string;
			float64_c element_value = 0.0;
			sint32_c element_index = 0; // for writing to fixed length out.
			char8_c const * c = in.character_list.get_internal_array();
			while ( ( *c != 0 ) )
			{
				if ( char16_is_decimal_digit( *c ) || *c == '.' || *c == '-' )
				{
					element_string = string8_c();
					while ( char16_is_decimal_digit( *c ) || *c == '.' || *c == '-' )
					{
						element_string += *c;
						c++;
					}
					convert_string8_to_float64( element_string, element_value );
					if ( out.get_mode_is_dynamic() )
					{
						out.insert( -1, element_value );
					}
					else
					{
						if ( element_index < out.get_length() )
						{
							out[ element_index ] = element_value;
							element_index++;
							if ( element_index == out.get_length() )
							{
								return true;
							}
						}
					}
				}
				else
				{
					c++;
				}
			}
			return out.get_mode_is_dynamic();
		}

		void_c convert_float64xn_to_string8( core_list_c< float64_c > const & in, string8_c & out )
		{
			out = in.get_length() > 1 ? "(" : "";
			for ( sint32_c i = 0; i < in.get_length(); i++ )
			{
				if ( i > 0 )
				{
					out += ", ";
				}
				string8_c element_string;
				convert_float64_to_string8( in[ i ], element_string );
				out += element_string;
			}
			if ( in.get_length() > 1 )
			{
				out += ")";
			}
		}

		boolean_c convert_string8_to_boolean( string8_c const & in, boolean_c & out )
		{
			if ( in == "true" || in == "1" )
			{
				out = true;
				return true;
			}
			else if ( in == "false" || in == "0" )
			{
				out = false;
				return true;
			}
			return false;
		}

		void_c convert_boolean_to_string8( boolean_c in, string8_c & out )
		{
			if ( in )
			{
				out = string8_c( core_list_mode_e_static, "true" );
			}
			else
			{
				out = string8_c( core_list_mode_e_static, "false" );
			}
		}

		boolean_c convert_rgb_to_hexadecimal_string8( vector64x3_c const & rgb, string8_c & string )
		{
			uint8_c r = static_cast< uint8_c >( math_saturate( rgb.a ) * 255.0 );
			uint8_c g = static_cast< uint8_c >( math_saturate( rgb.b ) * 255.0 );
			uint8_c b = static_cast< uint8_c >( math_saturate( rgb.c ) * 255.0 );
			string.character_list.construct_mode_dynamic( 7 );
			string.character_list[ 0 ] = convert_digit_to_char8( ( r >> 4 ) & 0x0f );
			string.character_list[ 1 ] = convert_digit_to_char8( r & 0x0f );
			string.character_list[ 2 ] = convert_digit_to_char8( ( g >> 4 ) & 0x0f );
			string.character_list[ 3 ] = convert_digit_to_char8( g & 0x0f );
			string.character_list[ 4 ] = convert_digit_to_char8( ( b >> 4 ) & 0x0f );
			string.character_list[ 5 ] = convert_digit_to_char8( b & 0x0f );
			string.character_list[ 6 ] = 0;
			return true;
		}

		boolean_c convert_rgba_to_hexadecimal_string8( vector64x4_c const & rgba, string8_c & string )
		{
			uint8_c r = static_cast< uint8_c >( math_saturate( rgba.a ) * 255.0 );
			uint8_c g = static_cast< uint8_c >( math_saturate( rgba.b ) * 255.0 );
			uint8_c b = static_cast< uint8_c >( math_saturate( rgba.c ) * 255.0 );
			uint8_c a = static_cast< uint8_c >( math_saturate( rgba.d ) * 255.0 );
			string.character_list.construct_mode_dynamic( 9 );
			string.character_list[ 0 ] = convert_digit_to_char8( ( r >> 4 ) & 0x0f );
			string.character_list[ 1 ] = convert_digit_to_char8( r & 0x0f );
			string.character_list[ 2 ] = convert_digit_to_char8( ( g >> 4 ) & 0x0f );
			string.character_list[ 3 ] = convert_digit_to_char8( g & 0x0f );
			string.character_list[ 4 ] = convert_digit_to_char8( ( b >> 4 ) & 0x0f );
			string.character_list[ 5 ] = convert_digit_to_char8( b & 0x0f );
			string.character_list[ 6 ] = convert_digit_to_char8( ( a >> 4 ) & 0x0f );
			string.character_list[ 7 ] = convert_digit_to_char8( a & 0x0f );
			string.character_list[ 8 ] = 0;
			return true;
		}

		boolean_c convert_string8_to_rgb( string8_c const & string, vector32x3_c & rgb )
		{
			vector32x4_c rgba;
			if ( convert_string8_to_rgba( string, rgba ) )
			{
				rgb = vector32x3_c( rgba.a, rgba.b, rgba.c );
				return true;
			}
			return false;
		}

		boolean_c convert_string8_to_rgb( string8_c const & string, vector64x3_c & rgb )
		{
			vector64x4_c rgba;
			if ( convert_string8_to_rgba( string, rgba ) )
			{
				rgb = vector64x3_c( rgba.a, rgba.b, rgba.c );
				return true;
			}
			return false;
		}

		boolean_c convert_string8_to_rgba( string8_c const & string, vector32x4_c & rgba )
		{
			vector64x4_c t;
			if ( convert_string8_to_rgba( string, t ) )
			{
				rgba = vector32x4_c( t );
				return true;
			}
			return false;
		}

		boolean_c convert_string8_to_rgba( string8_c const & string, vector64x4_c & rgba )
		{
			boolean_c hex = false;
			sint32_c hex_start = 0;
			if ( string8_starts_with( string.character_list.get_internal_array(), "#" ) )
			{
				hex = true;
				hex_start = 1;
			}
			else if ( string8_starts_with( string.character_list.get_internal_array(), "0x" ) )
			{
				hex = true;
				hex_start = 2;
			}
			else if ( string.get_length() == 3 || string.get_length() == 4 || string.get_length() == 6 || string.get_length() == 8 )
			{
				hex = true;
				for ( sint32_c i = 0; i < string.get_length(); i++ )
				{
					if ( !char16_is_hexadecimal_digit( string.character_list[ i ] ) )
					{
						hex = false;
						break;
					}
				}
			}
			if ( hex )
			{
				string8_c color_string = string8_sub_string( string, hex_start, string.get_length() - hex_start );

				uint32_c color_value = 0xFFFFFFFF;
				sint32_c r = ( color_value >> 24 ) & 0xFF;
				sint32_c g = ( color_value >> 16 ) & 0xFF;
				sint32_c b = ( color_value >> 8 ) & 0xFF;
				sint32_c a = ( color_value ) & 0xFF;
				if ( color_string.get_length() == 3 ) // RGB
				{
					r = convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 0 ] ) ) << 4;
					g = convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 1 ] ) ) << 4;
					b = convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 2 ] ) ) << 4;
				}
				else if ( color_string.get_length() == 4 ) // RGBA
				{
					r = convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 0 ] ) ) << 4;
					g = convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 1 ] ) ) << 4;
					b = convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 2 ] ) ) << 4;
					a = convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 3 ] ) ) << 4;
				}
				else if ( color_string.get_length() == 6 ) // RRGGBB
				{
					r = ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 0 ] ) ) << 4 ) | ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 1 ] ) ) );
					g = ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 2 ] ) ) << 4 ) | ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 3 ] ) ) );
					b = ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 4 ] ) ) << 4 ) | ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 5 ] ) ) );
				}
				else if ( color_string.get_length() == 8 ) // RRGGBBAA
				{
					r = ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 0 ] ) ) << 4 ) | ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 1 ] ) ) );
					g = ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 2 ] ) ) << 4 ) | ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 3 ] ) ) );
					b = ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 4 ] ) ) << 4 ) | ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 5 ] ) ) );
					a = ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 6 ] ) ) << 4 ) | ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 7 ] ) ) );
				}
				rgba.a = static_cast< float32_c >( r ) / 255.0f;
				rgba.b = static_cast< float32_c >( g ) / 255.0f;
				rgba.c = static_cast< float32_c >( b ) / 255.0f;
				rgba.d = static_cast< float32_c >( a ) / 255.0f;
				return true;
			}
			else
			{
				core_list_c< float64_c > result;
				assert( result.get_mode_is_dynamic() );
				if ( convert_string8_to_float64xn( string, result ) )
				{
					if ( result.get_length() >= 3 )
					{
						rgba.a = result[ 0 ];
						rgba.b = result[ 1 ];
						rgba.c = result[ 2 ];
						rgba.d = result.get_length() >= 4 ? result[ 3 ] : 1.0;
						return true;
					}
				}
			}
			return false;
		}

		boolean_c char16_is_printable( char16_c a )
		{
			return !( ( a >= 0 && a <= 31 ) || ( a == 127 ) );
		}

		boolean_c char16_is_space( char16_c a )
		{
			return ( a == ' ' ) || ( a == '\t' );
		}

		boolean_c char16_is_latin_letter( char16_c a )
		{
			return ( ( a >= 'a' ) && ( a <= 'z' ) ) || ( ( a >= 'A' && a <= 'Z' ) );
		}

		boolean_c char16_is_decimal_digit( char16_c a )
		{
			return a >= '0' && a <= '9';
		}

		boolean_c char16_is_hexadecimal_digit( char16_c a )
		{
			return ( a >= '0' && a <= '9' ) || ( a >= 'a' && a <= 'f' ) || ( a >= 'A' && a <= 'F' );
		}

		boolean_c char16_is_punctuation( char16_c a )
		{
			return
				( a >= 0x0021 && a <= 0x002F ) ||
				( a >= 0x003A && a <= 0x0040 ) ||
				( a >= 0x005B && a <= 0x0060 ) ||
				( a >= 0x007B && a <= 0x007E ) ||
				( a >= 0x3000 && a <= 0x303F ); // "CJK Symbols and Punctuation"
		}

		boolean_c char16_is_control( char16_c a )
		{
			return iscntrl( a ) != 0;
		}

		boolean_c char8_compare( char8_c a, char8_c b, boolean_c case_sensitive )
		{
			return char16_compare( a, b, case_sensitive );
		}

		boolean_c char16_compare( char16_c a, char16_c b, boolean_c case_sensitive )
		{
			if ( !case_sensitive )
			{
				a = char16_to_lower( a );
				b = char16_to_lower( b );
			}
			return a == b;
		}

		char8_c char8_to_upper( char8_c a )
		{
			if ( a >= 'a' && a <= 'z' )
			{
				return a - ( 'a' - 'A' );
			}
			return a;
		}

		char16_c char16_to_upper( char16_c a )
		{
			if ( a >= 'a' && a <= 'z' )
			{
				return a - ( 'a' - 'A' );
			}
			return a;
		}

		char8_c char8_to_lower( char8_c a )
		{
			if ( a >= 'A' && a <= 'Z' )
			{
				return a + ( 'a' - 'A' );
			}
			return a;
		}

		char16_c char16_to_lower( char16_c a )
		{
			if ( a >= 'A' && a <= 'Z' )
			{
				return a + ( 'a' - 'A' );
			}
			return a;
		}

		void_c string8_to_upper( string8_c & string )
		{
			char8_c * c = string.character_list.get_internal_array();
			while ( *c )
			{
				*c = char8_to_upper( *c );
				c++;
			}
		}

		void_c string16_to_upper( string16_c & string )
		{
			char16_c * c = string.character_list.get_internal_array();
			while ( *c )
			{
				*c = char16_to_upper( *c );
				c++;
			}
		}

		void_c string8_to_lower( string8_c & string )
		{
			char8_c * c = string.character_list.get_internal_array();
			while ( *c )
			{
				*c = char8_to_lower( *c );
				c++;
			}
		}

		void_c string16_to_lower( string16_c & string )
		{
			char16_c * c = string.character_list.get_internal_array();
			while ( *c )
			{
				*c = char16_to_lower( *c );
				c++;
			}
		}

		void_c string8_pad_with_leading_zeros( string8_c & string, sint32_c desired_length )
		{
			sint32_c how_many = desired_length - string.get_length();
			if ( how_many > 0 )
			{
				string.character_list.emplace( 0, how_many );
				char8_c * c = string.character_list.get_internal_array();
				for ( sint32_c i = 0; i < how_many; i++ )
				{
					c[ i ] = 0;
				}
			}
		}

		template< typename string_type_c >
		string_type_c _string_trim_front( string_type_c const & string )
		{
			string_type_c result;
			sint32_c length = string.get_length();
			sint32_c trim_front_count = 0;
			for ( sint32_c i = 0; i < length; i++ )
			{
				auto b = string.character_list[ i ];
				if ( char16_is_space( b ) )
				{
					trim_front_count++;
				}
				else
				{
					break;
				}
			}
			result = _string_sub_string< string_type_c >( string, trim_front_count, length - trim_front_count );
			return result;
		}

		template< typename string_type_c >
		string_type_c _string_trim_back( string_type_c const & string )
		{
			string_type_c result;
			sint32_c length = string.get_length();
			sint32_c trim_end_length = 0;
			for ( sint32_c i = length - 1; i >= 0; i-- )
			{
				auto b = string.character_list[ i ];
				if ( char16_is_space( b ) )
				{
					trim_end_length++;
				}
				else
				{
					break;
				}
			}
			result = _string_sub_string< string_type_c >( result, 0, length - trim_end_length );
			return result;
		}

		template< typename string_type_c >
		string_type_c _string_trim( string_type_c const & string )
		{
			string_type_c result;
			sint32_c start = 0;
			sint32_c end = result.get_length();
			while ( start <= end )
			{
				auto b = string.character_list[ start ];
				if ( char16_is_space( b ) )
				{
					start++;
				}
				else
				{
					break;
				}
			}
			while ( end > start )
			{
				auto b = string.character_list[ end ];
				if ( char16_is_space( b ) )
				{
					end--;
				}
				else
				{
					break;
				}
			}
			result = string;
			result.character_list.remove( -1, end - start );
			result.character_list.remove( 0, start );
			return result;
		}

		string8_c string8_trim_front( string8_c const & string )
		{
			return _string_trim_front< string8_c >( string );
		}

		string8_c string8_trim_back( string8_c const & string )
		{
			return _string_trim_back< string8_c >( string );
		}

		string8_c string8_trim( string8_c const & string )
		{
			return _string_trim< string8_c >( string );
		}

		string16_c string16_trim_front( string16_c const & string )
		{
			return _string_trim_front< string16_c >( string );
		}

		string16_c string16_trim_back( string16_c const & string )
		{
			return _string_trim_back< string16_c >( string );
		}

		string16_c string16_trim( string16_c const & string )
		{
			return _string_trim< string16_c >( string );
		}

		template< typename string_type_c >
		boolean_c _string_find_index_of( string_type_c const & string, string_type_c const & find, sint32_c & result )
		{
			result = -1;
			sint32_c i = 0; // current position in string.
			sint32_c j = 0; // current position in needle.
			auto const * a = string.character_list.get_internal_array();
			auto const * b = find.character_list.get_internal_array();
			while ( a[ i ] )
			{
				j = 0;
				while ( b[ j ] )
				{
					if ( a[ i + j ] == b[ j ] )
					{
						j++;
						if ( b[ j ] == 0 )
						{
							result = i;
							return true;
						}
					}
					else
					{
						break;
					}
				}
				i++;
			}
			return false;
		}

		boolean_c string8_find_index_of( string8_c const & string, string8_c const & find, sint32_c & result )
		{
			return _string_find_index_of< string8_c >( string, find, result );
		}

		boolean_c string16_find_index_of( string16_c const & string, string16_c const & find, sint32_c & result )
		{
			return _string_find_index_of< string16_c >( string, find, result );
		}

		template< typename string_type_c >
		boolean_c _string_find_indices_of( string_type_c const & string, string_type_c const & find, core_list_c< sint32_c > & result )
		{
			result.remove_all();
			sint32_c i = 0; // current position in string.
			sint32_c j = 0; // current position in needle.
			auto const * a = string.character_list.get_internal_array();
			auto const * b = find.character_list.get_internal_array();
			while ( a[ i ] )
			{
				j = 0;
				while ( a[ i + j ] == b[ j ] )
				{
					j++;
					if ( b[ j ] == 0 )
					{
						result.insert( -1, i );
						i += j;
						break;
					}
				}
				i++;
			}
			return result.get_length() != 0;
		}

		boolean_c string8_find_indices_of( string8_c const & string, string8_c const & find, core_list_c< sint32_c > & result )
		{
			return _string_find_indices_of< string8_c >( string, find, result );
		}

		boolean_c string16_find_indices_of( string16_c const & string, string16_c const & find, core_list_c< sint32_c > & result )
		{
			return _string_find_indices_of< string16_c >( string, find, result );
		}

		template< typename string_type_c >
		string_type_c _string_find_and_replace_all( string_type_c const & string, string_type_c const & find, string_type_c const & replace, boolean_c case_sensitive )
		{
			assert( find.get_length() > 0 );
			string_type_c result;
			result.character_list.remove_all();
			auto const * s = string.character_list.get_internal_array();
			auto const * f = find.character_list.get_internal_array();
			auto const * r = replace.character_list.get_internal_array();
			sint32_c i = 0; // current position in string.
			while ( s[ i ] )
			{
				sint32_c j = 0; // current position in needle, and current offset relative to current position in string.
				while ( char16_compare( s[ i + j ], f[ j ], case_sensitive ) )
				{
					j++;
					if ( f[ j ] == 0 )
					{
						sint32_c k = 0;
						while ( r[ k ] )
						{
							result.character_list.insert( -1, r[ k ] );
							k++;
						}
						i += j;
						goto replaced;
					}
				}
				result.character_list.insert( -1, s[ i ] );
				i++;
			replaced:
				continue;
			}
			result.character_list.insert( -1, 0 );
			return result;
		}

		string8_c string8_find_and_replace_all( string8_c const & string, string8_c const & find, string8_c const & replace, boolean_c case_sensitive )
		{
			return _string_find_and_replace_all< string8_c >( string, find, replace, case_sensitive );
		}

		string16_c string16_find_and_replace_all( string16_c const & string, string16_c const & find, string16_c const & replace, boolean_c case_sensitive )
		{
			return _string_find_and_replace_all< string16_c >( string, find, replace, case_sensitive );
		}

		template< typename string_type_c >
		void_c _string_split_at_spaces( string_type_c const & string, core_list_c< string_type_c > & result )
		{
			result.remove_all();
			sint32_c sub_string_start = 0;
			sint32_c sub_string_count = 0;
			sint32_c length = string.get_length();
			for ( sint32_c i = 0; i < length; i++ )
			{
				if ( char16_is_space( string.character_list[ i ] ) )
				{
					if ( sub_string_count > 0 )
					{
						result.insert( -1, _string_sub_string< string_type_c >( string, sub_string_start, sub_string_count ) );
					}
					sub_string_count = 0;
				}
				else
				{
					if ( sub_string_count == 0 )
					{
						sub_string_start = i;
					}
					sub_string_count++;
				}
			}
			if ( sub_string_count > 0 )
			{
				result.insert( -1, _string_sub_string< string_type_c >( string, sub_string_start, sub_string_count ) );
			}
		}

		template< typename string_type_c >
		void_c _string_split_at_delimiter( string_type_c const & string, string_type_c const & delimiter, core_list_c< string_type_c > & result )
		{
			result.remove_all();
			if ( delimiter.get_length() >= string.get_length() )
			{
				result.insert( -1, string );
			}
			else
			{
				sint32_c sub_string_start = 0;
				sint32_c sub_string_count = 0;
				sint32_c j = 0;
				sint32_c length = string.get_length();
				for ( sint32_c i = 0; i < length; i++ )
				{
					if ( delimiter.character_list[ j ] == string.character_list[ i ] )
					{
						j++;
						sub_string_count++;
						if ( j == delimiter.get_length() )
						{
							sub_string_count -= j;
							if ( sub_string_count > 0 )
							{
								result.insert( -1, _string_sub_string< string_type_c >( string, sub_string_start, sub_string_count ) );
							}
							j = 0;
							sub_string_start = i + 1;
							sub_string_count = 0;
						}
					}
					else
					{
						j = 0;
						sub_string_count++;
					}
				}
				if ( sub_string_count > 0 )
				{
					result.insert( -1, _string_sub_string< string_type_c >( string, sub_string_start, sub_string_count ) );
				}
			}
		}

		void_c string8_split_at_spaces( string8_c const & string, core_list_c< string8_c > & result )
		{
			_string_split_at_spaces< string8_c >( string, result );
		}

		void_c string8_split_at_delimiter( string8_c const & string, string8_c const & delimiter, core_list_c< string8_c > & result )
		{
			_string_split_at_delimiter< string8_c >( string, delimiter, result );
		}

		void_c string16_split_at_spaces( string16_c const & string, core_list_c< string16_c > & result )
		{
			_string_split_at_spaces< string16_c >( string, result );
		}

		void_c string16_split_at_delimiter( string16_c const & string, string16_c const & delimiter, core_list_c< string16_c > & result )
		{
			_string_split_at_delimiter< string16_c >( string, delimiter, result );
		}

		template< typename string_type_c >
		string_type_c _string_sub_string( string_type_c const & string, sint32_c start, sint32_c length )
		{
			assert( start >= 0 && length >= 0 && start + length <= string.get_length() );
			string_type_c result;
			result.character_list.construct_mode_dynamic_from_array( &string.character_list.get_internal_array()[ start ], length );
			result.character_list.insert( -1, 0 );
			return result;
		}

		string8_c string8_sub_string( string8_c const & string, sint32_c start, sint32_c length )
		{
			return _string_sub_string< string8_c >( string, start, length );
		}

		string16_c string16_sub_string( string16_c const & string, sint32_c start, sint32_c length )
		{
			return _string_sub_string< string16_c >( string, start, length );
		}


		template< typename char_type_c >
		boolean_c _string_starts_with( char_type_c const * a, char_type_c const * b )
		{
			while ( *a && *b )
			{
				if ( *a != *b )
				{
					return false;
				}
				a++;
				b++;
			}
			return *b == 0;
		}


		boolean_c string8_starts_with( char8_c const * a, char8_c const * b )
		{
			return _string_starts_with< char8_c >( a, b );
		}

		boolean_c string8_starts_with( char8_c const * a, char16_c const * b )
		{
			char8_c c_buffer[ 3 ];
			while ( *a && *b )
			{
				char8_c * c_buffer_pointer = c_buffer;
				sint8_c byte_count = convert_char16_to_utf8( *b, c_buffer_pointer );
				for ( sint8_c i = 0; i < byte_count; i++ )
				{
					if ( a[ i ] != c_buffer[ i ] )
					{
						return false;
					}
				}
				a += byte_count;
				b += 1;
			}
			return *b == 0;// || *a == *b; // *b will == 0 if pass, *a will == *b if both are 0 which is also pass.
		}

		boolean_c string16_starts_with( char16_c const * a, char8_c const * b )
		{
			while ( *a && *b )
			{
				char16_c c;
				convert_utf8_to_char16( b, c );
				if ( *a != c )
				{
					return false;
				}
				a += 1;
			}
			return *b == 0;// || *a == *b; // *b will == 0 if pass, *a will == *b if both are 0 which is also pass.
		}

		boolean_c string16_starts_with( char16_c const * a, char16_c const * b )
		{
			return _string_starts_with< char16_c >( a, b );
		}

		template< typename char_type_c >
		boolean_c _string_ends_with( char_type_c const * a, char_type_c const * b )
		{
			sint32_c a_length = _string_find_length< char_type_c >( a );
			sint32_c b_length = _string_find_length< char_type_c >( b );
			sint32_c a_index = a_length - 1;
			sint32_c b_index = b_length - 1;
			while ( a_index >= 0 && b_index >= 0 )
			{
				if ( a[ a_index ] != b[ b_index ] )
				{
					return false;
				}
				a_index--;
				b_index--;
			}
			return b_index == -1 || a_index == b_index; // b_index will == -1 if pass, a_index will == b_index if both are -1, which is also pass.
		}

		boolean_c string8_ends_with( char8_c const * a, char8_c const * b )
		{
			return _string_ends_with< char8_c >( a, b );
		}

		boolean_c string8_ends_with( char8_c const * a, char16_c const * b )
		{
			sint32_c a_length = string8_find_length( a );
			sint32_c b_length = string16_find_length( b );

			sint32_c a_index = a_length - 1;
			sint32_c b_index = b_length - 1;

			char8_c b_buffer[ 3 ];
			while ( a_index >= 0 && b_index >= 0 )
			{
				char8_c * b_buffer_pointer = b_buffer;
				sint8_c b_bytes_length = convert_char16_to_utf8( b[ b_index ], b_buffer_pointer );
				sint8_c b_bytes_index = b_bytes_length - 1;
				while ( a_index >= 0 && b_bytes_index >= 0 )
				{
					if ( a[ a_index ] != b_buffer[ b_bytes_index ] )
					{
						return false;
					}
					a_index -= 1;
					b_bytes_index -= 1;
				}
				b_index -= 1;
			}
			return b_index == -1 || a_index == b_index; // b_index will == -1 if pass, a_index will == b_index if both are -1, which is also pass.
		}

		boolean_c string16_ends_with( char16_c const * a, char8_c const * b )
		{
			0x80; // 1 byte sequence: first byte signature mask, 1 bit, for 1 byte sequence.

			0xC0; // 2 byte sequence: first byte signature, 3 bits.
			0xE0; // 2 byte sequence: first byte signature mask, 3 bits.
			
			0xE0; // 3 byte sequence: first byte signature, 4 bits.
			0xF0; // 3 byte sequence: first byte sitnature bit mask, 4 bits.

			0x80; // 2 and 3 byte sequences: subsequent byte signature, 2 bits.
			0xC0; // 2 and 3 byte sequences: subsequent byte sitnature bit mask, 2 bits.

			sint32_c a_length = string16_find_length( a );
			sint32_c b_length = string8_find_length( b );

			sint32_c a_index = a_length - 1;
			sint32_c b_index = b_length - 1;

			while ( a_index >= 0 && b_index >= 0 )
			{
				char16_c b_character = 0;
				char8_c const * b_buffer = nullptr;
				if ( ( b[ b_index ] & 0x80 ) == 0x00 )
				{
					// 1 byte sequence.
					b_buffer = &b[ b_index ];
					b_index -= 1;
				}
				else if ( ( b[ b_index ] & 0xC0 ) == 0x80 )
				{
					if ( ( b_index >= 1 ) && ( b[ b_index - 1 ] & 0xE0 ) == 0xC0 )
					{
						// 2 byte sequence
						b_buffer = &b[ b_index - 1 ];
						b_index -= 2;
					}
					else if ( ( b_index >= 1 ) && ( b[ b_index - 1 ] & 0xC0 ) == 0x80 )
					{
						if ( ( b_index >= 2 ) && ( b[ b_index - 2 ] & 0xF0 ) == 0xE0 )
						{
							// 3 byte sequence.
							b_buffer = &b[ b_index - 2 ];
							b_index -= 3;
						}
						else
						{
							assert( false );
						}
					}
					else
					{
						assert( false );
					}
				}
				else
				{
					assert( false );
				}
				convert_utf8_to_char16( b_buffer, b_character );
				if ( a[ a_index ] != b_character )
				{
					return false;
				}
				a_index -= 1;
			}
			return b_index == -1 || a_index == b_index; // b_index will == -1 if pass, a_index will == b_index if both are -1, which is also pass.
		}

		boolean_c string16_ends_with( char16_c const * a, char16_c const * b )
		{
			return _string_ends_with< char16_c >( a, b );
		}

		boolean_c string8_sort_compare( string8_c const & a, string8_c const & b )
		{
			return strnatcmp::strnatcmp0< char8_c >( a.character_list.get_internal_array(), b.character_list.get_internal_array(), false ) > 0;
		}

		boolean_c string8_sort_compare_case_insensitive( string8_c const & a, string8_c const & b )
		{
			return strnatcmp::strnatcmp0< char8_c >( a.character_list.get_internal_array(), b.character_list.get_internal_array(), true ) > 0;
		}

		boolean_c string16_sort_compare( string16_c const & a, string16_c const & b )
		{
			return strnatcmp::strnatcmp0< char16_c >( a.character_list.get_internal_array(), b.character_list.get_internal_array(), false ) > 0;
		}

		boolean_c string16_sort_compare_case_insensitive( string16_c const & a, string16_c const & b )
		{
			return strnatcmp::strnatcmp0< char16_c >( a.character_list.get_internal_array(), b.character_list.get_internal_array(), true ) > 0;
		}

		boolean_c string8_compare( char8_c const * a, char8_c const * b )
		{
			assert( a != nullptr && b != nullptr );

			if ( a == b )
			{
				return true;
			}
			while ( *a && *b )
			{
				if ( *a != *b )
				{
					return false;
				}
				a++;
				b++;
			}
			return *a == *b;
		}

		boolean_c string8_compare_case_insensitive( char8_c const * a, char8_c const * b )
		{
			assert( a != nullptr && b != nullptr );
			if ( a == b )
			{
				return true;
			}
			while ( *a && *b )
			{
				if ( char8_to_lower( *a ) != char8_to_lower( *b ) )
				{
					return false;
				}
				a++;
				b++;
			}
			return *a == *b;
		}

		boolean_c string16_compare( char16_c const * a, char16_c const * b )
		{
			assert( a != nullptr && b != nullptr );
			if ( a == b )
			{
				return true;
			}
			while ( *a && *b )
			{
				if ( *a != *b )
				{
					return false;
				}
				a++;
				b++;
			}
			return *a == *b;
		}

		boolean_c string16_compare_case_insensitive( char16_c const * a, char16_c const * b )
		{
			assert( a != nullptr && b != nullptr );
			if ( a == b )
			{
				return true;
			}
			while ( *a && *b )
			{
				if ( char16_to_lower( *a ) != char16_to_lower( *b ) )
				{
					return false;
				}
				a++;
				b++;
			}
			return *a == *b;;
		}

		boolean_c string16_compare( char16_c const * a, char8_c const * b )
		{
			assert( a != nullptr && b != nullptr );
			while ( *a != 0 && *b != 0 )
			{
				char16_c character;
				convert_utf8_to_char16( b, character );
				if ( *a != character )
				{
					return false;
				}
				a++;
			}
			return *a == *b;
		}

		boolean_c string16_compare_case_insensitive( char16_c const * a, char8_c const * b )
		{
			assert( a != nullptr && b != nullptr );
			while ( *a != 0 && *b != 0 )
			{
				if ( char16_to_lower( *a ) != char16_to_lower( *b ) )
				{
					return false;
				}
				a++;
				b++;
			}
			return *a == *b;
		}

		template< typename char_type_c >
		sint32_c _string_find_length( char_type_c const * string )
		{
			assert( string );
			char_type_c const * string_start = string;
			while ( *string != 0 )
			{
				string++;
			}
			return static_cast< sint32_c >( string - string_start );
		}

		sint32_c string8_find_length( char8_c const * string )
		{
			return _string_find_length< char8_c >( string );
		}

		sint32_c string16_find_length( char16_c const * string )
		{
			return _string_find_length< char16_c >( string );
		}

		//sint32_c string8_find_length_safe( char8_c const * string, char8_c const * limit )
		//{
		//	assert( string != nullptr );
		//	assert( string < limit );
		//	char8_c const * string_start = string;
		//	while ( string < limit && *string != 0 )
		//	{
		//		string++;
		//	}
		//	if ( string < limit )
		//	{
		//		return static_cast< sint32_c >( string - string_start );
		//	}
		//	return -1;
		//}

		boolean_c string8_is_valid_for_key( string8_c const & string )
		{
			sint32_c string_length = string.get_length();
			for ( sint32_c i = 0; i < string_length; i++ )
			{
				char8_c character = string.character_list[ i ];
				if ( !( ( character >= 'a' && character <= 'z' ) || ( character >= 'A' && character <= 'Z' ) || ( character >= '0' && character <= '9' ) || ( character == '_' || character == '-' || character == '.' || character == '[' || character == ']' ) ) )
				{
					// multi-byte utf-8 encodings characters will reach here too.
					return false;
				}
			}
			return true;
		}

		uint8_c convert_utf8_to_char16( char8_c const * & in, char16_c & out )
		{
			assert( in != nullptr );
			out = 0;
			if ( ( in[ 0 ] & 0x80 ) == 0x00 )
			{
				// out 7 bits
				// in 1 byte
				// 0bbbbbbb
				assert( in[ 0 ] != 0 );
				out |= static_cast< char16_c >( in[ 0 ] );
				in += 1;
				return 1;
			}
			else if ( ( in[0] & 0xE0 ) == 0xC0 )
			{
				// out 11 bits
				// in 2 bytes
				// 110bbbbb 10bbbbbb
				assert( in[ 0 ] != 0 );
				assert( in[ 1 ] != 0 );
				out |= static_cast<char16_c>( in[ 0 ] & 0x1F ) << 6;
				out |= static_cast<char16_c>( in[ 1 ] & 0x3F );
				in += 2;
				return 2;
			}
			else if ( ( in[0] & 0xF0 ) == 0xE0 )
			{
				// out 16 bits
				// in 3 bytes
				// 1110bbbb 10bbbbbb 10bbbbbb
				assert( in[ 0 ] != 0 );
				assert( in[ 1 ] != 0 );
				assert( in[ 2 ] != 0 );
				out |= static_cast<char16_c>( in[ 0 ] & 0x0F ) << 12;
				out |= static_cast<char16_c>( in[ 1 ] & 0x3F ) << 6;
				out |= static_cast<char16_c>( in[ 2 ] & 0x3F );
				in += 3;
				return 3;
			}
			assert( false );
			return 0;
		}

		void_c convert_string8_to_string16( core_list_c< char8_c > const & in, core_list_c< char16_c > & out )
		{
			assert( out.get_mode_is_dynamic() == true );
			if ( out.get_length() > 0 )
			{
				assert( out[ out.get_length() - 1 ] != 0 );
			}
			assert( in[ in.get_length() - 1 ] == 0 );
			char8_c const * byte = in.get_internal_array();
			while ( *byte != 0 )
			{
				char16_c character;
				convert_utf8_to_char16( byte, character );
				out.insert( -1, character );
			}
			out.insert( -1, 0 );
		}

		void_c convert_string8_to_string16_bytes( core_list_c< char8_c > const & in, core_list_c< char8_c > & out )
		{
			assert( out.get_mode_is_dynamic() == true );
			if ( out.get_length() > 0 )
			{
				assert( out[ out.get_length() - 1 ] != 0 );
			}
			assert( in[ in.get_length() - 1 ] == 0 );
			char8_c const * byte = in.get_internal_array();
			while ( *byte != 0 )
			{
				char16_c character;
				convert_utf8_to_char16( byte, character );
				char8_c * character_bytes = reinterpret_cast< char8_c * >( &character );
				out.insert( -1, character_bytes[ 0 ] );
				out.insert( -1, character_bytes[ 1 ] );
			}
			out.insert( -1, 0 );
			out.insert( -1, 0 );
		}

		sint8_c convert_char16_to_utf8( char16_c in, char8_c * & out )
		{
			if ( in < 0x80 )
			{
				// in 7 bits
				// out 1 byte
				// 0bbbbbbb
				out[ 0 ] = static_cast< char8_c >( in );
				out += 1;
				return 1;
			}
			else if ( in < 0x800 )
			{
				// in 11 bits
				// out 2 bytes
				// 110bbbbb 10bbbbbb
				out[ 1 ] = static_cast< char8_c >( ( in | 0x80 ) & 0xBF ); in >>= 6;
				out[ 0 ] = static_cast< char8_c >( in | 0xC0 );
				out += 2;
				return 2;
			}
			else if ( in < 0x10000 )
			{
				// in 16 bits
				// out 3 bytes
				// 1110bbbb 10bbbbbb 10bbbbbb
				out[ 2 ] = static_cast< char8_c >( ( in | 0x80 ) & 0xBF ); in >>= 6;
				out[ 1 ] = static_cast< char8_c >( ( in | 0x80 ) & 0xBF ); in >>= 6;
				out[ 0 ] = static_cast< char8_c >( in | 0xE0 );
				out += 3;
				return 3;
			}
			//else if ( in < 0x110000 )
			//{
			//	out[ 3 ] = static_cast< char8_c >( ( in | 0x80 ) & 0xBF ); in >>= 6;
			//	out[ 2 ] = static_cast< char8_c >( ( in | 0x80 ) & 0xBF ); in >>= 6;
			//	out[ 1 ] = static_cast< char8_c >( ( in | 0x80 ) & 0xBF ); in >>= 6;
			//	out[ 0 ] = static_cast< char8_c >( in | 0xF0 );
			//	out += 4;
			//}
			assert( false );
			return 0;
		}

		void_c convert_string16_to_string8( core_list_c< char16_c > const & in, core_list_c< char8_c > & out )
		{
			assert( out.get_mode_is_dynamic() == true );
			if ( out.get_length() > 0 )
			{
				assert( out[ out.get_length() - 1 ] != 0 ); // i need to delete this check once i have peace of mind.
			}
			char16_c const * character = in.get_internal_array();
			char16_c const * character_end = character + in.get_length();
			char8_c buffer_base[ 3 ];
			while ( character < character_end && * character )
			{
				char8_c * buffer = buffer_base;
				sint8_c bytes_count = convert_char16_to_utf8( character[ 0 ], buffer );
				for ( sint8_c i = 0; i < bytes_count; i++ )
				{
					out.insert( -1, buffer_base[ i ] );
				}
				character++;
			}
			out.insert( -1, 0 );
		}


		//
		//
		// memory operations.
		//
		//

		void_c * memory_aligned_allocate( sint32_c size, sint32_c alignment )
		{
			return _aligned_malloc( size, alignment );
		}

		void_c memory_aligned_deallocate( void_c * block )
		{
			_aligned_free( block );
		}

		void_c * memory_allocate( sint32_c size )
		{
			assert( size > 0 );
			return std::malloc( size );
		}

		void_c * memory_reallocate( void_c * block, sint32_c new_size )
		{
			assert( new_size > 0 );
			return std::realloc( block, new_size );
		}

		void_c memory_deallocate( void_c * block )
		{
			std::free( block );
		}

		void_c memory_move( void_c const * buffer_source, void_c * buffer_destination, sint32_c buffer_size )
		{
			std::memmove( buffer_destination, buffer_source, buffer_size );
		}

		void_c memory_copy( void_c const * buffer_source, void_c * buffer_destination, sint32_c buffer_size )
		{
			std::memcpy( buffer_destination, buffer_source, buffer_size );
		}

		void_c memory_zero( void_c * buffer, sint32_c buffer_size )
		{
			std::memset( buffer, 0, buffer_size );
		}

		boolean_c memory_is_zero( void_c const * buffer, sint32_c buffer_size )
		{
			uint8_c const * bytes = reinterpret_cast< uint8_c const * >( buffer );
			for ( sint32_c i = 0; i < buffer_size; i++ )
			{
				if ( bytes[ i ] != 0 )
				{
					return false;
				}
			}
			return true;

			/*
			uint_native_c bytes_before = reinterpret_cast< uint_native_c >( buffer ) % sizeof( uint_native_c );
			if ( bytes_before != 0 )
			{
				bytes_before = sizeof( uint_native_c ) - bytes_before;
			}
			uint_native_c bytes_after = ( reinterpret_cast< uint_native_c >( buffer ) + buffer_size ) % sizeof( uint_native_c );
			uint_native_c natives_in_the_middle = ( reinterpret_cast< uint_native_c >( buffer ) + bytes_before + buffer_size - bytes_after - reinterpret_cast< uint_native_c >( buffer ) ) / sizeof( uint_native_c );
			uint8_c const * buffer_uint8 = reinterpret_cast< uint8_c const * >( buffer );
			for ( uint_native_c i = 0; i < bytes_before; i++ )
			{
				if ( buffer_uint8[ i ] != 0 )
				{
					return false;
				}
			}
			assert( reinterpret_cast< uint_native_c >( buffer_uint8 + bytes_before ) % sizeof( uint_native_c ) == 0 );
			uint_native_c const * buffer_uint_native = reinterpret_cast< uint_native_c const * >( buffer_uint8 + bytes_before );
			for ( uint_native_c i = 0; i < natives_in_the_middle; i++ )
			{
				if( buffer_uint_native[ i ] != 0 )
				{
					return false;
				}
			}
			buffer_uint8 = reinterpret_cast< uint8_c const * >( buffer_uint_native + natives_in_the_middle );
			for ( uint_native_c i = 0; i < bytes_after; i++ )
			{
				if ( buffer_uint8[ i ] != 0 )
				{
					return false;
				}
			}
			return true;
			*/
		}

		boolean_c memory_compare( void_c const * buffer_a, void_c const * buffer_b, sint32_c byte_count )
		{
			return memcmp( buffer_a, buffer_b, byte_count ) == 0;
		}


		//
		//
		// data operations.
		//
		//

		sint32_c get_data_type_size( data_type_e type )
		{
			switch( type )
			{
			case data_type_e_uint8:
			case data_type_e_sint8:
				return 1;
			case data_type_e_uint16:
			case data_type_e_sint16:
				return 2;
			case data_type_e_uint32:
			case data_type_e_sint32:
			case data_type_e_float32:
				return 4;
			case data_type_e_uint64:
			case data_type_e_sint64:
			case data_type_e_float64:
				return 8;
			}
			return 0;
		}

		byte_order_e get_native_byte_order()
		{
			const union { uint32_c u; uint8_c c[ 4 ]; } probe = { 1 };
			return probe.c[ 0 ] != 0 ? byte_order_e_little : byte_order_e_big;
		}

		void_c flip_byte_order( void_c * buffer, sint32_c buffer_size )
		{
			assert( buffer_size > 0 && buffer_size < 16 );
			uint8_c * data_bytes = reinterpret_cast< uint8_c * >( buffer );
			for ( sint32_c i = 0; i < buffer_size; i++ )
			{
				uint8_c t = data_bytes[ i ];
				data_bytes[ i ] = data_bytes[ buffer_size - i - 1 ];
				data_bytes[ buffer_size - i - 1 ] = t;
			}
		}


		//
		//
		// hash operations.
		//
		//

		uint32_c xxhash32( void_c const * data, uint_native_c data_size )
		{
			return XXH32( static_cast< const void_c * >( data ), static_cast< size_t >( data_size ), 0 );
		}

		uint64_c xxhash64( void_c const * data, uint_native_c data_size )
		{
			return XXH64( static_cast< const void_c * >( data ), static_cast< size_t >( data_size ), 0 );
		}

	}
}
