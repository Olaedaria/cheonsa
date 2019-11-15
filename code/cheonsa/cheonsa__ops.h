#pragma once

#include "cheonsa___build.h"
#include "cheonsa__types.h"
#include "cheonsa_data_types.h"
#include "cheonsa_string8.h"
#include "cheonsa_string16.h"

namespace cheonsa
{
	namespace ops
	{


		//
		//
		// basic numeric operations.
		//
		//

		boolean_c math_is_real( float32_c a ); // returns true if the a is a real number, meaning that it is not NaN or infinity.
		boolean_c math_is_real( float64_c a );
		boolean_c math_compare_close( float32_c a, float32_c b, float32_c margin );
		boolean_c math_compare_close( float64_c a, float64_c b, float64_c margin );
		sint32_c math_minimum( sint32_c a, sint32_c b );
		float32_c math_minimum( float32_c a, float32_c b );
		float64_c math_minimum( float64_c a, float64_c b );
		sint32_c math_maximum( sint32_c a, sint32_c b );
		float32_c math_maximum( float32_c a, float32_c b );
		float64_c math_maximum( float64_c a, float64_c b );
		sint32_c math_absolute_value( sint32_c a );
		float32_c math_absolute_value( float32_c a );
		float64_c math_absolute_value( float64_c a );
		sint16_c math_clamp( sint16_c value, sint16_c minimum, sint16_c maximum );
		sint32_c math_clamp( sint32_c value, sint32_c minimum, sint32_c maximum );
		float32_c math_clamp( float32_c value, float32_c minimum, float32_c maximum );
		float64_c math_clamp( float64_c value, float64_c minimum, float64_c maximum );
		sint32_c math_next_power_of_two( sint32_c a );
		uint32_c math_next_power_of_two( uint32_c a );
		float32_c math_saturate( float32_c value );
		float64_c math_saturate( float64_c value );
		float32_c math_nearest_multiple( float32_c value, float32_c multiple );
		float64_c math_nearest_multiple( float64_c value, float64_c multiple );
		sint32_c math_next_nearest_multiple( sint32_c value, sint32_c multiple );
		float32_c math_round( float32_c value );
		float64_c math_round( float64_c value );
		float32_c math_round_down( float32_c value );
		float64_c math_round_down( float64_c value );
		float32_c math_round_up( float32_c value );
		float64_c math_round_up( float64_c value );
		float32_c math_round_to_decimal_place( float32_c value, sint32_c places );
		float64_c math_round_to_decimal_place( float64_c value, sint32_c places );
		float32_c math_remainder( float32_c value, float32_c multiple );
		float64_c math_remainder( float64_c value, float64_c multiple );
		float32_c math_natural_logarithm( float32_c value );
		float32_c math_square( float32_c value );
		float64_c math_square( float64_c value );
		float32_c math_cube( float32_c value );
		float64_c math_cube( float64_c value );
		sint32_c math_power( sint32_c value, sint32_c exponent );
		float32_c math_power( float32_c value, float32_c exponent );
		float64_c math_power( float64_c value, float64_c exponent );
		float32_c math_root( float32_c value, float32_c exponent );
		float64_c math_root( float64_c value, float64_c exponent );
		sint32_c math_square_root( sint32_c value );
		float32_c math_square_root( float32_c value );
		float64_c math_square_root( float64_c value );
		float32_c math_sine( float32_c angle );
		float64_c math_sine( float64_c angle );
		float32_c math_arc_sine( float32_c dot );
		float64_c math_arc_sine( float64_c dot );
		float32_c math_cosine( float32_c angle );
		float64_c math_cosine( float64_c angle );
		float32_c math_arc_cosine( float32_c dot );
		float64_c math_arc_cosine( float64_c dot );
		float32_c math_tangent( float32_c angle );
		float64_c math_tangent( float64_c angle );
		float32_c math_arc_tangent( float32_c dot );
		float64_c math_arc_tangent( float64_c dot );
		float32_c math_arc_tangent( float32_c b, float32_c a );
		float64_c math_arc_tangent( float64_c b, float64_c a );
		float32_c math_wrap( float32_c value, float32_c range ); // wraps a value between 0 and range. range can be set to 2*pi for radians, or 360 for degrees, or something else for something else.
		float64_c math_wrap( float64_c value, float64_c range );
		float32_c math_find_shortest_angle_difference( float32_c angle_a, float32_c angle_b ); // finds the shortest radian difference between two angles, such that angle_a + difference is equiavlent to angle_b, but angle_a + difference may end up lying outside of >= 0 and < pi * 2.0 range.
		float64_c math_find_shortest_angle_difference( float64_c angle_a, float64_c angle_b );
		float32_c math_find_angle_around_axis( vector32x3_c const & tangent_to_axis_that_defines_angle_zero, vector32x3_c const & tangent_to_axis_that_defines_angle_quarter, vector32x3_c const & tangent_to_find_angle_of ); // returns a result in the range -pi to pi. tangent_to_axis_that_defines_angle_zero can be thought of as the forward vector. tangent_to_axis_that_defines_angle_quarter can be thought of as the right vector and positive angle space. the cross product of tangent_to_axis_that_defines_angle_zero and tangent_to_axis_that_defines_angle_quarter would be the axis itself.
		float64_c math_find_angle_around_axis( vector64x3_c const & tangent_to_axis_that_defines_angle_zero, vector64x3_c const & tangent_to_axis_that_defines_angle_quarter, vector64x3_c const & tangent_to_find_angle_of ); // returns a result in the range -pi to pi. tangent_to_axis_that_defines_angle_zero can be thought of as the forward vector. tangent_to_axis_that_defines_angle_quarter can be thought of as the right vector and positive angle space. the cross product of tangent_to_axis_that_defines_angle_zero and tangent_to_axis_that_defines_angle_quarter would be the axis itself.
		float64_c math_wobble( float64_c time, float64_c frequency );
		sint32_c math_sign( float32_c value ); // returns 1 for value > 0, 0 for value == 0, and -1 for value < 0.
		sint32_c math_sign( float64_c value ); // returns 1 for value > 0, 0 for value == 0, and -1 for value < 0.
		sint32_c math_non_zero_sign( float32_c value ); // returns 1 for value > 0, and -1 for value <= 0. 
		sint32_c math_non_zero_sign( float64_c value ); // returns 1 for value > 0, and -1 for value <= 0.




		//
		//
		// vector operations.
		//
		//

		float32_c make_float32_length_squared( vector32x2_c const & vector );
		float64_c make_float64_length_squared( vector64x2_c const & vector );
		float32_c make_float32_length_squared( vector32x3_c const & vector );
		float64_c make_float64_length_squared( vector64x3_c const & vector );
		float32_c make_float32_length_squared( vector32x4_c const & vector );
		float64_c make_float64_length_squared( vector64x4_c const & vector );

