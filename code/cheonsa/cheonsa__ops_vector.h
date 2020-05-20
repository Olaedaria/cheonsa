#pragma once

#include "cheonsa__types.h"

namespace cheonsa
{
	namespace ops
	{

		float32_c length_squared_float32( vector32x2_c const & vector );
		float64_c length_squared_float64( vector64x2_c const & vector );
		float32_c length_squared_float32( vector32x3_c const & vector );
		float64_c length_squared_float64( vector64x3_c const & vector );
		float32_c length_squared_float32( vector32x4_c const & vector );
		float64_c length_squared_float64( vector64x4_c const & vector );

		float32_c length_float32( vector32x2_c const & vector );
		float64_c length_float64( vector64x2_c const & vector );
		float32_c length_float32( vector32x3_c const & vector );
		float64_c length_float64( vector64x3_c const & vector );
		float32_c length_float32( vector32x4_c const & vector );
		float64_c length_float64( vector64x4_c const & vector );

		vector32x2_c normal_vector32x2( vector32x2_c const & vector ); // returns normalized copy of vector, but if vector's length is close to 0 then it will just return a copy of the vector as is.
		vector64x2_c normal_vector64x2( vector64x2_c const & vector ); // returns normalized copy of vector, but if vector's length is close to 0 then it will just return a copy of the vector as is.
		vector32x3_c normal_vector32x3( vector32x3_c const & vector ); // returns normalized copy of vector, but if vector's length is close to 0 then it will just return a copy of the vector as is.
		vector64x3_c normal_vector64x3( vector64x3_c const & vector ); // returns normalized copy of vector, but if vector's length is close to 0 then it will just return a copy of the vector as is.
		vector32x4_c normal_vector32x4( vector32x4_c const & vector ); // returns normalized copy of vector, but if vector's length is close to 0 then it will just return a copy of the vector as is.
		vector64x4_c normal_vector64x4( vector64x4_c const & vector ); // returns normalized copy of vector, but if vector's length is close to 0 then it will just return a copy of the vector as is.
		quaternion32_c normal_quaternion32( quaternion32_c const & quaternion );
		void_c normalize_quaternion32( quaternion32_c & quaternion );

		float32_c dot_product_float32( vector32x2_c const & vector_a, vector32x2_c const & vector_b );
		float64_c dot_product_float64( vector64x2_c const & vector_a, vector64x2_c const & vector_b );
		float32_c dot_product_float32( vector32x3_c const & vector_a, vector32x3_c const & vector_b );
		float64_c dot_product_float64( vector64x3_c const & vector_a, vector64x3_c const & vector_b );
		float32_c dot_product_float32( vector32x4_c const & vector_a, vector32x4_c const & vector_b );
		float64_c dot_product_float64( vector64x4_c const & vector_a, vector64x4_c const & vector_b );
		float32_c dot_product_float32( quaternion32_c const & quaternion_a, quaternion32_c const & quaternion_b );

		vector32x3_c cross_product_vector32x3( vector32x3_c const & vector_a, vector32x3_c const & vector_b );
		vector64x3_c cross_product_vector64x3( vector64x3_c const & vector_a, vector64x3_c const & vector_b );

		vector32x2_c orthogonalized_vector32x2( vector32x2_c const & vector ); // kind of analogous to 3D cross product, creates a tangent vector that points right of input vector.
		vector64x2_c orthogonalized_vector64x2( vector64x2_c const & vector ); // kind of analogous to 3D cross product, creates a tangent vector that points right of input vector.

		vector32x2_c orthonormalized_vector32x2( vector32x2_c const & vector, boolean_c polarity, boolean_c allow_zero );
		vector64x2_c orthonormalized_vector64x2( vector64x2_c const & vector, boolean_c polarity, boolean_c allow_zero );

		vector32x3_c rotate_vector32x3( vector32x3_c const & vector, quaternion32_c const & rotation ); // 16 multiplications, 6 additions, 6 subtractions.
		vector64x3_c rotate_vector64x3( vector64x3_c const & vector, quaternion32_c const & rotation ); // 16 multiplications, 6 additions, 6 subtractions.

		vector32x2_c rotate_vector32x2( vector32x2_c const & point, float32_c const angle );
		vector64x2_c rotate_vector64x2( vector64x2_c const & point, float64_c const angle );
		
