#pragma once

#include "cheonsa__types_vector64x2.h"

namespace cheonsa
{

	// a is conventionally x, width, negative is towards left, positive is towards right.
	// b is conventionally y, height, negative is towards up, positive is towards down.
	class box64x2_c
	{
	public:
		vector64x2_c minimum;
		vector64x2_c maximum;

	public:
		explicit box64x2_c();
		explicit box64x2_c( vector64x2_c const & minimum, vector64x2_c const & maximum );
		explicit box64x2_c( float64_c minimum_a, float64_c minimum_b, float64_c maximum_a, float64_c maximum_b );
		explicit box64x2_c( float64_c const * array );

		inline float64_c * as_array() { return reinterpret_cast< float64_c * >( this ); }
		inline float64_c const * as_array() const { return reinterpret_cast< float64_c const * >( this ); }

		vector64x2_c get_center() const; // center point.
		inline vector64x2_c get_size() const { return vector64x2_c( maximum.a - minimum.a, maximum.b - minimum.b ); }
		inline float64_c get_width() const { return maximum.a - minimum.a; }
		inline float64_c get_height() const { return maximum.b - minimum.b; }

	};

	boolean_c operator == ( box64x2_c const & a, box64x2_c const & b );
	boolean_c operator != ( box64x2_c const & a, box64x2_c const & b );

}
