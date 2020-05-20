#pragma once

#include "cheonsa__types__fundamental.h"

namespace cheonsa
{

	class vector32x3_c;

	// double precision 3D vector.
	class vector64x3_c
	{
	public:
		float64_c a;
		float64_c b;
		float64_c c;

	public:
		explicit vector64x3_c();
		explicit vector64x3_c( float64_c const a, float64_c const b, float64_c const c );
		explicit vector64x3_c( float64_c const * values );
		explicit vector64x3_c( float32_c const * values );
		explicit vector64x3_c( vector32x3_c const & other );

		inline float64_c * as_array() { return reinterpret_cast< float64_c * >( this ); }
		inline float64_c const * as_array() const { return reinterpret_cast< float64_c const * >( this ); }

		float64_c & get_element_at_index( sint32_c const index );
		float64_c const & get_element_at_index( sint32_c const index ) const;

		vector64x3_c & operator *= ( float64_c const b );
		vector64x3_c & operator /= ( float64_c const b );
		vector64x3_c & operator += ( vector64x3_c const & b );
		vector64x3_c & operator -= ( vector64x3_c const & b );
		vector64x3_c & operator *= ( vector64x3_c const & b );
		vector64x3_c & operator /= ( vector64x3_c const & b );

	};

	boolean_c operator == ( vector64x3_c const & a, vector64x3_c const & b );
	boolean_c operator != ( vector64x3_c const & a, vector64x3_c const & b );

	vector64x3_c operator - ( vector64x3_c const & a );
	vector64x3_c operator * ( vector64x3_c a, float64_c b );
	vector64x3_c operator / ( vector64x3_c a, float64_c b );
	vector64x3_c operator + ( vector64x3_c a, vector64x3_c const & b );
	vector64x3_c operator - ( vector64x3_c a, vector64x3_c const & b );
	vector64x3_c operator * ( vector64x3_c a, vector64x3_c const & b );
	vector64x3_c operator / ( vector64x3_c a, vector64x3_c const & b );

	vector64x3_c operator + ( vector64x3_c a, vector32x3_c const & b );
	vector64x3_c operator - ( vector64x3_c a, vector32x3_c const & b );
	vector64x3_c operator * ( vector64x3_c a, float32_c b );
	vector64x3_c operator * ( vector64x3_c a, vector32x3_c const & b );

}