		vector32x3_c rotate_vector32x3( vector32x3_c const & point, vector32x3_c const & axis, float32_c const angle );
		vector64x3_c rotate_vector64x3( vector64x3_c const & point, vector64x3_c const & axis, float64_c const angle );

		vector32x2_c rotate_and_scale_vector32x2( vector32x2_c const & point, matrix32x2x2_c const & transform );

		vector32x3_c rotate_and_scale_vector32x3( vector32x3_c const & point, matrix32x3x3_c const & transform ); // 9 multiplications, 6 additions.
		vector64x3_c rotate_and_scale_vector64x3( vector64x3_c const & point, matrix32x3x3_c const & transform ); // 9 multiplications, 6 additions.

		vector32x3_c rotate_and_scale_vector32x3( vector32x3_c const & vector, matrix32x4x4_c const & transform ); // 9 multiplications, 6 additions. applies rotation and scale, disregards position|translation.
		vector64x3_c rotate_and_scale_vector64x3( vector64x3_c const & vector, matrix32x4x4_c const & transform ); // 9 multiplications, 6 additions. applies rotation and scale, disregards position|translation.

		vector32x3_c rotate_scale_and_translate_vector32x3( vector32x3_c const & point, matrix32x4x4_c const & transform ); // 9 multiplications, 9 additions. same as the mul() hlsl gpu shader function.
		vector64x3_c rotate_scale_and_translate_vector64x3( vector64x3_c const & point, matrix32x4x4_c const & transform ); // 9 multiplications, 9 additions. same as the mul() hlsl gpu shader function.

		vector32x4_c rotate_scale_and_translate_vector32x4( vector32x3_c const & point, matrix32x4x4_c const & transform ); // 12 multiplications, 12 additions. same as the mul() hlsl gpu shader function.

		vector64x3_c rotate_scale_and_translate_vector64x3( vector64x3_c const & point, transform3d_c const & transform ); // applies rotation, scale, and translation (in that order).

		vector32x3_c constrained_normal_vector32x3( vector32x3_c const & normal, vector32x3_c const & normal_target, float32_c const maximum_angle );
		vector64x3_c constrained_normal_vector64x3( vector64x3_c const & normal, vector64x3_c const & normal_target, float64_c const maximum_angle );

		vector64x3_c reflected_vector64x3( vector64x3_c const & direction_vector, vector64x3_c const & normal_vector ); // reflects vector across imaginary plane with surface normal defined by normal_vector.
		vector64x3_c reflected_vector64x3( vector64x3_c const & point, plane64_c const & plane ); // finds the point that is opposite of plane.

		vector64x3_c parallel_component_vector64x3( vector64x3_c const & normal_vector, vector64x3_c const & other_vector ); // returns a vector that is parallel to normal_vector and is as long as other_vector is in agreement with normal_vector.
		vector64x3_c perpendicular_component_vector64x3( vector64x3_c const & normal_vector, vector64x3_c const & other_vector ); // returns a vector that is perpendicular to normal_vector and is as long as other_vector is in disagreement with normal_vector.

		vector32x3_c parallel_component_vector32x3( vector32x3_c const & normal_vector, vector32x3_c const & other_vector );
		vector32x3_c perpendicular_component_vector32x3( vector32x3_c const & normal_vector, vector32x3_c const & other_vector );

		vector32x3_c basis_vector32x3_x_from_rotation_quaternion32( quaternion32_c const & rotation ); // x axis.
		vector32x3_c basis_vector32x3_y_from_rotation_quaternion32( quaternion32_c const & rotation ); // y axis.
		vector32x3_c basis_vector32x3_z_from_rotation_quaternion32( quaternion32_c const & rotation ); // z axis.

		matrix32x4x4_c transform_matrix32x4x4_c_from_transform3d( transform3d_c const & transform );
		matrix32x4x4_c transform_matrix32x4x4_c_from_transform3d( transform3d_c const & transform, vector64x3_c const & relative_to_origin );

		// creates a basis matrix from a rotation quaternion.
		// just sets the basis elements (3x3), everything else is left as identity.
		matrix32x4x4_c basis_matrix32x4x4_from_rotation_quaternion32( quaternion32_c const & rotation );

