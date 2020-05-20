#include "cheonsa__types_vector64x4.h"
#include "cheonsa__types_vector32x4.h"
#include <cassert>

namespace cheonsa
{

	vector64x4_c::vector64x4_c()
		: a( 0.0 )
		, b( 0.0 )
		, c( 0.0 )
		, d( 0.0 )
	{
	}

	vector64x4_c::vector64x4_c( float64_c const a, float64_c const b, float64_c const c, float64_c const d )
		: a( a )
		, b( b )
		, c( c )
		, d( d )
	{
	}

	vector64x4_c::vector64x4_c( float64_c const * values )
		: a( values[ 0 ])
		, b( values[ 1 ] )
		, c( values[ 2 ] )
		, d( values[ 3 ] )
	{
	}

	vector64x4_c::vector64x4_c( vector32x4_c const & other )
		: a( other.a )
		, b( other.b )
		, c( other.c )
		, d( other.d )
	{
	}

	float64_c & vector64x4_c::get_element_at_index( sint32_c index )
	{
		assert( index >= 0 && index < 4 );
		return reinterpret_cast< float64_c * >( this )[ index ];
	}

	float64_c const & vector64x4_c::get_element_at_index( sint32_c index ) const
	{
		assert( index >= 0 && index < 4 );
		return reinterpret_cast< float64_c const * >( this )[ index ];
	}

	vector64x4_c & vector64x4_c::operator *= ( float64_c const b )
	{
		this->a *= b;
		this->b *= b;
		this->c *= b;
		this->d *= b;
		return *this;
	}

	vector64x4_c & vector64x4_c::operator /= ( float64_c const b )
	{
		this->a /= b;
		this->b /= b;
		this->c /= b;
		this->d /= b;
		return *this;
	}

	vector64x4_c & vector64x4_c::operator += ( vector64x4_c const & b )
	{
		this->a += b.a;
		this->b += b.b;
		this->c += b.c;
		this->d += b.d;
		return *this;
	}

	vector64x4_c & vector64x4_c::operator -= ( vector64x4_c const & b )
	{
		this->a -= b.a;
		this->b -= b.b;
		this->c -= b.c;
		this->d -= b.d;
		return *this;
	}

	vector64x4_c & vector64x4_c::operator *= ( vector64x4_c const & b )
	{
		this->a *= b.a;
		this->b *= b.b;
		this->c *= b.c;
		this->d *= b.d;
		return *this;
	}

	vector64x4_c & vector64x4_c::operator /= ( vector64x4_c const & b )
	{
		this->a /= b.a;
		this->b /= b.b;
		this->c /= b.c;
		this->d /= b.d;
		return *this;
	}

	boolean_c operator == ( vector64x4_c const & a, vector64x4_c const & b )
	{
		return ( a.a == b.a ) && ( a.b == b.b ) && ( a.c == b.c ) && ( a.d == b.d );
	}

	boolean_c operator != ( vector64x4_c const & a, vector64x4_c const & b )
	{
		return ( a.a != b.a ) || ( a.b != b.b ) || ( a.c != b.c ) || ( a.d != b.d );
	}

	vector64x4_c operator - ( vector64x4_c const & a )
	{
		return vector64x4_c( -a.a, -a.b, -a.c, -a.d );
	}

	vector64x4_c operator * ( vector64x4_c a, float64_c b )
	{
		return a *= b;
	}

	vector64x4_c operator / ( vector64x4_c a, float64_c b )
	{
		return a /= b;
	}

	vector64x4_c operator + ( vector64x4_c a, vector64x4_c const & b )
	{
		return a += b;
	}

	vector64x4_c operator - ( vector64x4_c a, vector64x4_c const & b )
	{
		return a -= b;
	}

	vector64x4_c operator * ( vector64x4_c a, vector64x4_c const & b )
	{
		return a *= b;
	}

	vector64x4_c operator / ( vector64x4_c a, vector64x4_c const & b )
	{
		return a /= b;
	}

}
