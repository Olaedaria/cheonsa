#include "cheonsa__types_vector32x4.h"
#include "cheonsa__types_vector64x4.h"
#include <cassert>

namespace cheonsa
{

	vector32x4_c::vector32x4_c()
		: a( 0.0f )
		, b( 0.0f )
		, c( 0.0f )
		, d( 0.0f )
	{
	}

	vector32x4_c::vector32x4_c( float32_c const a, float32_c const b, float32_c const c, float32_c const d )
		: a( a )
		, b( b )
		, c( c )
		, d( d )
	{
	}

	vector32x4_c::vector32x4_c( float32_c const * values )
		: a( values[ 0 ] )
		, b( values[ 1 ] )
		, c( values[ 2 ] )
		, d( values[ 3 ] )
	{
	}

	vector32x4_c::vector32x4_c( vector64x4_c const & other )
		: a( static_cast< float32_c >( other.a ) )
		, b( static_cast< float32_c >( other.b ) )
		, c( static_cast< float32_c >( other.c ) )
		, d( static_cast< float32_c >( other.d ) )
	{
	}

	float32_c & vector32x4_c::get_element_at_index( sint32_c index )
	{
		assert( index >= 0 && index < 4 );
		return reinterpret_cast< float32_c * >( this )[ index ];
	}

	float32_c const & vector32x4_c::get_element_at_index( sint32_c index ) const
	{
		assert( index >= 0 && index < 4 );
		return reinterpret_cast< float32_c const * >( this )[ index ];
	}

	vector32x4_c & vector32x4_c::operator *= ( float32_c const b )
	{
		this->a *= b;
		this->b *= b;
		this->c *= b;
		this->d *= b;
		return *this;
	}

	vector32x4_c & vector32x4_c::operator /= ( float32_c const b )
	{
		this->a /= b;
		this->b /= b;
		this->c /= b;
		this->d /= b;
		return *this;
	}

	vector32x4_c & vector32x4_c::operator += ( vector32x4_c const & b )
	{
		this->a += b.a;
		this->b += b.b;
		this->c += b.c;
		this->d += b.d;
		return *this;
	}

	vector32x4_c & vector32x4_c::operator -= ( vector32x4_c const & b )
	{
		this->a -= b.a;
		this->b -= b.b;
		this->c -= b.c;
		this->d -= b.d;
		return *this;
	}

	vector32x4_c & vector32x4_c::operator *= ( vector32x4_c const & b )
	{
		this->a *= b.a;
		this->b *= b.b;
		this->c *= b.c;
		this->d *= b.d;
		return *this;
	}

	vector32x4_c & vector32x4_c::operator /= ( vector32x4_c const & b )
	{
		this->a /= b.a;
		this->b /= b.b;
		this->c /= b.c;
		this->d /= b.d;
		return *this;
	}

	boolean_c operator == ( vector32x4_c const & a, vector32x4_c const & b )
	{
		return ( a.a == b.a ) && ( a.b == b.b ) && ( a.c == b.c ) && ( a.d == b.d );
	}

	boolean_c operator != ( vector32x4_c const & a, vector32x4_c const & b )
	{
		return ( a.a != b.a ) || ( a.b != b.b ) || ( a.c != b.c ) || ( a.d != b.d );
	}

	vector32x4_c operator - ( vector32x4_c const & a )
	{
		return vector32x4_c( -a.a, -a.b, -a.c, -a.d );
	}

	vector32x4_c operator * ( vector32x4_c a, float32_c b )
	{
		return a *= b;
	}

	vector32x4_c operator / ( vector32x4_c a, float32_c b )
	{
		return a /= b;
	}

	vector32x4_c operator + ( vector32x4_c a, vector32x4_c const & b )
	{
		return a += b;
	}

	vector32x4_c operator - ( vector32x4_c a, vector32x4_c const & b )
	{
		return a -= b;
	}

	vector32x4_c operator * ( vector32x4_c a, vector32x4_c const & b )
	{
		return a *= b;
	}

	vector32x4_c operator / ( vector32x4_c a, vector32x4_c const & b )
	{
		return a /= b;
	}

}
