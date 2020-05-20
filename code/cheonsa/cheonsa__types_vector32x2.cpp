#include "cheonsa__types_vector32x2.h"
#include "cheonsa__types_vector64x2.h"
#include <cassert>

namespace cheonsa
{

	vector32x2_c::vector32x2_c()
		: a( 0.0f )
		, b( 0.0f )
	{
	}

	vector32x2_c::vector32x2_c( float32_c const a, float32_c const b )
		: a( a )
		, b( b )
	{
	}

	vector32x2_c::vector32x2_c( float32_c const * values )
		: a( values[ 0 ] )
		, b( values[ 1 ] )
	{
	}

	vector32x2_c::vector32x2_c( vector64x2_c const & other )
		: a( static_cast< float32_c >( other.a ) )
		, b( static_cast< float32_c >( other.b ) )
	{
	}

	float32_c & vector32x2_c::get_element_at_index( sint32_c const index )
	{
		assert( index >= 0 && index < 2 );
		return reinterpret_cast< float32_c * >( this )[ index ];
	}

	float32_c const & vector32x2_c::get_element_at_index( sint32_c const index ) const
	{
		assert( index >= 0 && index < 2 );
		return reinterpret_cast< float32_c const * >( this )[ index ];
	}

	vector32x2_c & vector32x2_c::operator *= ( float32_c b )
	{
		this->a *= b;
		this->b *= b;
		return *this;
	}

	vector32x2_c & vector32x2_c::operator /= ( float32_c b )
	{
		this->a /= b;
		this->b /= b;
		return *this;
	}

	vector32x2_c & vector32x2_c::operator += ( vector32x2_c const & b )
	{
		this->a += b.a;
		this->b += b.b;
		return *this;
	}

	vector32x2_c & vector32x2_c::operator -= ( vector32x2_c const & b )
	{
		this->a -= b.a;
		this->b -= b.b;
		return *this;
	}

	vector32x2_c & vector32x2_c::operator *= ( vector32x2_c const & b )
	{
		this->a *= b.a;
		this->b *= b.b;
		return *this;
	}

	vector32x2_c & vector32x2_c::operator /= ( vector32x2_c const & b )
	{
		this->a /= b.a;
		this->b /= b.b;
		return *this;
	}

	boolean_c operator == ( vector32x2_c const & a, vector32x2_c const & b )
	{
		return ( a.a == b.a ) && ( a.b == b.b );
	}

	boolean_c operator != ( vector32x2_c const & a, vector32x2_c const & b )
	{
		return ( a.a != b.a ) || ( a.b != b.b );
	}

	vector32x2_c operator - ( vector32x2_c const & a )
	{
		return vector32x2_c( -a.a, -a.b );
	}

	vector32x2_c operator * ( vector32x2_c a, float32_c const b )
	{
		return a *= b;
	}

	vector32x2_c operator / ( vector32x2_c a, float32_c const b )
	{
		return a /= b;
	}

	vector32x2_c operator + ( vector32x2_c a, vector32x2_c const & b )
	{
		return a += b;
	}

	vector32x2_c operator - ( vector32x2_c a, vector32x2_c const & b )
	{
		return a -= b;
	}

	vector32x2_c operator * ( vector32x2_c a, vector32x2_c const & b )
	{
		return a *= b;
	}

	vector32x2_c operator / ( vector32x2_c a, vector32x2_c const & b )
	{
		return a /= b;
	}

}
