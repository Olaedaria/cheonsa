#include "cheonsa_math_types.h"
#include "cheonsa_ops.h"
#include <assert.h>

namespace cheonsa
{

	color32_c::color32_c()
	{
		value_uint32 = 0;
	}

	color32_c::color32_c( uint32_c uint32 )
	{
	}

	color32_c::color32_c( vector32x4_c const & vector32x4 )
	{
	}

	uint32_c color32_c::as_uint32() const
	{
		return value_uint32;
	}

	vector32x4_c color32_c::as_vector32x4() const
	{
		return vector32x4_c( 
			static_cast< float32_c >( value_bytes[ 0 ] ) / 255.0f,
			static_cast< float32_c >( value_bytes[ 1 ] ) / 255.0f,
			static_cast< float32_c >( value_bytes[ 2 ] ) / 255.0f,
			static_cast< float32_c >( value_bytes[ 3 ] ) / 255.0f );
	}

	float32_c color32_c::r_get() const
	{
		return static_cast< float32_c >( value_bytes[ 0 ] ) / 255.0f;
	}

	float32_c color32_c::g_get() const
	{
		return static_cast< float32_c >( value_bytes[ 1 ] ) / 255.0f;
	}

	float32_c color32_c::b_get() const
	{
		return static_cast< float32_c >( value_bytes[ 2 ] ) / 255.0f;
	}

	float32_c color32_c::a_get() const
	{
		return static_cast< float32_c >( value_bytes[ 3 ] ) / 255.0f;
	}

	void_c color32_c::r_set( float32_c value )
	{
		if ( value > 1.0f )
		{
			value = 1.0f;
		}
		else if ( value < 0.0f )
		{
			value = 0.0f;
		}
		value_bytes[ 0 ] = static_cast< uint8_c >( value * 255.0f );
	}

	void_c color32_c::g_set( float32_c value )
	{
		if ( value > 1.0f )
		{
			value = 1.0f;
		}
		else if ( value < 0.0f )
		{
			value = 0.0f;
		}
		value_bytes[ 1 ] = static_cast< uint8_c >( value * 255.0f );
	}

	void_c color32_c::b_set( float32_c value )
	{
		if ( value > 1.0f )
		{
			value = 1.0f;
		}
		else if ( value < 0.0f )
		{
			value = 0.0f;
		}
		value_bytes[ 2 ] = static_cast< uint8_c >( value * 255.0f );
	}

	void_c color32_c::a_set( float32_c value )
	{
		if ( value > 1.0f )
		{
			value = 1.0f;
		}
		else if ( value < 0.0f )
		{
			value = 0.0f;
		}
		value_bytes[ 3 ] = static_cast< uint8_c >( value * 255.0f );
	}

	boolean_c color32_c::operator == ( color32_c const & other ) const
	{
		return value_uint32 == other.value_uint32;
	}