		float32_c make_float32_length( vector32x2_c const & vector );
		float64_c make_float64_length( vector64x2_c const & vector );
		float32_c make_float32_length( vector32x3_c const & vector );
		float64_c make_float64_length( vector64x3_c const & vector );
		float32_c make_float32_length( vector32x4_c const & vector );
		float64_c make_float64_length( vector64x4_c const & vector );

		vector32x2_c make_vector32x2_normalized( vector32x2_c const & vector ); // returns normalized copy of vector, but if vector's length is close to 0 then it will just return a copy of the vector as is.
		vector64x2_c make_vector64x2_normalized( vector64x2_c const & vector ); // returns normalized copy of vector, but if vector's length is close to 0 then it will just return a copy of the vector as is.
		vector32x3_c make_vector32x3_normalized( vector32x3_c const & vector ); // returns normalized copy of vector, but if vector's length is close to 0 then it will just return a copy of the vector as is.
		vector64x3_c make_vector64x3_normalized( vector64x3_c const & vector ); // returns normalized copy of vector, but if vector's length is close to 0 then it will just return a copy of the vector as is.
		vector32x4_c make_vector32x4_normalized( vector32x4_c const & vector ); // returns normalized copy of vector, but if vector's length is close to 0 then it will just return a copy of the vector as is.
		vector64x4_c make_vector64x4_normalized( vector64x4_c const & vector ); // returns normalized copy of vector, but if vector's length is close to 0 then it will just return a copy of the vector as is.
		quaternion32_c make_quaternion32_normalized( quaternion32_c const & quaternion );

		float32_c make_float32_dot_product( vector32x2_c const & vector_a, vector32x2_c const & vector_b );
		float64_c make_float64_dot_product( vector64x2_c const & vector_a, vector64x2_c const & vector_b );
		float32_c make_float32_dot_product( vector32x3_c const & vector_a, vector32x3_c const & vector_b );
		float64_c make_float64_dot_product( vector64x3_c const & vector_a, vector64x3_c const & vector_b );
		float32_c make_float32_dot_product( vector32x4_c const & vector_a, vector32x4_c const & vector_b );
		float64_c make_float64_dot_product( vector64x4_c const & vector_a, vector64x4_c const & vector_b );
		float32_c make_float32_dot_product( quaternion32_c const & quaternion_a, quaternion32_c const & quaternion_b );

		vector32x3_c make_vector32x3_cross_product( vector32x3_c const & vector_a, vector32x3_c const & vector_b );
		vector64x3_c make_vector64x3_cross_product( vector64x3_c const & vector_a, vector64x3_c const & vector_b );

		vector32x2_c make_vector32x2_orthogonal( vector32x2_c const & vector ); // kind of analogous to 3D cross product, creates a tangent vector that points right of input vector.
		vector64x2_c make_vector64x2_orthogonal( vector64x2_c const & vector ); // kind of analogous to 3D cross product, creates a tangent vector that points right of input vector.

		vector32x2_c make_vector32x2_orthonormal( vector32x2_c const & vector, boolean_c polarity, boolean_c allow_zero );
		vector64x2_c make_vector64x2_orthonormal( vector64x2_c const & vector, boolean_c polarity, boolean_c allow_zero );

		vector32x3_c make_vector32x3_rotated_vector( vector32x3_c const & vector, quaternion32_c const & rotation ); // 16 multiplications, 6 additions, 6 subtractions.
		vector64x3_c make_vector64x3_rotated_vector( vector64x3_c const & vector, quaternion32_c const & rotation ); // 16 multiplications, 6 additions, 6 subtractions.

		vector32x2_c make_vector32x2_rotated_vector( vector32x2_c const & point, float32_c const angle );
		vector64x2_c make_vector64x2_rotated_vector( vector64x2_c const & point, float64_c const angle );
		
		vector32x3_c make_vector32x3_rotated_vector( vector32x3_c const & point, vector32x3_c const & axis, float32_c const angle );
		vector64x3_c make_vector64x3_rotated_vector( vector64x3_c const & point, vector64x3_c const & axis, float64_c const angle );

		vector32x2_c make_vector32x2_transformed_point( vector32x2_c const & point, matrix32x2x2_c const & transform );

		vector32x3_c make_vector32x3_transformed_point( vector32x3_c const & point, matrix32x3x3_c const & transform ); // 9 multiplications, 6 additions.
		vector64x3_c make_vector64x3_transformed_point( vector64x3_c const & point, matrix32x3x3_c const & transform ); // 9 multiplications, 6 additions.

		vector32x3_c make_vector32x3_transformed_vector( vector32x3_c const & vector, matrix32x4x4_c const & transform ); // 9 multiplications, 6 additions. applies rotation and scale, disregards position|translation.
		vector64x3_c make_vector64x3_transformed_vector( vector64x3_c const & vector, matrix32x4x4_c const & transform ); // 9 multiplications, 6 additions. applies rotation and scale, disregards position|translation.

		vector32x3_c make_vector32x3_transformed_point( vector32x3_c const & point, matrix32x4x4_c const & transform ); // 9 multiplications, 9 additions.
		vector64x3_c make_vector64x3_transformed_point( vector64x3_c const & point, matrix32x4x4_c const & transform ); // 9 multiplications, 9 additions.

		vector32x4_c make_vector32x4_transformed_point( vector32x3_c const & point, matrix32x4x4_c const & transform ); // 12 multiplications, 12 additions. same as what the gpu does to transform 3d points by the view projection matrix, results in a 4d point, then xy can be divided by w to get screen space coordinates.

		vector64x3_c make_vector64x3_transformed_point( vector64x3_c const & point, space_transform_c const & transform ); // applies rotation, scale, and translation (in that order).

		vector32x3_c make_vector32x3_basis_a_from_quaternion32( quaternion32_c const & rotation );
		vector32x3_c make_vector32x3_basis_b_from_quaternion32( quaternion32_c const & rotation );
		vector32x3_c make_vector32x3_basis_c_from_quaternion32( quaternion32_c const & rotation );

		vector32x3_c make_vector32x3_constrained_normal( vector32x3_c const & normal, vector32x3_c const & normal_target, float32_c const maximum_angle );
		vector64x3_c make_vector64x3_constrained_normal( vector64x3_c const & normal, vector64x3_c const & normal_target, float64_c const maximum_angle );

		vector64x3_c make_vector64x3_reflected_vector( vector64x3_c const & direction_vector, vector64x3_c const & normal_vector ); // reflects vector across imaginary plane defined by normal_vector and zero origin.
		vector64x3_c make_vector64x3_reflected_vector( vector64x3_c const & point, plane64_c const & plane ); // finds the point that is opposite of plane.

