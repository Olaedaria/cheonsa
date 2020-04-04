#pragma once

#include "cheonsa_resource_file.h"
#include "cheonsa_audio2.h"

namespace cheonsa
{

	class resource_file_sound_c
		: public resource_file_c
	{
	public:
		typedef resource_file_reference_c< resource_file_sound_c > reference_c;

	public:
		static char8_c const * get_type_static() { return "sound"; }
		virtual char8_c const * get_type() const override { return get_type_static(); }
	
	private:
		audio2_wave_buffer_c * _audio_wave_buffer;

	public:
		virtual boolean_c _load( data_stream_c * stream ) override;
		virtual void_c _unload() override;

	public:
		resource_file_sound_c();
		resource_file_sound_c( resource_file_sound_c const & ) = delete;
		virtual ~resource_file_sound_c() override;

		audio2_wave_buffer_c * get_audio_wave_buffer() const;

	};

}