#pragma once

#include "cheonsa__types_vector32x4.h"
#include "cheonsa__types_vector32x3.h"

namespace cheonsa
{

	// single precision 3d transform matrix, used to rotate, scale, and translate 3d points and vectors.
	// row major, GPU compatible.
	class matrix32x4x4_c
	{
	public:
		vector32x4_c a; // usually basis_a.
		vector32x4_c b; // usually basis_b.
		vector32x4_c c; // usually basis_c.
		vector32x4_c d; // usually position.

	public:
		explicit matrix32x4x4_c();
		explicit matrix32x4x4_c( vector32x3_c const & basis_a, vector32x3_c const & basis_b, vector32x3_c const & basis_c, vector32x3_c const & position );
		explicit matrix32x4x4_c( vector32x4_c const & a, vector32x4_c const & b, vector32x4_c const & c, vector32x4_c const & d );
		explicit matrix32x4x4_c( float32_c const aa, float32_c const ab, float32_c const ac, float32_c const ad, float32_c const ba, float32_c const bb, float32_c const bc, float32_c const bd, float32_c const ca, float32_c const cb, float32_c const cc, float32_c const cd, float32_c const da, float32_c const db, float32_c const dc, float32_c const dd );
		explicit matrix32x4x4_c( float32_c const * values );

		inline float32_c * as_array() { return reinterpret_cast< float32_c * >( this ); }
		inline float32_c const * as_array() const { return reinterpret_cast< float32_c const * >( this ); }

		vector32x4_c & get_element_at_index( sint32_c const index );
		vector32x4_c const & get_element_at_index( sint32_c const index ) const;

		vector32x3_c & get_basis_element_at_index( sint32_c const index );
		vector32x3_c const & get_basis_element_at_index( sint32_c const index ) const;

		inline vector32x3_c & get_basis_a() { return * reinterpret_cast< vector32x3_c * >( & a ); }
		inline vector32x3_c const & get_basis_a() const { return * reinterpret_cast< vector32x3_c const * >( & a ); }
		inline vector32x3_c & get_basis_b() { return * reinterpret_cast< vector32x3_c * >( & b ); }
		inline vector32x3_c const & get_basis_b() const { return * reinterpret_cast< vector32x3_c const * >( & b ); }
		inline vector32x3_c & get_basis_c() { return * reinterpret_cast< vector32x3_c * >( & c ); }
		inline vector32x3_c const & get_basis_c() const { return * reinterpret_cast< vector32x3_c const * >( & c ); }
		inline vector32x3_c & get_position() { return * reinterpret_cast< vector32x3_c * >( & d ); }
		inline vector32x3_c const & get_position() const { return * reinterpret_cast< vector32x3_c const * >( & d ); }

	};

	boolean_c operator == ( matrix32x4x4_c const & a, matrix32x4x4_c const & b );
	boolean_c operator != ( matrix32x4x4_c const & a, matrix32x4x4_c const & b );

	matrix32x4x4_c operator * ( matrix32x4x4_c const & a, matrix32x4x4_c const & b );

}
