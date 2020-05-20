#pragma once

#include "cheonsa__types.h"

namespace cheonsa
{
	namespace ops
	{

		boolean_c math_is_real( float32_c a ); // returns true if the a is a real number, meaning that it is not NaN or infinity.
		boolean_c math_is_real( float64_c a );
		boolean_c math_compare_close( float32_c a, float32_c b, float32_c margin );
		boolean_c math_compare_close( float64_c a, float64_c b, float64_c margin );
		sint32_c math_minimum( sint32_c a, sint32_c b );
		float32_c math_minimum( float32_c a, float32_c b );
		float64_c math_minimum( float64_c a, float64_c b );
		sint32_c math_maximum( sint32_c a, sint32_c b );
		float32_c math_maximum( float32_c a, float32_c b );
		float64_c math_maximum( float64_c a, float64_c b );
		sint32_c math_absolute_value( sint32_c a );
		float32_c math_absolute_value( float32_c a );
		float64_c math_absolute_value( float64_c a );
		sint16_c math_clamp( sint16_c value, sint16_c minimum, sint16_c maximum );
		sint32_c math_clamp( sint32_c value, sint32_c minimum, sint32_c maximum );
		float32_c math_clamp( float32_c value, float32_c minimum, float32_c maximum );
		float64_c math_clamp( float64_c value, float64_c minimum, float64_c maximum );
		sint32_c math_next_power_of_two( sint32_c a );
		uint32_c math_next_power_of_two( uint32_c a );
		float32_c math_saturate( float32_c value );
		float64_c math_saturate( float64_c value );
		float32_c math_nearest_multiple( float32_c value, float32_c multiple );
		float64_c math_nearest_multiple( float64_c value, float64_c multiple );
		sint32_c math_next_nearest_multiple( sint32_c value, sint32_c multiple );
		float32_c math_round( float32_c value );
		float64_c math_round( float64_c value );
		float32_c math_round_down( float32_c value );
		float64_c math_round_down( float64_c value );
		float32_c math_round_up( float32_c value );
		float64_c math_round_up( float64_c value );
		float32_c math_round_to_decimal_place( float32_c value, sint32_c places );
		float64_c math_round_to_decimal_place( float64_c value, sint32_c places );
		float32_c math_modulo( float32_c value, float32_c multiple );
		float64_c math_modulo( float64_c value, float64_c multiple );
		float32_c math_natural_logarithm( float32_c value );
		float32_c math_square( float32_c value );
		float64_c math_square( float64_c value );
		float32_c math_cube( float32_c value );
		float64_c math_cube( float64_c value );
		sint32_c math_power( sint32_c value, sint32_c exponent );
		float32_c math_power( float32_c value, float32_c exponent );
		float64_c math_power( float64_c value, float64_c exponent );
		float32_c math_root( float32_c value, float32_c exponent );
		float64_c math_root( float64_c value, float64_c exponent );
		sint32_c math_square_root( sint32_c value );
		float32_c math_square_root( float32_c value );
		float64_c math_square_root( float64_c value );
		float32_c math_sine( float32_c angle );
		float64_c math_sine( float64_c angle );
		float32_c math_arc_sine( float32_c dot );
		float64_c math_arc_sine( float64_c dot );
		float32_c math_cosine( float32_c angle );
		float64_c math_cosine( float64_c angle );
		float32_c math_arc_cosine( float32_c dot );
		float64_c math_arc_cosine( float64_c dot );
		float32_c math_tangent( float32_c angle );
		float64_c math_tangent( float64_c angle );
		float32_c math_arc_tangent( float32_c dot );
		float64_c math_arc_tangent( float64_c dot );
		float32_c math_arc_tangent( float32_c b, float32_c a );
		float64_c math_arc_tangent( float64_c b, float64_c a );
		float32_c math_wrap( float32_c value, float32_c range ); // wraps a value between 0 and range. range can be set to 2*pi for radians, or 360 for degrees, or something else for something else.
		float64_c math_wrap( float64_c value, float64_c range );
		float32_c math_find_shortest_angle_difference( float32_c angle_a, float32_c angle_b ); // finds the shortest radian difference between two angles, such that angle_a + difference is equiavlent to angle_b, but angle_a + difference may end up lying outside of >= 0 and < pi * 2.0 range.
		float64_c math_find_shortest_angle_difference( float64_c angle_a, float64_c angle_b );
		float32_c math_find_angle_around_axis( vector32x3_c const & tangent_to_axis_that_defines_angle_zero, vector32x3_c const & tangent_to_axis_that_defines_angle_quarter, vector32x3_c const & tangent_to_find_angle_of ); // returns a result in the range -pi to pi. tangent_to_axis_that_defines_angle_zero can be thought of as the forward vector. tangent_to_axis_that_defines_angle_quarter can be thought of as the right vector and positive angle space. the cross product of tangent_to_axis_that_defines_angle_zero and tangent_to_axis_that_defines_angle_quarter would be the axis itself.
		float64_c math_find_angle_around_axis( vector64x3_c const & tangent_to_axis_that_defines_angle_zero, vector64x3_c const & tangent_to_axis_that_defines_angle_quarter, vector64x3_c const & tangent_to_find_angle_of ); // returns a result in the range -pi to pi. tangent_to_axis_that_defines_angle_zero can be thought of as the forward vector. tangent_to_axis_that_defines_angle_quarter can be thought of as the right vector and positive angle space. the cross product of tangent_to_axis_that_defines_angle_zero and tangent_to_axis_that_defines_angle_quarter would be the axis itself.
		float64_c math_wobble( float64_c time, float64_c frequency );
		sint32_c math_sign( float32_c value ); // returns 1 for value > 0, 0 for value == 0, and -1 for value < 0.
		sint32_c math_sign( float64_c value ); // returns 1 for value > 0, 0 for value == 0, and -1 for value < 0.
		sint32_c math_non_zero_sign( float32_c value ); // returns 1 for value > 0, and -1 for value <= 0. 
		sint32_c math_non_zero_sign( float64_c value ); // returns 1 for value > 0, and -1 for value <= 0.

	}
}