		vector64x3_c make_vector64x3_parallel_component_vector2( vector64x3_c const & normal_vector, vector64x3_c const & other_vector ); // returns a vector that is parallel to normal_vector and is as long as other_vector is in agreement with normal_vector.
		vector64x3_c make_vector64x3_perpendicular_component_vector2( vector64x3_c const & normal_vector, vector64x3_c const & other_vector ); // returns a vector that is perpendicular to normal_vector and is as long as other_vector is in disagreement with normal_vector.

		vector32x3_c make_vector32x3_parallel_component_vector2( vector32x3_c const & normal_vector, vector32x3_c const & other_vector );
		vector32x3_c make_vector32x3_perpendicular_component_vector2( vector32x3_c const & normal_vector, vector32x3_c const & other_vector );
		

		//
		//
		// matrix, quaternion, and angle conversions.
		//
		//

		matrix32x4x4_c make_matrix32x4x4_from_space_transform( space_transform_c const & space_transform );
		matrix32x4x4_c make_matrix32x4x4_from_space_transform( space_transform_c const & space_transform, vector64x3_c const & relative_to_origin );

		matrix32x4x4_c make_matrix32x4x4_basis_from_quaternion32( quaternion32_c const & rotation );
		matrix32x3x3_c make_matrix32x3x3_basis_from_quaternion32( quaternion32_c const & rotation );
		matrix32x3x3_c make_matrix32x3x3_basis_from_axis_angle( vector32x4_c const & axis_angle );
		matrix32x3x3_c make_matrix32x3x3_basis_socket_rotated_b( matrix32x3x3_c basis, vector32x3_c const & basis_b_target );
		matrix32x3x3_c make_matrix32x3x3_basis_socket_rotated_c( matrix32x3x3_c basis, vector32x3_c const & basis_c_target );
		matrix32x3x3_c make_matrix32x3x3_transposed( matrix32x3x3_c const & matrix );

		matrix32x4x4_c make_matrix32x4x4_view_from_look_at( vector32x3_c const & position, vector32x3_c const & forward, vector32x3_c const & up );
		//matrix32x4x4_c make_matrix32x4x4_view2( vector32x3_c const & position, vector32x3_c const & z_forward, vector32x3_c const & x_right );

		float32_c make_float32_fov_a( float32_c fov_b, float32_c aspect_ratio ); // convenience function to calculate angle fov counter part, because our make_matrix32x4x4_projection_perspective3 function takes fov_a and fov_b instead of the typical fov_b and aspect_ratio.
		float32_c make_float32_fov_b( float32_c fov_a, float32_c aspect_ratio ); // convenience function to calculate angle fov counter part, because our make_matrix32x4x4_projection_perspective3 function takes fov_a and fov_b instead of the typical fov_b and aspect_ratio.
		float32_c make_float32_fov_b_off_center_scale_factor( float32_c fov_b, float32_c clip_near ); // convenience function to calculate scale factor for converting canvas size to near clip plane size.

		matrix32x4x4_c make_matrix32x4x4_projection_orthographic( float32_c const size_a, float32_c const size_b, float32_c const clip_near, float32_c const clip_far );
		matrix32x4x4_c make_matrix32x4x4_projection_orthographic_off_center( float32_c const left, float32_c right, float32_c const bottom, float32_c const top, float32_c const clip_near, float32_c const clip_far ); // note that in right-handed projection, left < right, bottom < top.
		matrix32x4x4_c make_matrix32x4x4_projection_perspective( float32_c const vertical_fov, float32_c const horizontal_to_vertical_aspect_ratio, float32_c const clip_near, float32_c const clip_far );
		matrix32x4x4_c make_matrix32x4x4_projection_perspective_off_center( float32_c const left, float32_c const right, float32_c const bottom, float32_c const top, float32_c const clip_near, float32_c const clip_far ); // note that in right-handed projection, left < right, bottom < top. left, top, right, and bottom are units of distance over the near clip plane, so these values should be scaled with the near clip plane distance (the closer the clip plane the smaller the values, and the farther the clip plane the larger the values).
		matrix32x4x4_c make_matrix32x4x4_transposed( matrix32x4x4_c const & matrix );
		matrix32x4x4_c make_matrix32x4x4_inverted_fast( matrix32x4x4_c const & matrix ); // can be used to convert an object transform to a view transform, and vice versa. foregoes doing determinant calculation and all that, which is the typical way a matrix would be inverted in math.

		float32_c make_matrix32x2x2_determinant( matrix32x2x2_c const & matrix );
		matrix32x2x2_c make_matrix32x2x2_inverted( matrix32x2x2_c const & matrix );
		matrix32x2x2_c make_matrix32x2x2_transform( float32_c const angle, float32_c const scale );

		quaternion32_c make_quaternion32_from_matrix32x3x3_basis( matrix32x3x3_c const & basis ); // basis must be right-handed, normalized, and orthogonal.
		quaternion32_c make_quaternion32_inverted( quaternion32_c const & quaternion );
		quaternion32_c make_quaternion32_from_axis_angle( vector32x4_c const & axis_angle ); // axis_angle.abc is normalized unit length axis, axis_angle.d is angle around axis in radians (IDK if it's clockwise or coutnerclockwise).
		vector32x4_c make_axis_angle_from_quaternion32( quaternion32_c const & rotation ); // rotation is assumed to be normalized unit length quaternion.
		quaternion32_c make_quaternion32_from_euler_angles( vector32x3_c const & euler_angles ); // euler_angles are angles in radians for each axis, rotations are applied in the order of a, b, then c.
		vector32x3_c make_euler_angles_from_quaternion32( quaternion32_c const & rotation ); // rotation should 
		quaternion32_c make_quaternion32_from_look_at( vector32x3_c const & z_forward, vector32x3_c const & x_right ); // inputs should be pre-normalized.


		//
		//
		// line operations.
		//
		//

		line32_c make_line32_from_normal_and_distance( vector32x2_c const & normal, float32_c distance );
		line32_c make_line32_from_normal_and_point( vector32x2_c const & normal, vector32x2_c const & point );
		line32_c make_line32_from_segment( vector32x2_c const & point_a, vector32x2_c const & point_b ); // normal/tangent of result points to right of vector point_b - point_a.
		line32_c make_line32_normalized( line32_c const & line );

		//vector32x2_c get_rectangle_point( box32x2_c const & rectangle, matrix32x2x2_c const & rectangle_basis, vector32x2_c const & rectangle_origin, sint32_c rectangle_point_index ); // rectangle_point_index, 0 is top left, 1 is top right, 2 is bottom left, 3 is bottom right.
		//line32_c get_rectangle_line( box32x2_c const & rectangle, matrix32x2x2_c const & rectangle_basis, vector32x2_c const & rectangle_origin, sint32_c rectangle_edge_index ); // rectangle_edge_index, 0 is left, 1 is top, 2 is right, 3 is bottom. normal of lines point towards the outside of the rectangle.

