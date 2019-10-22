#pragma once

#include "cheonsa_audio_player.h"
#include "cheonsa_audio_buffer_raw.h"

namespace cheonsa
{

	// audio player to play raw format audio buffer.
	class audio_player_raw_c : public audio_player_c
	{
	protected:
		sint32_c _current_sample;
	
		virtual void_c _sample_and_mix( float64_c play_back_speed, sint32_c output_channel_count, sint32_c output_sample_rate, sint32_c output_sample_count, float32_c * output_sample_buffer, float32_c * output_channel_volumes ) override;

	public:
		audio_player_raw_c( audio_buffer_raw_c const * raw_buffer );
		virtual ~audio_player_raw_c() override;

		virtual void_c play( boolean_c randomize_seek ) override;

	};

}
