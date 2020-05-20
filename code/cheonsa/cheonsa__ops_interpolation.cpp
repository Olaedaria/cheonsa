#include "cheonsa__ops_interpolation.h"
#include "cheonsa__ops_math.h"
#include "cheonsa__ops_vector.h"

namespace cheonsa
{
	namespace ops
	{

		float32_c interpolate_linear( float32_c const source, float32_c const destination, float32_c const amount )
		{
			return ( source * ( 1.0f - amount ) ) + ( destination * amount );
		}

		vector32x2_c interpolate_linear( vector32x2_c const & source, vector32x2_c const & destination, float32_c const amount )
		{
			return ( source * ( 1.0f - amount ) ) + ( destination * amount );
		}

		vector32x3_c interpolate_linear( vector32x3_c const & source, vector32x3_c const & destination, float32_c const amount )
		{
			return ( source * ( 1.0f - amount ) ) + ( destination * amount );
		}

		vector32x4_c interpolate_linear( vector32x4_c const & source, vector32x4_c const & destination, float32_c const amount )
		{
			return ( source * ( 1.0f - amount ) ) + ( destination * amount );
		}

		float64_c interpolate_linear( float64_c const source, float64_c const destination, float64_c const amount )
		{
			return ( source * ( 1.0 - amount ) ) + ( destination * amount );
		}

		vector64x2_c interpolate_linear( vector64x2_c const & source, vector64x2_c const & destination, float64_c const amount )
		{
			return ( source * ( 1.0 - amount ) ) + ( destination * amount );
		}

		vector64x3_c interpolate_linear( vector64x3_c const & source, vector64x3_c const & destination, float64_c const amount )
		{
			return ( source * ( 1.0f - amount ) ) + ( destination * amount );
		}

		vector64x4_c interpolate_linear( vector64x4_c const & source, vector64x4_c const & destination, float64_c const amount )
		{
			return ( source * ( 1.0f - amount ) ) + ( destination * amount );
		}

		float32_c interpolate_cosine( float32_c const source, float32_c const destination, float32_c const amount )
		{
			float32_c ratio = ( 1.0f - cos( amount * constants< float32_c >::pi() ) ) / 2.0f;
			return ( source * ( 1.0f - ratio ) ) + ( destination * ratio );
		}

		vector32x2_c interpolate_cosine( vector32x2_c const & source, vector32x2_c const & destination, float32_c const amount )
		{
			float32_c ratio = ( 1.0f - cos( amount * constants< float32_c >::pi() ) ) / 2.0f;
			return ( source * ( 1.0f - ratio ) ) + ( destination * ratio );
		}

		vector32x3_c interpolate_cosine( vector32x3_c const & source, vector32x3_c const & destination, float32_c const amount )
		{
			float32_c ratio = ( 1.0f - cos( amount * constants< float32_c >::pi() ) ) / 2.0f;
			return ( source * ( 1.0f - ratio ) ) + ( destination * ratio );
		}

		vector32x4_c interpolate_cosine( vector32x4_c const & source, vector32x4_c const & destination, float32_c const amount )
		{
			float32_c ratio = ( 1.0f - cos( amount * constants< float32_c >::pi() ) ) / 2.0f;
			return ( source * ( 1.0f - ratio ) ) + ( destination * ratio );
		}

		float32_c interpolate_cubic( float32_c const previous_source, float32_c const source, float32_c const destination, float32_c const next_destination, float32_c const amount )
		{
			float32_c amount_squared = amount * amount;
			float32_c a0 = next_destination - destination - previous_source + source;
			float32_c a1 = previous_source - source - a0;
			float32_c a2 = destination - previous_source;
			float32_c a3 = source;
			return ( a0 * amount * amount_squared ) + ( a1 * amount_squared ) + ( a2 * amount ) + ( a3 );
		}

		vector32x2_c interpolate_cubic( vector32x2_c const & previous_source, vector32x2_c const & source, vector32x2_c const & destination, vector32x2_c const & next_destination, float32_c const amount )
		{
			float32_c amount_squared = amount * amount;
			vector32x2_c a0 = next_destination - destination - previous_source + source;
			vector32x2_c a1 = previous_source - source - a0;
			vector32x2_c a2 = destination - previous_source;
			vector32x2_c a3 = source;
			return ( a0 * amount * amount_squared ) + ( a1 * amount_squared ) + ( a2 * amount ) + ( a3 );
		}

