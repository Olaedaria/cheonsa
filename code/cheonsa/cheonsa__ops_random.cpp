#include "cheonsa__ops_random.h"
#include "cheonsa__ops_math.h"
#include <chrono>

namespace cheonsa
{
	namespace ops
	{

		void_c random_start()
		{
			srand( static_cast< unsigned int >( time( nullptr ) ) );
		}

		float32_c random_float32( float32_c minimum, float32_c maximum )
		{
			return static_cast< float32_c >( static_cast< float64_c >( rand() ) / static_cast< float64_c >( RAND_MAX ) * ( maximum - minimum ) + minimum );
		}

		float64_c random_float64( float64_c minimum, float64_c maximum )
		{
			return static_cast< float64_c >( rand() ) / static_cast< float64_c >( RAND_MAX ) * ( maximum - minimum ) + minimum;
		}

		sint32_c random_sint32( sint32_c minimum, sint32_c maximum )
		{
			return static_cast< sint32_c >( static_cast< float64_c >( rand() ) / static_cast< float64_c >( RAND_MAX ) * static_cast< float64_c >( maximum - minimum ) ) + minimum;
		}

		uint32_c random_uint32( uint32_c maximum )
		{
			return static_cast< uint32_c >( static_cast< float64_c >( rand() ) / static_cast< float64_c >( RAND_MAX ) * static_cast< float64_c >( maximum ) );
		}

		vector32x2_c random_point_on_circle()
		{
			float32_c random_angle = random_float32( 0.0f, constants< float32_c >::pi() * 2.0f );
			return vector32x2_c( math_cosine( random_angle ), math_sine( random_angle ) );
		}

		vector32x3_c random_point_on_sphere()
		{
			// found at https://math.stackexchange.com/questions/44689/how-to-find-a-random-axis-or-unit-vector-in-3d
			float32_c random_angle = random_float32( 0.0f, constants< float32_c >::pi() * 2.0f );
			float32_c random_z = random_float32( -1.0f, 1.0f );
			return vector32x3_c(
				math_cosine( math_square_root( 1.0f - ( random_z * random_z ) ) ),
				math_sine( math_square_root( 1.0f - ( random_z * random_z ) ) ),
				random_z );
		}

	}
}
