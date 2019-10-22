#pragma once

#include "cheonsa_audio_buffer.h"

namespace cheonsa
{

	// ogg format audio buffer.
	class audio_buffer_ogg_c : public audio_buffer_c
	{
	protected:
		friend class audio_player_ogg_c;

		uint8_c * _data; // ogg file data.
		sint32_c _data_size; // size in bytes of ogg file data.

	public:
		audio_buffer_ogg_c( void_c * data, sint32_c data_size );
		virtual ~audio_buffer_ogg_c();

		virtual audio_player_c * spawn_new_audio_player() const override;

	};

}
