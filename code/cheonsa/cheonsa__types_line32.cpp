#include "cheonsa__types_line32.h"

namespace cheonsa
{

	line32_c::line32_c()
		: a( 0.0f )
		, b( 0.0f )
		, c( 0.0f )
	{
	}

	line32_c::line32_c( float32_c a, float32_c b, float32_c c )
		: a( a )
		, b( b )
		, c( c )
	{
	}

	line32_c::line32_c( float32_c const * value )
		: a( value[ 0 ] )
		, b( value[ 1 ] )
		, c( value[ 2 ] )
	{
	}

	vector32x2_c line32_c::get_point() const
	{
		return vector32x2_c( a * -c, b * -c );
	}

	void_c line32_c::set_point( vector32x2_c const & value )
	{
		c = -( a * value.a + b * value.b );
	}

}