		vector32x3_c interpolate_cubic( vector32x3_c const & previous_source, vector32x3_c const & source, vector32x3_c const & destination, vector32x3_c const & next_destination, float32_c const amount )
		{
			float32_c amount_squared = amount * amount;
			vector32x3_c a0 = next_destination - destination - previous_source + source;
			vector32x3_c a1 = previous_source - source - a0;
			vector32x3_c a2 = destination - previous_source;
			vector32x3_c a3 = source;
			return ( a0 * amount * amount_squared ) + ( a1 * amount_squared ) + ( a2 * amount ) + ( a3 );
		}

		vector32x4_c interpolate_cubic( vector32x4_c const & previous_source, vector32x4_c const & source, vector32x4_c const & destination, vector32x4_c const & next_destination, float32_c const amount )
		{
			float32_c amount_squared = amount * amount;
			vector32x4_c a0 = next_destination - destination - previous_source + source;
			vector32x4_c a1 = previous_source - source - a0;
			vector32x4_c a2 = destination - previous_source;
			vector32x4_c a3 = source;
			return ( a0 * amount * amount_squared ) + ( a1 * amount_squared ) + ( a2 * amount ) + ( a3 );
		}

		float32_c interpolate_hermite( float32_c const previous_source, float32_c const source, float32_c const destination, float32_c const next_destination, float32_c const amount, float32_c const tension, float32_c const bias )
		{
			float32_c m0 = ( ( source - previous_source ) * ( 1.0f + bias ) * ( 1.0f - tension ) / 2.0f ) + ( ( destination - source ) * ( 1.0f - bias ) * ( 1.0f - tension ) / 2.0f );
			float32_c m1 = ( ( destination - source ) * ( 1.0f + bias ) * ( 1.0f - tension ) / 2.0f ) + ( ( next_destination - destination ) * ( 1.0f - bias ) * ( 1.0f - tension ) / 2.0f );
			float32_c mu2 = amount * amount;
			float32_c mu3 = mu2 * amount;
			float32_c a0 = ( 2.0f * mu3 ) - ( 3.0f * mu2 ) + 1.0f;
			float32_c a1 = mu3 - ( 2.0f * mu2 ) + amount;
			float32_c a2 = mu3 - mu2;
			float32_c a3 = ( -2.0f * mu3 ) + ( 3.0f * mu2 );
			return ( ( source * a0 ) + ( m0 * a1 ) + ( m1 * a2 ) + ( destination * a3 ) );
		}

		vector32x2_c interpolate_hermite( vector32x2_c const & previous_source, vector32x2_c const & source, vector32x2_c const & destination, vector32x2_c const & next_destination, float32_c const amount, float32_c const tension, float32_c const bias )
		{
			vector32x2_c m0 = ( ( source - previous_source ) * ( 1.0f + bias ) * ( 1.0f - tension ) / 2.0f ) + ( ( destination - source ) * ( 1.0f - bias ) * ( 1.0f - tension ) / 2.0f );
			vector32x2_c m1 = ( ( destination - source ) * ( 1.0f + bias ) * ( 1.0f - tension ) / 2.0f ) + ( ( next_destination - destination ) * ( 1.0f - bias ) * ( 1.0f - tension ) / 2.0f );
			float32_c mu2 = amount * amount;
			float32_c mu3 = mu2 * amount;
			float32_c a0 = ( 2.0f * mu3 ) - ( 3.0f * mu2 ) + 1.0f;
			float32_c a1 = mu3 - ( 2.0f * mu2 ) + amount;
			float32_c a2 = mu3 - mu2;
			float32_c a3 = ( -2.0f * mu3 ) + ( 3.0f * mu2 );
			return ( ( source * a0 ) + ( m0 * a1 ) + ( m1 * a2 ) + ( destination * a3 ) );
		}

		vector32x3_c interpolate_hermite( vector32x3_c const & previous_source, vector32x3_c const & source, vector32x3_c const & destination, vector32x3_c const & next_destination, float32_c const amount, float32_c const tension, float32_c const bias )
		{
			vector32x3_c m0 = ( ( source - previous_source ) * ( 1.0f + bias ) * ( 1.0f - tension ) / 2.0f ) + ( ( destination - source ) * ( 1.0f - bias ) * ( 1.0f - tension ) / 2.0f );
			vector32x3_c m1 = ( ( destination - source ) * ( 1.0f + bias ) * ( 1.0f - tension ) / 2.0f ) + ( ( next_destination - destination ) * ( 1.0f - bias ) * ( 1.0f - tension ) / 2.0f );
			float32_c mu2 = amount * amount;
			float32_c mu3 = mu2 * amount;
			float32_c a0 = ( 2.0f * mu3 ) - ( 3.0f * mu2 ) + 1.0f;
			float32_c a1 = mu3 - ( 2.0f * mu2 ) + amount;
			float32_c a2 = mu3 - mu2;
			float32_c a3 = ( -2.0f * mu3 ) + ( 3.0f * mu2 );
			return ( ( source * a0 ) + ( m0 * a1 ) + ( m1 * a2 ) + ( destination * a3 ) );
		}

