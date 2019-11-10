#include "cheonsa__types.h"
#include "cheonsa__ops.h"
#include <cassert>

namespace cheonsa
{

	uint32_c fourcc( char8_c const * chars, boolean_c flipped )
	{
		return static_cast< uint32_c >( chars[ flipped ? 3 : 0 ] ) << 24 |
				static_cast< uint32_c >( chars[ flipped ? 2 : 1 ] ) << 16 |
				static_cast< uint32_c >( chars[ flipped ? 1 : 2 ] ) << 8 |
				static_cast< uint32_c >( chars[ flipped ? 0 : 3 ] );
	}

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


	//vector32x2_c & vector32x2_c::operator += ( float32_c b )
	//{
	//	this->a += b;
	//	this->b += b;
	//	return *this;
	//}

	//vector32x2_c & vector32x2_c::operator -= ( float32_c b )
	//{
	//	this->a -= b;
	//	this->b -= b;
	//	return *this;
	//}

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

	//vector32x2_c operator + ( vector32x2_c a, float32_c const b )
	//{
	//	return a += b;
	//}

	//vector32x2_c operator - ( vector32x2_c a, float32_c const b )
	//{
	//	return a -= b;
	//}

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

	//vector64x2_c & vector64x2_c::operator += ( float64_c const b )
	//{
	//	this->a += b;
	//	this->b += b;
	//	return *this;
	//}

	//vector64x2_c & vector64x2_c::operator -= ( float64_c const b )
	//{
	//	this->a -= b;
	//	this->b -= b;
	//	return *this;
	//}

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

	//vector64x2_c operator + ( vector64x2_c a, float64_c b )
	//{
	//	return a += b;
	//}

	//vector64x2_c operator - ( vector64x2_c a, float64_c b )
	//{
	//	return a -= b;
	//}

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

	//vector32x3_c & vector32x3_c::operator += ( float32_c b )
	//{
	//	this->a += b;
	//	this->b += b;
	//	this->c += b;
	//	return * this;
	//}

	//vector32x3_c & vector32x3_c::operator -= ( float32_c b )
	//{
	//	this->a -= b;
	//	this->b -= b;
	//	this->c -= b;
	//	return * this;
	//}

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

	//vector32x3_c operator + ( vector32x3_c a, float32_c b )
	//{
	//	return a += b;
	//}

	//vector32x3_c operator - ( vector32x3_c a, float32_c b )
	//{
	//	return a -= b;
	//}

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

