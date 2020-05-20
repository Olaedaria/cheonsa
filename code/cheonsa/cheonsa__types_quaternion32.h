#pragma once

#include "cheonsa__types__fundamental.h"

namespace cheonsa
{

	// single precision quaternion, used to rotate 3d points and vectors.
	class quaternion32_c
	{
	public:
		float32_c a;
		float32_c b;
		float32_c c;
		float32_c d;

	public:
		explicit quaternion32_c();
		explicit quaternion32_c( float32_c const a, float32_c const b, float32_c const c, float32_c const d );
		explicit quaternion32_c( float32_c const * values );

		inline float32_c * as_array() { return reinterpret_cast< float32_c * >( this ); }
		inline float32_c const * as_array() const { return reinterpret_cast< float32_c const * >( this ); }

		// used when saving quaternions to files, since quaternions are always unit length, we can save just the first three elements, and upon load we can calculate the fourth element.
		float32_c const * get_abc() const { return reinterpret_cast< float32_c const * >( this ); }
		// used when loading quaternions from files, since quaternions are always unit length, we can save just the first three elements, and upon load we can calculate the fourth element.
		void_c set_abc_calculate_d( float32_c const * abc );
		void_c calculate_d_from_abc();

		quaternion32_c & operator *= ( quaternion32_c const & other );

	};

	boolean_c operator == ( quaternion32_c const & a, quaternion32_c const & b );
	boolean_c operator != ( quaternion32_c const & a, quaternion32_c const & b );

	quaternion32_c operator + ( quaternion32_c const & a, quaternion32_c const & b );

	quaternion32_c operator * ( quaternion32_c const & a, float32_c const b );
	quaternion32_c operator * ( quaternion32_c const & a, quaternion32_c const & b );

}
