#include "cheonsa_audio_interface.h"
#include "cheonsa_audio_buffer.h"
#include "cheonsa_audio_buffer_raw.h"
#include "cheonsa_audio_buffer_ogg.h"
#include "cheonsa_audio_source.h"
#include "cheonsa_ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	audio_interface_c::settings_c::settings_c()
	{
		reset_to_default();
	}

	void_c audio_interface_c::settings_c::reset_to_default()
	{
		master_volume = 0.75f;
		music_volume = 0.5f;
		ambient_volume = 0.75f;
		effect_volume = 1.0f;
		voice_volume = 1.0f;
		menu_volume = 1.0f;
	}

	void_c audio_interface_c::_save_settings( data_scribe_ini_c & scribe )
	{
		// [audio]
		scribe.add_section( string8_c( mode_e_static, "audio" ) );
		// master_volume
		string8_c value2;
		ops::convert_sint32_to_string8( static_cast< sint32_c >( settings.master_volume * 100.0f ), value2 );
		scribe.add_property( string8_c( mode_e_static, "master_volume" ), value2 );
		// music_volume
		ops::convert_sint32_to_string8( static_cast< sint32_c >( settings.music_volume * 100.0f ), value2 );
		scribe.add_property( string8_c( mode_e_static, "music_volume" ), value2 );
		// ambient_volume
		ops::convert_sint32_to_string8( static_cast< sint32_c >( settings.ambient_volume * 100.0f ), value2 );
		scribe.add_property( string8_c( mode_e_static, "ambient_voume" ), value2 );
		// effect_volume
		ops::convert_sint32_to_string8( static_cast< sint32_c >( settings.effect_volume * 100.0f ), value2 );
		scribe.add_property( string8_c( mode_e_static, "effect_volume" ), value2 );
		// voice_volume
		ops::convert_sint32_to_string8( static_cast< sint32_c >( settings.voice_volume * 100.0f ), value2 );
		scribe.add_property( string8_c( mode_e_static, "voice_volume" ), value2 );
	}

	void_c audio_interface_c::_load_settings( data_scribe_ini_c & scribe )
	{
		settings.reset_to_default();

		string8_c value;

		if ( scribe.find_property_value( string8_c( mode_e_static, "audio" ), string8_c( mode_e_static, "master_volume" ), value ) )
		{
			sint32_c value2;
			if ( ops::convert_string8_to_sint32( value, value2 ) )
			{
				settings.master_volume = ops::math_saturate( static_cast< float32_c >( value2 ) / 100.0f );
			}
		}

		if ( scribe.find_property_value( string8_c( mode_e_static, "audio" ), string8_c( mode_e_static, "music_volume" ), value ) )
		{
			sint32_c value2;
			if ( ops::convert_string8_to_sint32( value, value2 ) )
			{
				settings.music_volume = ops::math_saturate( static_cast< float32_c >( value2 ) / 100.0f );
			}
		}

		if ( scribe.find_property_value( string8_c( mode_e_static, "audio" ), string8_c( mode_e_static, "ambient_volume" ), value ) )
		{
			sint32_c value2;
			if ( ops::convert_string8_to_sint32( value, value2 ) )
			{
				settings.ambient_volume = ops::math_saturate( static_cast< float32_c >( value2 ) / 100.0f );
			}
		}

		if ( scribe.find_property_value( string8_c( mode_e_static, "audio" ), string8_c( mode_e_static, "effect_volume" ), value ) )
		{
			sint32_c value2;
			if ( ops::convert_string8_to_sint32( value, value2 ) )
			{
				settings.effect_volume = ops::math_saturate( static_cast< float32_c >( value2 ) / 100.0f );
			}
		}

		if ( scribe.find_property_value( string8_c( mode_e_static, "audio" ), string8_c( mode_e_static, "voice_volume" ), value ) )
		{
			sint32_c value2;
			if ( ops::convert_string8_to_sint32( value, value2 ) )
			{
				settings.voice_volume = ops::math_saturate( static_cast< float32_c >( value2 ) / 100.0f );
			}
		}

		if ( scribe.find_property_value( string8_c( mode_e_static, "audio" ), string8_c( mode_e_static, "menu_volume" ), value ) )
		{
			sint32_c value2;
			if ( ops::convert_string8_to_sint32( value, value2 ) )
			{
				settings.menu_volume = ops::math_saturate( static_cast< float32_c >( value2 ) / 100.0f );
			}
		}
	}

	audio_interface_c::audio_interface_c()
		: _channel_count( 0 )
		, _sample_rate( 0 )
		, _frames_per_second( 0 )
		, _samples_per_frame( 0 )
		, _audio_sources()
		, _audio_scenes()
		, _critical_section()
		, settings()
	{
	}

	audio_interface_c::~audio_interface_c()
	{
		for ( sint32_c i = 0; i < _audio_sources.get_length(); i++ )
		{
			_audio_sources[ i ]->remove_reference();
		}
		_audio_sources.remove_all();

		for ( sint32_c i = 0; i < _audio_scenes.get_length(); i++ )
		{
			_audio_scenes[ i ]->remove_reference();
		}
		_audio_scenes.remove_all();
	}

	void_c audio_interface_c::add_audio_scene( audio_scene_c * audio_scene )
	{
		assert( audio_scene != nullptr );
		assert( audio_scene->_audio_interface == nullptr );
		_critical_section.enter();
		audio_scene->_audio_interface = this;
		_audio_scenes.insert_at_end( audio_scene );
		_critical_section.exit();
	}

	void_c audio_interface_c::remove_audio_scene( audio_scene_c * audio_scene )
	{
		assert( audio_scene );
		assert( audio_scene->_audio_interface == this );
		_critical_section.enter();
		_audio_scenes.remove( audio_scene );
		audio_scene->_audio_interface = nullptr;
		audio_scene->remove_reference();
		_critical_section.exit();
	}

	void_c audio_interface_c::play_2d( audio_buffer_c * buffer, audio_layer_e layer, float32_c volume, float32_c speed )
	{
		assert( buffer );
		assert( buffer->get_is_loaded() );
		audio_source_c * audio_source = new audio_source_c( buffer );
		audio_source->_layer = layer;
		audio_source->_volume = volume;
		audio_source->_speed = speed;
		audio_source->start( false );

		_critical_section.enter();
		_audio_sources.insert_at_end( audio_source );
		_critical_section.exit();
	}

	void_c audio_interface_c::play_music( string16_c const & file_path_relative )
	{
		resource_object_sound_c::reference_c sound = global_engine_instance.interfaces.resource_manager->load_sound( file_path_relative, true );
		if ( !sound.is_reference_set_and_loaded() )
		{
			return;
		}
	}

	void_c audio_interface_c::stop_music()
	{
	}

}
