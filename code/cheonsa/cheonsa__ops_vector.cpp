#include "cheonsa__ops.h"

namespace cheonsa
{
	namespace ops
	{

		float32_c length_squared_float32( vector32x2_c const & vector )
		{
			return ( vector.a * vector.a ) + ( vector.b * vector.b );
		}

		float64_c length_squared_float64( vector64x2_c const & vector )
		{
			return ( vector.a * vector.a ) + ( vector.b * vector.b );
		}

		float32_c length_squared_float32( vector32x3_c const & vector )
		{
			return ( vector.a * vector.a ) + ( vector.b * vector.b ) + ( vector.c * vector.c );
		}

		float64_c length_squared_float64( vector64x3_c const & vector )
		{
			return ( vector.a * vector.a ) + ( vector.b * vector.b ) + ( vector.c * vector.c );
		}

		float32_c length_squared_float32( vector32x4_c const & vector )
		{
			return ( vector.a * vector.a ) + ( vector.b * vector.b ) + ( vector.c * vector.c ) + ( vector.d * vector.d );
		}

		float64_c length_squared_float64( vector64x4_c const & vector )
		{
			return ( vector.a * vector.a ) + ( vector.b * vector.b ) + ( vector.c * vector.c ) + ( vector.d * vector.d );
		}

		float32_c length_float32( vector32x2_c const & vector )
		{
			return math_square_root( ( vector.a * vector.a ) + ( vector.b * vector.b ) );
		}

		float64_c length_float64( vector64x2_c const & vector )
		{
			return math_square_root( ( vector.a * vector.a ) + ( vector.b * vector.b ) );
		}

		float32_c length_float32( vector32x3_c const & vector )
		{
			return math_square_root( ( vector.a * vector.a ) + ( vector.b * vector.b ) + ( vector.c * vector.c ) );
		}

		float64_c length_float64( vector64x3_c const & vector )
		{
			return math_square_root( ( vector.a * vector.a ) + ( vector.b * vector.b ) + ( vector.c * vector.c ) );
		}

		float32_c length_float32( vector32x4_c const & vector )
		{
			return math_square_root( ( vector.a * vector.a ) + ( vector.b * vector.b ) + ( vector.c * vector.c ) + ( vector.d * vector.d ) );
		}

		float64_c length_float64( vector64x4_c const & vector )
		{
			return math_square_root( ( vector.a * vector.a ) + ( vector.b * vector.b ) + ( vector.c * vector.c ) + ( vector.d * vector.d ) );
		}

		vector32x2_c normal_vector32x2( vector32x2_c const & vector )
		{
			vector32x2_c result;
			float32_c a_vector_length = length_float32( vector );
			if ( a_vector_length > constants< float32_c >::division_near_zero() )
			{
				result.a = vector.a / a_vector_length;
				result.b = vector.b / a_vector_length;
			}
			return result;
		}

		vector64x2_c normal_vector64x2( vector64x2_c const & vector )
		{
			vector64x2_c result;
			float64_c a_vector_length = length_float64( vector );
			if ( a_vector_length > constants< float64_c >::division_near_zero() )
			{
				result.a = vector.a / a_vector_length;
				result.b = vector.b / a_vector_length;
			}
			return result;
		}

		vector32x3_c normal_vector32x3( vector32x3_c const & vector )
		{
			vector32x3_c result;
			float32_c a_vector_length = length_float32( vector );
			if ( a_vector_length > constants< float32_c >::division_near_zero() )
			{
				result.a = vector.a / a_vector_length;
				result.b = vector.b / a_vector_length;
				result.c = vector.c / a_vector_length;
			}
			return result;
		}

		vector64x3_c normal_vector64x3( vector64x3_c const & vector )
		{
			vector64x3_c result;
			float64_c a_vector_length = length_float64( vector );
			if ( a_vector_length > constants< float64_c >::division_near_zero() )
			{
				result.a = vector.a / a_vector_length;
				result.b = vector.b / a_vector_length;
				result.c = vector.c / a_vector_length;
			}
			return result;
		}

		vector32x4_c normal_vector32x4( vector32x4_c const & vector )
		{
			vector32x4_c result;
			float32_c a_vector_length = length_float32( vector );
			if ( a_vector_length > constants< float32_c >::division_near_zero() )
			{
				result.a = vector.a / a_vector_length;
				result.b = vector.b / a_vector_length;
				result.c = vector.c / a_vector_length;
				result.d = vector.d / a_vector_length;
			}
			return result;
		}

		vector64x4_c normal_vector64x4( vector64x4_c const & vector )
		{
			vector64x4_c result;
			float64_c a_vector_length = length_float64( vector );
			if ( a_vector_length > constants< float64_c >::division_near_zero() )
			{
				result.a = vector.a / a_vector_length;
				result.b = vector.b / a_vector_length;
				result.c = vector.c / a_vector_length;
				result.d = vector.d / a_vector_length;
			}
			return result;
		}

		quaternion32_c normal_quaternion32( quaternion32_c const & quaternion )
		{
			quaternion32_c result;
			float32_c length_inverse = 1.0f / math_square_root( ( quaternion.a * quaternion.a ) + ( quaternion.b * quaternion.b ) + ( quaternion.c * quaternion.c ) + ( quaternion.d * quaternion.d ) );
			result.a = quaternion.a * length_inverse;
			result.b = quaternion.b * length_inverse;
			result.c = quaternion.c * length_inverse;
			result.d = quaternion.d * length_inverse;
			return result;
		}

		void_c normalize_quaternion32( quaternion32_c & quaternion )
		{
			float32_c length_inverse = 1.0f / math_square_root( ( quaternion.a * quaternion.a ) + ( quaternion.b * quaternion.b ) + ( quaternion.c * quaternion.c ) + ( quaternion.d * quaternion.d ) );
			quaternion.a /= length_inverse;
			quaternion.b /= length_inverse;
			quaternion.c /= length_inverse;
			quaternion.d /= length_inverse;
		}

		float32_c dot_product_float32( vector32x2_c const & vector_a, vector32x2_c const & vector_b )
		{
			return ( vector_a.a * vector_b.a ) + ( vector_a.b * vector_b.b );
		}

		float64_c dot_product_float64( vector64x2_c const & vector_a, vector64x2_c const & vector_b )
		{
			return ( vector_a.a * vector_b.a ) + ( vector_a.b * vector_b.b );
		}

		float32_c dot_product_float32( vector32x3_c const & vector_a, vector32x3_c const & vector_b )
		{
			return ( vector_a.a * vector_b.a ) + ( vector_a.b * vector_b.b ) + ( vector_a.c * vector_b.c );
		}

