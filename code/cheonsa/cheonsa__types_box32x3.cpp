#include "cheonsa__types_box32x3.h"
#include "cheonsa__types_box64x3.h"

namespace cheonsa
{

	box32x3_c::box32x3_c()
		: minimum()
		, maximum()
	{
	}

	box32x3_c::box32x3_c( vector32x3_c const & minimum, vector32x3_c const & maximum )
		: minimum( minimum )
		, maximum( maximum )
	{
	}

	box32x3_c::box32x3_c( float32_c const minimum_a, float32_c const minimum_b, float32_c const minimum_c, float32_c const maximum_a, float32_c const maximum_b, float32_c const maximum_c )
		: minimum( minimum_a, minimum_b, minimum_c )
		, maximum( maximum_a, maximum_b, maximum_c )
	{
	}

	box32x3_c::box32x3_c( float32_c const * array )
		: minimum( array[ 0 ], array[ 1 ], array[ 2 ] )
		, maximum( array[ 3 ], array[ 4 ], array[ 5 ] )
	{
	}

	box32x3_c::box32x3_c( box64x3_c const & other )
		: minimum( other.minimum )
		, maximum( other.maximum )
	{
	}

	vector32x3_c box32x3_c::get_center() const
	{
		return vector32x3_c( ( minimum.a + maximum.a ) * 0.5f, ( minimum.b + maximum.b ) * 0.5f, ( minimum.c + maximum.c ) * 0.5f );
	}

	vector32x3_c box32x3_c::get_size() const
	{
		return maximum - minimum;
	}

	vector32x3_c box32x3_c::get_point( sint32_c index ) const
	{
		return vector32x3_c( index & 1 ? maximum.a : minimum.a, index & 2 ? maximum.b : minimum.b, index & 4 ? maximum.c : minimum.c );
	}

	void_c box32x3_c::set_bounds( vector32x3_c const & point, float32_c radius )
	{
		minimum.a = point.a - radius;
		minimum.b = point.b - radius;
		minimum.c = point.c - radius;
		maximum.a = point.a + radius;
		maximum.b = point.b + radius;
		maximum.c = point.c + radius;
	}

	void_c box32x3_c::accumulate_bounds( vector32x3_c const & point )
	{
		if ( point.a < minimum.a )
		{
			minimum.a = point.a;
		}
		else if ( point.a > maximum.a )
		{
			maximum.a = point.a;
		}
		if ( point.b < minimum.b )
		{
			minimum.b = point.b;
		}
		else if ( point.b > maximum.b )
		{
			maximum.b = point.b;
		}
		if ( point.c < minimum.c )
		{
			minimum.c = point.c;
		}
		else if ( point.c > maximum.c )
		{
			maximum.c = point.c;
		}
	}

	void_c box32x3_c::accumulate_bounds( vector32x3_c const & point, float32_c radius )
	{
		if ( minimum.a > point.a - radius )
		{
			minimum.a = point.a - radius;
		}
		else if ( maximum.a < point.a + radius )
		{
			maximum.a = point.a + radius;
		}
		if ( minimum.b > point.b - radius )
		{
			minimum.b = point.b - radius;
		}
		else if ( maximum.b < point.b + radius )
		{
			maximum.b = point.b - radius;
		}
		if ( minimum.c > point.c - radius )
		{
			minimum.c = point.c - radius;
		}
		else if ( maximum.c < point.c + radius )
		{
			maximum.c = point.c + radius;
		}
	}

	boolean_c operator == ( box64x3_c const & a, box64x3_c const & b )
	{
		return ( a.minimum == b.minimum ) && ( a.maximum == b.maximum );
	}

	boolean_c operator != ( box64x3_c const & a, box64x3_c const & b )
	{
		return ( a.minimum != b.minimum ) || ( a.maximum != b.maximum );
	}

}
