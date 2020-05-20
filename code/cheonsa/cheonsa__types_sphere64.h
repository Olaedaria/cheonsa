#pragma once

#include "cheonsa__types_vector64x3.h"

namespace cheonsa
{

	class sphere64_c
	{
	public:
		vector64x3_c position;
		float64_c radius;

	public:
		inline sphere64_c() : position(), radius( 0 ) {}
		inline sphere64_c( vector64x3_c const & position, float64_c const radius ) : position( position ), radius( radius ) {}
		inline sphere64_c( float64_c const * values ) : position( values[ 0 ], values[ 1 ], values[ 2 ] ), radius( values[ 3 ] ) {}

		inline float64_c * as_array() { return reinterpret_cast< float64_c * >( this ); }
		inline float64_c const * as_array() const { return reinterpret_cast< float64_c const * >( this ); }

	};

}
