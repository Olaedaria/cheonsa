#include "cheonsa_audio_player_ogg.h"
#include "cheonsa_ops.h"
#include <assert.h>

namespace cheonsa
{

	audio_player_ogg_c::audio_player_ogg_c( audio_buffer_ogg_c const * ogg_buffer )
		: audio_player_c()
		, _stb_vorbis( nullptr )
		, _channel_count( 0 )
		, _sample_rate( 0 )
		, _sample_count( 0 )
	{
		assert( ogg_buffer );
		assert( ogg_buffer->get_is_loaded() );

		_audio_buffer = ogg_buffer;
		_audio_buffer->add_reference();

		int error = 0;
		int bytes_used = 0;

		_stb_vorbis = stb_vorbis_open_memory( ogg_buffer->_data, ogg_buffer->_data_size, &error, nullptr );
		if ( _stb_vorbis == nullptr )
		{
			return;
		}

		stb_vorbis_info info = stb_vorbis_get_info( _stb_vorbis );
		_channel_count = info.channels;
		_sample_rate = info.sample_rate;
		_sample_count = stb_vorbis_stream_length_in_samples( _stb_vorbis );

		assert( _channel_count == 1 || _channel_count == 2 || _channel_count == 6 );
	}

	audio_player_ogg_c::~audio_player_ogg_c()
	{
		assert( _reference_count == 0 );

		if ( _stb_vorbis != nullptr )
		{
			stb_vorbis_close( _stb_vorbis );
			_stb_vorbis = nullptr;
		}

		_audio_buffer->remove_reference();
		_audio_buffer = nullptr;
	}

	void_c audio_player_ogg_c::_sample_and_mix( float64_c play_back_speed, sint32_c output_channel_count, sint32_c output_sample_rate, sint32_c output_sample_count, float32_c * output_sample_buffer, float32_c * output_channel_volumes )
	{
		// audio thread should not call us if we aren't ready.
		assert( _stb_vorbis != nullptr && _is_playing != false );

		// decode the ogg, sample from and advance the stream.
		float64_c output_duration = static_cast< float64_c >( output_sample_count ) / static_cast< float64_c >( output_sample_rate );
		sint32_c input_sample_count_needed = static_cast< sint32_c >( output_duration * play_back_speed * static_cast< float64_c >( _sample_rate ) ); // number of samples that we need to decode from the stream to satisfy the output.
		float32_c input_sample_buffer[ 0xFFFF ];
		assert( input_sample_count_needed * _channel_count < 0xFFFF );
		sint32_c input_sample_count_found = stb_vorbis_get_samples_float_interleaved( _stb_vorbis, _channel_count, input_sample_buffer, input_sample_count_needed * _channel_count );
		sint32_c zero_start = input_sample_count_found * _channel_count;
		sint32_c zero_end = input_sample_count_needed * _channel_count;
		if ( input_sample_count_found < input_sample_count_needed )
		{
			for ( sint32_c i = zero_start; i < zero_end; i++ )
			{
				input_sample_buffer[ i ] = 0.0f;
			}
		}

		// do the mix.
		audio_mix( audio_sample_format_e_float32, _channel_count, input_sample_count_needed, input_sample_buffer, output_channel_count, output_sample_count, output_sample_buffer, output_channel_volumes );
	}

	void_c audio_player_ogg_c::play( boolean_c randomize_seek )
	{
		if ( _stb_vorbis != nullptr )
		{
			stb_vorbis_seek( _stb_vorbis, randomize_seek ? ops::random_uint32( _sample_count ) : 0 );
			_is_playing = true;
		}
	}

}