		float64_c dot_product_float64( vector64x3_c const & vector_a, vector64x3_c const & vector_b )
		{
			return ( vector_a.a * vector_b.a ) + ( vector_a.b * vector_b.b ) + ( vector_a.c * vector_b.c );
		}

		float32_c dot_product_float32( vector32x4_c const & vector_a, vector32x4_c const & vector_b )
		{
			return ( vector_a.a * vector_b.a ) + ( vector_a.b * vector_b.b ) + ( vector_a.c * vector_b.c ) + ( vector_a.d * vector_b.d );
		}

		float64_c dot_product_float64( vector64x4_c const & vector_a, vector64x4_c const & vector_b )
		{
			return ( vector_a.a * vector_b.a ) + ( vector_a.b * vector_b.b ) + ( vector_a.c * vector_b.c ) + ( vector_a.d * vector_b.d );
		}

		float32_c dot_product_float32( quaternion32_c const & quaternion_a, quaternion32_c const & quaternion_b )
		{
			return ( quaternion_a.a * quaternion_b.a ) + ( quaternion_a.b * quaternion_b.b ) + ( quaternion_a.c * quaternion_b.c ) + ( quaternion_a.d * quaternion_b.d );
		}

		vector32x3_c cross_product_vector32x3( vector32x3_c const & vector_a, vector32x3_c const & vector_b )
		{
			return vector32x3_c( ( vector_a.b * vector_b.c ) - ( vector_a.c * vector_b.b ), ( vector_a.c * vector_b.a ) - ( vector_a.a * vector_b.c ), ( vector_a.a * vector_b.b ) - ( vector_a.b * vector_b.a ) );
		}

		vector64x3_c cross_product_vector64x3( vector64x3_c const & vector_a, vector64x3_c const & vector_b )
		{
			return vector64x3_c( ( vector_a.b * vector_b.c ) - ( vector_a.c * vector_b.b ), ( vector_a.c * vector_b.a ) - ( vector_a.a * vector_b.c ), ( vector_a.a * vector_b.b ) - ( vector_a.b * vector_b.a ) );
		}

		vector32x2_c orthogonalized_vector32x2( vector32x2_c const & vector )
		{
			return vector32x2_c( vector.b, -vector.a );
		}

		vector64x2_c orthogonalized_vector64x2( vector64x2_c const & vector )
		{
			return vector64x2_c( vector.b, -vector.a );
		}

		vector32x2_c orthonormalized_vector32x2( vector32x2_c const & vector, boolean_c polarity, boolean_c allow_zero )
		{
			float32_c length = length_float32( vector );
			if ( length == 0.0f )
			{
				return polarity ? vector32x2_c( 0.0f, !allow_zero ? 1.0f : 0.0f ) : vector32x2_c( 0.0f, !allow_zero ? -1.0f : 0.0f );
			}
			return polarity ? vector32x2_c( -vector.b / length, vector.a / length ) : vector32x2_c( vector.b / length, -vector.a / length );
		}

		vector64x2_c orthonormalized_vector64x2( vector64x2_c const & vector, boolean_c polarity, boolean_c allow_zero )
		{
			float64_c length = length_float64( vector );
			if ( length == 0.0 )
			{
				return polarity ? vector64x2_c( 0.0, !allow_zero ) : vector64x2_c( 0.0, -!allow_zero );
			}
			return polarity ? vector64x2_c( -vector.b / length, vector.a / length ) : vector64x2_c( vector.b / length, -vector.a / length );
		}

		vector32x3_c rotate_vector32x3( vector32x3_c const & vector, quaternion32_c const & rotation )
		{
			// nVidia SDK implementation
			vector32x3_c qvec( rotation.a, rotation.b, rotation.c );
			vector32x3_c uv = cross_product_vector32x3( qvec, vector );
			vector32x3_c uuv = cross_product_vector32x3( qvec, uv ) * 2.0f;
			uv *= 2.0f * rotation.d;
			return vector + uv + uuv;
		}

		vector64x3_c rotate_vector64x3( vector64x3_c const & vector, quaternion32_c const & rotation )
		{
			// 16 multiplications, 6 additions, 6 subtractions.
			// nVidia SDK implementation
			vector64x3_c qvec( rotation.a, rotation.b, rotation.c );
			vector64x3_c uv = cross_product_vector64x3( qvec, vector ); // 6 multiplications, 3 subtractions.
			vector64x3_c uuv = cross_product_vector64x3( qvec, uv ) * 2.0f; // 6 multiplications, 3 subtractions.
			uv *= 2.0 * rotation.d; // 4 multiplications.
			return vector + uv + uuv; // 6 additions.
		}

		vector32x2_c rotate_vector32x2( vector32x2_c const & point, float32_c const angle )
		{
			float32_c s = math_sine( angle );
			float32_c c = math_cosine( angle );
			return vector32x2_c( point.a * c - point.b * s, point.a * s + point.b * c );
		}

		vector64x2_c rotate_vector64x2( vector64x2_c const & point, float64_c const angle )
		{
			float64_c s = math_sine( angle );
			float64_c c = math_cosine( angle );
			return vector64x2_c( point.a * c - point.b * s, point.a * s + point.b * c );
		}

		vector32x3_c rotate_vector32x3( vector32x3_c const & point, vector32x3_c const & axis, float32_c const angle )
		{
			float32_c s = math_sine( angle );
			float32_c c = math_cosine( angle );
			return ( point * c ) + ( axis * dot_product_float32( axis, point ) * ( 1.0f - c ) ) + ( cross_product_vector32x3( point, axis ) * s );
		}

		vector64x3_c rotate_vector64x3( vector64x3_c const & point, vector64x3_c const & axis, float64_c const angle )
		{
			float64_c s = math_sine( angle );
			float64_c c = math_cosine( angle );
			return ( point * c ) + ( axis * dot_product_float64( axis, point ) * ( 1.0f - c ) ) + ( cross_product_vector64x3( point, axis ) * s );
		}

		vector32x2_c rotate_and_scale_vector32x2( vector32x2_c const & point, matrix32x2x2_c const & transform )
		{
			return vector32x2_c( ( point.a * transform.a.a ) + ( point.b * transform.b.a ), ( point.a * transform.a.b ) + ( point.b * transform.b.b ) );
		}

		vector32x3_c rotate_and_scale_vector32x3( vector32x3_c const & point, matrix32x3x3_c const & transform )
		{
			// 9 multiplications, 6 additions.
			vector32x3_c result;
			result.a = ( transform.a.a * point.a ) + ( transform.b.a * point.b ) + ( transform.c.a * point.c );
			result.b = ( transform.a.b * point.a ) + ( transform.b.b * point.b ) + ( transform.c.b * point.c );
			result.c = ( transform.a.c * point.a ) + ( transform.b.c * point.b ) + ( transform.c.c * point.c );
			return result;
		}