		// creates a basis matrix from a rotation quaternion.
		matrix32x3x3_c basis_matrix32x3x3_from_rotation_quaternion32( quaternion32_c const & rotation );
		// basis should be right-handed, normalized, and orthogonal.
		quaternion32_c rotation_quaternion32_from_basis_matrix32x3x3( matrix32x3x3_c const & basis );

		// creates a basis matrix from a rotation quaternion.
		// axis_angle is the normalized axis vector in abc and the angle in d in radians.
		matrix32x3x3_c basis_matrix32x3x3_from_axis_angle( vector32x4_c const & axis_angle );

		// rotates (like a socket joint) an existing basis matrix so that its y axis aligns with the provided basis_y_target.
		// basis is the original basis matrix that we want to recalculate.
		// basis_y_target is the new orientation for the y axis, it must already be normalized.
		matrix32x3x3_c joint_rotate_basis_matrix32x3x3_y( matrix32x3x3_c basis, vector32x3_c const & basis_y_target );

		// rotates (like a socket joint) an existing basis matrix so that its z axis aligns with the provided basis_z_target.
		// basis_z_target is the new orientation for the z axis, it must already be normalized.
		matrix32x3x3_c joint_rotate_basis_matrix32x3x3_z( matrix32x3x3_c basis, vector32x3_c const & basis_z_target );

		matrix32x3x3_c transpose_matrix32x3x3( matrix32x3x3_c const & matrix );

		// constructs a right-handed view matrix.
		// internally, the view matrix has to use a different convention when compared to the camera's world space transform matrix.
		// this is because the gpu rasterizer is hard wired to use the view matrix to convert world space coordinates to view space coordinates in a certain way.
		// the internal view matrix convention is that z-axis points backward, x-axis points right, and y-axis points up.
		// cheonsa's camera convention is that y-axis points forward, z-axis points up, and x-axis points right,
		// cheonsa's 2d screen space convention is that y-axis points down, and x-axis points right.
		// but, none of this is very important since the view matrix is like internal use only.
		matrix32x4x4_c view_matrix32x4x4_from_look_at( vector32x3_c const & position, vector32x3_c const & forward, vector32x3_c const & up );

		// width is the width of projection frustum (box) volume in meters.
		// height is the height of projection frustum (box) volume in meters.
		// clip_near is the distance in meters along the central forward axis to the near clip plane.
		// clip_far is the distance in meters to the central forward axis far clip plane. this value should be greater than clip_near.
		matrix32x4x4_c projection_orthographic_matrix32x4x4( float32_c const width, float32_c const height, float32_c const clip_near, float32_c const clip_far );

		// left is distance from central forward axis to left side of projection frustum (box) volume in meters. this value should be less than right.
		// right is distance from central forward axis to right side of projection frustum (box) volume in meters. this value should be greater than left.
		// top is distance from central forward axis to top side of projection frustum (box) volume in meters. this value should be less than bottom.
		// bottom is distance from central forward axis to bottom side of projection frustum (box) volume in meters. this value should be greater than top.
		// clip_near is the distance in meters along the central forward axis to the near clip plane.
		// clip_far is the distance in meters to the central forward axis far clip plane. this value should be greater than clip_near.
		matrix32x4x4_c projection_orthographic_off_center_matrix32x4x4_a( float32_c const left, float32_c right, float32_c const top, float32_c const bottom, float32_c const clip_near, float32_c const clip_far ); // note that in right-handed projection, left < right, bottom < top.

		// width is the width of the projection frustum (box) volume in meters.
		// height is the height of the projection frustum (box) volume in meters.
		// center_x is a unit percentage which defines the point in view space that will be the "center". a value if -1 is along the left edge, a value of 1 is along the right edge.
		// center_y is a unit percentage which defines the point in view space that will be the "center". a value of -1 is along the top edge, a value of 1 is along the bottom edge.
		// clip_near is the distance in meters along the central forward axis to the near clip plane.
		// clip_far is the distance in meters to the central forward axis far clip plane. this value should be greater than clip_near.
		matrix32x4x4_c projection_orthographic_off_center_matrix32x4x4_b( float32_c const width, float32_c const height, float32_c const center_x, float32_c const center_y, float32_c clip_near, float32_c clip_far );