	matrix32x3x3_c matrix32x3x3_c::get_zero()
	{
		static matrix32x3x3_c result = matrix32x3x3_c( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
		return result;
	}
	
	matrix32x3x3_c matrix32x3x3_c::get_identity()
	{
		static matrix32x3x3_c result = matrix32x3x3_c( 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f );
		return result;
	}

	matrix32x3x3_c::matrix32x3x3_c()
		: a()
		, b()
		, c()
	{
	}

	matrix32x3x3_c::matrix32x3x3_c( vector32x3_c const & a, vector32x3_c const & b, vector32x3_c const & c )
		: a( a )
		, b( b )
		, c( c )
	{
	}

	matrix32x3x3_c::matrix32x3x3_c( float32_c aa, float32_c ab, float32_c ac, float32_c ba, float32_c bb, float32_c bc, float32_c ca, float32_c cb, float32_c cc )
		: a( aa, ab, ac )
		, b( ba, bb, bc )
		, c( ca, cb, cc )
	{
	}

	vector32x3_c & matrix32x3x3_c::get_element_at_index( sint32_c i )
	{
		assert( i >= 0 && i < 3 );
		switch ( i )
		{
		case 0:
			return a;
		case 1:
			return b;
		case 2:
			return c;
		}
		return a;
	}

	vector32x3_c const & matrix32x3x3_c::get_element_at_index( sint32_c i ) const
	{
		assert( i >= 0 && i < 3 );
		switch ( i )
		{
		case 0:
			return a;
		case 1:
			return b;
		case 2:
			return c;
		}
		return a;
	}

	matrix32x3x3_c matrix32x3x3_c::operator *= ( float32_c b )
	{
		a *= b;
		b *= b;
		c *= b;
		return *this;
	}

	matrix32x3x3_c operator * ( matrix32x3x3_c a, matrix32x3x3_c const & b )
	{
		matrix32x3x3_c result;
		result.a.a = ( a.a.a * b.a.a ) + ( a.a.b * b.b.a ) + ( a.a.c * b.c.a );
		result.a.b = ( a.a.a * b.a.b ) + ( a.a.b * b.b.b ) + ( a.a.c * b.c.b );
		result.a.c = ( a.a.a * b.a.c ) + ( a.a.b * b.b.c ) + ( a.a.c * b.c.c );
		result.b.a = ( a.b.a * b.a.a ) + ( a.b.b * b.b.a ) + ( a.b.c * b.c.a );
		result.b.b = ( a.b.a * b.a.b ) + ( a.b.b * b.b.b ) + ( a.b.c * b.c.b );
		result.b.c = ( a.b.a * b.a.c ) + ( a.b.b * b.b.c ) + ( a.b.c * b.c.c );
		result.c.a = ( a.c.a * b.a.a ) + ( a.c.b * b.b.a ) + ( a.c.c * b.c.a );
		result.c.c = ( a.c.a * b.a.b ) + ( a.c.b * b.b.b ) + ( a.c.c * b.c.b );
		result.c.c = ( a.c.a * b.a.c ) + ( a.c.b * b.b.c ) + ( a.c.c * b.c.c );
		return result;
	}

	matrix32x4x4_c::matrix32x4x4_c()
		: a( 1.0f, 0.0f, 0.0f, 0.0f )
		, b( 0.0f, 1.0f, 0.0f, 0.0f )
		, c( 0.0f, 0.0f, 1.0f, 0.0f )
		, d( 0.0f, 0.0f, 0.0f, 1.0f )
	{
	}

	matrix32x4x4_c::matrix32x4x4_c( vector32x3_c const & basis_a, vector32x3_c const & basis_b, vector32x3_c const & basis_c, vector32x3_c const & position )
		: a( basis_a.a, basis_a.b, basis_a.c, 0.0f )
		, b( basis_b.a, basis_b.b, basis_b.c, 0.0f )
		, c( basis_c.a, basis_c.b, basis_c.c, 0.0f )
		, d( position.a, position.b, position.c, 1.0f )
	{
	}

	matrix32x4x4_c::matrix32x4x4_c( vector32x4_c const & a, vector32x4_c const & b, vector32x4_c const & c, vector32x4_c const & d )
		: a( a )
		, b( b )
		, c( c )
		, d( d )
	{
	}

	matrix32x4x4_c::matrix32x4x4_c( float32_c const aa, float32_c const ab, float32_c const ac, float32_c const ad, float32_c const ba, float32_c const bb, float32_c const bc, float32_c const bd, float32_c const ca, float32_c const cb, float32_c const cc, float32_c const cd, float32_c const da, float32_c const db, float32_c const dc, float32_c const dd )
		: a( aa, ab, ac, ad )
		, b( ba, bb, bc, bd )
		, c( ca, cb, cc, cd )
		, d( da, db, dc, dd )
	{
	}

	matrix32x4x4_c::matrix32x4x4_c( float32_c const * values )
		: a( values[ 0 ], values[ 1 ], values[ 2 ], values[ 3 ] )
		, b( values[ 4 ], values[ 5 ], values[ 6 ], values[ 7 ] )
		, c( values[ 8 ], values[ 9 ], values[ 10 ], values[ 11 ] )
		, d( values[ 12 ], values[ 13 ], values[ 14 ], values[ 15 ] )
	{
	}

	vector32x4_c & matrix32x4x4_c::get_element_at_index( sint32_c const index )
	{
		assert( index >= 0 && index < 4 );
		return reinterpret_cast< vector32x4_c * >( this )[ index ];
	}

	vector32x4_c const & matrix32x4x4_c::get_element_at_index( sint32_c const index ) const
	{
		assert( index >= 0 && index < 4 );
		return reinterpret_cast< vector32x4_c const * >( this )[ index ];
	}

	vector32x3_c & matrix32x4x4_c::get_basis_element_at_index( sint32_c const index )
	{
		assert( index >= 0 && index < 4 );
		return * reinterpret_cast< vector32x3_c * >( & reinterpret_cast< vector32x4_c * >( this )[ index ] );
	}

	vector32x3_c const & matrix32x4x4_c::get_basis_element_at_index( sint32_c const index ) const
	{
		assert( index >= 0 && index < 4 );
		return * reinterpret_cast< vector32x3_c const * >( & reinterpret_cast< vector32x4_c const * >( this )[ index ] );
	}

	boolean_c operator == ( matrix32x4x4_c const & a, matrix32x4x4_c const & b )
	{
		return ( a.a == b.a ) && ( a.b == b.b ) && ( a.c == b.c ) && ( a.d == b.d );
	}

	boolean_c operator != ( matrix32x4x4_c const & a, matrix32x4x4_c const & b )
	{
		return ( a.a != b.a ) || ( a.b != b.b ) || ( a.c != b.c ) || ( a.d != b.d );
	}

	matrix32x4x4_c operator * ( matrix32x4x4_c const & a, matrix32x4x4_c const & b )
	{
		matrix32x4x4_c result;
		result.a.a = ( a.a.a * b.a.a ) + ( a.a.b * b.b.a ) + ( a.a.c * b.c.a ) + ( a.a.d * b.d.a );
		result.a.b = ( a.a.a * b.a.b ) + ( a.a.b * b.b.b ) + ( a.a.c * b.c.b ) + ( a.a.d * b.d.b );
		result.a.c = ( a.a.a * b.a.c ) + ( a.a.b * b.b.c ) + ( a.a.c * b.c.c ) + ( a.a.d * b.d.c );
		result.a.d = ( a.a.a * b.a.d ) + ( a.a.b * b.b.d ) + ( a.a.c * b.c.d ) + ( a.a.d * b.d.d );
		result.b.a = ( a.b.a * b.a.a ) + ( a.b.b * b.b.a ) + ( a.b.c * b.c.a ) + ( a.b.d * b.d.a );
		result.b.b = ( a.b.a * b.a.b ) + ( a.b.b * b.b.b ) + ( a.b.c * b.c.b ) + ( a.b.d * b.d.b );
		result.b.c = ( a.b.a * b.a.c ) + ( a.b.b * b.b.c ) + ( a.b.c * b.c.c ) + ( a.b.d * b.d.c );
		result.b.d = ( a.b.a * b.a.d ) + ( a.b.b * b.b.d ) + ( a.b.c * b.c.d ) + ( a.b.d * b.d.d );
		result.c.a = ( a.c.a * b.a.a ) + ( a.c.b * b.b.a ) + ( a.c.c * b.c.a ) + ( a.c.d * b.d.a );
		result.c.b = ( a.c.a * b.a.b ) + ( a.c.b * b.b.b ) + ( a.c.c * b.c.b ) + ( a.c.d * b.d.b );
		result.c.c = ( a.c.a * b.a.c ) + ( a.c.b * b.b.c ) + ( a.c.c * b.c.c ) + ( a.c.d * b.d.c );
		result.c.d = ( a.c.a * b.a.d ) + ( a.c.b * b.b.d ) + ( a.c.c * b.c.d ) + ( a.c.d * b.d.d );
		result.d.a = ( a.d.a * b.a.a ) + ( a.d.b * b.b.a ) + ( a.d.c * b.c.a ) + ( a.d.d * b.d.a );
		result.d.b = ( a.d.a * b.a.b ) + ( a.d.b * b.b.b ) + ( a.d.c * b.c.b ) + ( a.d.d * b.d.b );
		result.d.c = ( a.d.a * b.a.c ) + ( a.d.b * b.b.c ) + ( a.d.c * b.c.c ) + ( a.d.d * b.d.c );
		result.d.d = ( a.d.a * b.a.d ) + ( a.d.b * b.b.d ) + ( a.d.c * b.c.d ) + ( a.d.d * b.d.d );
		return result;
	}

	matrix32x2x2_c::matrix32x2x2_c()
		: a( 1.0f, 0.0f )
		, b( 0.0f, 1.0f )
	{
	}

	matrix32x2x2_c::matrix32x2x2_c( vector32x2_c const & a, vector32x2_c const & b )
		: a( a )
		, b( b )
	{
	}

	matrix32x2x2_c::matrix32x2x2_c( float32_c const aa, float32_c const ab, float32_c const ba, float32_c const bb )
		: a( aa, ab )
		, b( ba, bb )
	{
	}

	matrix32x2x2_c::matrix32x2x2_c( float32_c const * values )
		: a( values[ 0 ], values[ 1 ] )
		, b( values[ 2 ], values[ 3 ] )
	{
	}

	boolean_c operator == ( matrix32x2x2_c const & a, matrix32x2x2_c const & b )
	{
		return ( a.a == b.a ) && ( b.a == b.b );
	}

	boolean_c operator != ( matrix32x2x2_c const & a, matrix32x2x2_c const & b )
	{
		return ( a.a != b.a ) || ( a.b != b.b );
	}

	matrix32x2x2_c operator * ( matrix32x2x2_c const & a, matrix32x2x2_c const & b )
	{
		return matrix32x2x2_c{
			( a.a.a * b.a.a ) + ( a.a.b * b.b.a ),
			( a.a.a * b.a.b ) + ( a.a.b * b.b.b ),
			( a.b.a * b.a.a ) + ( a.b.b * b.b.a ),
			( a.b.a * b.a.b ) + ( a.b.b * b.b.b ) };
	}

	quaternion32_c::quaternion32_c()
		: a( 0.0f )
		, b( 0.0f )
		, c( 0.0f )
		, d( 1.0f )
	{
	}

	quaternion32_c::quaternion32_c( float32_c const a, float32_c const b, float32_c const c, float32_c const d )
		: a( a )
		, b( b )
		, c( c )
		, d( d )
	{
	}

	quaternion32_c::quaternion32_c( float32_c const * values )
		: a( values[ 0 ] )
		, b( values[ 1 ] )
		, c( values[ 2 ] )
		, d( values[ 3 ] )
	{
	}

	void_c quaternion32_c::set_abc_calculate_d( float32_c const * abc )
	{
		a = abc[ 0 ];
		b = abc[ 1 ];
		c = abc[ 2 ];
		d = 1.0f - ( abc[ 0 ] * abc[ 0 ] ) - ( abc[ 1 ] * abc[ 1 ] ) - ( abc[ 2 ] * abc[ 2 ] );
		d = d < 0.0f ? 0.0f : -ops::math_square_root( d );
	}

	void_c quaternion32_c::calculate_d_from_abc()
	{
		d = 1.0f - ( a * a ) - ( b * b ) - ( c * c );
		d = d < 0.0f ? 0.0f : -ops::math_square_root( d );
	}

	boolean_c operator == ( quaternion32_c const & a, quaternion32_c const & b )
	{
		return ( a.a == b.a ) && ( a.b == b.b ) && ( a.c == b.c ) && ( a.d == b.d );
	}

	boolean_c operator != ( quaternion32_c const & a, quaternion32_c const & b )
	{
		return ( a.a != b.a ) || ( a.b != b.b ) || ( a.c != b.c ) || ( a.d != b.d );
	}

	quaternion32_c operator + ( quaternion32_c const & a, quaternion32_c const & b )
	{
		return quaternion32_c( a.a + b.a, a.b * b.b, a.c * b.c, a.d * b.d );
	}

	quaternion32_c operator * ( quaternion32_c const & a, float32_c const b )
	{
		return quaternion32_c( a.a * b, a.b * b, a.c * b, a.d * b );
	}

	quaternion32_c operator * ( quaternion32_c const & a, quaternion32_c const & b )
	{
		return quaternion32_c{
			( a.a * b.d ) + ( a.b * b.c ) - ( a.c * b.b ) + ( a.d * b.a ),
			( -a.a * b.c ) + ( a.b * b.d ) + ( a.c * b.a ) + ( a.d * b.b ),
			( a.a * b.b ) - ( a.b * b.a ) + ( a.c * b.d ) + ( a.d * b.c ),
			( -a.a * b.a ) - ( a.b * b.b ) - ( a.c * b.c ) + ( a.d * b.d ) };
	}

	line32_c::line32_c()
		: a( 0.0f )
		, b( 0.0f )
		, c( 0.0f )
	{
	}

	line32_c::line32_c( float32_c a, float32_c b, float32_c c )
		: a( a )
		, b( b )
		, c( c )
	{
	}

	line32_c::line32_c( float32_c const * value )
		: a( value[ 0 ] )
		, b( value[ 1 ] )
		, c( value[ 2 ] )
	{
	}

	vector32x2_c line32_c::get_point() const
	{
		return vector32x2_c( a * -c, b * -c );
	}

	void_c line32_c::set_point( vector32x2_c const & value )
	{
		c = -( a * value.a + b * value.b );
	}

	plane32_c::plane32_c()
		: a( 0 )
		, b( 0 )
		, c( 0 )
		, d( 0 )
	{
	}

	plane32_c::plane32_c( float32_c const a, float32_c const b, float32_c const c, float32_c const d )
		: a( a )
		, b( b )
		, c( c )
		, d( d )
	{
	}

	plane32_c::plane32_c( float32_c const * values )
		: a( values[ 0 ] )
		, b( values[ 1 ] )
		, c( values[ 2 ] )
		, d( values[ 3 ] )
	{
	}

	vector32x3_c plane32_c::get_point() const
	{
		return get_normal() * -d;
	}

	void_c plane32_c::set_point( vector32x3_c const & value )
	{
		d = -( a * value.a + b * value.b + c * value.c );
	}

	boolean_c operator == ( plane32_c const & a, plane32_c const & b )
	{
		return ( a.a == b.a ) && ( a.b == b.b ) && ( a.c == b.c ) && ( a.d == b.d );
	}

	boolean_c operator != ( plane32_c const & a, plane32_c const & b )
	{
		return ( a.a != b.a ) || ( a.b != b.b ) || ( a.c != b.c ) || ( a.d != b.d );
	}

	plane64_c::plane64_c()
		: a( 0 )
		, b( 0 )
		, c( 0 )
		, d( 0 )
	{
	}

	plane64_c::plane64_c( float64_c const a, float64_c const b, float64_c const c, float64_c const d )
		: a( a )
		, b( b )
		, c( c )
		, d( d )
	{
	}

	plane64_c::plane64_c( float64_c const * values )
		: a( values[ 0 ] )
		, b( values[ 1 ] )
		, c( values[ 2 ] )
		, d( values[ 3 ] )
	{
	}

	vector64x3_c plane64_c::get_point() const
	{
		return vector64x3_c( a * -d, b * -d, c * -d );
	}

	void_c plane64_c::set_point( vector64x3_c const & value )
	{
		d = -( a * value.a + b * value.b + c * value.c );
	}

	boolean_c operator == ( plane64_c const & a, plane64_c const & b )
	{
		return ( a.a == b.a ) && ( a.b == b.b ) && ( a.c == b.c ) && ( a.d == b.d );
	}

	boolean_c operator != ( plane64_c const & a, plane64_c const & b )
	{
		return ( a.a != b.a ) || ( a.b != b.b ) || ( a.c != b.c ) || ( a.d != b.d );
	}

	frustum64_c::frustum64_c()
		: planes{}
		, points{}
	{
	}

	void_c frustum64_c::make_planes_from_points()
	{
		planes[ frustum_plane_index_e_top ] = ops::make_plane64_from_triangle( points[ frustum_point_index_e_near_top_right ], points[ frustum_point_index_e_near_top_left ], points[ frustum_point_index_e_far_top_right ] );
		planes[ frustum_plane_index_e_bottom ] = ops::make_plane64_from_triangle( points[ frustum_point_index_e_near_bottom_left ], points[ frustum_point_index_e_near_bottom_right ], points[ frustum_point_index_e_far_bottom_left ] );
		planes[ frustum_plane_index_e_left ] = ops::make_plane64_from_triangle( points[ frustum_point_index_e_near_top_left ], points[ frustum_point_index_e_near_bottom_left ], points[ frustum_point_index_e_far_top_left ] );
		planes[ frustum_plane_index_e_right ] = ops::make_plane64_from_triangle( points[ frustum_point_index_e_near_bottom_right ], points[ frustum_point_index_e_near_top_right ], points[ frustum_point_index_e_far_bottom_right ] );
		planes[ frustum_plane_index_e_near ] = ops::make_plane64_from_triangle( points[ frustum_point_index_e_near_top_left ], points[ frustum_point_index_e_near_top_right ], points[ frustum_point_index_e_near_bottom_left ] );
		planes[ frustum_plane_index_e_far ] = ops::make_plane64_from_triangle( points[ frustum_point_index_e_far_top_right ], points[ frustum_point_index_e_far_top_left ], points[ frustum_point_index_e_far_bottom_right ] );
	}

	void_c frustum64_c::make_points_from_planes()
	{
		ops::intersect_plane_vs_plane_vs_plane( planes[ frustum_plane_index_e_near ], planes[ frustum_plane_index_e_top ], planes[ frustum_plane_index_e_left ], &points[ frustum_point_index_e_near_top_left ] );
		ops::intersect_plane_vs_plane_vs_plane( planes[ frustum_plane_index_e_near ], planes[ frustum_plane_index_e_top ], planes[ frustum_plane_index_e_right ], &points[ frustum_point_index_e_near_top_right ] );
		ops::intersect_plane_vs_plane_vs_plane( planes[ frustum_plane_index_e_near ], planes[ frustum_plane_index_e_bottom ], planes[ frustum_plane_index_e_left ], &points[ frustum_point_index_e_near_bottom_left ] );
		ops::intersect_plane_vs_plane_vs_plane( planes[ frustum_plane_index_e_near ], planes[ frustum_plane_index_e_bottom ], planes[ frustum_plane_index_e_right ], &points[ frustum_point_index_e_near_bottom_right ] );
		ops::intersect_plane_vs_plane_vs_plane( planes[ frustum_plane_index_e_far ], planes[ frustum_plane_index_e_top ], planes[ frustum_plane_index_e_left ], &points[ frustum_point_index_e_far_top_left ] );
		ops::intersect_plane_vs_plane_vs_plane( planes[ frustum_plane_index_e_far ], planes[ frustum_plane_index_e_top ], planes[ frustum_plane_index_e_right ], &points[ frustum_point_index_e_far_top_right ] );
		ops::intersect_plane_vs_plane_vs_plane( planes[ frustum_plane_index_e_far ], planes[ frustum_plane_index_e_bottom ], planes[ frustum_plane_index_e_left ], &points[ frustum_point_index_e_far_bottom_left ] );
		ops::intersect_plane_vs_plane_vs_plane( planes[ frustum_plane_index_e_far ], planes[ frustum_plane_index_e_bottom ], planes[ frustum_plane_index_e_right ], &points[ frustum_point_index_e_far_bottom_right ] );
	}

	void_c frustum64_c::translate( vector64x3_c const & offset )
	{
		for ( sint32_c i = 0; i < 8; i++ )
		{
			planes[ i ].d += ops::make_float64_dot_product( planes[ i ].get_normal(), offset );
		}
		make_points_from_planes();
	}

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

	boolean_c operator == ( box32x3_c const & a, box32x3_c const & b )
	{
		return ( a.minimum == b.minimum ) && ( a.maximum == b.maximum );
	}

	boolean_c operator != ( box32x3_c const & a, box32x3_c const & b )
	{
		return ( a.minimum != b.minimum ) || ( a.maximum != b.maximum );
	}

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

	vector64x3_c box64x3_c::get_point( sint32_c const index ) const
	{
		return vector64x3_c( index & 1 ? maximum.a : minimum.a, index & 2 ? maximum.b : minimum.b, index & 4 ? maximum.c : minimum.c );
	}

	boolean_c operator == ( box64x3_c const & a, box64x3_c const & b )
	{
		return ( a.minimum == b.minimum ) && ( a.maximum == b.maximum );
	}

	boolean_c operator != ( box64x3_c const & a, box64x3_c const & b )
	{
		return ( a.minimum != b.minimum ) || ( a.maximum != b.maximum );
	}

	void_c triangle64_c::make_planes_from_points()
	{
		face_plane = ops::make_plane64_from_triangle( point_a, point_b, point_c );
		edge_plane_ab = ops::make_plane64_from_normal_and_point( ops::make_vector64x3_normalized( ops::make_vector64x3_cross_product( point_a - point_b, face_plane.get_normal() ) ), point_a );
		edge_plane_bc = ops::make_plane64_from_normal_and_point( ops::make_vector64x3_normalized( ops::make_vector64x3_cross_product( point_b - point_c, face_plane.get_normal() ) ), point_b );
		edge_plane_ca = ops::make_plane64_from_normal_and_point( ops::make_vector64x3_normalized( ops::make_vector64x3_cross_product( point_c - point_a, face_plane.get_normal() ) ), point_c );
	}

	space_transform_c::space_transform_c()
		: position()
		, rotation()
		, scale( 1.0f, 1.0f, 1.0f )
	{
	}

	space_transform_c::space_transform_c( vector64x3_c const & position, quaternion32_c const & rotation, float32_c uniform_scale )
		: position( position )
		, rotation( rotation )
		, scale( uniform_scale, uniform_scale, uniform_scale )
	{
	}

	space_transform_c::space_transform_c( vector64x3_c const & position, quaternion32_c const & rotation, vector32x3_c const & scale )
		: position( position )
		, rotation( rotation )
		, scale( scale )
	{
	}

	void_c space_transform_c::reset()
	{
		position.a = 0.0;
		position.b = 0.0;
		position.c = 0.0;
		rotation.a = 0.0f;
		rotation.b = 0.0f;
		rotation.c = 0.0f;
		rotation.d = 1.0f;
		scale.a = 1.0f;
		scale.b = 1.0f;
		scale.c = 1.0f;
	}

	//vector32x3_c space_transform_c::get_rotation_euler_angles() const
	//{
	//	return ops::make_euler_angles_from_quaternion32( rotation );
	//}

	//void_c space_transform_c::set_rotation_euler_angles( vector32x3_c const & euler_angles )
	//{
	//	rotation = ops::make_quaternion32_from_euler_angles( euler_angles );
	//}

	matrix32x3x3_c space_transform_c::get_scaled_basis() const
	{
		matrix32x3x3_c result = ops::make_matrix32x3x3_basis_from_quaternion32( rotation );
		result.a *= scale.a;
		result.b *= scale.b;
		result.c *= scale.c;
		return result;
	}

	vector32x3_c space_transform_c::get_scaled_basis_a() const
	{
		vector32x3_c result = ops::make_vector32x3_basis_a_from_quaternion32( rotation );
		result *= scale;
		return result;
	}

	vector32x3_c space_transform_c::get_scaled_basis_b() const
	{
		vector32x3_c result = ops::make_vector32x3_basis_b_from_quaternion32( rotation );
		result *= scale;
		return result;
	}

	vector32x3_c space_transform_c::get_scaled_basis_c() const
	{
		vector32x3_c result = ops::make_vector32x3_basis_c_from_quaternion32( rotation );
		result *= scale;
		return result;
	}

	matrix32x3x3_c space_transform_c::get_unscaled_basis() const
	{
		return ops::make_matrix32x3x3_basis_from_quaternion32( rotation );
	}

	vector32x3_c space_transform_c::get_unscaled_basis_a() const
	{
		return ops::make_vector32x3_basis_a_from_quaternion32( rotation );
	}

	vector32x3_c space_transform_c::get_unscaled_basis_b() const
	{
		return ops::make_vector32x3_basis_b_from_quaternion32( rotation );
	}

	vector32x3_c space_transform_c::get_unscaled_basis_c() const
	{
		return ops::make_vector32x3_basis_c_from_quaternion32( rotation );
	}

	void_c space_transform_c::set_unscaled_basis( matrix32x3x3_c const & value )
	{
		rotation = ops::make_quaternion32_from_matrix32x3x3_basis( value );
	}

	void_c space_transform_c::set_uniform_scale( float32_c value )
	{
		scale.a = value;
		scale.b = value;
		scale.c = value;
	}

	space_transform_c space_transform_c::get_inverted() const
	{
		space_transform_c result;
		result.rotation.a = -rotation.a;
		result.rotation.b = -rotation.b;
		result.rotation.c = -rotation.c;
		result.rotation.d =  rotation.d;
		result.scale.a = 1.0f / scale.a;
		result.scale.b = 1.0f / scale.b;
		result.scale.c = 1.0f / scale.c;
		result.position = ops::make_vector64x3_rotated_vector( position, result.rotation ) * -result.scale;
		return result;
	}

	void_c space_transform_c::invert()
	{
		rotation.a = -rotation.a;
		rotation.b = -rotation.b;
		rotation.c = -rotation.c;
		scale.a = 1.0f / scale.a;
		scale.b = 1.0f / scale.b;
		scale.c = 1.0f / scale.c;
		position = ops::make_vector64x3_rotated_vector( position, rotation ) * -scale;
	}

	space_transform_c & space_transform_c::operator = ( space_transform_c const & other )
	{
		position = other.position;
		rotation = other.rotation;
		scale = other.scale;
		return *this;
	}

	space_transform_c & space_transform_c::operator *= ( space_transform_c const & b )
	{
		this->rotation = this->rotation * b.rotation;
		this->scale = this->scale * b.scale;
		this->position = this->position + ops::make_vector64x3_rotated_vector( b.position, this->rotation ) * this->scale;
		return *this;
	}

	boolean_c operator == ( space_transform_c const & a, space_transform_c const & b )
	{
		return ( a.position == b.position ) && ( a.rotation == b.rotation ) && ( a.scale == b.scale );
	}

	boolean_c operator != ( space_transform_c const & a, space_transform_c const & b )
	{
		return ( a.position != b.position ) || ( a.rotation != b.rotation ) || ( a.scale != b.scale );
	}

	space_transform_c operator * ( space_transform_c a, space_transform_c const & b )
	{
		return a *= b;
	}

	scene_matrix_c scene_matrix_c::operator * ( scene_matrix_c const & other ) const
	{
		scene_matrix_c result;
		result.basis = basis * other.basis;
		result.position = position + ops::make_vector64x3_transformed_point( other.position, basis );
		return result;
	}

	polygon32x2_c::polygon32x2_c()
		: origin()
		, basis()
		, points_count( 0 )
		, points{}
	{
	}

	vector32x2_c polygon32x2_c::get_point( sint32_c point_index ) const
	{
		assert( point_index >= 0 && point_index < points_count );
		return ops::make_vector32x2_transformed_point( points[ point_index ], basis ) + origin;
	}

	vector32x2_c polygon32x2_c::get_edge_vector( sint32_c edge_index ) const
	{
		assert( edge_index >= 0 && edge_index < points_count );
		return get_point( ( edge_index + 1 ) % points_count ) - get_point( edge_index );
	}

	line32_c polygon32x2_c::get_edge_line( sint32_c edge_index ) const
	{
		assert( edge_index >= 0 && edge_index < points_count );
		return ops::make_line32_from_segment( get_point( ( edge_index + 1 ) % points_count ), get_point( edge_index ) ); 
	}

}
