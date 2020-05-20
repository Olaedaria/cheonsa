#pragma once

#include "cheonsa__types__fundamental.h"

namespace cheonsa
{

	class vector64x3_c;

	// single precision 3D vector.
	class vector32x3_c
	{
	public:
		float32_c a;
		float32_c b;
		float32_c c;

	public:
		explicit vector32x3_c();
		explicit vector32x3_c( float32_c const a, float32_c const b, float32_c const c );
		explicit vector32x3_c( float32_c const * values );
		explicit vector32x3_c( vector64x3_c const & other );

		inline float32_c * as_array() { return reinterpret_cast< float32_c * >( this ); }
		inline float32_c const * as_array() const { return reinterpret_cast< float32_c const * >( this ); }

		float32_c & get_element_at_index( sint32_c const index );
		float32_c const & get_element_at_index( sint32_c const index ) const;

		float32_c get_largest_element() const; // gets the value of the component with the largest magnitude.

		vector32x3_c & operator *= ( float32_c b );
		vector32x3_c & operator /= ( float32_c b );
		vector32x3_c & operator += ( vector32x3_c const & b );
		vector32x3_c & operator -= ( vector32x3_c const & b );
		vector32x3_c & operator *= ( vector32x3_c const & b );
		vector32x3_c & operator /= ( vector32x3_c const & b );

	};

	boolean_c operator == ( vector32x3_c const & a, vector32x3_c const & b );
	boolean_c operator != ( vector32x3_c const & a, vector32x3_c const & b );

	vector32x3_c operator - ( vector32x3_c const & a );
	vector32x3_c operator * ( vector32x3_c a, float32_c b );
	vector32x3_c operator / ( vector32x3_c a, float32_c b );
	vector32x3_c operator + ( vector32x3_c a, vector32x3_c const & b );
	vector32x3_c operator - ( vector32x3_c a, vector32x3_c const & b );
	vector32x3_c operator * ( vector32x3_c a, vector32x3_c const & b );
	vector32x3_c operator / ( vector32x3_c a, vector32x3_c const & b );
	
	vector64x3_c operator * ( vector32x3_c a, float64_c b );
	vector64x3_c operator * ( vector32x3_c a, vector64x3_c const & b );

}
