#include "cheonsa__types_vector64x3.h"
#include "cheonsa__types_vector32x3.h"
#include <cassert>

namespace cheonsa
{

	vector64x3_c::vector64x3_c()
		: a( 0.0 )
		, b( 0.0 )
		, c( 0.0 )
	{
	}

	vector64x3_c::vector64x3_c( float64_c const a, float64_c const b, float64_c const c )
		: a( a )
		, b( b )
		, c( c )
	{
	}

	vector64x3_c::vector64x3_c( float64_c const * values )
		: a( values[ 0 ] )
		, b( values[ 1 ] )
		, c( values[ 2 ] )
	{
	}

	vector64x3_c::vector64x3_c( float32_c const * values )
		: a( values[ 0 ] )
		, b( values[ 1 ] )
		, c( values[ 2 ] )
	{
	}

	vector64x3_c::vector64x3_c( vector32x3_c const & other )
		: a( other.a )
		, b( other.b )
		, c( other.c )
	{
	}

	float64_c & vector64x3_c::get_element_at_index( sint32_c const index )
	{
		assert( index >= 0 && index < 3 );
		return reinterpret_cast< float64_c * >( this )[ index ];
	}

	float64_c const & vector64x3_c::get_element_at_index( sint32_c const index ) const
	{
		assert( index >= 0 && index < 3 );
		return reinterpret_cast< float64_c const * >( this )[ index ];
	}

	vector64x3_c & vector64x3_c::operator *= ( float64_c const b )
	{
		this->a *= b;
		this->b *= b;
		this->c *= b;
		return *this;
	}

	vector64x3_c & vector64x3_c::operator /= ( float64_c const b )
	{
		this->a /= b;
		this->b /= b;
		this->c /= b;
		return *this;
	}

	vector64x3_c & vector64x3_c::operator += ( vector64x3_c const & b )
	{
		this->a += b.a;
		this->b += b.b;
		this->c += b.c;
		return *this;
	}

	vector64x3_c & vector64x3_c::operator -= ( vector64x3_c const & b )
	{
		this->a -= b.a;
		this->b -= b.b;
		this->c -= b.c;
		return *this;
	}

	vector64x3_c & vector64x3_c::operator *= ( vector64x3_c const & b )
	{
		this->a *= b.a;
		this->b *= b.b;
		this->c *= b.c;
		return *this;
	}

	vector64x3_c & vector64x3_c::operator /= ( vector64x3_c const & b )
	{
		this->a /= b.a;
		this->b /= b.b;
		this->c /= b.c;
		return *this;
	}

	boolean_c operator == ( vector64x3_c const & a, vector64x3_c const & b )
	{
		return ( a.a == b.a ) && ( a.b == b.b ) && ( a.c == b.c );
	}

	boolean_c operator != ( vector64x3_c const & a, vector64x3_c const & b )
	{
		return ( a.a != b.a ) || ( a.b != b.b ) || ( a.c != b.c );
	}

	vector64x3_c operator - ( vector64x3_c const & a )
	{
		return vector64x3_c( -a.a, -a.b, -a.c );
	}

	vector64x3_c operator * ( vector64x3_c a, float64_c b )
	{
		return a *= b;
	}

	vector64x3_c operator / ( vector64x3_c a, float64_c b )
	{
		return a /= b;
	}

	vector64x3_c operator + ( vector64x3_c a, vector64x3_c const & b )
	{
		return a += b;
	}

	vector64x3_c operator - ( vector64x3_c a, vector64x3_c const & b )
	{
		return a -= b;
	}

	vector64x3_c operator * ( vector64x3_c a, vector64x3_c const & b )
	{
		return a *= b;
	}

	vector64x3_c operator / ( vector64x3_c a, vector64x3_c const & b )
	{
		return a /= b;
	}

	vector64x3_c operator + ( vector64x3_c a, vector32x3_c const & b )
	{
		a.a += b.a;
		a.b += b.b;
		a.c += b.c;
		return a;
	}

	vector64x3_c operator - ( vector64x3_c a, vector32x3_c const & b )
	{
		a.a -= b.a;
		a.b -= b.b;
		a.c -= b.c;
		return a;
	}

	vector64x3_c operator * ( vector64x3_c a, float32_c b )
	{
		a.a *= b;
		a.b *= b;
		a.c *= b;
		return a;
	}

	vector64x3_c operator * ( vector64x3_c a, vector32x3_c const & b )
	{
		a.a *= b.a;
		a.b *= b.b;
		a.c *= b.c;
		return a;
	}

}
