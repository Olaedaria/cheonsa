#include "cheonsa_audio_buffer_raw.h"
#include "cheonsa_audio_player_raw.h"
#include "cheonsa_ops.h"
#include <assert.h>

namespace cheonsa
{

	audio_buffer_raw_c::audio_buffer_raw_c( audio_sample_format_e sample_format, sint32_c channel_count, sint32_c sample_rate, void_c * sample_buffer, sint32_c sample_buffer_size )
		: audio_buffer_c()
		, _channel_count( 0 )
		, _sample_format( audio_sample_format_e_none )
		, _sample_rate( 0 )
		, _sample_count( 0 )
		, _sample_buffer( nullptr )
	{
		assert( channel_count == 1 || channel_count == 2 || channel_count == 6 );

		_channel_count = channel_count;
		_sample_format = sample_format;
		_sample_rate = sample_rate;
		sint32_c sample_format_size = 0;
		if ( sample_format == audio_sample_format_e_sint8 )
		{
			sample_format_size = 1;
		}
		else if ( sample_format == audio_sample_format_e_sint16 )
		{
			sample_format_size = 2;
		}
		else if ( sample_format == audio_sample_format_e_float32 )
		{
			sample_format_size = 4;
		}
		else
		{
			return;
		}
		assert( sample_buffer_size % ( sample_format_size * _channel_count ) == 0 );
		_sample_count = sample_buffer_size / ( sample_format_size * _channel_count );
		_sample_buffer_size = sample_buffer_size;
		_sample_buffer = new uint8_c[ sample_buffer_size ];
		ops::memory_copy( sample_buffer, _sample_buffer, sample_buffer_size );

		_is_loaded = true;
	}

	audio_buffer_raw_c::~audio_buffer_raw_c()
	{
		assert( _reference_count == 0 );
		delete[] _sample_buffer;
		_sample_buffer = nullptr;
		_sample_buffer_size = 0;
	}

	audio_player_c * audio_buffer_raw_c::spawn_new_audio_player() const
	{
		return new audio_player_raw_c( this );
	}

}
