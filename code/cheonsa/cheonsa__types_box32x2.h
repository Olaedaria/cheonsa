#pragma once

#include "cheonsa__types__fundamental.h"
#include "cheonsa__types_vector32x2.h"

namespace cheonsa
{

	// a is conventionally x, width, negative is towards left, positive is towards right.
	// b is conventionally y, height, negative is towards up, positive is towards down.
	class box32x2_c
	{
	public:
		vector32x2_c minimum; // left, top.
		vector32x2_c maximum; // right, bottom.

	public:
		explicit box32x2_c();
		explicit box32x2_c( vector32x2_c const & minimum, vector32x2_c const & maximum );
		explicit box32x2_c( float32_c const minimum_a, float32_c const minimum_b, float32_c const maximum_a, float32_c const maximum_b );
		explicit box32x2_c( float32_c const * array );
		
		static inline box32x2_c make_regular( float32_c width, float32_c height ) // top left at (0, 0), bottom right at (width, height).
		{
			return box32x2_c( 0.0f, 0.0f, width, height );
		}

		static inline box32x2_c make_regular( float32_c x, float32_c y, float32_c width, float32_c height )
		{
			return box32x2_c( x, y, x + width, y + height );
		}

		static inline box32x2_c make_centered( float32_c width, float32_c height ) // ( width * -0.5f, height * -0.5f, width * 0.5f, height * 0.5f ).
		{
			return box32x2_c( width * -0.5f, height * -0.5f, width * 0.5f, height * 0.5f );
		}

		inline float32_c * as_array() { return reinterpret_cast< float32_c * >( this ); }
		inline float32_c const * as_array() const { return reinterpret_cast< float32_c const * >( this ); }

		vector32x2_c get_center() const; // center point.
		inline vector32x2_c get_size() const { return vector32x2_c( maximum.a - minimum.a, maximum.b - minimum.b ); }
		inline float32_c get_width() const { return maximum.a - minimum.a; }
		inline float32_c get_height() const { return maximum.b - minimum.b; }

		void_c accumulate_bounds( box32x2_c const & other );

	};

	boolean_c operator == ( box32x2_c const & a, box32x2_c const & b );
	boolean_c operator != ( box32x2_c const & a, box32x2_c const & b );

}