		// field_of_view is vertical field of view angle in radians.
		// aspect_ratio is the horizontal width to vertical height ratio. this is typically the render target width divided by the render target height. this is used by the function to calculate the the horizontal field of view angle in radians.
		// clip_near is the distance in meters along the central forward axis to the near clip plane. this value should be greater than 0 and as large as is reasonable given your scene's design.
		// clip_far is the distance in meters to the central forward axis far clip plane. this value should be greater than clip_near.
		matrix32x4x4_c projection_perspective_matrix32x4x4( float32_c const field_of_view, float32_c const aspect_ratio, float32_c const clip_near, float32_c const clip_far );

		// left is the minimum x value of view volume. this value should be less than right. a default value is -1.
		// right is the maximum x value of the view volume. this value should be greater than left. a default value is 1.
		// top is the minimum y value of the view volume. this value should be less than bottom. a default value is -1.
		// bottom is the maximum y value of the view volume. this value should be greater than top. a default value is 1.
		// clip_near is the distance in meters along the central forward axis to the near clip plane. this value should be greater than 0 and as large as is reasonable given your scene's design.
		// clip_far is the distance in meters to the central forward axis far clip plane. this value should be greater than clip_near.
		matrix32x4x4_c projection_perspective_off_center_matrix32x4x4_a( float32_c const left, float32_c const right, float32_c const top, float32_c const bottom, float32_c const clip_near, float32_c const clip_far ); // note that in right-handed projection, left < right, bottom < top. left, top, right, and bottom are units of distance over the near clip plane, so these values should be scaled with the near clip plane distance (the closer the clip plane the smaller the values, and the farther the clip plane the larger the values).

		// field_of_view is the vertical field of view in radians.
		// aspect_ratio is the horizontal width to vertical height ratio. this is typically the render target width divided by the render target height.
		// center_x defines the point in view space which will be the off-center center of the resulting projection matrix. a value of -1 is along the left edge, a value of 1 is along the right edge.
		// center_y defines the point in view space which will be the off-center center of the resulting projection matrix. a value of -1 is along the top edge, a value of 1 is along the bottom edge.
		// clip_near is the distance in meters along the central forward axis to the near clip plane. this value should be greater than 0 and as large as is reasonable given your scene's design.
		// clip_far is the distance in meters to the central forward axis far clip plane. this value should be greater than clip_near.
		matrix32x4x4_c projection_perspective_off_center_matrix32x4x4_b( float32_c const field_of_view, float32_c const aspect_ratio, float32_c center_x, float32_c center_y, float32_c clip_near, float32_c clip_far );

		matrix32x4x4_c transpose_matrix32x4x4( matrix32x4x4_c const & matrix );

		matrix32x4x4_c invert_matrix32x4x4_fast( matrix32x4x4_c const & matrix ); // can be used to convert a world space transform to a view transform and vice versa. foregoes doing determinant calculation and all that, which is the formal mathematical way a matrix inversion would be done, so it's not a proper invert, but for what we do in scenes this is fine.

		matrix32x2x2_c invert_matrix32x2x2( matrix32x2x2_c const & matrix );

		matrix32x2x2_c basis_matrix32x2x2_from_angle_scale( float32_c const angle, float32_c const scale );

		quaternion32_c invert_quaternion32( quaternion32_c const & quaternion ); // creates an inverted form of quaternion, such that invert_quaternion32( quaternion ) * quaternion equals the identity quaternion.

		quaternion32_c rotation_quaternion32_from_axis_angle( vector32x4_c const & axis_angle ); // axis_angle.abc is normalized unit length axis, axis_angle.d is angle around axis in radians (IDK if it's clockwise or coutnerclockwise).
		quaternion32_c rotation_quaternion32_from_look_at2( vector32x3_c const & forward, vector32x3_c const & up );

		vector32x4_c axis_angle_from_rotation_quaternion32( quaternion32_c const & rotation ); // rotation is assumed to be normalized unit length quaternion.
		quaternion32_c rotation_quaternion32_from_euler_angles( vector32x3_c const & euler_angles ); // euler_angles are angles in radians for each axis, rotations are applied in the order of a, b, then c.
		vector32x3_c euler_angles_from_rotation_quaternion32( quaternion32_c const & rotation ); // euler angles are applied in the following order: x, y, z.
		//quaternion32_c make_rotation_quaternion32_from_look_at( vector32x3_c const & z_forward, vector32x3_c const & x_right ); // inputs should be pre-normalized.


	}
}