		vector64x3_c rotate_and_scale_vector64x3( vector64x3_c const & point, matrix32x3x3_c const & transform )
		{
			// 9 multiplications, 6 additions.
			vector64x3_c result;
			result.a = ( transform.a.a * point.a ) + ( transform.b.a * point.b ) + ( transform.c.a * point.c );
			result.b = ( transform.a.b * point.a ) + ( transform.b.b * point.b ) + ( transform.c.b * point.c );
			result.c = ( transform.a.c * point.a ) + ( transform.b.c * point.b ) + ( transform.c.c * point.c );
			return result;
		}

		vector32x3_c rotate_and_scale_vector32x3( vector32x3_c const & vector, matrix32x4x4_c const & transform )
		{
			// 9 multiplications, 6 additions.
			vector32x3_c result;
			result.a = ( transform.a.a * vector.a ) + ( transform.b.a * vector.b ) + ( transform.c.a * vector.c );
			result.b = ( transform.a.b * vector.a ) + ( transform.b.b * vector.b ) + ( transform.c.b * vector.c );
			result.c = ( transform.a.c * vector.a ) + ( transform.b.c * vector.b ) + ( transform.c.c * vector.c );
			return result;
		}

		vector64x3_c rotate_and_scale_vector64x3( vector64x3_c const & vector, matrix32x4x4_c const & transform )
		{
			// 9 multiplications, 9 additions.
			vector64x3_c result;
			result.a = ( transform.a.a * vector.a ) + ( transform.b.a * vector.b ) + ( transform.c.a * vector.c );
			result.b = ( transform.a.b * vector.a ) + ( transform.b.b * vector.b ) + ( transform.c.b * vector.c );
			result.c = ( transform.a.c * vector.a ) + ( transform.b.c * vector.b ) + ( transform.c.c * vector.c );
			return result;
		}

		vector32x3_c rotate_scale_and_translate_vector32x3( vector32x3_c const & point, matrix32x4x4_c const & transform )
		{
			// 9 multiplications, 9 additions.
			vector32x3_c result;
			result.a = ( transform.a.a * point.a ) + ( transform.b.a * point.b ) + ( transform.c.a * point.c ) + ( transform.d.a );
			result.b = ( transform.a.b * point.a ) + ( transform.b.b * point.b ) + ( transform.c.b * point.c ) + ( transform.d.b );
			result.c = ( transform.a.c * point.a ) + ( transform.b.c * point.b ) + ( transform.c.c * point.c ) + ( transform.d.c );
			return result;
		}

		vector64x3_c rotate_scale_and_translate_vector64x3( vector64x3_c const & point, matrix32x4x4_c const & transform )
		{
			// 9 multiplications, 9 additions.
			vector64x3_c result;
			result.a = ( transform.a.a * point.a ) + ( transform.b.a * point.b ) + ( transform.c.a * point.c ) + ( transform.d.a );
			result.b = ( transform.a.b * point.a ) + ( transform.b.b * point.b ) + ( transform.c.b * point.c ) + ( transform.d.b );
			result.c = ( transform.a.c * point.a ) + ( transform.b.c * point.b ) + ( transform.c.c * point.c ) + ( transform.d.c );
			return result;
		}

		vector32x4_c rotate_scale_and_translate_vector32x4( vector32x3_c const & point, matrix32x4x4_c const & transform )
		{
			// 12 multiplications, 12 additions.
			vector32x4_c result;
			result.a = ( transform.a.a * point.a ) + ( transform.b.a * point.b ) + ( transform.c.a * point.c ) + ( transform.d.a );
			result.b = ( transform.a.b * point.a ) + ( transform.b.b * point.b ) + ( transform.c.b * point.c ) + ( transform.d.b );
			result.c = ( transform.a.c * point.a ) + ( transform.b.c * point.b ) + ( transform.c.c * point.c ) + ( transform.d.c );
			result.d = ( transform.a.d * point.a ) + ( transform.b.d * point.b ) + ( transform.c.d * point.c ) + ( transform.d.d );
			return result;
		}

		vector64x3_c rotate_scale_and_translate_vector64x3( vector64x3_c const & point, transform3d_c const & transform )
		{
			vector64x3_c result;
			result = rotate_vector64x3( point, transform.rotation ) * transform.scale + transform.position;
			return result;
		}

		vector32x3_c constrained_normal_vector32x3( vector32x3_c const & normal, vector32x3_c const & normal_target, float32_c const maximum_angle )
		{
			float32_c angle = math_arc_cosine( math_clamp( dot_product_float32( normal, normal_target ), -1.0f, 1.0f ) );
			if ( angle > maximum_angle )
			{
				return rotate_vector32x3( normal_target, cross_product_vector32x3( normal, normal_target ), maximum_angle );
			}
			return normal;
		}

		vector64x3_c constrained_normal_vector64x3( vector64x3_c const & normal, vector64x3_c const & normal_target, float64_c const maximum_angle )
		{
			float64_c angle = math_arc_cosine( math_clamp( dot_product_float64( normal, normal_target ), -1.0, 1.0 ) );
			if ( angle > maximum_angle )
			{
				return rotate_vector64x3( normal_target, cross_product_vector64x3( normal, normal_target ), maximum_angle );
			}
			return normal;
		}

		vector64x3_c reflected_vector64x3( vector64x3_c const & direction_vector, vector64x3_c const & normal_vector )
		{
			return direction_vector - ( normal_vector * ( dot_product_float64( direction_vector, normal_vector ) * 2.0 ) );
		}

		vector64x3_c reflected_vector64x3( vector64x3_c const & point, plane64_c const & plane )
		{
			return point + ( plane.get_normal() * ( distance_between_point_and_plane( point , plane ) * -2.0 ) );
		}

		vector64x3_c parallel_component_vector64x3( vector64x3_c const & normal_vector, vector64x3_c const & other_vector )
		{
			return normal_vector * dot_product_float64( normal_vector, other_vector );
		}

		vector64x3_c perpendicular_component_vector64x3( vector64x3_c const & normal_vector, vector64x3_c const & other_vector )
		{
			return other_vector - parallel_component_vector64x3( normal_vector, other_vector );
		}

		vector32x3_c parallel_component_vector32x3( vector32x3_c const & normal_vector, vector32x3_c const & other_vector )
		{
			return normal_vector * dot_product_float32( normal_vector, other_vector );
		}

		vector32x3_c perpendicular_component_vector32x3( vector32x3_c const & normal_vector, vector32x3_c const & other_vector )
		{
			return other_vector - parallel_component_vector32x3( normal_vector, other_vector );
		}


		//
		//
		// matrix, quaternion, and angle conversions.
		//
		//

