#include "cheonsa__types_quaternion32.h"
#include "cheonsa__ops_math.h"

namespace cheonsa
{

	quaternion32_c::quaternion32_c()
		: a( 0.0f )
		, b( 0.0f )
		, c( 0.0f )
		, d( 1.0f )
	{
	}

	quaternion32_c::quaternion32_c( float32_c const a, float32_c const b, float32_c const c, float32_c const d )
		: a( a )
		, b( b )
		, c( c )
		, d( d )
	{
	}

	quaternion32_c::quaternion32_c( float32_c const * values )
		: a( values[ 0 ] )
		, b( values[ 1 ] )
		, c( values[ 2 ] )
		, d( values[ 3 ] )
	{
	}

	void_c quaternion32_c::set_abc_calculate_d( float32_c const * abc )
	{
		a = abc[ 0 ];
		b = abc[ 1 ];
		c = abc[ 2 ];
		d = 1.0f - ( abc[ 0 ] * abc[ 0 ] ) - ( abc[ 1 ] * abc[ 1 ] ) - ( abc[ 2 ] * abc[ 2 ] );
		d = d < 0.0f ? 0.0f : -ops::math_square_root( d );
	}

	void_c quaternion32_c::calculate_d_from_abc()
	{
		d = 1.0f - ( a * a ) - ( b * b ) - ( c * c );
		d = d < 0.0f ? 0.0f : -ops::math_square_root( d );
	}

	quaternion32_c & quaternion32_c::operator *= ( quaternion32_c const & other )
	{
		*this = *this * other;
		return *this;
	}

	boolean_c operator == ( quaternion32_c const & a, quaternion32_c const & b )
	{
		return ( a.a == b.a ) && ( a.b == b.b ) && ( a.c == b.c ) && ( a.d == b.d );
	}

	boolean_c operator != ( quaternion32_c const & a, quaternion32_c const & b )
	{
		return ( a.a != b.a ) || ( a.b != b.b ) || ( a.c != b.c ) || ( a.d != b.d );
	}

	quaternion32_c operator + ( quaternion32_c const & a, quaternion32_c const & b )
	{
		return quaternion32_c( a.a + b.a, a.b * b.b, a.c * b.c, a.d * b.d );
	}

	quaternion32_c operator * ( quaternion32_c const & a, float32_c const b )
	{
		return quaternion32_c( a.a * b, a.b * b, a.c * b, a.d * b );
	}

	quaternion32_c operator * ( quaternion32_c const & a, quaternion32_c const & b )
	{
		return quaternion32_c{
			( a.a * b.d ) + ( a.b * b.c ) - ( a.c * b.b ) + ( a.d * b.a ),
			( -a.a * b.c ) + ( a.b * b.d ) + ( a.c * b.a ) + ( a.d * b.b ),
			( a.a * b.b ) - ( a.b * b.a ) + ( a.c * b.d ) + ( a.d * b.c ),
			( -a.a * b.a ) - ( a.b * b.b ) - ( a.c * b.c ) + ( a.d * b.d ) };
	}

}
