#pragma once

#include "cheonsa__types_vector64x2.h"

namespace cheonsa
{

	class circle64_c
	{
	public:
		vector64x2_c position;
		float64_c radius;

	public:
		inline circle64_c() : position(), radius( 0.0 ) {}
		inline circle64_c( vector64x2_c position, float64_c radius ) : position( position ), radius( radius ) {}
		inline circle64_c( float64_c const * values ) : position( values[ 0 ], values[ 1 ] ), radius( values[ 2 ] ) {}

		inline float64_c * as_array() { return reinterpret_cast< float64_c * >( this ); }
		inline float64_c const * as_array() const { return reinterpret_cast< float64_c const * >( this ); }

	};

}
