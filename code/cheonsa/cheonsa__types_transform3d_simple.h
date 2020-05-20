#pragma once

#include "cheonsa__types_matrix32x3x3.h"
#include "cheonsa__types_vector64x3.h"

namespace cheonsa
{

	// a kind of 4x3 matrix.
	// multiplies like a 4x4 matrix, but with an imaginary 4th row of [0, 0, 0, 1].
	class transform3d_simple_c
	{
	public:
		matrix32x3x3_c basis;
		vector64x3_c position;

	};

	transform3d_simple_c operator * ( transform3d_simple_c const & a, transform3d_simple_c const & b );

}