		vector32x3_c basis_vector32x3_x_from_rotation_quaternion32( quaternion32_c const & rotation )
		{
			vector32x3_c result;
			result.a = ( 1.0f - 2.0f * rotation.b * rotation.b - 2.0f * rotation.c * rotation.c );
			result.b = (        2.0f * rotation.a * rotation.b + 2.0f * rotation.c * rotation.d );
			result.c = (        2.0f * rotation.a * rotation.c - 2.0f * rotation.b * rotation.d );
			return result;
		}

		vector32x3_c basis_vector32x3_y_from_rotation_quaternion32( quaternion32_c const & rotation )
		{
			vector32x3_c result;
			result.a = (        2.0f * rotation.a * rotation.b - 2.0f * rotation.c * rotation.d );
			result.b = ( 1.0f - 2.0f * rotation.a * rotation.a - 2.0f * rotation.c * rotation.c );
			result.c = (        2.0f * rotation.c * rotation.b + 2.0f * rotation.a * rotation.d );
			return result;
		}

		vector32x3_c basis_vector32x3_z_from_rotation_quaternion32( quaternion32_c const & rotation )
		{
			vector32x3_c result;
			result.a = (        2.0f * rotation.a * rotation.c + 2.0f * rotation.b * rotation.d );
			result.b = (        2.0f * rotation.c * rotation.b - 2.0f * rotation.a * rotation.d );
			result.c = ( 1.0f - 2.0f * rotation.a * rotation.a - 2.0f * rotation.b * rotation.b );
			return result;
		}

		inline matrix32x4x4_c make_matrix32x4x4_basis_from_quaternion32_blender( quaternion32_c const & rotation )
		{
			// taken from blender, math_rotation.c, quat_to_mat4.
			// this isn't used right now, but it's here for reference.
			matrix32x4x4_c result;
			float64_c q0 = constants< float64_c >::square_root_of_2() * static_cast< float64_c >( rotation.d );
			float64_c q1 = constants< float64_c >::square_root_of_2() * static_cast< float64_c >( rotation.a );
			float64_c q2 = constants< float64_c >::square_root_of_2() * static_cast< float64_c >( rotation.b );
			float64_c q3 = constants< float64_c >::square_root_of_2() * static_cast< float64_c >( rotation.c );
			float64_c qda = q0 * q1;
			float64_c qdb = q0 * q2;
			float64_c qdc = q0 * q3;
			float64_c qaa = q1 * q1;
			float64_c qab = q1 * q2;
			float64_c qac = q1 * q3;
			float64_c qbb = q2 * q2;
			float64_c qbc = q2 * q3;
			float64_c qcc = q3 * q3;
			result.a.a = static_cast< float32_c >(  1.0 - qbb - qcc );
			result.a.b = static_cast< float32_c >(  qdc + qab );
			result.a.c = static_cast< float32_c >( -qdb + qac );
			result.b.a = static_cast< float32_c >( -qdc + qab );
			result.b.b = static_cast< float32_c >(  1.0 - qaa - qcc );
			result.b.c = static_cast< float32_c >(  qda + qbc );
			result.c.a = static_cast< float32_c >(  qdb + qac );
			result.c.b = static_cast< float32_c >( -qda + qbc );
			result.c.c = static_cast< float32_c >(  1.0 - qaa - qbb );
			return result;
		}

		inline matrix32x4x4_c make_matrix32x4x4_basis_from_quaternion32_irrlicht( quaternion32_c const & rotation )
		{
			// taken from iirlicht.
			// this isn't used right now, but it's here for reference.
			matrix32x4x4_c result;
			result.a.a = ( 1.0f - 2.0f * rotation.b * rotation.b - 2.0f * rotation.c * rotation.c );
			result.a.b = ( 2.0f * rotation.a * rotation.b + 2.0f * rotation.c * rotation.d );
			result.a.c = ( 2.0f * rotation.a * rotation.c - 2.0f * rotation.b * rotation.d );
			result.b.a = ( 2.0f * rotation.a * rotation.b - 2.0f * rotation.c * rotation.d );
			result.b.b = ( 1.0f - 2.0f * rotation.a * rotation.a - 2.0f * rotation.c * rotation.c );
			result.b.c = ( 2.0f * rotation.c * rotation.b + 2.0f * rotation.a * rotation.d );
			result.c.a = ( 2.0f * rotation.a * rotation.c + 2.0f * rotation.b * rotation.d );
			result.c.b = ( 2.0f * rotation.c * rotation.b - 2.0f * rotation.a * rotation.d );
			result.c.c = ( 1.0f - 2.0f * rotation.a * rotation.a - 2.0f * rotation.b * rotation.b );
			return result;
		}

		matrix32x4x4_c transform_matrix32x4x4_c_from_transform3d( transform3d_c const & transform )\
		{
			matrix32x4x4_c result = basis_matrix32x4x4_from_rotation_quaternion32( transform.rotation );
			result.d.a = static_cast< float32_c >( transform.position.a );
			result.d.b = static_cast< float32_c >( transform.position.b );
			result.d.c = static_cast< float32_c >( transform.position.c );
			return result;
		}

		matrix32x4x4_c transform_matrix32x4x4_c_from_transform3d( transform3d_c const & transform, vector64x3_c const & relative_to_origin )
		{
			matrix32x4x4_c result = basis_matrix32x4x4_from_rotation_quaternion32( transform.rotation );
			result.d.a = static_cast< float32_c >( transform.position.a - relative_to_origin.a );
			result.d.b = static_cast< float32_c >( transform.position.b - relative_to_origin.b );
			result.d.c = static_cast< float32_c >( transform.position.c - relative_to_origin.c );
			return result;
		}

		matrix32x4x4_c basis_matrix32x4x4_from_rotation_quaternion32( quaternion32_c const & rotation )
		{
			return make_matrix32x4x4_basis_from_quaternion32_blender( rotation );
		}

		inline matrix32x3x3_c basis_matrix32x3x3_from_rotation_quaternion32_blender( quaternion32_c const & rotation )
		{
			// taken from blender, math_rotation.c, quat_to_mat4.
			matrix32x3x3_c result;
			float64_c q0 = constants< float64_c >::square_root_of_2() * static_cast< float64_c >( rotation.d );
			float64_c q1 = constants< float64_c >::square_root_of_2() * static_cast< float64_c >( rotation.a );
			float64_c q2 = constants< float64_c >::square_root_of_2() * static_cast< float64_c >( rotation.b );
			float64_c q3 = constants< float64_c >::square_root_of_2() * static_cast< float64_c >( rotation.c );
			float64_c qda = q0 * q1;
			float64_c qdb = q0 * q2;
			float64_c qdc = q0 * q3;
			float64_c qaa = q1 * q1;
			float64_c qab = q1 * q2;
			float64_c qac = q1 * q3;
			float64_c qbb = q2 * q2;
			float64_c qbc = q2 * q3;
			float64_c qcc = q3 * q3;
			result.a.a = static_cast< float32_c >(  1.0 - qbb - qcc );
			result.a.b = static_cast< float32_c >(  qdc + qab );
			result.a.c = static_cast< float32_c >( -qdb + qac );
			result.b.a = static_cast< float32_c >( -qdc + qab );
			result.b.b = static_cast< float32_c >(  1.0 - qaa - qcc );
			result.b.c = static_cast< float32_c >(  qda + qbc );
			result.c.a = static_cast< float32_c >(  qdb + qac );
			result.c.b = static_cast< float32_c >( -qda + qbc );
			result.c.c = static_cast< float32_c >(  1.0 - qaa - qbb );
			return result;
		}

