#include "cheonsa_audio_types.h"
#include "cheonsa_ops.h"
#include <assert.h>

namespace cheonsa
{

	void_c audio_convert_sint16_to_sint8( sint32_c samples_count, sint16_c const * samples_in, sint8_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			samples_out[ i ] = static_cast< sint8_c >( samples_in[ i ] * 0xFF / 0xFFFF );
		}
	}

	void_c audio_convert_float32_to_sint8( sint32_c samples_count, float32_c const * samples_in, sint8_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			samples_out[ i ] = static_cast< sint8_c >( samples_in[ i ] * 127.0f );
		}
	}

	void_c audio_convert_sint8_to_sint16( sint32_c samples_count, sint8_c const * samples_in, sint16_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			samples_out[ i ] = static_cast< sint16_c >( samples_in[i] * 0xFFFF / 0xFF );
		}
	}

	void_c audio_convert_float32_to_sint16( sint32_c samples_count, float32_c const * samples_in, sint16_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			samples_out[ i ] = static_cast< sint16_c >( samples_in[ i ] * 32767.0f );
		}
	}

	void_c audio_convert_sint8_to_float32( sint32_c samples_count, sint8_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			samples_out[ i ] = static_cast< float32_c >( samples_in[ i ] ) / 127.0f;
		}
	}

	void_c audio_convert_sint16_to_float32( sint32_c samples_count, sint16_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			samples_out[ i ] = static_cast< float32_c >( samples_in[ i ] ) / 32767.0f;
		}
	}

	void_c audio_convert_6_to_1( sint32_c samples_count, float32_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			sint32_c j = i * 6;
			samples_out[ i ] = ( samples_in[ j ] + samples_in[ j + 1 ] + samples_in[ j + 2 ] + samples_in[ j + 3 ] + samples_in[ j + 4 ] + samples_in[ j + 5 ] ) / 6.0f;
		}
	}

	void_c audio_convert_2_to_1( sint32_c samples_count, float32_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			sint32_c j = i * 2;
			samples_out[ i ] = ( samples_in[ j ] + samples_in[ j + 1 ] ) / 2.0f;
		}
	}

	void_c audio_convert_1_to_2( sint32_c samples_count, float32_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			sint32_c j = i * 2;
			samples_out[ j + 1 ] = samples_out[ j ] = ( samples_in[ i ] );
		}
	}

	void_c audio_convert_6_to_2( sint32_c samples_count, float32_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			sint32_c j = i * 6;
			sint32_c k = i * 2;
			samples_out[ k ] = ( samples_in[ j ] + samples_in[ j + 4 ] ) / 2.0f; // left = front left + back left.
			samples_out[ k + 1 ] = ( samples_in[ j + 1 ] + samples_in[ j + 5 ] ) / 2.0f; // right = front right + back right.
		}
	}

	void_c audio_convert_1_to_6( sint32_c samples_count, float32_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			sint32_c j = i * 6;
			samples_out[ j ] = samples_out[ j + 1 ] = samples_out[ j + 4 ] = samples_out[ j + 5 ] = samples_in[ i ];
			samples_out[ j + 2 ] = samples_out[ j + 3 ] = 0.0f;
		}
	}

	void_c audio_convert_2_to_6( sint32_c samples_count, float32_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			sint32_c j = i * 2;
			sint32_c k = i * 6;
			samples_out[ k ] = samples_out[ k + 4 ] = samples_in[ j ];
			samples_out[ k + 1 ] = samples_out[ k + 5 ] = samples_in[ j + 1 ];
			samples_out[ k + 2 ] = samples_out[ k + 3 ] = 0.0f;
		}
	}

	void_c audio_mix(
		audio_sample_format_e input_sample_format,
		sint32_c input_channel_count,
		sint32_c input_sample_count,
		void_c const * input_sample_buffer,
		sint32_c output_channel_count,
		sint32_c output_sample_count,
		float32_c * output_sample_buffer,
		float32_c * output_channel_volumes )
	{
		// is 0xFFFF (65535) enough samples for a temporary buffer?
		// output sample rate is typically 44100 samples per second.
		// this is 1470 samples per frame at 30 frames per second.
		// this is 735 samples per second at mono.
		// this is 1470 samples per second at stereo.
		// this is 4410 samples per second at 5.1 surround.
		// play back speed will scale the number of samples needed, which could incur buffer over flow, so it will need to be limited.
		// play back speed of 5 with 6 channels is 22050 samples per frame, still well below 65535.
		// at 60 frames per second these values are halved.
		static float32_c float32_temp_buffer_a[ 0xFFFF ];
		static float32_c float32_temp_buffer_b[ 0xFFFF ];

		if ( input_sample_count == 0 || output_sample_count == 0 )
		{
			return;
		}

		assert( input_channel_count == 1 || input_channel_count == 2 || input_channel_count == 6 );
		assert( input_sample_format == audio_sample_format_e_sint8 || input_sample_format == audio_sample_format_e_sint16 || input_sample_format == audio_sample_format_e_float32 );
		assert( input_sample_count > 0 );
		assert( input_sample_buffer != nullptr );
		assert( output_channel_count == 1 || output_channel_count == 2 || output_channel_count == 6 );
		assert( output_sample_count > 0 );
		assert( output_sample_buffer != nullptr );
		assert( output_channel_volumes != nullptr );
		assert( input_sample_count * input_channel_count < 0xFFFF );
		assert( output_sample_count * output_channel_count < 0xFFFF );

		// convert from input sample format to utput sample format (float32_c).
		float32_c const * stage_0 = nullptr;
		if ( input_sample_format == audio_sample_format_e_sint8 )
		{
			audio_convert_sint8_to_float32( input_sample_count * input_channel_count, static_cast< sint8_c const * >( input_sample_buffer ), float32_temp_buffer_a );
			stage_0 = float32_temp_buffer_a;
		}
		else if ( input_sample_format == audio_sample_format_e_sint16 )
		{
			audio_convert_sint16_to_float32( input_sample_count * input_channel_count, static_cast< sint16_c const * >( input_sample_buffer ), float32_temp_buffer_a );
			stage_0 = float32_temp_buffer_a;
		}
		else if ( input_sample_format == audio_sample_format_e_float32 )
		{
			stage_0 = static_cast< float32_c const * >( input_sample_buffer );
		}
		assert( stage_0 );

		// convert from input channel count to output channel count.
		float32_c const * stage_1 = nullptr;
		if ( input_channel_count == output_channel_count )
		{
			stage_1 = stage_0;
		}
		else if ( output_channel_count == 1 )
		{
			if ( input_channel_count == 2 )
			{
				audio_convert_2_to_1( input_sample_count, stage_0, float32_temp_buffer_b );
				stage_1 = float32_temp_buffer_b;
			}
			else if ( input_channel_count == 6 )
			{
				audio_convert_6_to_1( input_sample_count, stage_0, float32_temp_buffer_b );
				stage_1 = float32_temp_buffer_b;
			}
		}
		else if ( output_channel_count == 2 )
		{
			if ( input_channel_count == 1 )
			{
				audio_convert_1_to_2( input_sample_count, stage_0, float32_temp_buffer_b );
				stage_1 = float32_temp_buffer_b;
			}
			else if ( input_channel_count == 6 )
			{
				audio_convert_6_to_2( input_sample_count, stage_0, float32_temp_buffer_b );
				stage_1 = float32_temp_buffer_b;
			}
		}
		else if ( output_channel_count == 6 )
		{
			if ( input_channel_count == 1 )
			{
				audio_convert_1_to_6( input_sample_count, stage_0, float32_temp_buffer_b );
				stage_1 = float32_temp_buffer_b;
			}
			else if ( input_channel_count == 2 )
			{
				audio_convert_2_to_6( input_sample_count, stage_0, float32_temp_buffer_b );
				stage_1 = float32_temp_buffer_b;
			}
		}
		assert( stage_1 );

		// convert input sample count to output sample count and apply volume.
		float32_c const * input_samples = stage_1;
		if ( input_sample_count >= output_sample_count )
		{
			// input is at same or higher frequency to output so we will use nearest neighbor sampling.
			for ( sint32_c output_frame_index = 0; output_frame_index < output_sample_count; output_frame_index++ )
			{
				sint32_c output_sample_index = output_frame_index * output_channel_count;
				for ( sint32_c output_channel_index = 0; output_channel_index < output_channel_count; output_channel_index++ )
				{
					output_sample_buffer[ output_sample_index + output_channel_index ] += input_samples[ output_sample_index + output_channel_index ] * output_channel_volumes[ output_channel_index ];
				}
			}
		}
		else
		{
			// input is at lower frequency than output so we will use linearly interpolated sampling.
			for ( sint32_c output_index = 0; output_index < output_sample_count; output_index++ )
			{
				sint32_c output_index_2 = output_index * output_channel_count;
				float64_c input_index_scalar = static_cast< float64_c >( output_index ) / static_cast< float64_c >( output_sample_count ) * static_cast< float64_c >( input_sample_count );
				sint32_c input_index_2_a = static_cast< sint32_c >( input_index_scalar ) * output_channel_count;
				sint32_c input_index_2_b = input_index_2_a + output_channel_count;
				float32_c input_blend = static_cast< float32_c >( ops::math_remainder( input_index_scalar, 1.0 ) );
				for ( sint32_c channel_index = 0; channel_index < output_channel_count; channel_index++ )
				{
					output_sample_buffer[ output_index_2 + channel_index ] += ( ( input_samples[ input_index_2_a + channel_index ] * ( 1.0f - input_blend ) ) + ( input_samples[ input_index_2_b + channel_index ] * input_blend ) ) * output_channel_volumes[ channel_index ];
				}
			}
		}
	}

}