		//
		//
		// plane operations.
		//
		//

		plane64_c make_plane64_from_normal_and_distance( vector64x3_c const & normal, float64_c distance ); // normal needs to be normalized already.
		plane64_c make_plane64_from_normal_and_point( vector64x3_c const & normal, vector64x3_c const & point ); // normal needs to be normalized already.
		plane64_c make_plane64_from_triangle( vector64x3_c const & point_a, vector64x3_c const & point_b, vector64x3_c const & point_c ); // if points are laid on the face of a clock, and wind order is counter clock wise, then plane normal will point out of the face of the clock.
		plane64_c make_plane64_normalized( plane64_c const & plane );


		//
		//
		// frustum operations.
		//
		//

		frustum64_c make_frustum64_perspective3( vector64x3_c const & position, vector64x3_c const & forward, vector64x3_c const & up, float64_c const fov_a, float64_c const fov_b, float64_c const clip_near, float64_c const clip_far );
		frustum64_c make_frustum64_orthographic3( vector64x3_c const & position, vector64x3_c const & forward, vector64x3_c const & up, float64_c const fov_a, float64_c const fov_b, float64_c const clip_near, float64_c const clip_far );
		frustum64_c make_frustum64_from_view_projection( matrix32x4x4_c const & view_projection );


		//
		//
		// color format conversions.
		//
		//

		void_c convert_rgba_to_rgba( vector32x4_c const & value, uint32_c & result );
		void_c convert_rgba_to_rgba( uint32_c const & value, vector32x4_c & result );

		boolean_c convert_rgb_to_hsv( vector64x3_c const & rgb, vector64x3_c & hsv ); // hue is in degrees, saturation and value are in range 0 to 1. red green and blue are in range 0 to 1.
		boolean_c convert_hsv_to_rgb( vector64x3_c const & hsv, vector64x3_c & rgb ); // hue is in degrees, saturation and value are in range 0 to 1. red green and blue are in range 0 to 1.

		boolean_c convert_rgb_to_husl( float64_c const * rgb, float64_c * husl );
		boolean_c convert_husl_to_rgb( float64_c const * husl, float64_c * rgb );


		//
		//
		// miscellaneous geometry operations.
		//
		//

		// sets the points and points_count of polygon_out, leaves everything else alone.
		// result is a counter clock wise wind order polygon, starting with the top left vertex of the rectangle.
		void_c make_polygon32x2_from_rectangle32( box32x2_c const & rectangle_in, polygon32x2_c & polygon_out );

		//ray64_c make_transformed_ray( ray64_c const & ray, matrix64x3x4_c const & transform );
		vector64x3_c make_vector64x3_normal_from_triangle( vector64x3_c a, vector64x3_c b, vector64x3_c c );
		box64x3_c make_aabb_from_obb( box64x3_c const & obb, space_transform_c const & obb_transform );
		box32x2_c make_aabb_from_obb( box32x2_c const & obb, vector32x2_c const & obb_position, matrix32x2x2_c obb_basis );
		//plane64_c make_transformed_plane( plane64_c const & plane, space_transform_c const & transform );


		//
		//
		// interpolation operations.
		//
		//

		float32_c interpolate_linear( float32_c const source, float32_c const destination, float32_c const amount );
		vector32x2_c interpolate_linear( vector32x2_c const & source, vector32x2_c const & destination, float32_c const amount );
		vector32x3_c interpolate_linear( vector32x3_c const & source, vector32x3_c const & destination, float32_c const amount );
		vector32x4_c interpolate_linear( vector32x4_c const & source, vector32x4_c const & destination, float32_c const amount );

		float64_c interpolate_linear( float64_c const source, float64_c const destination, float64_c const amount );
		vector64x2_c interpolate_linear( vector64x2_c const & source, vector64x2_c const & destination, float64_c const amount );
		vector64x3_c interpolate_linear( vector64x3_c const & source, vector64x3_c const & destination, float64_c const amount );
		vector64x4_c interpolate_linear( vector64x4_c const & source, vector64x4_c const & destination, float64_c const amount );

		float32_c interpolate_cosine( float32_c const source, float32_c const destination, float32_c const amount );
		vector32x2_c interpolate_cosine( vector32x2_c const & source, vector32x2_c const & destination, float32_c const amount );
		vector32x3_c interpolate_cosine( vector32x3_c const & source, vector32x3_c const & destination, float32_c const amount );
		vector32x4_c interpolate_cosine( vector32x4_c const & source, vector32x4_c const & destination, float32_c const amount );

		float32_c interpolate_cubic( float32_c const previous_source, float32_c const source, float32_c const destination, float32_c const next_destination, float32_c const amount );
		vector32x2_c interpolate_cubic( vector32x2_c const & previous_source, vector32x2_c const & source, vector32x2_c const & destination, vector32x2_c const & next_destination, float32_c const amount );
		vector32x3_c interpolate_cubic( vector32x3_c const & previous_source, vector32x3_c const & source, vector32x3_c const & destination, vector32x3_c const & next_destination, float32_c const amount );
		vector32x4_c interpolate_cubic( vector32x4_c const & previous_source, vector32x4_c const & source, vector32x4_c const & destination, vector32x4_c const & next_destination, float32_c const amount );
		
		float32_c interpolate_hermite( float32_c const previous_source, float32_c const source, float32_c const destination, float32_c const next_destination, float32_c const amount, float32_c const tension, float32_c const bias );
		vector32x2_c interpolate_hermite( vector32x2_c const & previous_source, vector32x2_c const & source, vector32x2_c const & destination, vector32x2_c const & next_destination, float32_c const amount, float32_c const tension, float32_c const bias );
		vector32x3_c interpolate_hermite( vector32x3_c const & previous_source, vector32x3_c const & source, vector32x3_c const & destination, vector32x3_c const & next_destination, float32_c const amount, float32_c const tension, float32_c const bias );
		vector32x4_c interpolate_hermite( vector32x4_c const & previous_source, vector32x4_c const & source, vector32x4_c const & destination, vector32x4_c const & next_destination, float32_c const amount, float32_c const tension, float32_c const bias );

		quaternion32_c interpolate_spherical_linear( quaternion32_c const & source, quaternion32_c const & destination, float32_c const amount );

		quaternion32_c traverse( quaternion32_c const & source, quaternion32_c const & destination, float32_c amount ); // traverses from source to destination by amount in radians.

		float32_c traverse( float32_c const source, float32_c const destination, float32_c const amount );

		float32_c traverse_with_ease( float32_c const source, float32_c const destination, float32_c amount, float32_c const margin );

		//float32_c traverse_with_momentum( float32_c source, float32_c destination, float32_c ease_in_acceleration, float32_c ease_out_runway, float32_c speed_target, float32_c & speed_current );


		//
		//
		// geometry nearest point operations.
		//
		//

