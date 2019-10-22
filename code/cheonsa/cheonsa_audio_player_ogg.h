#pragma once

#include "cheonsa_audio_player.h"
#include "cheonsa_audio_buffer_ogg.h"
#include "third_party/stb_vorbis.h"

namespace cheonsa
{

	// audio player to play ogg format audio buffer.
	class audio_player_ogg_c : public audio_player_c
	{
	protected:
		stb_vorbis * _stb_vorbis;

		sint32_c _channel_count; // number of channels of stream.
		uint32_c _sample_rate; // sample rate of stream.
		uint32_c _sample_count; // total number of samples in the audio buffer stream. real number of sample points is _sample_count * _channel_count.

		virtual void_c _sample_and_mix( float64_c play_back_speed, sint32_c output_channel_count, sint32_c output_sample_rate, sint32_c output_sample_count, float32_c * output_sample_buffer, float32_c * output_channel_volumes ) override;

	public:
		audio_player_ogg_c( audio_buffer_ogg_c const * ogg_buffer );
		virtual ~audio_player_ogg_c() override;

		virtual void_c play( boolean_c randomize_seek ) override;

	};

}
