#include "cheonsa__types_matrix32x2x2.h"
#include "cheonsa__types_vector32x4.h"

namespace cheonsa
{

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

	vector32x4_c const & matrix32x2x2_c::as_vector32x4() const
	{
		static_assert( sizeof( vector32x4_c ) == sizeof( matrix32x2x2_c ), "just checking." );
		return *reinterpret_cast< vector32x4_c const * >( this );
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

}