		float32_c distance_between_point_and_line( vector32x2_c const & point, line32_c const & line ); // positive distance means that point lies above line, negative distance means that point lies beneath line.
		float64_c distance_between_point_and_plane( vector64x3_c const & point, plane64_c const & plane ); // positive distance means that point lies above plane, negative distance means that point lies beneath plane.
		vector64x3_c nearest_point_on_box( vector64x3_c const & point, box64x3_c const & box );
		vector64x3_c nearest_point_on_box( vector64x3_c const & point, box64x3_c const & box, space_transform_c const & box_transform );
		vector64x3_c nearest_point_on_plane( vector64x3_c const & point, plane64_c const & plane );
		vector64x3_c nearest_point_on_ray( vector64x3_c const & point, ray64_c const & ray );
		vector64x3_c nearest_point_on_segment( vector64x3_c const & point, segment64_c const & segment );
		vector64x2_c nearest_point_on_rectangle( vector64x2_c const & point, box64x2_c const & rectangle );


		//
		//
		// geometry intersection test operations.
		//
		//

		boolean_c intersect_frustum_vs_point( frustum64_c const & frustum, vector64x3_c const & point );
		boolean_c intersect_frustum_vs_sphere( frustum64_c const & frustum, sphere64_c const & sphere );
		boolean_c intersect_frustum_vs_box( frustum64_c const & frustum, box64x3_c const & box );
		boolean_c intersect_frustum_vs_box( frustum64_c const & frustum, box64x3_c const & box, space_transform_c const & box_transform );
		boolean_c intersect_sphere_vs_point( sphere64_c const & sphere, vector64x3_c const & point );
		boolean_c intersect_sphere_vs_sphere( sphere64_c const & sphere_a, sphere64_c const & sphere_b );
		boolean_c intersect_box_vs_point( box64x3_c const & box, vector64x3_c const & point );
		boolean_c intersect_box_vs_box( box64x3_c const & box_a, box64x3_c const & box_b );
		boolean_c intersect_box_vs_box( box64x3_c const & box_a, space_transform_c const & box_a_transform, box64x3_c const & box_b, space_transform_c const & box_b_transform );
		boolean_c intersect_box_vs_sphere( box64x3_c const & box, sphere64_c const & sphere );
		boolean_c intersect_box_vs_sphere( box64x3_c const & box, space_transform_c const & box_transform, sphere64_c const & b );
		boolean_c intersect_rectangle_vs_point( box32x2_c const & rectangle, vector32x2_c const & point );
		boolean_c intersect_rectangle_vs_point( box64x2_c const & rectangle, vector64x2_c const & point );
		boolean_c intersect_rectangle_vs_rectangle( box32x2_c const & rectangle_a, box32x2_c const & rectangle_b );
		boolean_c intersect_rectangle_vs_rectangle( box64x2_c const & rectangle_a, box64x2_c const & rectangle_b );
		boolean_c intersect_rectangle_vs_circle( box64x2_c const & rectangle, circle64_c const & circle );
		boolean_c intersect_plane_vs_plane_vs_plane( plane64_c const & a, plane64_c const & b, plane64_c const & c, vector64x3_c * intersection_point );
		//boolean_c intersect_ray_vs_plane( ray64_c const & ray, plane64_c const & plane, float64_c & t );
		//boolean_c intersect_ray_vs_box( ray64_c const & ray, box64x3_c const & box );
		boolean_c intersect_segment_vs_box( segment64_c const & segment, box64x3_c const & box );
		//boolean_c intersect_ray_vs_sphere( const ray32_c& a, sphere32_c const & b, float32_c * intersection_distance );
		//boolean_c intersect_ray_vs_sphere( vector32x3_c const & ray_start, vector32x3_c const & ray_end, sphere32_c const & sphere32_c, float32_c * intersection_distance );
		//boolean_c intersect_ray_vs_box( const ray32_c& a, box32x3_c const & b, float32_c * intersection_distance );
		//boolean_c intersect_ray_vs_box( vector32x3_c const & ray_start, vector32x3_c const & ray_end, box32x3_c const & box32x3_c, float32_c * intersection_distance );
		boolean_c intersect_point_vs_polygon( vector32x2_c const & point, polygon32x2_c const & polygon ); // returns true if point lies in or on polygon, returns false if not. wind order of polygon should be counter clock wise.
		sint32_c intersect_polygon_vs_polygon( polygon32x2_c const & polygon_a, polygon32x2_c const & polygon_b ); // returns 0 if both polygons do not intersect, returns 1 if polygons partially intersect, returns 2 if polygon_b is fully contained in polygon_a.


		//
		//
		// geometry sweep test operations.
		//
		//

		boolean_c sweep_ray_vs_plane( ray64_c const & ray, plane64_c const & plane, float64_c & t );
		boolean_c sweep_ray_vs_box( ray64_c const & ray, box64x3_c const & box, float64_c & t_minimum, float64_c & t_maximum );
		boolean_c sweep_point_vs_plane( segment64_c const & point_path, plane64_c const & plane, float64_c & t );
		boolean_c sweep_point_vs_sphere( segment64_c const & point_path, sphere64_c const & sphere, float64_c & t );
		boolean_c sweep_point_vs_capsule( segment64_c const & point_path, capsule64_c const & capsule, float64_c & t );
		boolean_c sweep_point_vs_box( segment64_c const & point_path, box64x3_c const & box, float64_c & t );
		boolean_c sweep_point_vs_box( segment64_c const & point_path, box64x3_c const & box, space_transform_c const & box_transform, float64_c & t );
		boolean_c sweep_sphere_vs_box( segment64_c const & sphere_path, float64_c const sphere_radius, box64x3_c const & box, space_transform_c const & box_transform, float64_c & t );
		boolean_c sweep_point_vs_triangle( segment64_c const & point_path, triangle64_c const & triangle, float64_c & t );


		//
		//
		// file path operations.
		// file paths always use string16_c instances.
		//
		//

		boolean_c char16_is_valid_for_file_path( char16_c character ); // allows all printable ascii characters, except for '<', '>', '"', '|', '?', '*', '%'.
		boolean_c char16_is_valid_for_file_name( char16_c character ); // allows all printable ascii characters, except for '<', '>', '"', '|', '?', '*', '%', ':', '/', '\'.

		boolean_c string16_is_valid_for_file_path( string16_c const & string );
		boolean_c string16_is_valid_for_file_name( string16_c const & string );

		boolean_c path_is_formatted_for_cheonsa( string16_c const & path, boolean_c has_trailing_slash ); // checks that path only contains valid file path characters. path may contain forward slashes, but not back slashes (this is cheonsa's file path convention).
		void_c path_format_for_cheonsa( string16_c const & windows_file_path_in, string16_c & cheonsa_file_path_out, boolean_c add_trailing_slash ); // replaces back slashes with forward slashes, and if add_trailing_slash is true then adds a forward slash at the end if there isn't one there already. cheonsa should save all file paths in this format so that they can be ported to other formats as needed.

