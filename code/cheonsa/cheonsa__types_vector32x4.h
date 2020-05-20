#pragma once

#include "cheonsa__types__fundamental.h"

namespace cheonsa
{

	class vector64x4_c;

	// single precision 4D vector.
	class vector32x4_c
	{
	public:
		float32_c a;
		float32_c b;
		float32_c c;
		float32_c d;

	public:
		explicit vector32x4_c();
		explicit vector32x4_c( float32_c const a, float32_c const b, float32_c const c, float32_c const d );
		explicit vector32x4_c( float32_c const * values );
		explicit vector32x4_c( vector64x4_c const & other );

		inline float32_c * as_array() { return reinterpret_cast< float32_c * >( this ); }
		inline float32_c const * as_array() const { return reinterpret_cast< float32_c const * >( this ); }

		float32_c & get_element_at_index( sint32_c const index );
		float32_c const & get_element_at_index( sint32_c const index ) const;

		vector32x4_c & operator *= ( float32_c const b );
		vector32x4_c & operator /= ( float32_c const b );
		vector32x4_c & operator += ( vector32x4_c const & b );
		vector32x4_c & operator -= ( vector32x4_c const & b );
		vector32x4_c & operator *= ( vector32x4_c const & b );
		vector32x4_c & operator /= ( vector32x4_c const & b );

	};

	boolean_c operator == ( vector32x4_c const & a, vector32x4_c const & b );
	boolean_c operator != ( vector32x4_c const & a, vector32x4_c const & b );

	vector32x4_c operator - ( vector32x4_c const & a );
	vector32x4_c operator * ( vector32x4_c a, float32_c b );
	vector32x4_c operator / ( vector32x4_c a, float32_c b );
	vector32x4_c operator + ( vector32x4_c a, vector32x4_c const & b );
	vector32x4_c operator - ( vector32x4_c a, vector32x4_c const & b );
	vector32x4_c operator * ( vector32x4_c a, vector32x4_c const & b );
	vector32x4_c operator / ( vector32x4_c a, vector32x4_c const & b );

}
