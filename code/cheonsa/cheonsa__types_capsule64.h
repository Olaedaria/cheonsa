#pragma once

#include "cheonsa__types_vector64x3.h"

namespace cheonsa
{

	class capsule64_c
	{
	public:
		vector64x3_c point_a;
		float64_c radius_a;
		vector64x3_c point_b;
		float64_c radius_b;

	public:
		inline capsule64_c() : point_a(), radius_a( 0 ), point_b(), radius_b( 0 ) {}
		inline capsule64_c( vector64x3_c const & point_a, float64_c const radius_a, vector64x3_c const & point_b, float64_c const radius_b ) : point_a( point_a ), radius_a( radius_a ), point_b( point_b ), radius_b( radius_b ) {}
		inline capsule64_c( float64_c const * values ) : point_a( values[ 0 ], values[ 1 ], values[ 2 ] ), radius_a( values[ 3 ] ), point_b( values[ 4 ], values[ 5 ], values[ 6 ] ), radius_b( values[ 7 ] ) {}

		inline float64_c * as_array() { return reinterpret_cast< float64_c * >( this ); }
		inline float64_c const * as_array() const { return reinterpret_cast< float64_c const * >( this ); }

	};

}
