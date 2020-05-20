#include "cheonsa__types_box32x2.h"

namespace cheonsa
{

	box32x2_c::box32x2_c()
		: minimum( 0, 0 )
		, maximum( 0, 0 )
	{
	}

	box32x2_c::box32x2_c( vector32x2_c const & minimum, vector32x2_c const & maximum )
		: minimum( minimum )
		, maximum( maximum )
	{
	}

	box32x2_c::box32x2_c( float32_c const minimum_a, float32_c const minimum_b, float32_c const maximum_a, float32_c const maximum_b )
		: minimum( minimum_a, minimum_b )
		, maximum( maximum_a, maximum_b )
	{
	}

	box32x2_c::box32x2_c( float32_c const * array )
		: minimum( array[ 0 ], array[ 1 ] )
		, maximum( array[ 2 ], array[ 3 ] )
	{
	}

	vector32x2_c box32x2_c::get_center() const
	{
		return vector32x2_c( ( minimum.a + maximum.a ) * 0.5f, ( minimum.b + maximum.b ) * 0.5f );
	}

	void_c box32x2_c::accumulate_bounds( box32x2_c const & other )
	{
		if ( other.minimum.a < minimum.a )
		{
			minimum.a = other.minimum.a;
		}
		if ( other.minimum.b < minimum.b )
		{
			minimum.b = other.minimum.b;
		}
		if ( other.maximum.a > maximum.a )
		{
			maximum.a = other.maximum.a;
		}
		if ( other.maximum.b > maximum.b )
		{
			maximum.b = other.maximum.b;
		}
	}

	boolean_c operator == ( box32x2_c const & a, box32x2_c const & b )
	{
		return ( a.minimum == b.minimum ) && ( a.maximum == b.maximum );
	}

	boolean_c operator != ( box32x2_c const & a, box32x2_c const & b )
	{
		return ( a.minimum != b.minimum ) || ( a.maximum != b.maximum );
	}

}