		vector32x4_c interpolate_hermite( vector32x4_c const & previous_source, vector32x4_c const & source, vector32x4_c const & destination, vector32x4_c const & next_destination, float32_c const amount, float32_c const tension, float32_c const bias )
		{
			vector32x4_c m0 = ( ( source - previous_source ) * ( 1.0f + bias ) * ( 1.0f - tension ) / 2.0f ) + ( ( destination - source ) * ( 1.0f - bias ) * ( 1.0f - tension ) / 2.0f );
			vector32x4_c m1 = ( ( destination - source ) * ( 1.0f + bias ) * ( 1.0f - tension ) / 2.0f ) + ( ( next_destination - destination ) * ( 1.0f - bias ) * ( 1.0f - tension ) / 2.0f );
			float32_c mu2 = amount * amount;
			float32_c mu3 = mu2 * amount;
			float32_c a0 = ( 2.0f * mu3 ) - ( 3.0f * mu2 ) + 1.0f;
			float32_c a1 = mu3 - ( 2.0f * mu2 ) + amount;
			float32_c a2 = mu3 - mu2;
			float32_c a3 = ( -2.0f * mu3 ) + ( 3.0f * mu2 );
			return ( ( source * a0 ) + ( m0 * a1 ) + ( m1 * a2 ) + ( destination * a3 ) );
		}

		quaternion32_c interpolate_spherical_linear( quaternion32_c const & source, quaternion32_c const & destination, float32_c const amount )
		{
			// this code taken from:
			// http://www.eudclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/index.html

			if ( amount <= 0.0f )
			{
				return source;
			}
			else if ( amount >= 1.0f )
			{
				return destination;
			}

			float32_c cosine_half_theta = dot_product_float32( source, destination );
			if ( math_absolute_value( cosine_half_theta ) >= 1.0f ) // difference is 0 degrees, or 360 degrees, or greater than 360 degrees (if quaternions are not normalized), so source and destination are the same.
			{
				return source;
			}
			float32_c half_theta = math_arc_cosine( cosine_half_theta );
			float32_c sine_half_theta = math_square_root( 1.0f - ( cosine_half_theta * cosine_half_theta ) );
			if ( math_absolute_value( sine_half_theta ) < 0.0f )
			{
				return destination;
			}

			float32_c ratio_source = math_sine( ( 1.0f - amount ) * half_theta ) / sine_half_theta;
			float32_c ratio_destination = math_sine( amount * half_theta ) / sine_half_theta;

			quaternion32_c result(
				( source.a * ratio_source ) + ( destination.a * ratio_destination ),
				( source.b * ratio_source ) + ( destination.b * ratio_destination ),
				( source.c * ratio_source ) + ( destination.c * ratio_destination ),
				( source.d * ratio_source ) + ( destination.d * ratio_destination ) );
			return normal_quaternion32( result );
		}

		quaternion32_c traverse( quaternion32_c const & source, quaternion32_c const & destination, float32_c amount )
		{
			float32_c cosine_half_theta = dot_product_float32( source, destination );
			if ( math_absolute_value( cosine_half_theta ) >= 1.0f ) // difference is 0 degrees, or 360 degrees, or greater than 360 degrees (if quaternions are not normalized), so source and destination are the same.
			{
				return source;
			}
			float32_c half_theta = math_arc_cosine( cosine_half_theta );
			if ( amount > half_theta )
			{
				return destination;
			}
			return interpolate_spherical_linear( source, destination, amount / half_theta );
		}

		float32_c traverse( float32_c const source, float32_c const destination, float32_c const amount )
		{
			float32_c result = destination;
			if ( destination > source )
			{
				result = source + amount;
				if ( result > destination )
				{
					return destination;
				}
			}
			else if ( destination < source )
			{
				result = source - amount;
				if ( result < destination )
				{
					return destination;
				}
			}
			return result;
		}

		float32_c traverse_with_ease( float32_c const source, float32_c const destination, float32_c amount, float32_c const margin )
		{
			float32_c distance = math_absolute_value( source - destination );
			if ( margin > 0.0f && distance < margin ) // ease
			{
				amount *= distance / margin;
			}

			float32_c result = destination;
			if ( destination > source )
			{
				result = source + amount;
				if ( result > destination )
				{
					return destination;
				}
			}
			else if ( destination < source )
			{
				result = source - amount;
				if ( result < destination )
				{
					return destination;
				}
			}
			return result;
		}

	}
}