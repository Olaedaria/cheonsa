#include "cheonsa__types_vector32x3.h"
#include "cheonsa__types_vector64x3.h"
#include <cassert>

namespace cheonsa
{

	vector32x3_c::vector32x3_c()
		: a( 0.0f )
		, b( 0.0f )
		, c( 0.0f )
	{
	}

	vector32x3_c::vector32x3_c( float32_c const a, float32_c const b, float32_c const c )
		: a( a )
		, b( b )
		, c( c )
	{
	}

	vector32x3_c::vector32x3_c( float32_c const * values )
		: a( values[ 0 ] )
		, b( values[ 1 ] )
		, c( values[ 2 ] )
	{
	}

	vector32x3_c::vector32x3_c( vector64x3_c const & other )
		: a( static_cast< float32_c >( other.a ) )
		, b( static_cast< float32_c >( other.b ) )
		, c( static_cast< float32_c >( other.c ) )
	{
	}

	float32_c & vector32x3_c::get_element_at_index( sint32_c index )
	{
		assert( index >= 0 && index < 3 );
		return reinterpret_cast< float32_c * >( this )[ index ];
	}

	float32_c const & vector32x3_c::get_element_at_index( sint32_c index ) const
	{
		assert( index >= 0 && index < 3 );
		return reinterpret_cast< float32_c const * >( this )[ index ];
	}

	float32_c vector32x3_c::get_largest_element() const
	{
		float32_c result = a > b ? a : b;
		result = c > result ? c : result;
		return result;
	}

	vector32x3_c & vector32x3_c::operator *= ( float32_c b )
	{
		this->a *= b;
		this->b *= b;
		this->c *= b;
		return * this;
	}

	vector32x3_c & vector32x3_c::operator /= ( float32_c b )
	{
		this->a /= b;
		this->b /= b;
		this->c /= b;
		return * this;
	}

	vector32x3_c & vector32x3_c::operator += ( vector32x3_c const & b )
	{
		this->a += b.a;
		this->b += b.b;
		this->c += b.c;
		return * this;
	}

	vector32x3_c & vector32x3_c::operator -= ( vector32x3_c const & b )
	{
		this->a -= b.a;
		this->b -= b.b;
		this->c -= b.c;
		return * this;
	}

	vector32x3_c & vector32x3_c::operator *= ( vector32x3_c const & b )
	{
		this->a *= b.a;
		this->b *= b.b;
		this->c *= b.c;
		return * this;
	}

	vector32x3_c & vector32x3_c::operator /= ( vector32x3_c const & b )
	{
		this->a /= b.a;
		this->b /= b.b;
		this->c /= b.c;
		return * this;
	}

	boolean_c operator == ( vector32x3_c const & a, vector32x3_c const & b )
	{
		return ( a.a == b.a ) && ( a.b == b.b ) && ( a.c == b.c );
	}

	boolean_c operator != ( vector32x3_c const & a, vector32x3_c const & b )
	{
		return ( a.a != b.a ) || ( a.b != b.b ) || ( a.c != b.c );
	}

	vector32x3_c operator - ( vector32x3_c const & a )
	{
		return vector32x3_c( -a.a, -a.b, -a.c );
	}

	vector32x3_c operator * ( vector32x3_c a, float32_c b )
	{
		return a *= b;
	}

	vector32x3_c operator / ( vector32x3_c a, float32_c b )
	{
		return a /= b;
	}

	vector32x3_c operator + ( vector32x3_c a, vector32x3_c const & b )
	{
		return a += b;
	}

	vector32x3_c operator - ( vector32x3_c a, vector32x3_c const & b )
	{
		return a -= b;
	}

	vector32x3_c operator * ( vector32x3_c a, vector32x3_c const & b )
	{
		return a *= b;
	}

	vector32x3_c operator / ( vector32x3_c a, vector32x3_c const & b )
	{
		return a /= b;
	}

	vector64x3_c operator * ( vector32x3_c a, float64_c b )
	{
		return vector64x3_c( a.a * b, a.b * b, a.c * b );
	}

	vector64x3_c operator * ( vector32x3_c a, vector64x3_c const & b )
	{
		return vector64x3_c( a.a * b.a, a.b * b.b, a.c * b.c );
	}

}
