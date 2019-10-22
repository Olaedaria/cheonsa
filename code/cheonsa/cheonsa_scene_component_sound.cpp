#include "cheonsa_scene_component_sound.h"
#include "cheonsa_scene_object.h"
#include "cheonsa_scene.h"
#include "cheonsa_user_interface.h"
#include "cheonsa_audio_source.h"
#include "cheonsa_ops.h"

namespace cheonsa
{

	void_c scene_component_sound_c::_handle_after_added_to_scene()
	{
		assert( _scene_object != nullptr );
		assert( _scene_object->get_scene() != nullptr );
		if ( _audio_source )
		{
			_scene_object->get_scene()->get_audio_scene()->add_source( _audio_source );
		}
	}

	void_c scene_component_sound_c::_handle_before_removed_from_scene()
	{
		assert( _scene_object != nullptr );
		assert( _scene_object->get_scene() != nullptr );
		if ( _audio_source && _scene_object->get_scene() )
		{
			_scene_object->get_scene()->get_audio_scene()->remove_source( _audio_source );
		}
	}

	void_c scene_component_sound_c::_handle_sound_resource_on_load( resource_object_c * resource )
	{
		assert( _audio_source == nullptr );
		_audio_source = new audio_source_c( _sound_resource->get_audio_buffer() );
		_audio_source->set_layer( _audio_layer );
		_audio_source->set_volume( ops::random_float32( _volume_minimum, _volume_maximum ) );
		_audio_source->set_speed( ops::random_float32( _speed_minimum, _speed_maximum ) );

		if ( _scene_object != nullptr )
		{
			_audio_source->set_world_space_transform( &_scene_object->get_world_space_transform() );
		}
		_audio_source->start( true );
		if ( _scene_object && _scene_object->get_scene() )
		{
			_scene_object->get_scene()->get_audio_scene()->add_source( _audio_source );
		}
	}

	void_c scene_component_sound_c::_handle_sound_resource_on_un_load( resource_object_c * resource )
	{
		if ( _audio_source != nullptr )
		{
			if ( _scene_object && _scene_object->get_scene() )
			{
				_scene_object->get_scene()->get_audio_scene()->remove_source( _audio_source );
			}
			_audio_source->set_world_space_transform( nullptr );
			_audio_source->remove_reference();
			_audio_source = nullptr;
		}

		if ( _sound_resource != nullptr )
		{
			_sound_resource->on_load.unsubscribe( this, &scene_component_sound_c::_handle_sound_resource_on_load );
			_sound_resource->on_unload.unsubscribe( this, &scene_component_sound_c::_handle_sound_resource_on_un_load );
		}
	}

	scene_component_sound_c::scene_component_sound_c()
		: scene_component_c()
		, _audio_source( nullptr )
		, _sound_resource()
		, _audio_layer( audio_layer_e_ambient )
		, _volume_minimum( 1.0f )
		, _volume_maximum( 1.0f )
		, _speed_minimum( 1.0f )
		, _speed_maximum( 1.0f )
		, _interval_minimum( 0.0f )
		, _interval_maximum( 0.0f )
	{
	}

	scene_component_sound_c::~scene_component_sound_c()
	{
		_handle_sound_resource_on_un_load( _sound_resource );
	}

	resource_object_sound_c * scene_component_sound_c::get_sound_resource()
	{
		return _sound_resource;
	}

	void_c scene_component_sound_c::set_sound_resource( resource_object_sound_c * value )
	{
		_handle_sound_resource_on_un_load( nullptr );
		_sound_resource = value;
		_handle_sound_resource_on_load( nullptr );
	}

	audio_layer_e scene_component_sound_c::get_audio_layer() const
	{
		return _audio_layer;
	}

	void_c scene_component_sound_c::set_audio_layer( audio_layer_e value )
	{
		_audio_layer = value;
	}

	void_c scene_component_sound_c::get_volume( float32_c & minimum, float32_c & maximum ) const
	{
		minimum = _volume_minimum;
		maximum = _volume_maximum;
	}

	void_c scene_component_sound_c::set_volume( float32_c minimum, float32_c maximum )
	{
		static float32_c const limit_minimum = 0.0f;
		static float32_c const limit_maximum = 100.0f;
		_volume_minimum = ops::math_clamp( minimum, limit_minimum, limit_maximum );
		_volume_maximum = ops::math_clamp( maximum, _volume_minimum, limit_maximum );
	}

	void_c scene_component_sound_c::get_speed( float32_c & minimum, float32_c & maximum ) const
	{
		minimum = _speed_minimum;
		maximum = _speed_maximum;
	}

	void_c scene_component_sound_c::set_speed( float32_c minimum, float32_c maximum )
	{
		static float32_c const limit_minimum = 0.01f;
		static float32_c const limit_maximum = 10.0f;
		_speed_minimum = ops::math_clamp( minimum, limit_minimum, limit_maximum );
		_speed_maximum = ops::math_clamp( maximum, _speed_minimum, limit_maximum );
	}

	void_c scene_component_sound_c::get_interval( float32_c & minimum, float32_c & maximum ) const
	{
		minimum = _interval_minimum;
		maximum = _interval_maximum;
	}

	void_c scene_component_sound_c::set_interval( float32_c minimum, float32_c maximum )
	{
		static float32_c const limit_minimum = 0.0f;
		static float32_c const limit_maximum = 1000000.0f;
		_interval_minimum = ops::math_clamp( minimum, limit_minimum, limit_maximum );
		_interval_maximum = ops::math_clamp( maximum, _interval_minimum, limit_maximum );
	}

}
