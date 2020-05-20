#pragma once

#include "cheonsa__types__fundamental.h"
#include "cheonsa__types_vector32x2.h"
#include "cheonsa__types_vector32x3.h"

namespace cheonsa
{

	// single precision 2D line.
	// ab represents the line normal (which is tangent to the line), a unit length vector.
	// c represents a distance along the line normal between the line and the world origin point.
	// by convention, the normal points towards the "outside" or "topside" (in the case of a polygon, each normal will face outside of the polygon), so the distance will be a negative value if the line is "above" the world origin.
	class line32_c
	{
	public:
		float32_c a; // normal_a component.
		float32_c b; // normal_b component.
		float32_c c; // distance component.

	public:
		explicit line32_c();
		explicit line32_c( float32_c a, float32_c b, float32_c c );
		explicit line32_c( float32_c const * value );

		inline float32_c * as_float32_array() { return reinterpret_cast< float32_c * >( this ); }
		inline float32_c const * as_float32_array() const { return reinterpret_cast< float32_c const * >( this ); }

		inline vector32x3_c & as_vector32x3() { return *reinterpret_cast< vector32x3_c * >( this ); }
		inline vector32x3_c const & as_vector32x3() const { return *reinterpret_cast< vector32x3_c const * >( this ); }

		inline vector32x2_c & get_normal() { return *reinterpret_cast< vector32x2_c * >( this ); }
		inline vector32x2_c const & get_normal() const { return *reinterpret_cast< vector32x2_c const * >( this ); }

		inline float32_c & get_distance() { return c; }
		inline float32_c const & get_distance() const { return c; }

		vector32x2_c get_point() const;
		void_c set_point( vector32x2_c const & value );

	};

}
