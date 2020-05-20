#include "cheonsa__types_plane64.h"

namespace cheonsa
{

	plane64_c::plane64_c()
		: a( 0 )
		, b( 0 )
		, c( 0 )
		, d( 0 )
	{
	}

	plane64_c::plane64_c( float64_c const a, float64_c const b, float64_c const c, float64_c const d )
		: a( a )
		, b( b )
		, c( c )
		, d( d )
	{
	}

	plane64_c::plane64_c( float64_c const * values )
		: a( values[ 0 ] )
		, b( values[ 1 ] )
		, c( values[ 2 ] )
		, d( values[ 3 ] )
	{
	}

	vector64x3_c plane64_c::get_point() const
	{
		return vector64x3_c( a * -d, b * -d, c * -d );
	}

	void_c plane64_c::set_point( vector64x3_c const & value )
	{
		d = -( a * value.a + b * value.b + c * value.c );
	}

	boolean_c operator == ( plane64_c const & a, plane64_c const & b )
	{
		return ( a.a == b.a ) && ( a.b == b.b ) && ( a.c == b.c ) && ( a.d == b.d );
	}

	boolean_c operator != ( plane64_c const & a, plane64_c const & b )
	{
		return ( a.a != b.a ) || ( a.b != b.b ) || ( a.c != b.c ) || ( a.d != b.d );
	}

}
