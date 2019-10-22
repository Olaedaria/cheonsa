#pragma once

#include "cheonsa_audio_buffer.h"

namespace cheonsa
{

	// raw format audio buffer.
	class audio_buffer_raw_c : public audio_buffer_c
	{
	protected:
		friend class audio_player_raw_c;

		sint32_c _channel_count; // how many channels are in the source data.
		audio_sample_format_e _sample_format; // format of each sample in the source data.
		sint32_c _sample_rate; // how many samples per second.
		sint32_c _sample_count; // total number of samples in the source data. real number of sample points is _sample_count * _channel_count.
		uint8_c * _sample_buffer; // an array of samples of type _sample_format, interleaved by channel index.
		sint32_c _sample_buffer_size; // size in bytes of _sample_buffer.

	public:
		// this is a bit wasteful, we will create a new copy of the input raw data, probably right after the caller just made a copy of it for themselves.
		audio_buffer_raw_c( audio_sample_format_e sample_format, sint32_c channel_count, sint32_c sample_rate, void_c * sample_buffer, sint32_c sample_buffer_size );
		virtual ~audio_buffer_raw_c() override;

		virtual audio_player_c * spawn_new_audio_player() const override;

	};

}
