#include "cheonsa__types_matrix32x4x4.h"
#include <cassert>

namespace cheonsa
{

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

}
