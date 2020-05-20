#include "cheonsa__ops_math.h"
#include "cheonsa__ops_vector.h"
#include <cassert>

namespace cheonsa
{
	namespace ops
	{

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

	}
}
