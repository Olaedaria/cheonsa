#include "cheonsa_audio_source.h"
#include "cheonsa_audio_buffer.h"
#include "cheonsa_audio_player.h"
#include "cheonsa_audio_buffer_raw.h"
#include "cheonsa_audio_player_raw.h"
#include "cheonsa_audio_buffer_ogg.h"
#include "cheonsa_audio_player_ogg.h"
#include "cheonsa_ops.h"
#include <assert.h>

namespace cheonsa
{

	audio_source_c::audio_source_c( audio_buffer_c const * buffer )
		: _layer( audio_layer_e_effect ) // what layer this source plays on. default is ELayer_Effect.
		, _volume( 1.0f )
		, _speed( 1.0f )
		, _is_3d( false )
		//, _world_space_position( 0.0f, 0.0f, 0.0f )
		, _world_space_transform( nullptr )
		, _audio_scene( nullptr )
		, _audio_player( nullptr )
		, _interval_timer( 0.0f )
		, _interval_timer_target( 0.0f )
		, _reference_count( 1 )
	{
		assert( buffer && buffer->get_is_loaded() );

		_audio_buffer = buffer;
		_audio_buffer->add_reference();
	}

	audio_source_c::~audio_source_c()
	{
		assert( _reference_count == 0 );

		if ( _audio_player != nullptr )
		{
			_audio_player->remove_reference();
			_audio_player = nullptr;
		}

		_audio_buffer->remove_reference();
		_audio_buffer = nullptr;
	}

	boolean_c audio_source_c::get_is_playing() const
	{
		if ( _audio_player != nullptr )
		{
			return _audio_player->_is_playing;
		}
		return false;
	}

	boolean_c audio_source_c::start( boolean_c randomize_seek )
	{
		assert( _audio_buffer && _audio_buffer->get_is_loaded() );

		if ( _audio_player )
		{
			_audio_player->remove_reference();
			_audio_player = nullptr;
		}

		_audio_player = _audio_buffer->spawn_new_audio_player();

		assert( _audio_player != nullptr );

		_audio_player->play( randomize_seek );

		if ( !_audio_player->_is_playing ) // this could happen if the source data stream is not valid.
		{
			_audio_player->remove_reference();
			_audio_player = nullptr;
		}

		return true;
	}

	void_c audio_source_c::add_reference() const
	{
		const_cast< audio_source_c * >( this )->_reference_count++;
	}

	void_c audio_source_c::remove_reference() const
	{
		assert( _reference_count > 0 );
		const_cast< audio_source_c * >( this )->_reference_count--;
		if ( _reference_count == 0 )
		{
			delete this;
		}
	}

	audio_layer_e audio_source_c::get_layer() const
	{
		return _layer;
	}

	void_c audio_source_c::set_layer( audio_layer_e value )
	{
		_layer = value;
	}

	float32_c audio_source_c::get_volume() const
	{
		return _volume;
	}

	void_c audio_source_c::set_volume( float32_c value )
	{
		_volume = value;
	}

	float32_c audio_source_c::get_speed() const
	{
		return _speed;
	}

	void_c audio_source_c::set_speed( float32_c value )
	{
		_speed = value;
	}

	boolean_c audio_source_c::get_is_3d() const
	{
		return _is_3d;
	}

	void_c audio_source_c::set_is_3d( boolean_c value )
	{
		_is_3d = value;
	}

	//vector64x3_c const & audio_source_c::get_world_space_position() const
	//{
	//	return _world_space_position;
	//}

	//void_c audio_source_c::set_world_space_position( vector64x3_c const & value )
	//{
	//	_world_space_position = value;
	//}

	void_c audio_source_c::set_world_space_transform( space_transform_c const * value )
	{
		_world_space_transform = value;
	}

}
