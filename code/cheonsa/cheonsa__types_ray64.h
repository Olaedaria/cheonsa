#pragma once

#include "cheonsa__types_vector64x3.h"

namespace cheonsa
{

	class ray64_c
	{
	public:
		vector64x3_c position;
		vector64x3_c normal;

	public:
		inline ray64_c() : position(), normal() {}
		inline ray64_c( vector64x3_c const & position, vector64x3_c const & normal ) : position( position ), normal( normal ) {}
		inline ray64_c( float64_c const * values ) : position( values[ 0 ], values[ 1 ], values[ 2 ] ), normal( values[ 3 ], values[ 4 ], values[ 5 ] ) {}

		inline float64_c * as_array() { return reinterpret_cast< float64_c * >( this ); }
		inline float64_c const * as_array() const { return reinterpret_cast< float64_c const * >( this ); }

	};

}
