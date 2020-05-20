#include "cheonsa__types_box64x3.h"
#include "cheonsa__types_box32x3.h"
#include "cheonsa__ops_math.h"

namespace cheonsa
{

	box64x3_c::box64x3_c()
		: minimum()
		, maximum()
	{
	}

	box64x3_c::box64x3_c( vector64x3_c const & minimum, vector64x3_c const & maximum )
		: minimum( minimum )
		, maximum( maximum )
	{
	}

	box64x3_c::box64x3_c( float64_c const minimum_a, float64_c const minimum_b, float64_c const minimum_c, float64_c const maximum_a, float64_c const maximum_b, float64_c const maximum_c )
		: minimum( minimum_a, minimum_b, minimum_c )
		, maximum( maximum_a, maximum_b, maximum_c )
	{
	}

	box64x3_c::box64x3_c( box32x3_c const & other )
		: minimum( other.minimum.a, other.minimum.b, other.minimum.c )
		, maximum( other.maximum.a, other.maximum.b, other.maximum.c )
	{
	}

	vector64x3_c box64x3_c::get_center() const
	{
		return vector64x3_c( ( minimum.a + maximum.a ) * 0.5, ( minimum.b + maximum.b ) * 0.5, ( minimum.c + maximum.c ) * 0.5 );
	}

	vector64x3_c box64x3_c::get_size() const
	{
		return maximum - minimum;
	}

	float64_c box64x3_c::get_longest_dimension() const
	{
		return ops::math_maximum( ops::math_maximum( maximum.a - minimum.a, maximum.b - minimum.b ), maximum.c - minimum.c );
	}

	float64_c box64x3_c::get_longest_half_extent() const
	{
		float64_c result;
		result = ops::math_maximum( ops::math_absolute_value( minimum.a ), ops::math_absolute_value( maximum.a ) );
		result = ops::math_maximum( ops::math_maximum( ops::math_absolute_value( minimum.b ), ops::math_absolute_value( maximum.b ) ), result );
		result = ops::math_maximum( ops::math_maximum( ops::math_absolute_value( minimum.c ), ops::math_absolute_value( maximum.c ) ), result );
		return result;
	}

	vector64x3_c box64x3_c::get_point( sint32_c const index ) const
	{
		return vector64x3_c( index & 1 ? maximum.a : minimum.a, index & 2 ? maximum.b : minimum.b, index & 4 ? maximum.c : minimum.c );
	}

	boolean_c operator == ( box32x3_c const & a, box32x3_c const & b )
	{
		return ( a.minimum == b.minimum ) && ( a.maximum == b.maximum );
	}

	boolean_c operator != ( box32x3_c const & a, box32x3_c const & b )
	{
		return ( a.minimum != b.minimum ) || ( a.maximum != b.maximum );
	}

}
