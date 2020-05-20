#pragma once

#include "cheonsa__types_vector32x2.h"

namespace cheonsa
{

	class vector32x4_c;

	// single precision 2d transform matrix, used to rotate and scale 2d points.
	// for 2d coordinate systems in cheonsa, the origin is conventionally in the top left, and the horizontal axis coordinates ascend in values towards the right, and the vertical axis coordinates ascend in values towards the bottom.
	class matrix32x2x2_c
	{
	public:
		vector32x2_c a;
		vector32x2_c b;

	public:
		explicit matrix32x2x2_c();
		explicit matrix32x2x2_c( vector32x2_c const & a, vector32x2_c const & b );
		explicit matrix32x2x2_c( float32_c const aa, float32_c const ab, float32_c const ba, float32_c const bb );
		explicit matrix32x2x2_c( float32_c const * values );
		
		inline float32_c * as_array() { return reinterpret_cast< float32_c * >( this ); }
		inline float32_c const * as_array() const { return reinterpret_cast< float32_c const * >( this ); }

		vector32x4_c const & as_vector32x4() const;

	};

	boolean_c operator == ( matrix32x2x2_c const & a, matrix32x2x2_c const & b );
	boolean_c operator != ( matrix32x2x2_c const & a, matrix32x2x2_c const & b );

	matrix32x2x2_c operator * ( matrix32x2x2_c const & a, matrix32x2x2_c const & b );

}
