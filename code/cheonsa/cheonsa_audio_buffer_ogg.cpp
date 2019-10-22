#include "cheonsa_audio_buffer_ogg.h"
#include "cheonsa_audio_player_ogg.h"
#include "cheonsa_ops.h"
#include <assert.h>

namespace cheonsa
{

	audio_buffer_ogg_c::audio_buffer_ogg_c( void_c * data, sint32_c data_size )
		: audio_buffer_c()
		, _data( nullptr )
		, _data_size( 0 )
	{
		if ( data == nullptr || data_size == 0 )
		{
			return;
		}
		_data = new uint8_c[ data_size ];
		_data_size = data_size;
		ops::memory_copy( data, _data, data_size );
		_is_loaded = true;
	}

	audio_buffer_ogg_c::~audio_buffer_ogg_c()
	{
		assert( _reference_count == 0 );
		if ( _data )
		{
			delete[] _data;
			_data = nullptr;
			_data_size = 0;
		}
	}

	audio_player_c * audio_buffer_ogg_c::spawn_new_audio_player() const
	{
		return new audio_player_ogg_c( this );
	}

}
