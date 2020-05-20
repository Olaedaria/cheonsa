#pragma once

#include "cheonsa__types__fundamental.h"
#include "cheonsa__types_vector32x3.h"
#include "cheonsa__types_vector32x4.h"

namespace cheonsa
{

	// single precision 3D plane.
	// abc represents the plane normal (which is tangent to the plane), a unit length vector.
	// d represents the distance along the plane normal between the plane and the world origin point.
	// by convention, the normal points towards the "outside" or "topside" of the plane (in the case of a convex bounding volume, each normal will face outside of the volume), so the distance will be a negative value if the line is "above" the world origin.
	class plane32_c
	{
	public:
		float32_c a; // normal_a component.
		float32_c b; // normal_b component.
		float32_c c; // normal_c component.
		float32_c d; // distance component.

	public:
		explicit plane32_c();
		explicit plane32_c( float32_c const a, float32_c const b, float32_c const c, float32_c const d );
		explicit plane32_c( float32_c const * values );

		inline float32_c * as_float32_array() { return reinterpret_cast< float32_c * >( this ); }
		inline float32_c const * as_float32_array() const { return reinterpret_cast< float32_c const * >( this ); }

		inline vector32x4_c & as_vector32x4() { return * reinterpret_cast< vector32x4_c * >( this ); }
		inline vector32x4_c const & as_vector32x4() const { return * reinterpret_cast< vector32x4_c const * >( this ); }

		inline vector32x3_c & get_normal() { return * reinterpret_cast< vector32x3_c * >( this ); }
		inline vector32x3_c const & get_normal() const { return * reinterpret_cast< vector32x3_c const * >( this ); }

		inline float32_c & get_distance() { return d; }
		inline float32_c const get_distance() const { return d; }

		vector32x3_c get_point() const;
		void_c set_point( vector32x3_c const & value );

	};

	boolean_c operator == ( plane32_c const & a, plane32_c const & b );
	boolean_c operator != ( plane32_c const & a, plane32_c const & b );

}