	boolean_c color32_c::operator != ( color32_c const & other ) const
	{
		return value_uint32 != other.value_uint32;
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

	boolean_c vector32x2_c::operator == ( vector32x2_c const & other ) const
	{
		return ( a == other.a ) && ( b == other.b );
	}

	boolean_c vector32x2_c::operator != ( vector32x2_c const & other ) const
	{
		return ( a != other.a ) || ( b != other.b );
	}

	vector32x2_c vector32x2_c::operator - () const
	{
		return vector32x2_c( -a, -b );
	}

	vector32x2_c vector32x2_c::operator + ( float32_c const other ) const
	{
		return vector32x2_c( a + other, b + other );
	}

	vector32x2_c vector32x2_c::operator - ( float32_c const other ) const
	{
		return vector32x2_c( a - other, b - other );
	}

	vector32x2_c vector32x2_c::operator * ( float32_c other ) const
	{
		return vector32x2_c( a * other, b * other );
	}

	vector32x2_c vector32x2_c::operator / ( float32_c const other ) const
	{
		return vector32x2_c( a / other, b / other );
	}

	vector32x2_c vector32x2_c::operator + ( vector32x2_c const & other ) const
	{
		return vector32x2_c( a + other.a, b + other.b );
	}

	vector32x2_c vector32x2_c::operator - ( vector32x2_c const & other ) const
	{
		return vector32x2_c( a - other.a, b - other.b );
	}

	vector32x2_c vector32x2_c::operator * ( vector32x2_c const & other ) const
	{
		return vector32x2_c( a * other.a, b * other.b );
	}

	vector32x2_c vector32x2_c::operator / ( vector32x2_c const & other ) const
	{
		return vector32x2_c( a / other.a, b / other.b );
	}

	vector32x2_c & vector32x2_c::operator += ( float32_c const other )
	{
		a += other;
		b += other;
		return * this;
	}

	vector32x2_c & vector32x2_c::operator -= ( float32_c const other )
	{
		a -= other;
		b -= other;
		return * this;
	}

	vector32x2_c & vector32x2_c::operator *= ( float32_c const other )
	{
		a *= other;
		b *= other;
		return * this;
	}

	vector32x2_c & vector32x2_c::operator /= ( float32_c const other )
	{
		a /= other;
		b /= other;
		return * this;
	}

	vector32x2_c & vector32x2_c::operator += ( vector32x2_c const & other )
	{
		a += other.a;
		b += other.b;
		return * this;
	}

	vector32x2_c & vector32x2_c::operator -= ( vector32x2_c const & other )
	{
		a -= other.a;
		b -= other.b;
		return * this;
	}

	vector32x2_c & vector32x2_c::operator *= ( vector32x2_c const & other )
	{
		a *= other.a;
		b *= other.b;
		return * this;
	}

	vector32x2_c & vector32x2_c::operator /= ( vector32x2_c const & other )
	{
		a /= other.a;
		b /= other.b;
		return * this;
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

	boolean_c vector64x2_c::operator == ( vector64x2_c const & other ) const
	{
		return ( a == other.a ) && ( b == other.b );
	}

	boolean_c vector64x2_c::operator != ( vector64x2_c const & other ) const
	{
		return ( a != other.a ) || ( b != other.b );
	}

	vector64x2_c vector64x2_c::operator - () const
	{
		return vector64x2_c( -a, -b );
	}

	vector64x2_c vector64x2_c::operator + ( float64_c const other ) const
	{
		return vector64x2_c( a + other, b + other );
	}

	vector64x2_c vector64x2_c::operator - ( float64_c const other ) const
	{
		return vector64x2_c( a - other, b - other );
	}

	vector64x2_c vector64x2_c::operator * ( float64_c const other ) const
	{
		return vector64x2_c( a * other, b * other );
	}

	vector64x2_c vector64x2_c::operator / ( float64_c const other ) const
	{
		return vector64x2_c( a / other, b / other );
	}

	vector64x2_c vector64x2_c::operator + ( vector64x2_c const & other ) const
	{
		return vector64x2_c( a + other.a, b + other.b );
	}

	vector64x2_c vector64x2_c::operator - ( vector64x2_c const & other ) const
	{
		return vector64x2_c( a - other.a, b - other.b );
	}

	vector64x2_c vector64x2_c::operator * ( vector64x2_c const & other ) const
	{
		return vector64x2_c( a * other.a, b * other.b );
	}

	vector64x2_c vector64x2_c::operator / ( vector64x2_c const & other ) const
	{
		return vector64x2_c( a / other.a, b / other.b );
	}

	vector64x2_c & vector64x2_c::operator += ( float64_c const other )
	{
		a += other;
		b += other;
		return * this;
	}

	vector64x2_c & vector64x2_c::operator -= ( float64_c const other )
	{
		a -= other;
		b -= other;
		return * this;
	}

	vector64x2_c & vector64x2_c::operator *= ( float64_c const other )
	{
		a *= other;
		b *= other;
		return * this;
	}

	vector64x2_c & vector64x2_c::operator /= ( float64_c const other )
	{
		a *= other;
		b *= other;
		return * this;
	}

	vector64x2_c & vector64x2_c::operator += ( vector64x2_c const & other )
	{
		a += other.a;
		b += other.b;
		return * this;
	}

	vector64x2_c & vector64x2_c::operator -= ( vector64x2_c const & other )
	{
		a -= other.a;
		b -= other.b;
		return * this;
	}

	vector64x2_c & vector64x2_c::operator *= ( vector64x2_c const & other )
	{
		a *= other.a;
		b *= other.b;
		return * this;
	}

	vector64x2_c & vector64x2_c::operator /= ( vector64x2_c const & other )
	{
		a /= other.a;
		b /= other.b;
		return * this;
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

	boolean_c vector32x3_c::operator == ( vector32x3_c const & other ) const
	{
		return ( a == other.a ) && ( b == other.b ) && ( c == other.c );
	}

	boolean_c vector32x3_c::operator != ( vector32x3_c const & other ) const
	{
		return ( a != other.a ) || ( b != other.b ) || ( c != other.c );
	}

	vector32x3_c vector32x3_c::operator - () const
	{
		return vector32x3_c( -a, -b, -c );
	}

	vector32x3_c vector32x3_c::operator + ( float32_c other ) const
	{
		return vector32x3_c( a + other, b + other, c + other );
	}

	vector32x3_c vector32x3_c::operator - ( float32_c other ) const
	{
		return vector32x3_c( a - other, b - other, c - other );
	}

	vector32x3_c vector32x3_c::operator * ( float32_c other ) const
	{
		return vector32x3_c( a * other, b * other, c * other );
	}

	vector32x3_c vector32x3_c::operator / ( float32_c other ) const
	{
		return vector32x3_c( a / other, b / other, c / other );
	}

	vector32x3_c vector32x3_c::operator + ( vector32x3_c const & other ) const
	{
		return vector32x3_c( a + other.a, b + other.b, c + other.c );
	}

	vector32x3_c vector32x3_c::operator - ( vector32x3_c const & other ) const
	{
		return vector32x3_c( a - other.a, b - other.b, c - other.c );
	}

	vector32x3_c vector32x3_c::operator * ( vector32x3_c const & other ) const
	{
		return vector32x3_c( a * other.a, b * other.b, c * other.c );
	}

	vector32x3_c vector32x3_c::operator / ( vector32x3_c const & other ) const
	{
		return vector32x3_c( a / other.a, b / other.b, c / other.c );
	}

	vector32x3_c & vector32x3_c::operator += ( float32_c const other )
	{
		a += other;
		b += other;
		c += other;
		return * this;
	}

	vector32x3_c & vector32x3_c::operator -= ( float32_c const other )
	{
		a -= other;
		b -= other;
		c -= other;
		return * this;
	}

	vector32x3_c & vector32x3_c::operator *= ( float32_c const other )
	{
		a *= other;
		b *= other;
		c *= other;
		return * this;
	}

	vector32x3_c & vector32x3_c::operator /= ( float32_c const other )
	{
		a /= other;
		b /= other;
		c /= other;
		return * this;
	}

	vector32x3_c & vector32x3_c::operator += ( vector32x3_c const & other )
	{
		a += other.a;
		b += other.b;
		c += other.c;
		return * this;
	}

	vector32x3_c & vector32x3_c::operator -= ( vector32x3_c const & other )
	{
		a -= other.a;
		b -= other.b;
		c -= other.c;
		return * this;
	}

	vector32x3_c & vector32x3_c::operator *= ( vector32x3_c const & other )
	{
		a *= other.a;
		b *= other.b;
		c *= other.c;
		return * this;
	}

	vector32x3_c & vector32x3_c::operator /= ( vector32x3_c const & other )
	{
		a /= other.a;
		b /= other.b;
		c /= other.c;
		return * this;
	}

	vector64x3_c vector32x3_c::operator * ( float64_c other ) const
	{
		vector64x3_c result;
		result.a = static_cast< float64_c >( a ) * other;
		result.b = static_cast< float64_c >( b ) * other;
		result.c = static_cast< float64_c >( c ) * other;
		return result;
	}

	vector64x3_c vector32x3_c::operator * ( vector64x3_c const & other ) const
	{
		vector64x3_c result;
		result.a = static_cast< float64_c >( a ) * other.a;
		result.b = static_cast< float64_c >( b ) * other.b;
		result.c = static_cast< float64_c >( c ) * other.c;
		return result;
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

	boolean_c vector64x3_c::operator == ( vector64x3_c const & other ) const
	{
		return ( a == other.a ) && ( b == other.b ) && ( c == other.c );
	}

	boolean_c vector64x3_c::operator != ( vector64x3_c const & other ) const
	{
		return ( a != other.a ) || ( b != other.b ) || ( c != other.c );
	}

	vector64x3_c vector64x3_c::operator - () const
	{
		return vector64x3_c( -a, -b, -c );
	}

	vector64x3_c vector64x3_c::operator + ( float64_c other ) const
	{
		return vector64x3_c( a + other, b + other, c + other );
	}

	vector64x3_c vector64x3_c::operator - ( float64_c other ) const
	{
		return vector64x3_c( a - other, b - other, c - other );
	}

	vector64x3_c vector64x3_c::operator * ( float64_c other ) const
	{
		return vector64x3_c( a * other, b * other, c * other );
	}

	vector64x3_c vector64x3_c::operator / ( float64_c other ) const
	{
		return vector64x3_c( a / other, b / other, c / other );
	}

	vector64x3_c vector64x3_c::operator + ( vector64x3_c const & other ) const
	{
		return vector64x3_c( a + other.a, b + other.b, c + other.c );
	}

	vector64x3_c vector64x3_c::operator - ( vector64x3_c const & other ) const
	{
		return vector64x3_c( a - other.a, b - other.b, c - other.c );
	}

	vector64x3_c vector64x3_c::operator * ( vector64x3_c const & other ) const
	{
		return vector64x3_c( a * other.a, b * other.b, c * other.c );
	}

	vector64x3_c vector64x3_c::operator / ( vector64x3_c const & other ) const
	{
		return vector64x3_c( a / other.a, b / other.b, c / other.c );
	}
	
	vector64x3_c & vector64x3_c::operator *= ( float64_c const other )
	{
		a *= other;
		b *= other;
		c *= other;
		return * this;
	}

	vector64x3_c & vector64x3_c::operator /= ( float64_c const other )
	{
		a /= other;
		b /= other;
		c /= other;
		return * this;
	}

	vector64x3_c & vector64x3_c::operator += ( vector64x3_c const & other )
	{
		a += other.a;
		b += other.b;
		c += other.c;
		return * this;
	}

	vector64x3_c & vector64x3_c::operator -= ( vector64x3_c const & other )
	{
		a -= other.a;
		b -= other.b;
		c -= other.c;
		return * this;
	}

	vector64x3_c & vector64x3_c::operator *= ( vector64x3_c const & other )
	{
		a *= other.a;
		b *= other.b;
		c *= other.c;
		return * this;
	}

	vector64x3_c & vector64x3_c::operator /= ( vector64x3_c const & other )
	{
		a /= other.a;
		b /= other.b;
		c /= other.c;
		return * this;
	}

	vector64x3_c vector64x3_c::operator + ( vector32x3_c const & other ) const
	{
		return vector64x3_c( a + other.a, b + other.b, c + other.c );
	}

	vector64x3_c vector64x3_c::operator * ( float32_c other ) const
	{
		return vector64x3_c( a * other, b * other, c * other );
	}

	vector64x3_c vector64x3_c::operator * ( vector32x3_c const & other ) const
	{
		return vector64x3_c( a * other.a, b * other.b, c * other.c );
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

	//vector32x4_c::vector32x4_c( vector32x3_c const & abc, float32_c d )
	//	: a( abc.a )
	//	, b( abc.b )
	//	, c( abc.c )
	//	, d( d )
	//{
	//}

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

	boolean_c vector32x4_c::operator == ( vector32x4_c const & other ) const
	{
		return ( a == other.a ) && ( b == other.b ) && ( c == other.c ) && ( d == other.d );
	}

	boolean_c vector32x4_c::operator != ( vector32x4_c const & other ) const
	{
		return ( a != other.a ) || ( b != other.b ) || ( c != other.c ) || ( d != other.d );
	}

	vector32x4_c vector32x4_c::operator - () const
	{
		return vector32x4_c( -a, -b, -c, -d );
	}

	vector32x4_c vector32x4_c::operator + ( float32_c other ) const
	{
		return vector32x4_c( a + other, b + other, c + other, d + other );
	}

	vector32x4_c vector32x4_c::operator - ( float32_c other ) const
	{
		return vector32x4_c( a - other, b - other, c - other, d - other );
	}

	vector32x4_c vector32x4_c::operator * ( float32_c other ) const
	{
		return vector32x4_c( a * other, b * other, c * other, d * other );
	}

	vector32x4_c vector32x4_c::operator / ( float32_c other ) const
	{
		return vector32x4_c( a / other, b / other, c / other, d / other );
	}

	vector32x4_c vector32x4_c::operator + ( vector32x4_c const & other ) const
	{
		return vector32x4_c( a + other.a, b + other.b, c + other.c, d + other.d );
	}

	vector32x4_c vector32x4_c::operator - ( vector32x4_c const & other ) const
	{
		return vector32x4_c( a - other.a, b - other.b, c - other.c, d - other.d );
	}

	vector32x4_c vector32x4_c::operator * ( vector32x4_c const & other ) const
	{
		return vector32x4_c( a * other.a, b * other.b, c * other.c, d * other.d );
	}

	vector32x4_c vector32x4_c::operator / ( vector32x4_c const & other ) const
	{
		return vector32x4_c( a / other.a, b / other.b, c / other.c, d / other.d );
	}

	vector32x4_c & vector32x4_c::operator += ( float32_c const other )
	{
		a += other;
		b += other;
		c += other;
		d += other;
		return * this;
	}

	vector32x4_c & vector32x4_c::operator -= ( float32_c const other )
	{
		a -= other;
		b -= other;
		c -= other;
		d -= other;
		return * this;
	}

	vector32x4_c & vector32x4_c::operator *= ( float32_c const other )
	{
		a *= other;
		b *= other;
		c *= other;
		d *= other;
		return * this;
	}

	vector32x4_c & vector32x4_c::operator /= ( float32_c const other )
	{
		a /= other;
		b /= other;
		c /= other;
		d /= other;
		return * this;
	}

	vector32x4_c & vector32x4_c::operator += ( vector32x4_c const & other )
	{
		a += other.a;
		b += other.b;
		c += other.c;
		d += other.d;
		return * this;
	}

	vector32x4_c & vector32x4_c::operator -= ( vector32x4_c const & other )
	{
		a -= other.a;
		b -= other.b;
		c -= other.c;
		d -= other.d;
		return * this;
	}

	vector32x4_c & vector32x4_c::operator *= ( vector32x4_c const & other )
	{
		a *= other.a;
		b *= other.b;
		c *= other.c;
		d *= other.d;
		return * this;
	}

	vector32x4_c & vector32x4_c::operator /= ( vector32x4_c const & other )
	{
		a /= other.a;
		b /= other.b;
		c /= other.c;
		d /= other.d;
		return * this;
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

	boolean_c vector64x4_c::operator == ( vector64x4_c const & other ) const
	{
		return ( a == other.a ) && ( b == other.b ) && ( c == other.c ) && ( d == other.d );
	}

	boolean_c vector64x4_c::operator != ( vector64x4_c const & other ) const
	{
		return ( a != other.a ) || ( b != other.b ) || ( c != other.c ) || ( d != other.d );
	}

	vector64x4_c vector64x4_c::operator - () const
	{
		return vector64x4_c( -a, -b, -c, -d );
	}

	vector64x4_c vector64x4_c::operator + ( float64_c other ) const
	{
		return vector64x4_c( a + other, b + other, c + other, d + other );
	}

	vector64x4_c vector64x4_c::operator - ( float64_c other ) const
	{
		return vector64x4_c( a - other, b - other, c - other, d - other );
	}

	vector64x4_c vector64x4_c::operator * ( float64_c other ) const
	{
		return vector64x4_c( a * other, b * other, c * other, d * other );
	}

	vector64x4_c vector64x4_c::operator / ( float64_c other ) const
	{
		return vector64x4_c( a / other, b / other, c / other, d / other );
	}

	vector64x4_c vector64x4_c::operator + ( vector64x4_c const & other ) const
	{
		return vector64x4_c( a + other.a, b + other.b, c + other.c, d + other.d );
	}

	vector64x4_c vector64x4_c::operator - ( vector64x4_c const & other ) const
	{
		return vector64x4_c( a - other.a, b - other.b, c - other.c, d - other.d );
	}

	vector64x4_c vector64x4_c::operator * ( vector64x4_c const & other ) const
	{
		return vector64x4_c( a * other.a, b * other.b, c * other.c, d * other.d );
	}

	vector64x4_c vector64x4_c::operator / ( vector64x4_c const & other ) const
	{
		return vector64x4_c( a / other.a, b / other.b, c / other.c, d / other.d );
	}

	vector64x4_c & vector64x4_c::operator += ( float64_c const other )
	{
		a += other;
		b += other;
		c += other;
		d += other;
		return * this;
	}

	vector64x4_c & vector64x4_c::operator -= ( float64_c const other )
	{
		a -= other;
		b -= other;
		c -= other;
		d -= other;
		return * this;
	}

	vector64x4_c & vector64x4_c::operator *= ( float64_c const other )
	{
		a *= other;
		b *= other;
		c *= other;
		d *= other;
		return * this;
	}

	vector64x4_c & vector64x4_c::operator /= ( float64_c const other )
	{
		a /= other;
		b /= other;
		c /= other;
		d /= other;
		return * this;
	}

	vector64x4_c & vector64x4_c::operator += ( vector64x4_c const & other )
	{
		a += other.a;
		b += other.b;
		c += other.c;
		d += other.d;
		return * this;
	}

	vector64x4_c & vector64x4_c::operator -= ( vector64x4_c const & other )
	{
		a -= other.a;
		b -= other.b;
		c -= other.c;
		d -= other.d;
		return * this;
	}

	vector64x4_c & vector64x4_c::operator *= ( vector64x4_c const & other )
	{
		a *= other.a;
		b *= other.b;
		c *= other.c;
		d *= other.d;
		return * this;
	}

	vector64x4_c & vector64x4_c::operator /= ( vector64x4_c const & other )
	{
		a /= other.a;
		b /= other.b;
		c /= other.c;
		d /= other.d;
		return * this;
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

	matrix32x3x3_c matrix32x3x3_c::operator * ( matrix32x3x3_c const & other ) const
	{
		matrix32x3x3_c result;
		result.a.a = ( a.a * other.a.a ) + ( a.b * other.b.a ) + ( a.c * other.c.a );
		result.a.b = ( a.a * other.a.b ) + ( a.b * other.b.b ) + ( a.c * other.c.b );
		result.a.c = ( a.a * other.a.c ) + ( a.b * other.b.c ) + ( a.c * other.c.c );
		result.b.a = ( b.a * other.a.a ) + ( b.b * other.b.a ) + ( b.c * other.c.a );
		result.b.b = ( b.a * other.a.b ) + ( b.b * other.b.b ) + ( b.c * other.c.b );
		result.b.c = ( b.a * other.a.c ) + ( b.b * other.b.c ) + ( b.c * other.c.c );
		result.c.a = ( c.a * other.a.a ) + ( c.b * other.b.a ) + ( c.c * other.c.a );
		result.c.c = ( c.a * other.a.b ) + ( c.b * other.b.b ) + ( c.c * other.c.b );
		result.c.c = ( c.a * other.a.c ) + ( c.b * other.b.c ) + ( c.c * other.c.c );
		return result;
	}

	matrix32x3x3_c matrix32x3x3_c::operator *= ( float32_c scale )
	{
		a *= scale;
		b *= scale;
		c *= scale;
		return *this;
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

	boolean_c matrix32x4x4_c::operator == ( matrix32x4x4_c const & other ) const
	{
		return ( a == other.a ) && ( b == other.b ) && ( c == other.c ) && ( d == other.d );
	}

	boolean_c matrix32x4x4_c::operator != ( matrix32x4x4_c const & other ) const
	{
		return ( a != other.a ) || ( b != other.b ) || ( c != other.c ) || ( d != other.d );
	}

	matrix32x4x4_c matrix32x4x4_c::operator * ( matrix32x4x4_c const & other ) const
	{
		matrix32x4x4_c result;
		result.a.a = ( a.a * other.a.a ) + ( a.b * other.b.a ) + ( a.c * other.c.a ) + ( a.d * other.d.a );
		result.a.b = ( a.a * other.a.b ) + ( a.b * other.b.b ) + ( a.c * other.c.b ) + ( a.d * other.d.b );
		result.a.c = ( a.a * other.a.c ) + ( a.b * other.b.c ) + ( a.c * other.c.c ) + ( a.d * other.d.c );
		result.a.d = ( a.a * other.a.d ) + ( a.b * other.b.d ) + ( a.c * other.c.d ) + ( a.d * other.d.d );
		result.b.a = ( b.a * other.a.a ) + ( b.b * other.b.a ) + ( b.c * other.c.a ) + ( b.d * other.d.a );
		result.b.b = ( b.a * other.a.b ) + ( b.b * other.b.b ) + ( b.c * other.c.b ) + ( b.d * other.d.b );
		result.b.c = ( b.a * other.a.c ) + ( b.b * other.b.c ) + ( b.c * other.c.c ) + ( b.d * other.d.c );
		result.b.d = ( b.a * other.a.d ) + ( b.b * other.b.d ) + ( b.c * other.c.d ) + ( b.d * other.d.d );
		result.c.a = ( c.a * other.a.a ) + ( c.b * other.b.a ) + ( c.c * other.c.a ) + ( c.d * other.d.a );
		result.c.b = ( c.a * other.a.b ) + ( c.b * other.b.b ) + ( c.c * other.c.b ) + ( c.d * other.d.b );
		result.c.c = ( c.a * other.a.c ) + ( c.b * other.b.c ) + ( c.c * other.c.c ) + ( c.d * other.d.c );
		result.c.d = ( c.a * other.a.d ) + ( c.b * other.b.d ) + ( c.c * other.c.d ) + ( c.d * other.d.d );
		result.d.a = ( d.a * other.a.a ) + ( d.b * other.b.a ) + ( d.c * other.c.a ) + ( d.d * other.d.a );
		result.d.b = ( d.a * other.a.b ) + ( d.b * other.b.b ) + ( d.c * other.c.b ) + ( d.d * other.d.b );
		result.d.c = ( d.a * other.a.c ) + ( d.b * other.b.c ) + ( d.c * other.c.c ) + ( d.d * other.d.c );
		result.d.d = ( d.a * other.a.d ) + ( d.b * other.b.d ) + ( d.c * other.c.d ) + ( d.d * other.d.d );
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

	boolean_c matrix32x2x2_c::operator == ( matrix32x2x2_c const & other ) const
	{
		return ( a == other.a ) && ( b == other.b );
	}

	boolean_c matrix32x2x2_c::operator != ( matrix32x2x2_c const & other ) const
	{
		return ( a != other.a ) || ( b != other.b );
	}

	matrix32x2x2_c matrix32x2x2_c::operator * ( matrix32x2x2_c const & other ) const
	{
		return matrix32x2x2_c(
			( a.a * other.a.a ) + ( a.b * other.b.a ),
			( a.a * other.a.b ) + ( a.b * other.b.b ),
			( b.a * other.a.a ) + ( b.b * other.b.a ),
			( b.a * other.a.b ) + ( b.b * other.b.b ) );
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

	//quaternion32_c::quaternion32_c( quaternion64_c const & other )
	//	: a( static_cast< float32_c >( other.a ) )
	//	, b( static_cast< float32_c >( other.b ) )
	//	, c( static_cast< float32_c >( other.c ) )
	//	, d( static_cast< float32_c >( other.d ) )
	//{
	//}

	boolean_c quaternion32_c::operator == ( quaternion32_c const & other ) const
	{
		return ( a == other.a ) && ( b == other.b ) && ( c == other.c ) && ( d == other.d );
	}

	boolean_c quaternion32_c::operator != ( quaternion32_c const & other ) const
	{
		return ( a != other.a ) || ( b != other.b ) || ( c != other.c ) || ( d != other.d );
	}

	quaternion32_c quaternion32_c::operator + ( quaternion32_c const & other ) const
	{
		return quaternion32_c( a + other.a, b * other.b, c * other.c, d * other.d );
	}

	quaternion32_c quaternion32_c::operator * ( float32_c const other ) const
	{
		return quaternion32_c( a * other, b * other, c * other, d * other );
	}

	quaternion32_c quaternion32_c::operator * ( quaternion32_c const & other ) const
	{
		return quaternion32_c(
			( a * other.d ) + ( b * other.c ) - ( c * other.b ) + ( d * other.a ),
			( -a * other.c ) + ( b * other.d ) + ( c * other.a ) + ( d * other.b ),
			( a * other.b ) - ( b * other.a ) + ( c * other.d ) + ( d * other.c ),
			( -a * other.a ) - ( b * other.b ) - ( c * other.c ) + ( d * other.d ) );
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

	boolean_c plane32_c::operator == ( plane32_c const & other ) const
	{
		return ( a == other.a ) && ( b == other.b ) && ( c == other.c ) && ( d == other.d );
	}

	boolean_c plane32_c::operator != ( plane32_c const & other ) const
	{
		return ( a != other.a ) || ( b != other.b ) || ( c != other.c ) || ( d != other.d );
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

	boolean_c plane64_c::operator == ( plane64_c const & other ) const
	{
		return ( a == other.a ) && ( b == other.b ) && ( c == other.c ) && ( d == other.d );
	}

	boolean_c plane64_c::operator != ( plane64_c const & other ) const
	{
		return ( a != other.a ) || ( b != other.b ) || ( c != other.c ) || ( d != other.d );
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

	boolean_c box32x2_c::operator == ( box32x2_c const & other ) const
	{
		return ( minimum == other.minimum ) && ( maximum == other.maximum );
	}

	boolean_c box32x2_c::operator != ( box32x2_c const & other ) const
	{
		return ( minimum != other.minimum ) || ( maximum != other.maximum );
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

	boolean_c box64x2_c::operator == ( box64x2_c const & other ) const
	{
		return ( minimum == other.minimum ) && ( maximum == other.maximum );
	}

	boolean_c box64x2_c::operator != ( box64x2_c const & other ) const
	{
		return ( minimum != other.minimum ) || ( maximum != other.maximum );
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

	boolean_c box32x3_c::operator == ( box32x3_c const & other ) const
	{
		return ( minimum == other.minimum ) && ( maximum == other.maximum );
	}

	boolean_c box32x3_c::operator != ( box32x3_c const & other ) const
	{
		return ( minimum != other.minimum ) || ( maximum != other.maximum );
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

	boolean_c box64x3_c::operator == ( box64x3_c const & other ) const
	{
		return ( minimum == other.minimum ) && ( maximum == other.maximum );
	}

	boolean_c box64x3_c::operator != ( box64x3_c const & other ) const
	{
		return ( minimum != other.minimum ) || ( maximum != other.maximum );
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

	boolean_c space_transform_c::operator == ( space_transform_c const & other ) const
	{
		return ( position == other.position ) && ( rotation == other.rotation ) && ( scale == other.scale );
	}

	boolean_c space_transform_c::operator != ( space_transform_c const & other ) const
	{
		return ( position != other.position ) || ( rotation != other.rotation ) || ( scale != other.scale );
	}

	space_transform_c space_transform_c::operator * ( space_transform_c const & other ) const
	{
		space_transform_c result;
		result.rotation = rotation * other.rotation;
		result.scale = scale * other.scale;
		result.position = position + ops::make_vector64x3_rotated_vector( other.position, rotation ) * scale;
		return result;
	}

	space_transform_c & space_transform_c::operator *= ( space_transform_c const & other )
	{
		rotation = rotation * other.rotation;
		scale = scale * other.scale;
		position = position + ops::make_vector64x3_rotated_vector( other.position, rotation ) * scale;
		return *this;
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
