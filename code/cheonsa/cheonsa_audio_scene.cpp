#include "cheonsa_audio_scene.h"
#include "cheonsa_audio_source.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	audio_scene_c::audio_scene_c()
		: _audio_interface( nullptr )
		, _audio_listener()
		, _audio_sources()
		, _critical_section()
		, _reference_count( 1 )
	{
	}

	audio_scene_c::~audio_scene_c()
	{
		for ( sint32_c i = 0; i < _audio_sources.get_length(); i++ )
		{
			_audio_sources[ i ]->remove_reference();
		}
	}

	audio_listener_c const & audio_scene_c::get_audio_listener() const
	{
		return _audio_listener;
	}

	audio_listener_c & audio_scene_c::get_audio_listener()
	{
		return _audio_listener;
	}

	void_c audio_scene_c::add_source( audio_source_c * audio_source )
	{
		assert( audio_source != nullptr );
		assert( audio_source->_audio_scene == nullptr );
		_critical_section.enter();
		audio_source->add_reference();
		audio_source->_audio_scene = this;
		_audio_sources.insert_at_end( audio_source );
		_critical_section.exit();
	}

	void_c audio_scene_c::remove_source( audio_source_c * audio_source )
	{
		assert( audio_source != nullptr );
		assert( audio_source->_audio_scene == this );
		_critical_section.enter();
		_audio_sources.remove( audio_source );
		audio_source->_audio_scene = nullptr;
		audio_source->remove_reference();
		_critical_section.exit();
	}

	boolean_c audio_scene_c::get_is_playing() const
	{
		return _is_playing;
	}

	void_c audio_scene_c::set_is_playing( boolean_c value )
	{
		_is_playing = value;
	}

	void_c audio_scene_c::add_reference() const
	{
		const_cast< audio_scene_c * >( this )->_reference_count++;
	}

	void_c audio_scene_c::remove_reference() const
	{
		assert( _reference_count > 0 );
		const_cast< audio_scene_c * >( this )->_reference_count--;
		if ( _reference_count == 0 )
		{
			delete this;
		}
	}

}
