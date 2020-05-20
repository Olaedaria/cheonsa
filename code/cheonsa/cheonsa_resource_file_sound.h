#pragma once

#include "cheonsa__types.h"
#include "cheonsa_resource_file.h"
#include "cheonsa_audio_interface.h"

namespace cheonsa
{

	// loads ".wav" and ".ogg" files.
	// any bit rate should work.
	// any channel count should work, but i have only tested with mono and stereo.
	class resource_file_sound_c
		: public resource_file_c
	{
	public:
		typedef resource_file_reference_c< resource_file_sound_c > reference_c;

	public:
		static char8_c const * get_resource_file_type_static() { return "sound"; }
		virtual char8_c const * get_resource_file_type() const override { return get_resource_file_type_static(); }
	
	private:
		audio_wave_buffer_c * _audio_wave_buffer;

	public:
		virtual void_c _load( data_stream_c * stream ) override;
		virtual void_c _unload() override;

	public:
		resource_file_sound_c( string16_c const & file_path );
		virtual ~resource_file_sound_c() override;

		audio_wave_buffer_c * get_audio_wave_buffer() const;

	};

}