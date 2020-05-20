#pragma once

#include "cheonsa__types__fundamental.h"
#include "cheonsa__types_vector32x2.h"
#include "cheonsa__types_vector32x3.h"
#include "cheonsa__types_vector32x4.h"
#include "cheonsa__types_vector64x2.h"
#include "cheonsa__types_vector64x3.h"
#include "cheonsa__types_vector64x4.h"
#include "cheonsa__types_quaternion32.h"

namespace cheonsa
{
	namespace ops
	{

		float32_c interpolate_linear( float32_c const source, float32_c const destination, float32_c const amount );
		vector32x2_c interpolate_linear( vector32x2_c const & source, vector32x2_c const & destination, float32_c const amount );
		vector32x3_c interpolate_linear( vector32x3_c const & source, vector32x3_c const & destination, float32_c const amount );
		vector32x4_c interpolate_linear( vector32x4_c const & source, vector32x4_c const & destination, float32_c const amount );

		float64_c interpolate_linear( float64_c const source, float64_c const destination, float64_c const amount );
		vector64x2_c interpolate_linear( vector64x2_c const & source, vector64x2_c const & destination, float64_c const amount );
		vector64x3_c interpolate_linear( vector64x3_c const & source, vector64x3_c const & destination, float64_c const amount );
		vector64x4_c interpolate_linear( vector64x4_c const & source, vector64x4_c const & destination, float64_c const amount );

		float32_c interpolate_cosine( float32_c const source, float32_c const destination, float32_c const amount );
		vector32x2_c interpolate_cosine( vector32x2_c const & source, vector32x2_c const & destination, float32_c const amount );
		vector32x3_c interpolate_cosine( vector32x3_c const & source, vector32x3_c const & destination, float32_c const amount );
		vector32x4_c interpolate_cosine( vector32x4_c const & source, vector32x4_c const & destination, float32_c const amount );

		float32_c interpolate_cubic( float32_c const previous_source, float32_c const source, float32_c const destination, float32_c const next_destination, float32_c const amount );
		vector32x2_c interpolate_cubic( vector32x2_c const & previous_source, vector32x2_c const & source, vector32x2_c const & destination, vector32x2_c const & next_destination, float32_c const amount );
		vector32x3_c interpolate_cubic( vector32x3_c const & previous_source, vector32x3_c const & source, vector32x3_c const & destination, vector32x3_c const & next_destination, float32_c const amount );
		vector32x4_c interpolate_cubic( vector32x4_c const & previous_source, vector32x4_c const & source, vector32x4_c const & destination, vector32x4_c const & next_destination, float32_c const amount );
		
		float32_c interpolate_hermite( float32_c const previous_source, float32_c const source, float32_c const destination, float32_c const next_destination, float32_c const amount, float32_c const tension, float32_c const bias );
		vector32x2_c interpolate_hermite( vector32x2_c const & previous_source, vector32x2_c const & source, vector32x2_c const & destination, vector32x2_c const & next_destination, float32_c const amount, float32_c const tension, float32_c const bias );
		vector32x3_c interpolate_hermite( vector32x3_c const & previous_source, vector32x3_c const & source, vector32x3_c const & destination, vector32x3_c const & next_destination, float32_c const amount, float32_c const tension, float32_c const bias );
		vector32x4_c interpolate_hermite( vector32x4_c const & previous_source, vector32x4_c const & source, vector32x4_c const & destination, vector32x4_c const & next_destination, float32_c const amount, float32_c const tension, float32_c const bias );

		quaternion32_c interpolate_spherical_linear( quaternion32_c const & source, quaternion32_c const & destination, float32_c const amount );

		quaternion32_c traverse( quaternion32_c const & source, quaternion32_c const & destination, float32_c amount ); // traverses from source to destination by amount in radians.

		float32_c traverse( float32_c const source, float32_c const destination, float32_c const amount );

		float32_c traverse_with_ease( float32_c const source, float32_c const destination, float32_c amount, float32_c const margin );

		//float32_c traverse_with_momentum( float32_c source, float32_c destination, float32_c ease_in_acceleration, float32_c ease_out_runway, float32_c speed_target, float32_c & speed_current );

	}
}
