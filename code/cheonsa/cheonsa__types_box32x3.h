#pragma once

#include "cheonsa__types_vector32x3.h"

namespace cheonsa
{

	class box64x3_c;

	// a is conventionally x, width, negative is towards left, positive is towards right.
	// b is conventionally y, depth, negative is towards back, positive is towards front.
	// c is conventionally z, height, negative is towards down, positive is towards up.
	class box32x3_c
	{
	public:
		vector32x3_c minimum;
		vector32x3_c maximum;

	public:
		explicit box32x3_c();
		explicit box32x3_c( vector32x3_c const & minimum, vector32x3_c const & maximum );
		explicit box32x3_c( float32_c const minimum_a, float32_c const minimum_b, float32_c const minimum_c, float32_c const maximum_a, float32_c const maximum_b, float32_c const maximum_c );
		explicit box32x3_c( float32_c const * array );
		explicit box32x3_c( box64x3_c const & other );

		inline float32_c * as_array() { return reinterpret_cast< float32_c * >( this ); }
		inline float32_c const * as_array() const { return reinterpret_cast< float32_c const * >( this ); }

		vector32x3_c get_center() const;
		vector32x3_c get_size() const;
		inline float32_c get_width() const { return maximum.a - minimum.a; }
		inline float32_c get_depth() const { return maximum.b - minimum.b; }
		inline float32_c get_height() const { return maximum.c - minimum.c; }
		vector32x3_c get_point( sint32_c index ) const;

		void_c set_bounds( vector32x3_c const & point, float32_c radius );
		void_c accumulate_bounds( vector32x3_c const & point );
		void_c accumulate_bounds( vector32x3_c const & point, float32_c radius );

	};

	boolean_c operator == ( box64x3_c const & a, box64x3_c const & b );
	boolean_c operator != ( box64x3_c const & a, box64x3_c const & b );

}
