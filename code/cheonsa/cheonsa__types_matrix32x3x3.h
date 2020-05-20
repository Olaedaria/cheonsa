#pragma once

#include "cheonsa__types_vector32x3.h"

namespace cheonsa
{

	// single precision 3d basis transform matrix, used to rotate and scale 3d points and vectors.
	class matrix32x3x3_c
	{
	public:
		vector32x3_c a;
		vector32x3_c b;
		vector32x3_c c;

	public:
		static matrix32x3x3_c get_zero();
		static matrix32x3x3_c get_identity();

	public:
		explicit matrix32x3x3_c();
		explicit matrix32x3x3_c( vector32x3_c const & a, vector32x3_c const & b, vector32x3_c const & c );
		explicit matrix32x3x3_c( float32_c aa, float32_c ab, float32_c ac, float32_c ba, float32_c bb, float32_c bc, float32_c ca, float32_c cb, float32_c cc );

		vector32x3_c & get_element_at_index( sint32_c i );
		vector32x3_c const & get_element_at_index( sint32_c i ) const;

		matrix32x3x3_c operator *= ( float32_c b );

	};

	matrix32x3x3_c operator * ( matrix32x3x3_c a, matrix32x3x3_c const & b );

}
