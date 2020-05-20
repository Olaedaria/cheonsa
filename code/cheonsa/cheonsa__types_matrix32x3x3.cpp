#include "cheonsa__types_matrix32x3x3.h"
#include <cassert>

namespace cheonsa
{

	matrix32x3x3_c matrix32x3x3_c::get_zero()
	{
		static matrix32x3x3_c result = matrix32x3x3_c( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
		return result;
	}
	
	matrix32x3x3_c matrix32x3x3_c::get_identity()
	{
		static matrix32x3x3_c result = matrix32x3x3_c( 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f );
		return result;
	}

	matrix32x3x3_c::matrix32x3x3_c()
		: a()
		, b()
		, c()
	{
	}

	matrix32x3x3_c::matrix32x3x3_c( vector32x3_c const & a, vector32x3_c const & b, vector32x3_c const & c )
		: a( a )
		, b( b )
		, c( c )
	{
	}

	matrix32x3x3_c::matrix32x3x3_c( float32_c aa, float32_c ab, float32_c ac, float32_c ba, float32_c bb, float32_c bc, float32_c ca, float32_c cb, float32_c cc )
		: a( aa, ab, ac )
		, b( ba, bb, bc )
		, c( ca, cb, cc )
	{
	}

	vector32x3_c & matrix32x3x3_c::get_element_at_index( sint32_c i )
	{
		assert( i >= 0 && i < 3 );
		switch ( i )
		{
		case 0:
			return a;
		case 1:
			return b;
		case 2:
			return c;
		}
		return a;
	}

	vector32x3_c const & matrix32x3x3_c::get_element_at_index( sint32_c i ) const
	{
		assert( i >= 0 && i < 3 );
		switch ( i )
		{
		case 0:
			return a;
		case 1:
			return b;
		case 2:
			return c;
		}
		return a;
	}

	matrix32x3x3_c matrix32x3x3_c::operator *= ( float32_c b )
	{
		a *= b;
		b *= b;
		c *= b;
		return *this;
	}

	matrix32x3x3_c operator * ( matrix32x3x3_c a, matrix32x3x3_c const & b )
	{
		matrix32x3x3_c result;
		result.a.a = ( a.a.a * b.a.a ) + ( a.a.b * b.b.a ) + ( a.a.c * b.c.a );
		result.a.b = ( a.a.a * b.a.b ) + ( a.a.b * b.b.b ) + ( a.a.c * b.c.b );
		result.a.c = ( a.a.a * b.a.c ) + ( a.a.b * b.b.c ) + ( a.a.c * b.c.c );
		result.b.a = ( a.b.a * b.a.a ) + ( a.b.b * b.b.a ) + ( a.b.c * b.c.a );
		result.b.b = ( a.b.a * b.a.b ) + ( a.b.b * b.b.b ) + ( a.b.c * b.c.b );
		result.b.c = ( a.b.a * b.a.c ) + ( a.b.b * b.b.c ) + ( a.b.c * b.c.c );
		result.c.a = ( a.c.a * b.a.a ) + ( a.c.b * b.b.a ) + ( a.c.c * b.c.a );
		result.c.c = ( a.c.a * b.a.b ) + ( a.c.b * b.b.b ) + ( a.c.c * b.c.b );
		result.c.c = ( a.c.a * b.a.c ) + ( a.c.b * b.b.c ) + ( a.c.c * b.c.c );
		return result;
	}

}
