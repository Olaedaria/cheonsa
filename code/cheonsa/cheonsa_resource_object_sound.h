#pragma once

#include "cheonsa_resource_object.h"
#include "cheonsa_audio_interface.h"

namespace cheonsa
{

	// mono or stereo sound.
	// can load from ogg or wav file.
	class resource_object_sound_c : public resource_object_c
	{
	public:
		typedef resource_object_reference_c< resource_object_sound_c > reference_c;

	public:
		static char8_c const * get_type_static() { return "sound"; }
		virtual char8_c const * get_type() const override { return get_type_static(); }

	protected:
		audio_buffer_c * _audio_buffer;

		virtual boolean_c _load( data_stream_c * stream ) override;
		virtual void_c _unload() override;

	public:
		resource_object_sound_c();
		resource_object_sound_c( resource_object_sound_c const & ) = delete;
		virtual ~resource_object_sound_c() override;
		audio_buffer_c const * get_audio_buffer() const;
		resource_object_sound_c & operator = ( resource_object_sound_c const & ) = delete;

	};

}
