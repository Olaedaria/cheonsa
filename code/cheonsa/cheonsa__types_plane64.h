#pragma once

#include "cheonsa__types__fundamental.h"
#include "cheonsa__types_vector64x3.h"
#include "cheonsa__types_vector64x4.h"

namespace cheonsa
{

	// double precision 3D plane.
	// abc represents the plane normal, a unit length vector.
	// d represents the distance along the plane normal between the plane and the world origin point.
	// by convention, the normal points towards the "outside" or "topside" of the plane (in the case of a convex bounding volume, each normal will face outside of the volume), so the distance will be a negative value if the line is "above" the world origin.
	class plane64_c
	{
	public:
		float64_c a; // normal_a component. points top side.
		float64_c b; // normal_b component. points top side.
		float64_c c; // normal_c component. points top side.
		float64_c d; // distance component.

	public:
		explicit plane64_c();
		explicit plane64_c( float64_c const a, float64_c const b, float64_c const c, float64_c const d );
		explicit plane64_c( float64_c const * values );

		inline float64_c * as_float64_array() { return reinterpret_cast< float64_c * >( this ); }
		inline float64_c const * as_float64_array() const { return reinterpret_cast< float64_c const * >( this ); }

		inline vector64x4_c & as_vector64x4() { return *reinterpret_cast< vector64x4_c * >( this ); }
		inline vector64x4_c const & as_vector64x4() const { return *reinterpret_cast< vector64x4_c const * >( this ); }

		inline vector64x3_c & get_normal() { return *reinterpret_cast< vector64x3_c * >( this ); }
		inline vector64x3_c const & get_normal() const { return *reinterpret_cast< vector64x3_c const * >( this ); }

		inline float64_c & get_distance() { return d; }
		inline float64_c const get_distance() const { return d; }

		vector64x3_c get_point() const;
		void_c set_point( vector64x3_c const & value );

	};

	boolean_c operator == ( plane64_c const & a, plane64_c const & b );
	boolean_c operator != ( plane64_c const & a, plane64_c const & b );

}
