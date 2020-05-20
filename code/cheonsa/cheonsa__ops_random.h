#pragma once

#include "cheonsa__types.h"

namespace cheonsa
{
	namespace ops
	{

		void_c random_start(); // initializes the random number generator, is called once at engine start up, needs to be called before any of the other random number generator functions can be called.
		float32_c random_float32( float32_c minimum = 0.0f, float32_c maximum = 1.0f );
		float64_c random_float64( float64_c minimum = 0.0, float64_c maximum = 1.0 );
		sint32_c random_sint32( sint32_c minimum = -2147483647, sint32_c maximum = 2147483647 );
		uint32_c random_uint32( uint32_c maximum = 4294967295 );
		vector32x2_c random_point_on_circle(); // returns random unit length vector.
		vector32x3_c random_point_on_sphere(); // returns random unit length vector.

	}
}