		inline matrix32x3x3_c basis_matrix32x3x3_from_rotation_quaternion32_irrlicht( quaternion32_c const & rotation )
		{
			// quaternion must be normalized already.
			// taken from iirlich, quaternion.h, inline void quaternion::getMatrixFast( matrix4 &dest) const.
			matrix32x3x3_c result;
			result.a.a = ( 1.0f - 2.0f * rotation.b * rotation.b - 2.0f * rotation.c * rotation.c );
			result.a.b = ( 2.0f * rotation.a * rotation.b + 2.0f * rotation.c * rotation.d );
			result.a.c = ( 2.0f * rotation.a * rotation.c - 2.0f * rotation.b * rotation.d );
			result.b.a = ( 2.0f * rotation.a * rotation.b - 2.0f * rotation.c * rotation.d );
			result.b.b = ( 1.0f - 2.0f * rotation.a * rotation.a - 2.0f * rotation.c * rotation.c );
			result.b.c = ( 2.0f * rotation.c * rotation.b + 2.0f * rotation.a * rotation.d );
			result.c.a = ( 2.0f * rotation.a * rotation.c + 2.0f * rotation.b * rotation.d );
			result.c.b = ( 2.0f * rotation.c * rotation.b - 2.0f * rotation.a * rotation.d );
			result.c.c = ( 1.0f - 2.0f * rotation.a * rotation.a - 2.0f * rotation.b * rotation.b );
			return result;
		}

		inline quaternion32_c rotation_quaternion32_from_basis_matrix32x3x3_blender( matrix32x3x3_c const & basis )
		{
			// basis must be normalized already.
			// taken from blender, math_rotation.c, mat3_normalized_to_quat.
			quaternion32_c result;
			float64_c s;
			float64_c tr = 0.25 * static_cast< float64_c >( 1.0f + basis.a.a + basis.b.b + basis.c.c );
			if ( tr > 1e-4 )
			{
				s = math_square_root( tr );
				result.d = static_cast< float32_c >( s );
				s = 1.0 / ( 4.0 * s );
				result.a = static_cast< float32_c >( static_cast< float64_c >( basis.b.c - basis.c.b ) * s );
				result.b = static_cast< float32_c >( static_cast< float64_c >( basis.c.a - basis.a.c ) * s );
				result.c = static_cast< float32_c >( static_cast< float64_c >( basis.a.b - basis.b.a ) * s );
			}
			else
			{
				if ( basis.a.a > basis.b.b && basis.a.a > basis.c.c )
				{
					s = 2.0f * math_square_root( 1.0f + basis.a.a - basis.b.b - basis.c.c );
					result.a = static_cast< float32_c >( 0.25 * s );
					s = 1.0 / s;
					result.d = static_cast< float32_c >( static_cast< float64_c >( basis.b.c - basis.c.b ) * s );
					result.b = static_cast< float32_c >( static_cast< float64_c >( basis.b.a + basis.a.b ) * s );
					result.c = static_cast< float32_c >( static_cast< float64_c >( basis.c.a + basis.a.c ) * s );
				}
				else if ( basis.b.b > basis.c.c )
				{
					s = 2.0f * math_square_root( 1.0f + basis.b.b - basis.a.a - basis.c.c );
					result.b = static_cast< float32_c >( 0.25 * s );
					s = 1.0 / s;
					result.d = static_cast< float32_c >( static_cast< float64_c >( basis.c.a - basis.a.c ) * s );
					result.a = static_cast< float32_c >( static_cast< float64_c >( basis.b.a + basis.a.b ) * s );
					result.c = static_cast< float32_c >( static_cast< float64_c >( basis.c.b + basis.b.c ) * s );
				}
				else
				{
					s = 2.0f * math_square_root( 1.0f + basis.c.c - basis.a.a - basis.b.b );
					result.c = static_cast< float32_c >( 0.25 * s );
					s = 1.0 / s;
					result.d = static_cast< float32_c >( static_cast< float64_c >( basis.a.b - basis.b.a ) * s );
					result.a = static_cast< float32_c >( static_cast< float64_c >( basis.c.a + basis.a.c ) * s );
					result.b = static_cast< float32_c >( static_cast< float64_c >( basis.c.b + basis.b.c ) * s );
				}
			}
			result = normal_quaternion32( result );
			return result;
		}

		inline quaternion32_c rotation_quaternion32_from_basis_matrix32x3x3_irrlicht( matrix32x3x3_c const & basis )
		{
			// basis does not need to be normalized.
			// taken from irrlicht, quaternion.h, inline quaternion& quaternion::operator=(const matrix4& m).
			quaternion32_c result;

			float32_c diagonal = basis.a.a + basis.b.b + basis.c.c + 1.0f;

			if( diagonal > 0.0f )
			{
				float32_c scale = sqrtf( diagonal ) * 2.0f; // get scale from diagonal

				// TODO: speed this up
				result.a = ( basis.b.c - basis.c.b ) / scale;
				result.b = ( basis.c.a - basis.a.c ) / scale;
				result.c = ( basis.a.b - basis.b.a ) / scale;
				result.d = 0.25f * scale;
			}
			else
			{
				if ( basis.a.a > basis.b.b && basis.a.a > basis.c.c )
				{
					// 1st element of diag is greatest value
					// find scale according to 1st element, and double it
					float32_c scale = sqrtf( 1.0f + basis.a.a - basis.b.b - basis.c.c ) * 2.0f;

					// TODO: speed this up
					result.a = 0.25f * scale;
					result.b = ( basis.b.a + basis.a.b ) / scale;
					result.c = ( basis.a.c + basis.c.a ) / scale;
					result.d = ( basis.b.c - basis.c.b ) / scale;
				}
				else if ( basis.b.b > basis.c.c )
				{
					// 2nd element of diag is greatest value
					// find scale according to 2nd element, and double it
					float32_c scale = sqrtf( 1.0f + basis.b.b - basis.a.a - basis.c.c ) * 2.0f;

					// TODO: speed this up
					result.a = ( basis.b.a + basis.a.b ) / scale;
					result.b = 0.25f * scale;
					result.c = ( basis.c.b + basis.b.c ) / scale;
					result.d = ( basis.c.a - basis.a.c ) / scale;
				}
				else
				{
					// 3rd element of diag is greatest value
					// find scale according to 3rd element, and double it
					float32_c scale = sqrtf( 1.0f + basis.c.c - basis.a.a - basis.b.b ) * 2.0f;

					// TODO: speed this up
					result.a = ( basis.c.a + basis.a.c ) / scale;
					result.b = ( basis.c.b + basis.b.c ) / scale;
					result.c = 0.25f * scale;
					result.d = ( basis.a.b - basis.b.a ) / scale;
				}
			}

			ops::normalize_quaternion32( result );

			return result;
		}

