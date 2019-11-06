#include "cheonsa_resource_file_sound.h"

namespace cheonsa
{

	boolean_c resource_file_sound_c::_load( data_stream_c * stream )
	{
		assert( stream );
		assert( _is_loaded == false );

		_audio_wave_buffer->load_new_state( stream );

		_is_loaded = true;

		return true;
	}

	void_c resource_file_sound_c::_unload()
	{
		assert( _is_loaded == true );
		_is_loaded = false;
		_audio_wave_buffer->release_state();
	}

	resource_file_sound_c::resource_file_sound_c()
		: resource_file_c()
		, _audio_wave_buffer( nullptr )
	{
		_audio_wave_buffer = new audio2_wave_buffer_c();
	}

	resource_file_sound_c::~resource_file_sound_c()
	{
		_audio_wave_buffer->remove_reference();
		_audio_wave_buffer = nullptr;
	}

	audio2_wave_buffer_c * resource_file_sound_c::get_audio_wave_buffer() const
	{
		return _audio_wave_buffer;
	}

}
