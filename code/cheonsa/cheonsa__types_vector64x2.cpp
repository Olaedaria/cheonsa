#include "cheonsa__types_vector64x2.h"
#include "cheonsa__types_vector32x2.h"
#include <cassert>

namespace cheonsa
{

	vector64x2_c::vector64x2_c()
		: a( 0.0 )
		, b( 0.0 )
	{
	}

	vector64x2_c::vector64x2_c( float64_c const a, float64_c const b )
		: a( a )
		, b( b )
	{
	}

	vector64x2_c::vector64x2_c( float64_c const * values )
		: a( values[ 0 ] )
		, b( values[ 1 ] )
	{
	}

	vector64x2_c::vector64x2_c( vector32x2_c const & other )
		: a( other.a )
		, b( other.b )
	{
	}

	float64_c & vector64x2_c::get_element_at_index( sint32_c const index )
	{
		assert( index >= 0 && index < 2 );
		return reinterpret_cast< float64_c * >( this )[ index ];
	}

	float64_c const & vector64x2_c::get_element_at_index( sint32_c const index ) const
	{
		assert( index >= 0 && index < 2 );
		return reinterpret_cast< float64_c const * >( this )[ index ];
	}

	vector64x2_c & vector64x2_c::operator *= ( float64_c const b )
	{
		this->a *= b;
		this->b *= b;
		return *this;
	}

	vector64x2_c & vector64x2_c::operator /= ( float64_c const b )
	{
		this->a /= b;
		this->b /= b;
		return *this;
	}

	vector64x2_c & vector64x2_c::operator += ( vector64x2_c const & b )
	{
		this->a += b.a;
		this->b += b.b;
		return *this;
	}

	vector64x2_c & vector64x2_c::operator -= ( vector64x2_c const & b )
	{
		this->a -= b.a;
		this->b -= b.b;
		return *this;
	}

	vector64x2_c & vector64x2_c::operator *= ( vector64x2_c const & b )
	{
		this->a *= b.a;
		this->b *= b.b;
		return *this;
	}

	vector64x2_c & vector64x2_c::operator /= ( vector64x2_c const & b )
	{
		this->a /= b.a;
		this->b /= b.b;
		return *this;
	}

	boolean_c operator == ( vector64x2_c const & a, vector64x2_c const & b )
	{
		return ( a.a == b.a ) && ( a.b == b.b );
	}

	boolean_c operator != ( vector64x2_c const & a, vector64x2_c const & b )
	{
		return ( a.a != b.a ) || ( a.b != b.b );
	}

	vector64x2_c operator - ( vector64x2_c const & a )
	{
		return vector64x2_c( -a.a, -a.b );
	}

	vector64x2_c operator * ( vector64x2_c a, float64_c b )
	{
		return a *= b;
	}

	vector64x2_c operator / ( vector64x2_c a, float64_c b )
	{
		return a /= b;
	}

	vector64x2_c operator + ( vector64x2_c a, vector64x2_c const & b )
	{
		return a += b;
	}

	vector64x2_c operator - ( vector64x2_c a, vector64x2_c const & b )
	{
		return a -= b;
	}

	vector64x2_c operator * ( vector64x2_c a, vector64x2_c const & b )
	{
		return a *= b;
	}

	vector64x2_c operator / ( vector64x2_c a, vector64x2_c const & b )
	{
		return a /= b;
	}

}