		matrix32x3x3_c basis_matrix32x3x3_from_rotation_quaternion32( quaternion32_c const & rotation )
		{
			return basis_matrix32x3x3_from_rotation_quaternion32_blender( rotation );
		}

		quaternion32_c rotation_quaternion32_from_basis_matrix32x3x3( matrix32x3x3_c const & basis )
		{
			return rotation_quaternion32_from_basis_matrix32x3x3_blender( basis );
		}

		matrix32x3x3_c basis_matrix32x3x3_from_axis_angle( vector32x4_c const & axis_angle )
		{
			matrix32x3x3_c basis;
			float32_c v1 = axis_angle.a;
			float32_c v2 = axis_angle.b;
			float32_c v3 = axis_angle.c;
			float32_c v1s = v1 * v1;
			float32_c v2s = v2 * v2;
			float32_c v3s = v3 * v3;
			float32_c c = math_cosine( axis_angle.d );
			float32_c s = math_sine( axis_angle.d );
			basis.a.a = ( v1s + ( ( 1.0f - v1s ) * c ) );
			basis.a.b = ( ( ( v1 * v2 ) * ( 1.0f - c ) ) + ( v3 * s ) );
			basis.a.c = ( ( ( v1 * v3 ) * ( 1.0f - c ) ) - ( v2 * s ) );
			basis.b.a = ( ( ( v1 * v2 ) * ( 1.0f - c ) ) - ( v3 * s ) );
			basis.b.b = ( v2s + ( ( 1.0f - v2s ) * c ) );
			basis.b.c = ( ( ( v2 * v3 ) * ( 1.0f - c ) ) + ( v1 * s ) );
			basis.c.a = ( ( ( v1 * v3 ) * ( 1.0f - c ) ) + ( v2 * s ) );
			basis.c.b = ( ( ( v2 * v3 ) * ( 1.0f - c ) ) - ( v1 * s ) );
			basis.c.c = ( v3s + ( ( 1.0f - v3s ) * c ) );
			return basis;
		}

		matrix32x3x3_c joint_rotate_basis_matrix32x3x3_y( matrix32x3x3_c basis, vector32x3_c const & basis_y_target )
		{
			matrix32x3x3_c result;
			float32_c basis_y_length = length_float32( basis.b );
			vector32x3_c basis_y_normal = basis.b / basis_y_length;
			float32_c d = dot_product_float32( basis_y_normal, basis_y_target );
			float32_c angle = math_arc_cosine( d );
			if ( angle > constants< float32_c >::angle_near_zero() )
			{
				float32_c s = math_sine( angle );
				float32_c c = math_cosine( angle );
				vector32x3_c axis = cross_product_vector32x3( basis_y_target, basis_y_normal );
				result.a = ( ( basis.a * c ) + ( axis * dot_product_float32( axis, basis.a ) * ( 1.0f - c ) ) + ( cross_product_vector32x3( basis.a, axis ) * s ) );
				result.b = basis_y_target * basis_y_length;
				result.b = ( ( basis.c * c ) + ( axis * dot_product_float32( axis, basis.c ) * ( 1.0f - c ) ) + ( cross_product_vector32x3( basis.c, axis ) * s ) );
			}
			return result;
		}

		matrix32x3x3_c joint_rotate_basis_matrix32x3x3_z( matrix32x3x3_c basis, vector32x3_c const & basis_z_target )
		{
			matrix32x3x3_c result;
			float32_c basis_z_length = length_float32( basis.c );
			vector32x3_c basis_z_normal = basis.c / basis_z_length;
			float32_c d = dot_product_float32( basis_z_normal, basis_z_target );
			float32_c angle = math_arc_cosine( d );
			if ( angle > constants< float32_c >::angle_near_zero() )
			{
				float32_c s = math_sine( angle );
				float32_c c = math_cosine( angle );
				vector32x3_c axis = cross_product_vector32x3( basis_z_target, basis_z_normal );
				result.a = ( ( basis.a * c ) + ( axis * dot_product_float32( axis, basis.a ) * ( 1.0f - c ) ) + ( cross_product_vector32x3( basis.a, axis ) * s ) );
				result.b = ( ( basis.b * c ) + ( axis * dot_product_float32( axis, basis.b ) * ( 1.0f - c ) ) + ( cross_product_vector32x3( basis.b, axis ) * s ) );
				result.c = basis_z_target * basis_z_normal;
			}
			return result;
		}

		matrix32x3x3_c transpose_matrix32x3x3( matrix32x3x3_c const & matrix )
		{
			matrix32x3x3_c result;
			result.a.a = matrix.a.a;
			result.a.b = matrix.b.a;
			result.a.c = matrix.c.a;
			result.b.a = matrix.a.b;
			result.b.b = matrix.b.b;
			result.b.c = matrix.c.b;
			result.c.a = matrix.a.c;
			result.c.b = matrix.b.c;
			result.c.c = matrix.c.c;
			return result;
		}

		matrix32x4x4_c view_matrix32x4x4_from_look_at( vector32x3_c const & position, vector32x3_c const & forward, vector32x3_c const & up )
		{
			matrix32x3x3_c basis;
			basis.c = -normal_vector32x3( forward ); // z-axis, backwards, relative to viewer's perspective.
			basis.a = normal_vector32x3( cross_product_vector32x3( up, basis.c ) ); // x-axis, right, relative to viewer's perspective.
			basis.b = cross_product_vector32x3( basis.c, basis.a); // y-axis, down, relative to viewer's perspective.
			matrix32x4x4_c result;
			result.a.a = basis.a.a;
			result.a.b = basis.b.a;
			result.a.c = basis.c.a;
			result.a.d = 0.0f;
			result.b.a = basis.a.b;
			result.b.b = basis.b.b;
			result.b.c = basis.c.b;
			result.b.d = 0.0f;
			result.c.a = basis.a.c;
			result.c.b = basis.b.c;
			result.c.c = basis.c.c;
			result.c.d = 0.0f;
			result.d.a = -dot_product_float32( basis.a, position );
			result.d.b = -dot_product_float32( basis.b, position );
			result.d.c = -dot_product_float32( basis.c, position );
			result.d.d = 1.0f;
			return result;
		}

