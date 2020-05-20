#pragma once

#include "cheonsa__types.h"

namespace cheonsa
{
	namespace ops
	{

		void_c convert_rgba_to_rgba( vector32x4_c const & value, uint32_c & result );
		void_c convert_rgba_to_rgba( uint32_c const & value, vector32x4_c & result );

		boolean_c convert_rgb_to_hsv( vector64x3_c const & rgb, vector64x3_c & hsv ); // hue is in degrees, saturation and value are in range 0 to 1. red green and blue are in range 0 to 1.
		boolean_c convert_hsv_to_rgb( vector64x3_c const & hsv, vector64x3_c & rgb ); // hue is in degrees, saturation and value are in range 0 to 1. red green and blue are in range 0 to 1.

		boolean_c convert_rgb_to_husl( float64_c const * rgb, float64_c * husl );
		boolean_c convert_husl_to_rgb( float64_c const * husl, float64_c * rgb );

		vector32x4_c adjust_color_brightness( vector32x4_c const & color, float32_c amount ); // adds each color channel by amount, clamps each color channel of result to range 0 to 1.
		vector32x4_c adjust_color_contrast( vector32x4_c const & color, float32_c amount ); // multiplies each color channel by amount, clamps each color channel of result to range 0 to 1.
		vector32x4_c adjust_color_saturation( vector32x4_c const & color, float32_c amount );

	}
}
