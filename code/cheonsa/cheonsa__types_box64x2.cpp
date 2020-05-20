#include "cheonsa__types_box64x2.h"

namespace cheonsa
{

	box64x2_c::box64x2_c()
		: minimum( 0, 0 )
		, maximum( 0, 0 )
	{
	}

	box64x2_c::box64x2_c( vector64x2_c const & minimum, vector64x2_c const & maximum )
		: minimum( minimum )
		, maximum( maximum )
	{
	}

	box64x2_c::box64x2_c( float64_c minimum_a, float64_c minimum_b, float64_c maximum_a, float64_c maximum_b )
		: minimum( minimum_a, minimum_b )
		, maximum( maximum_a, maximum_b )
	{
	}

	box64x2_c::box64x2_c( float64_c const * array )
		: minimum( array[ 0 ], array[ 1 ] )
		, maximum( array[ 2 ], array[ 3 ] )
	{
	}

	vector64x2_c box64x2_c::get_center() const
	{
		return vector64x2_c( ( minimum.a + maximum.a ) * 0.5, ( minimum.b + maximum.b ) * 0.5 );
	}
	
	boolean_c operator == ( box64x2_c const & a, box64x2_c const & b )
	{
		return ( a.minimum == b.minimum ) && ( a.maximum == b.maximum );
	}

	boolean_c operator != ( box64x2_c const & a, box64x2_c const & b )
	{
		return ( a.minimum != b.minimum ) || ( a.maximum != b.maximum );
	}

}