		#if defined( cheonsa_platform_windows )
		// given a file path that was formatted with path_format_for_cheonsa(), converts it to a file path for windows.
		// gets around 260 character MAX_PATH limit by prepending "\\?\" to a file path.
		// converts all forward slashes to back slashes.
		void_c path_format_for_windows( string16_c const & cheonsa_file_path_in, string16_c & windows_file_path_out );
		#endif

		string16_c path_get_mother( string16_c const & path ); // returns the mother path which will always end with '/'.
		string16_c path_get_file_extension( string16_c const & path ); // gets the file extension pointed to by the file path. returns the first dot in the file name and everything after it. supports multi-dot file extensions.
		string16_c path_get_file_name( string16_c const & path ); // gets the full name of the file pointed to by the path.
		string16_c path_get_file_name_without_extension( string16_c const & path ); // gets the name of the file minus its extension pointed to by the path.


		//
		//
		// file operations.
		//
		//

		boolean_c data_get_file_or_folder_modified_time( string16_c const & file_path, sint64_c & miliseconds_since_epoch );
		boolean_c data_set_file_or_folder_time_modified( string16_c const & file_path, sint64_c miliseconds_since_epoch );

		boolean_c data_does_file_exist( string16_c const & file_path );
		boolean_c data_create_file( string16_c const & file_path );

		boolean_c data_does_folder_exist( string16_c const & file_path );
		boolean_c data_create_folder( string16_c const & file_path );

		boolean_c data_move_file_or_folder( string16_c const & from_file_path, string16_c const & to_file_path );
		boolean_c data_delete_file_or_folder( string16_c const & file_path );

		void_c data_get_file_path_list( core_list_c< string16_c > & result_absolute_file_paths, string16_c const & absolute_folder_path, boolean_c const search_sub_folders, char8_c const * extension_filter ); // extension filter is an optional file extension (without asterisk, with the dot) like ".txt".
		void_c data_get_folder_path_list( core_list_c< string16_c > & result_absolute_folder_paths, string16_c const & absolute_folder_path, boolean_c const search_sub_folders );


		//
		//
		// time operations.
		//
		//

		sint64_c time_get_high_resolution_timer_frequency(); // returns how fast this system's high resolution timer ticks at, in hertz (ticks per second).
		sint64_c time_get_high_resolution_timer_count(); // returns the current number of ticks of this system's high resolution timer.
		sint64_c time_get_milliseconds_since_epoch(); // gets the number of the earth human's time measurement unit called "milliseconds" passed since the earth human's gregorian calendar date of January 1, 2000, 12:00:00 GMT.
		sint64_c time_get_local_time_zone_offset(); // gets the time zone and daylight savings delta that can be applied to the value returned by time_get_milliseconds_since_epoch() in order to produce a localized time_date_*_c instance.


		//
		//
		// random number generation.
		//
		//

		void_c random_start(); // initializes the random number generator, is called once at engine start up, needs to be called before any of the other random number generator functions can be called.
		float32_c random_float32( float32_c minimum = 0.0f, float32_c maximum = 1.0f );
		float64_c random_float64( float64_c minimum = 0.0, float64_c maximum = 1.0 );
		sint32_c random_sint32( sint32_c minimum = -2147483647, sint32_c maximum = 2147483647 );
		uint32_c random_uint32( uint32_c maximum = 4294967295 );
		vector32x2_c random_point_on_circle(); // returns random unit length vector.
		vector32x3_c random_point_on_sphere(); // returns random unit length vector.


		//
		//
		// string conversions and operations.
		// string conversions always use string8_c instances.
		//
		//

		char8_c convert_digit_to_char8( sint8_c const digit ); // converts a number value in range 0-15 to a character value in range '0'-'9' or 'A'-'F'.
		sint8_c convert_char8_to_digit( char8_c const character ); // converts a character value in range '0'-'9' or 'a'-'f' or 'A'-'F' to a number value in range 0-15. returns -1 (which in unsigned types is actually maximum: 0xFF, 255) if character is invalid.

		boolean_c convert_string8_to_sint8( string8_c const & in, sint8_c & out );
		void_c convert_sint8_to_string8( sint8_c in, string8_c & out );
		boolean_c convert_string8_to_sint8xn( string8_c const & in, core_list_c< sint8_c > & out );
		void_c convert_sint8xn_to_string8( core_list_c< sint8_c > const & in, string8_c & out );

		boolean_c convert_string8_to_uint8( string8_c const & in, uint8_c & out );
		void_c convert_uint8_to_string8( uint8_c in, string8_c & out );
		boolean_c convert_string8_to_uint8xn( string8_c const & in, core_list_c< uint8_c > & out );
		void_c convert_uint8xn_to_string8( core_list_c< uint8_c > const & in, string8_c & out );

		boolean_c convert_string8_to_sint16( string8_c const & in, sint16_c & out );
		void_c convert_sint16_to_string8( sint16_c in, string8_c & out ); // if desired_length > 0, then result string will add leading zeros or truncate leading digits.
		boolean_c convert_string8_to_sint16xn( string8_c const & in, core_list_c< sint16_c > & out ); // converts a string of number values into a list of numbers, each number is expected to be delimited by non-numeric characters, so it can be commas, spaces, letters, whatever. if out is a dynamic mode list then values will be appended to the end of it, otherwise if out is a static mode list then values in the list will be set in-place.
		void_c convert_sint16xn_to_string8( core_list_c< sint16_c > const & in, string8_c & out ); // converts strings to parenthesis enclosed comma separated list of number values, "(N1, N2, N3, N...)".

		boolean_c convert_string8_to_uint16( string8_c const & in, uint16_c & out );
		void_c convert_uint16_to_string8( uint16_c in, string8_c & out );
		boolean_c convert_string8_to_uint16xn( string8_c const & in, core_list_c< uint16_c > & out );
		void_c convert_uint16xn_to_string8( core_list_c< uint16_c > const & in, string8_c & out );

		boolean_c convert_string8_to_sint32( string8_c const & in, sint32_c & out );
		void_c convert_sint32_to_string8( sint32_c in, string8_c & out ); // if desired_length > 0, then result string will add leading zeros or truncate leading digits.
		boolean_c convert_string8_to_sint32xn( string8_c const & in, core_list_c< sint32_c > & out ); // converts a string of number values into a list of numbers, each number is expected to be delimited by non-numeric characters, so it can be commas, spaces, letters, whatever. if out is a dynamic mode list then values will be appended to the end of it, otherwise if out is a static mode list then values in the list will be set in-place.
		void_c convert_sint32xn_to_string8( core_list_c< sint32_c > const & in, string8_c & out ); // converts strings to parenthesis enclosed comma separated list of number values, "(N1, N2, N3, N...)".

