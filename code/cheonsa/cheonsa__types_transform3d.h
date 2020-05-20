#pragma once

#include "cheonsa__types_vector32x3.h"
#include "cheonsa__types_vector64x3.h"
#include "cheonsa__types_quaternion32.h"
#include "cheonsa__types_matrix32x3x3.h"

namespace cheonsa
{

	// double precision position, for undiscretized|unpartitioned open worlds.
	// single precision rotation quaternion.
	// single precision scale (independent axes|non-uniform).
	//
	// the axis orientation convention used for models, cameras, directional lights, and model bones:
	// +x axis points right.
	// +y axis points forward.
	// +z axis points up.
	//
	// the +y axis is the primary axis for cameras, directional lights, and model bones.
	//
	// these axes are used to construct the view matrix:
	// +y axis defines view forward, +z axis defines view up.
	// but the resulting view matrix is a typical right-handed view matrix, and so this convention does not apply, since +z axis goes towards the viewer, +x axis goes right, and +y axis goes up.
	//
	// for things that point (like bones, lights, and cameras), +z axis is forward, +x axis is right, and +y axis is down.
	// this scene transform class is uses 64-bit floats for position, and 32-bit floats for rotation and scale.
	// the larger position is supposed to allow for open world, and the smaller rotation and scale is to be more memory efficient.
	// if you are going to transform a few points or vectors, use make_transformed_point() and make_rotated_vector().
	// if you are going to transform a lot of points or vectors, then it is more cost efficient to calculate a matrix and use that, use make_basis_with_scale(), make_basis_without_scale(), or get_matrix32x4x4_relative_to_position().
	// the bullet physics engine however is designed to use a scalar type, that can be defined as either 32-bit or 64-bit float.
	// if we did not use third party physics, we could reduce duplication and waste, since the physics engine basically has to have its own copy of the scene.
	class transform3d_c
	{
	public:
		vector64x3_c position;
		quaternion32_c rotation;
		vector32x3_c scale; // need non-uniform scale for rare cases, for certain effects.

	public:
		transform3d_c();
		transform3d_c( vector64x3_c const & position, quaternion32_c const & rotation, float32_c uniform_scale );
		transform3d_c( vector64x3_c const & position, quaternion32_c const & rotation, vector32x3_c const & scale );

		void_c reset();

		matrix32x3x3_c get_scaled_basis() const; // basis axes.
		vector32x3_c get_scaled_basis_x() const; // x axis, right (with scale applied).
		vector32x3_c get_scaled_basis_y() const; // y axis, forward (with scale applied).
		vector32x3_c get_scaled_basis_z() const; // z axis, up (with scale applied).

		matrix32x3x3_c get_unscaled_basis() const; // basis axis normals.
		vector32x3_c get_unscaled_basis_x() const; // x axis, right (without scale applied|unit length|normalized).
		vector32x3_c get_unscaled_basis_y() const; // y axis, forward (without scale applied|unit length|normalized).
		vector32x3_c get_unscaled_basis_z() const; // z axis, up (without scale applied|unit length|normalized).

		void_c set_rotation_from_unscaled_basis( matrix32x3x3_c const & value ); // sets rotation quaternion from an unscaled (normalized) orthogonal basis 3x3 matrix.

		void_c set_uniform_scale( float32_c value );

		transform3d_c get_inverted() const; // makes an inverted copy of this instance.

		void_c invert(); // inverts this instance, such that original * inverted = identity.

		transform3d_c & operator = ( transform3d_c const & other );

		transform3d_c & operator *= ( transform3d_c const & other ); // multiplies this transform with other transform and assigns result to this transform.

	};

	boolean_c operator == ( transform3d_c const & a, transform3d_c const & b );
	boolean_c operator != ( transform3d_c const & a, transform3d_c const & b );
	transform3d_c operator * ( transform3d_c a, transform3d_c const & b ); // multiplies this transform with other transform.

}

