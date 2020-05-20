#include "cheonsa__types_plane32.h"

namespace cheonsa
{

	plane32_c::plane32_c()
		: a( 0 )
		, b( 0 )
		, c( 0 )
		, d( 0 )
	{
	}

	plane32_c::plane32_c( float32_c const a, float32_c const b, float32_c const c, float32_c const d )
		: a( a )
		, b( b )
		, c( c )
		, d( d )
	{
	}

	plane32_c::plane32_c( float32_c const * values )
		: a( values[ 0 ] )
		, b( values[ 1 ] )
		, c( values[ 2 ] )
		, d( values[ 3 ] )
	{
	}

	vector32x3_c plane32_c::get_point() const
	{
		return get_normal() * -d;
	}

	void_c plane32_c::set_point( vector32x3_c const & value )
	{
		d = -( a * value.a + b * value.b + c * value.c );
	}

	boolean_c operator == ( plane32_c const & a, plane32_c const & b )
	{
		return ( a.a == b.a ) && ( a.b == b.b ) && ( a.c == b.c ) && ( a.d == b.d );
	}

	boolean_c operator != ( plane32_c const & a, plane32_c const & b )
	{
		return ( a.a != b.a ) || ( a.b != b.b ) || ( a.c != b.c ) || ( a.d != b.d );
	}

}