		boolean_c convert_string8_to_uint32( string8_c const & in, uint32_c & out );
		void_c convert_uint32_to_string8( uint32_c in, string8_c & out );
		boolean_c convert_string8_to_uint32xn( string8_c const & in, core_list_c< uint32_c > & out );
		void_c convert_uint32xn_to_string8( core_list_c< uint32_c > const & in, string8_c & out );

		boolean_c convert_string8_to_sint64( string8_c const & in, sint64_c & out );
		void_c convert_sint64_to_string8( sint64_c in, string8_c & out );
		boolean_c convert_string8_to_sint64xn( string8_c const & in, core_list_c< sint64_c > & out );
		void_c convert_sint64xn_to_string8( core_list_c< sint64_c > const & in, string8_c & out );

		boolean_c convert_string8_to_uint64( string8_c const & in, uint64_c & out );
		void_c convert_uint64_to_string8( uint64_c in, string8_c & out );
		boolean_c convert_string8_to_uint64xn( string8_c const & in, core_list_c< uint64_c > & out );
		void_c convert_uint64xn_to_string8( core_list_c< uint64_c > const & in, string8_c & out );

		boolean_c convert_hexadecimal_string8_to_uint64( string8_c const & in, uint64_c & out );
		void_c convert_uint64_to_hexadecimal_string8( uint64_c in, string8_c & out );

		boolean_c convert_string8_to_float32( string8_c const & in, float32_c & out ); // at the moment, always returns true even if the string was not valid for conversion.
		void_c convert_float32_to_string8( float32_c in, string8_c & out );
		boolean_c convert_string8_to_float32xn( string8_c const & in, core_list_c< float32_c > & out ); // converts a string of number values into a list of numbers, each number is expected to be delimited by non-numeric characters, so it can be commas, spaces, letters, whatever. if out is a dynamic mode list then values will be appended to the end of it, otherwise if out is a static mode list then values in the list will be set in-place.
		void_c convert_float32xn_to_string8( core_list_c< float32_c > const & in, string8_c & out ); // converts strings to parenthesis enclosed comma separated list of number values, "(N1, N2, N3, N...)".

		boolean_c convert_string8_to_float64( string8_c const & in, float64_c & out );
		void_c convert_float64_to_string8( float64_c const in, string8_c & out );
		boolean_c convert_string8_to_float64xn( string8_c const & in, core_list_c< float64_c > & out ); // converts a string of number values into a list of numbers, each number is expected to be delimited by non-numeric characters, so it can be commas, spaces, letters, whatever. if out is a dynamic mode list then values will be appended to the end of it, otherwise if out is a static mode list then values in the list will be set in-place.
		void_c convert_float64xn_to_string8( core_list_c< float64_c > const & in, string8_c & out ); // converts strings to parenthesis enclosed comma separated list of number values, "(N1, N2, N3, N...)".

		boolean_c convert_string8_to_boolean( string8_c const & in, boolean_c & out );
		void_c convert_boolean_to_string8( boolean_c in, string8_c & out );

		boolean_c convert_rgb_to_hexadecimal_string8( vector64x3_c const & rgb, string8_c & string ); // converts an rgb color to a string of hexadecimal digits.
		boolean_c convert_rgba_to_hexadecimal_string8( vector64x4_c const & rgba, string8_c & string ); // converts an rgba color to a string of hexadecimal digits.

		boolean_c convert_string8_to_rgba( string8_c const & string, vector32x4_c & rgba );
		boolean_c convert_string8_to_rgba( string8_c const & string, vector64x4_c & rgba ); // converts an input string formatted as a list of floats (separated by garbage (anything)) or formatted as hexadecimal string (must be prefixed with '#' or '0x') to an rgba color.

		boolean_c char16_is_printable( char16_c a );
		boolean_c char16_is_space( char16_c a );
		boolean_c char16_is_letter( char16_c a );
		boolean_c char16_is_number( char16_c a );
		boolean_c char16_is_punctuation( char16_c a );
		boolean_c char16_is_control( char16_c a );

		boolean_c char8_compare( char8_c a, char8_c b, boolean_c case_sensitive );
		boolean_c char16_compare( char16_c a, char16_c b, boolean_c case_sensitive );

		char8_c char8_to_upper( char8_c a );
		char16_c char16_to_upper( char16_c a );

		char8_c char8_to_lower( char8_c a );
		char16_c char16_to_lower( char16_c a );

		void_c string8_to_upper( string8_c & string ); // converts the input string instance to upper case.
		void_c string16_to_upper( string16_c & string ); // converts the input string instance to upper case.

		void_c string8_to_lower( string8_c & string ); // converts the input string instance to lower case.
		void_c string16_to_lower( string16_c & string ); // converts the input string instance to lower case.

		void_c string8_pad_with_leading_zeros( string8_c & string, sint32_c desired_length ); // string will be padded with as many zeros as needed so that it's length reaches desired_length. if string's length is already >= desired_length then nothing will happen.

		string8_c string8_trim_front( string8_c const & string );
		string16_c string16_trim_front( string16_c const & string );

		string8_c string8_trim_back( string8_c const & string );
		string16_c string16_trim_back( string16_c const & string );

		string8_c string8_trim( string8_c const & string );
		string16_c string16_trim( string16_c const & string );

		boolean_c string8_find_index_of( string8_c const & string, string8_c const & find, sint32_c & result ); // finds the index of the first occurrence of find in the string. returns true if an instance is found, false if not.
		boolean_c string16_find_index_of( string16_c const & string, string16_c const & find, sint32_c & result ); // finds the index of the first occurrence of find in the string. returns true if an instance is found, false if not.

		boolean_c string8_find_indices_of( string8_c const & string, string8_c const & find, core_list_c< sint32_c > & result ); // finds the indices of occurrences of find in the string. overlapping matches are counted, so the whole entire string of "nanan" will be a match to "nan" 2 times instead of 1.
		boolean_c string16_find_indices_of( string16_c const & string, string16_c const & find, core_list_c< sint32_c > & result ); // finds the indices of occurrences of find in the string. overlapping matches are counted, so the whole entire string of "nanan" will be a match to "nan" 2 times instead of 1.

		string8_c string8_find_and_replace_all( string8_c const & string, string8_c const & find, string8_c const & replace, boolean_c case_sensitive );
		string16_c string16_find_and_replace_all( string16_c const & string, string16_c const & find, string16_c const & replace, boolean_c case_sensitive );

		void_c string8_split_at_spaces( string8_c const & string, core_list_c< string8_c > & result ); // splits string apart at all intervals of white space, result strings will not contain white space.
		void_c string8_split_at_delimiter( string8_c const & string, string8_c const & delimiter, core_list_c< string8_c > & result ); // splits string into multiple string instances wherever an occurrence of needle is found.