		matrix32x4x4_c projection_orthographic_matrix32x4x4( float32_c const width, float32_c const height, float32_c const clip_near, float32_c const clip_far )
		{
			matrix32x4x4_c result;
			result.a.a = 2.0f / width;
			result.a.b = 0.0;
			result.a.c = 0.0;
			result.a.d = 0.0;
			result.b.a = 0.0;
			result.b.b = 2.0f / height;
			result.b.c = 0.0;
			result.b.d = 0.0;
			result.c.a = 0.0;
			result.c.b = 0.0;
			result.c.c = 1.0f / ( clip_near - clip_far );
			result.c.d = 0.0;
			result.d.a = 0.0;
			result.d.b = 0.0;
			result.d.c = clip_near / ( clip_near - clip_far );
			result.d.d = 1.0f;
			return result;
		}

		matrix32x4x4_c projection_orthographic_off_center_matrix32x4x4_a( float32_c const left, float32_c const right, float32_c const top, float32_c const bottom, float32_c const clip_near, float32_c const clip_far )
		{
			matrix32x4x4_c result;
			result.a.a = 2.0f / ( right - left );
			result.a.b = 0.0f;
			result.a.c = 0.0f;
			result.a.d = 0.0f;
			result.b.a = 0.0f;
			result.b.b = 2.0f / ( bottom - top ); // ( top - bottom );
			result.b.c = 0.0f;
			result.b.d = 0.0f;
			result.c.a = 0.0f;
			result.c.b = 0.0f;
			result.c.c = 1.0f / ( clip_near - clip_far );
			result.c.d = 0.0f;
			result.d.a = ( left + right ) / ( left - right );
			result.d.b = ( top + bottom ) / ( top - bottom ); // ( bottom + top ) / ( bottom - top );
			result.d.c = clip_near / ( clip_near - clip_far );
			result.d.d = 1.0f;
			return result;
		}

		matrix32x4x4_c projection_orthographic_off_center_matrix32x4x4_b( float32_c const width, float32_c const height, float32_c const center_x, float32_c const center_y, float32_c clip_near, float32_c clip_far )
		{
			float32_c left = ( width * -0.5f ) - ( center_x * width * 0.5f );
			float32_c right = ( width * 0.5f ) - ( center_x * width * 0.5f );
			float32_c top = ( height * -0.5f ) - ( center_y * height * 0.5f );
			float32_c bottom = ( height * 0.5f ) - ( center_y * height * 0.5f );
			return projection_orthographic_off_center_matrix32x4x4_a( left, right, top, bottom, clip_near, clip_far );
		}

		matrix32x4x4_c projection_perspective_matrix32x4x4( float32_c const field_of_view, float32_c const aspect_ratio, float32_c const clip_near, float32_c const clip_far )
		{
			float32_c dn = 0.0f;
			float32_c df = 1.0f;
			float32_c s = 1.0f / math_tangent( field_of_view / 2.0f );
			matrix32x4x4_c result;
			result.a.a = s / aspect_ratio;
			result.a.b = 0.0f;
			result.a.c = 0.0f;
			result.a.d = 0.0f;
			result.b.a = 0.0f;
			result.b.b = s;
			result.b.c = 0.0f;
			result.b.d = 0.0f;
			result.c.a = 0.0f;
			result.c.b = 0.0f;
			result.c.c = ( df * clip_far - dn * clip_near ) / ( clip_near - clip_far );
			result.c.d = -1.0f;
			result.d.a = 0.0f;
			result.d.b = 0.0f;
			result.d.c = ( df - dn ) * ( clip_near * clip_far ) / ( clip_near - clip_far );
			result.d.d = 0.0f;
			return result;
		}

		matrix32x4x4_c projection_perspective_off_center_matrix32x4x4_a( float32_c const left, float32_c const right, float32_c const top, float32_c bottom, float32_c const clip_near, float32_c const clip_far )
		{
			matrix32x4x4_c result;
			result.a.a = ( 2.0f * clip_near ) / ( right - left );
			result.a.b = 0.0f;
			result.a.c = 0.0f;
			result.a.d = 0.0f;
			result.b.a = 0.0f;
			result.b.b = ( 2.0f * clip_near ) / ( bottom - top ); // ( top - bottom );
			result.b.c = 0.0f;
			result.b.d = 0.0f;
			result.c.a = ( left + right ) / ( right - left );
			result.c.b = ( bottom + top ) / ( bottom - top ); // ( top + bottom ) / ( top - bottom );
			result.c.c = clip_far / ( clip_near - clip_far );
			result.c.d = -1.0f;
			result.d.a = 0.0f;
			result.d.b = 0.0f;
			result.d.c = ( clip_near * clip_far ) / ( clip_near - clip_far );
			result.d.d = 0.0f;
			return result;
		}

		matrix32x4x4_c projection_perspective_off_center_matrix32x4x4_b( float32_c const field_of_view, float32_c const aspect_ratio, float32_c center_x, float32_c center_y, float32_c clip_near, float32_c clip_far )
		{
			float32_c s = 1.0f / math_tangent( field_of_view / 2.0f );
			float32_c left = -s / aspect_ratio;
			float32_c right = s / aspect_ratio;
			float32_c top = -s;
			float32_c bottom = s;
			float32_c offset_x = ( right - left ) * center_x;
			float32_c offset_y = ( bottom - top ) * center_y;
			return projection_perspective_off_center_matrix32x4x4_a( left, right, top, bottom, clip_near, clip_far );
		}

		matrix32x4x4_c transpose_matrix32x4x4( matrix32x4x4_c const & matrix )
		{
			matrix32x4x4_c result;
			result.a.a = matrix.a.a;
			result.a.b = matrix.b.a;
			result.a.c = matrix.c.a;
			result.a.d = matrix.d.a;
			result.b.a = matrix.a.b;
			result.b.b = matrix.b.b;
			result.b.c = matrix.c.b;
			result.b.d = matrix.d.b;
			result.c.a = matrix.a.c;
			result.c.b = matrix.b.c;
			result.c.c = matrix.c.c;
			result.c.d = matrix.d.c;
			result.d.a = matrix.a.d;
			result.d.b = matrix.b.d;
			result.d.c = matrix.c.d;
			result.d.d = matrix.d.d;
			return result;
		}

