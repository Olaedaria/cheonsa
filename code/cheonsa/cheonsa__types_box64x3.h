#pragma once

#include "cheonsa__types_vector64x3.h"

namespace cheonsa
{

	class box32x3_c;

	// a is conventionally x, width, negative is towards left, positive is towards right.
	// b is conventionally y, depth, negative is towards back, positive is towards front.
	// c is conventionally z, height, negative is towards down, positive is towards up.
	class box64x3_c
	{
	public:
		vector64x3_c minimum;
		vector64x3_c maximum;

	public:
		explicit box64x3_c();
		explicit box64x3_c( vector64x3_c const & minimum, vector64x3_c const & maximum );
		explicit box64x3_c( float64_c const minimum_a, float64_c const minimum_b, float64_c const minimum_c, float64_c const maximum_a, float64_c const maximum_b, float64_c const maximum_c );
		explicit box64x3_c( box32x3_c const & other );
		
		inline float64_c * as_array() { return reinterpret_cast< float64_c * >( this ); }
		inline float64_c const * as_array() const { return reinterpret_cast< float64_c const * >( this ); }

		vector64x3_c get_center() const;
		vector64x3_c get_size() const;
		inline float64_c get_width() const { return maximum.a - minimum.a; }
		inline float64_c get_depth() const { return maximum.b - minimum.b; }
		inline float64_c get_height() const { return maximum.c - minimum.c; }
		float64_c get_longest_dimension() const;
		float64_c get_longest_half_extent() const;
		vector64x3_c get_point( sint32_c const index ) const;

	};

	boolean_c operator == ( box32x3_c const & a, box32x3_c const & b );
	boolean_c operator != ( box32x3_c const & a, box32x3_c const & b );

}