		void_c string16_split_at_spaces( string16_c const & string, core_list_c< string16_c > & result ); // splits string apart at all intervals of white space, result strings will not contain white space.
		void_c string16_split_at_delimiter( string16_c const & string, string16_c const & delimiter, core_list_c< string16_c > & result ); // splits string into multiple string instances wherever an occurrence of needle is found.

		string8_c string8_sub_string( string8_c const & string, sint32_c start, sint32_c length );
		string16_c string16_sub_string( string16_c const & string, sint32_c start, sint32_c length );

		boolean_c string8_starts_with( char8_c const * a, char8_c const * b ); // checks if string a starts with string b.
		boolean_c string8_starts_with( char8_c const * a, char16_c const * b ); // checks if string a starts with string b, interprets b as utf-8 stream, so converts char16_c to 1 to 3 byte long utf-8 sequence then compares with a.

		boolean_c string16_starts_with( char16_c const * a, char8_c const * b ); // checks if string a starts with string b, interprets b as utf-8 stream, so reads char16_c characters one at a time from utf-8 then compares with b.
		boolean_c string16_starts_with( char16_c const * a, char16_c const * b ); // checks if string a starts with string b.

		boolean_c string8_ends_with( char8_c const * a, char8_c const * b );
		boolean_c string8_ends_with( char8_c const * a, char16_c const * b );

		boolean_c string16_ends_with( char16_c const * a, char8_c const * b );
		boolean_c string16_ends_with( char16_c const * a, char16_c const * b );

		sint32_c string8_sort_compare( string8_c const & a, string8_c const & b ); // returns 0 if a and b are equal. returns > 0 if a goes after b. returns < 0 if a goes before b. use with core_list_c< string16_c >::insertion_sort( &string_sort_compare );
		sint32_c string8_sort_compare_case_insensitive( string8_c const & a, string8_c const & b ); // returns 0 if a and b are equal. returns > 0 if a goes after b. returns < 0 if a goes before b. use with core_list_c< string16_c >::insertion_sort( &string_sort_compare_case_insensitive );

		sint32_c string16_sort_compare( string16_c const & a, string16_c const & b ); // returns 0 if a and b are equal. returns > 0 if a goes after b. returns < 0 if a goes before b. use with core_list_c< string16_c >::insertion_sort( &string_sort_compare );
		sint32_c string16_sort_compare_case_insensitive( string16_c const & a, string16_c const & b ); // returns 0 if a and b are equal. returns > 0 if a goes after b. returns < 0 if a goes before b. use with core_list_c< string16_c >::insertion_sort( &string_sort_compare_case_insensitive );

		boolean_c string8_compare( char8_c const * a, char8_c const * b );
		boolean_c string8_compare_case_insensitive( char8_c const * a, char8_c const * b );

		boolean_c string16_compare( char16_c const * a, char16_c const * b );
		boolean_c string16_compare_case_insensitive( char16_c const * a, char16_c const * b );

		boolean_c string16_compare( char16_c const * a, char8_c const * b ); // compares a char16_c string to a utf-8 encoded char8_c string.
		boolean_c string16_compare_case_insensitive( char16_c const * a, char8_c const * b ); // compares a char16_c string to a utf-8 encoded char8_c string.

		sint32_c string8_find_length( char8_c const * string ); // returns the length of a null terminated string in characters, not including the terminating null character.
		sint32_c string16_find_length( char16_c const * string ); // returns the length of a null terminated string in characters, not including the terminating null character.

		//sint32_c string8_find_length_safe( char8_c const * string, char8_c const * limit ); // limit points to the next character after the end of the buffer. returns -1 if limit is breached.

		boolean_c string8_is_valid_for_key( string8_c const & string );

		// converts a utf-8 encoded byte stream to a wide character.
		// in: buffer that contains a utf-8 encoded byte sequence that encodes a character code point. after return. upon return this pointer will be advanced by the number of bytes read.
		// out: the result of the decode.
		// returns: number of bytes read.
		uint8_c convert_utf8_to_char16( char8_c const * & in, char16_c & out );

		// converts a null terminated utf-8 encoded string to a null terminated wide string.
		// out is appended to, not replaced, so most likely you will want to remove the terminating null character from out before appending to it.
		void_c convert_string8_to_string16( core_list_c< char8_c > const & in, core_list_c< char16_c > & out );

		// converts a null terminated utf-8 encoded string to a null terminated wide string.
		// the out buffer is a char8_c buffer, but the bytes that are appended should be interpreted as a string of char16_c.
		void_c convert_string8_to_string16_bytes( core_list_c< char8_c > const & in, core_list_c< char8_c > & out );

		// converts a wide character to a utf-8 encoded byte stream.
		// in: the wide character to encode.
		// out: pointer to a buffer with enough room to store result, it must be at least 3 bytes. upon return, this pointer will be advanced by the number of bytes written.
		// out_end: end of buffer so that we don't write past end of buffer.
		// returns: number of bytes written.
		sint8_c convert_char16_to_utf8( char16_c in, char8_c * & out );

		// converts a null terminated wide string to a null terminated utf-8 string.
		// out is appended to, not replaced.
		// most likely you will want to remove the terminating null character from out before appending to it.
		void_c convert_string16_to_string8( core_list_c< char16_c > const & in, core_list_c< char8_c > & out );


		//
		//
		// memory operations.
		//
		//

		void_c * memory_aligned_allocate( sint32_c size, sint32_c alignment );
		void_c memory_aligned_deallocate( void_c * block );
		void_c * memory_allocate( sint32_c size );
		void_c * memory_reallocate( void_c * block, sint32_c new_size ); // reallocates or resizes a block of memory previously allocated with memory_allocate() or memory_reallocate().
		void_c memory_deallocate( void_c * block );
		void_c memory_move( void_c const * buffer_source, void_c * buffer_destination, sint32_c buffer_size );
		void_c memory_copy( void_c const * buffer_source, void_c * buffer_destination, sint32_c buffer_size );
		void_c memory_zero( void_c * buffer, sint32_c buffer_size );
		boolean_c memory_is_zero( void_c const * buffer, sint32_c buffer_size );


		//
		//
		// data operations.
		//
		//

		// gets the size in bytes of the given type.
		// returns a valid value for all numeric types.
		// returns 0 for anything else.
		sint32_c get_data_type_size( data_type_e type );
		byte_order_e get_native_byte_order(); // gets the native endianess of the cpu.
		void_c flip_byte_order( void_c * buffer, sint32_c buffer_size ); // swaps the byte order of the data in buffer.


		//
		//
		// hash operations.
		//
		//

		uint32_c xxhash32( void_c const * data, uint_native_c data_size );
		uint64_c xxhash64( void_c const * data, uint_native_c data_size );

	}
}
