#pragma once

#include "cheonsa__types_vector64x3.h"

namespace cheonsa
{

	class segment64_c
	{
	public:
		vector64x3_c point_a;
		vector64x3_c point_b;

	public:
		inline segment64_c() : point_a(), point_b() {}
		inline segment64_c( vector64x3_c const & point_a, vector64x3_c const & point_b ) : point_a( point_a ), point_b( point_b ) {}
		inline segment64_c( float64_c const * values ) : point_a( values[ 0 ], values[ 1 ], values[ 2 ] ), point_b( values[ 3 ], values[ 4 ], values[ 5 ] ) {}

		inline float64_c * as_array() { return reinterpret_cast< float64_c * >( this ); }
		inline float64_c const * as_array() const { return reinterpret_cast< float64_c const * >( this ); }

	};

}
