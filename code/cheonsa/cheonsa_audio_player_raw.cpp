#include "cheonsa_audio_player_raw.h"
#include "cheonsa_ops.h"
#include <assert.h>

namespace cheonsa
{

	audio_player_raw_c::audio_player_raw_c( audio_buffer_raw_c const * raw_buffer )
		: audio_player_c()
		, _current_sample( 0 )
	{
		assert( raw_buffer );
		assert( raw_buffer->get_is_loaded() );

		_audio_buffer = raw_buffer;
		_audio_buffer->add_reference();
	}

	audio_player_raw_c::~audio_player_raw_c()
	{
		assert( _reference_count == 0 );

		_audio_buffer->remove_reference();
		_audio_buffer = nullptr;
	}

	void_c audio_player_raw_c::_sample_and_mix( float64_c play_back_speed, sint32_c output_channel_count, sint32_c output_sample_rate, sint32_c output_sample_count, float32_c * output_sample_buffer, float32_c * output_channel_volumes )
	{
		assert( _is_playing == true );

		audio_buffer_raw_c const * audio_buffer_raw = static_cast< audio_buffer_raw_c const * >( _audio_buffer );

		// sample from the raw buffer.
		float64_c output_duration = static_cast< float64_c >( output_sample_count ) / static_cast< float64_c >( output_sample_rate );
		sint32_c input_sample_count_needed = static_cast< sint32_c >( output_duration * play_back_speed * static_cast< float64_c >( audio_buffer_raw->_sample_rate ) ); // number of samples that we need to decode from the stream to satisfy the output.
		sint32_c input_sample_count_found = _current_sample + input_sample_count_needed;
		if ( input_sample_count_found > audio_buffer_raw->_sample_count ) // not enough input to fit in output, resize how much we will output to fit proportionately with what input we have available.
		{
			input_sample_count_found = audio_buffer_raw->_sample_count - _current_sample;
			output_duration = static_cast< float64_c >( input_sample_count_found ) / static_cast< float64_c >( audio_buffer_raw->_sample_rate );
			output_sample_count = static_cast< sint32_c >( output_duration * static_cast< float64_c >( output_sample_rate ) );
		}

		// find input sample buffer offset from _current_sample.
		sint32_c sample_size = 1;
		if ( audio_buffer_raw->_sample_format == audio_sample_format_e_sint16 )
		{
			sample_size = 2;
		}
		else if ( audio_buffer_raw->_sample_format == audio_sample_format_e_float32 )
		{
			sample_size = 4;
		}
		void_c const * input_sample_buffer = &audio_buffer_raw->_sample_buffer[ _current_sample * sample_size * audio_buffer_raw->_channel_count ];
		_current_sample += input_sample_count_found;

		// do the mix.
		audio_mix( audio_buffer_raw->_sample_format, audio_buffer_raw->_channel_count, input_sample_count_found, input_sample_buffer, output_channel_count, output_sample_count, output_sample_buffer, output_channel_volumes );
	}

	void_c audio_player_raw_c::play( boolean_c randomize_seek )
	{
	}

}
