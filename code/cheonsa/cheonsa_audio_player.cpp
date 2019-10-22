#include "cheonsa_audio_player.h"
#include <assert.h>

namespace cheonsa
{

	audio_player_c::audio_player_c()
		: _reference_count( 1 )
		, _audio_buffer( nullptr )
		, _is_playing( false )
	{
	}

	audio_player_c::~audio_player_c()
	{
		assert( _reference_count == 0 );
	}

	boolean_c audio_player_c::get_is_playing() const
	{
		return _is_playing;
	}

	void_c audio_player_c::add_reference() const
	{
		const_cast< audio_player_c * >( this )->_reference_count++;
	}

	void_c audio_player_c::remove_reference() const
	{
		assert( _reference_count > 0 );
		const_cast< audio_player_c * >( this )->_reference_count--;
		if ( _reference_count == 0 )
		{
			delete this;
		}
	}

}