		matrix32x4x4_c invert_matrix32x4x4_fast( matrix32x4x4_c const & matrix )
		{
			matrix32x4x4_c result;
			result.a.a = matrix.a.a;
			result.a.b = matrix.b.a;
			result.a.c = matrix.c.a;
			result.b.a = matrix.a.b;
			result.b.b = matrix.b.b;
			result.b.c = matrix.c.b;
			result.c.a = matrix.a.c;
			result.c.b = matrix.b.c;
			result.c.c = matrix.c.c;
			result.d.a = -matrix.d.a;
			result.d.b = -matrix.d.b;
			result.d.c = -matrix.d.c;
			result.d.d = matrix.d.d;
			return result;
		}

		matrix32x2x2_c invert_matrix32x2x2( matrix32x2x2_c const & matrix )
		{
			float32_c matrix_determinant = ( matrix.a.a * matrix.b.b ) - ( matrix.a.b * matrix.b.a );
			return matrix32x2x2_c( matrix.a.a / matrix_determinant, matrix.a.b / -matrix_determinant, matrix.b.a / -matrix_determinant, matrix.b.b / matrix_determinant );
		}

		matrix32x2x2_c basis_matrix32x2x2_from_angle_scale( float32_c const angle, float32_c const scale )
		{
			float32_c c = math_cosine( angle ) * scale;
			float32_c s = math_sine( angle ) * scale;
			return matrix32x2x2_c( c, s, -s, c );
		}

		quaternion32_c invert_quaternion32( quaternion32_c const & quaternion )
		{
			quaternion32_c result;
			// if quaternion is always supposed to be unit length, then we don't need to divide by length squared because it will be equal to 1.
			//float32_c length_squared = quaternion.a * quaternion.a + quaternion.b * quaternion.b + quaternion.c * quaternion.c + quaternion.d * quaternion.d;
			//result.a = -quaternion.a / length_squared;
			//result.b = -quaternion.b / length_squared;
			//result.c = -quaternion.c / length_squared;
			//result.d =  quaternion.d / length_squared;
			result.a = -quaternion.a;
			result.b = -quaternion.b;
			result.c = -quaternion.c;
			result.d =  quaternion.d;
			return result;
		}

		quaternion32_c rotation_quaternion32_from_axis_angle( vector32x4_c const & axis_angle )
		{
			quaternion32_c rotation;
			float32_c half_angle = axis_angle.d / 2.0f;
			float32_c half_angle_sine = math_sine( half_angle );
			rotation.a = half_angle_sine * axis_angle.a;
			rotation.b = half_angle_sine * axis_angle.b;
			rotation.c = half_angle_sine * axis_angle.c;
			rotation.d = math_cosine( half_angle );
			return rotation;
		}

		quaternion32_c rotation_quaternion32_from_look_at2( vector32x3_c const & forward, vector32x3_c const & up )
		{
			matrix32x3x3_c basis;
			basis.a = normal_vector32x3( cross_product_vector32x3( forward, up ) ); // x-axis right.
			basis.b = normal_vector32x3( forward ); // y-axis forward.
			basis.c = cross_product_vector32x3( basis.a, basis.b ); // z-axis up.
			return rotation_quaternion32_from_basis_matrix32x3x3( basis );
		}

		vector32x4_c axis_angle_from_rotation_quaternion32( quaternion32_c const & rotation )
		{
			vector32x4_c axis_angle;
			axis_angle.a = rotation.a;
			axis_angle.b = rotation.b;
			axis_angle.c = rotation.c;
			axis_angle.d = 2.0f * math_arc_cosine( rotation.d );
			return axis_angle;
		}

		quaternion32_c rotation_quaternion32_from_euler_angles( vector32x3_c const & euler_angles )
		{
			// taken from iirlicht
			// thank you
			quaternion32_c result;
			float64_c angle;
			angle = euler_angles.a * 0.5;
			float64_c sr = math_sine( angle );
			float64_c cr = math_cosine( angle );
			angle = euler_angles.b * 0.5;
			float64_c sp = math_sine( angle );
			float64_c cp = math_cosine( angle );
			angle = euler_angles.c * 0.5;
			float64_c sy = math_sine( angle );
			float64_c cy = math_cosine( angle );
			float64_c cpcy = cp * cy;
			float64_c spcy = sp * cy;
			float64_c cpsy = cp * sy;
			float64_c spsy = sp * sy;
			result.a = static_cast< float32_c >( sr * cpcy - cr * spsy );
			result.b = static_cast< float32_c >( cr * spcy + sr * cpsy );
			result.c = static_cast< float32_c >( cr * cpsy - sr * spcy );
			result.d = static_cast< float32_c >( cr * cpcy + sr * spsy );
			return result;
		}

		vector32x3_c euler_angles_from_rotation_quaternion32( quaternion32_c const & rotation )
		{
			// taken from iirlicht
			// thank you
			vector32x3_c result;
			vector64x4_c rotation64( static_cast< float64_c >( rotation.a ), static_cast< float64_c >( rotation.b ), static_cast< float64_c >( rotation.c ), static_cast< float64_c >( rotation.d ) );
			float64_c sqx = rotation64.a * rotation64.a;
			float64_c sqy = rotation64.b * rotation64.b;
			float64_c sqz = rotation64.c * rotation64.c;
			float64_c sqw = rotation64.d * rotation64.d;
			float64_c test = 2.0 * ( rotation64.b * rotation64.d - rotation64.a * rotation64.c );
			if ( math_compare_close( test, 1.0, 0.000001 ) )
			{
				result.c = static_cast< float32_c >( -2.0 * math_arc_tangent( rotation64.a, rotation64.d ) ); // heading = rotation about z-axis
				result.a =  0.0f; // bank = rotation about x-axis
				result.b = constants< float32_c >::pi() * 0.5f; // attitude = rotation about y-axis
			}
			else if ( math_compare_close( test, -1.0, 0.000001 ) )
			{
				result.c = static_cast< float32_c >( 2.0 * math_arc_tangent( rotation64.a, rotation64.d ) ); // heading = rotation about z-axis
				result.a = 0.0f; // bank = rotation about x-axis
				result.b = constants< float32_c >::pi() * -0.5f; // attitude = rotation about y-axis
			}
			else
			{
				result.c = static_cast< float32_c >( math_arc_tangent( 2.0 * ( rotation64.a * rotation64.b + rotation64.c * rotation64.d ), (  sqx - sqy - sqz + sqw ) ) ); // heading = rotation about z-axis
				result.a = static_cast< float32_c >( math_arc_tangent( 2.0 * ( rotation64.b * rotation64.c + rotation64.a * rotation64.d ), ( -sqx - sqy + sqz + sqw ) ) ); // bank = rotation about x-axis
				result.b = static_cast< float32_c >( math_arc_sine( math_clamp( test, -1.0, 1.0 ) ) ); // attitude = rotation about y-axis
			}
			return result;
		}

	}
}
