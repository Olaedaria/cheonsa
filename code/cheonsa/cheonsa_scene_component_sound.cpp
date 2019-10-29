#include "cheonsa_scene_component_sound.h"
#include "cheonsa_scene_object.h"
#include "cheonsa_scene.h"
#include "cheonsa_user_interface.h"
#include "cheonsa_ops.h"

namespace cheonsa
{

	void_c scene_component_sound_c::_handle_after_added_to_scene()
	{
		assert( _scene_object != nullptr );
		assert( _scene_object->get_scene() != nullptr );
		_scene_object->get_scene()->_sound_list.insert_at_end( &_sound_list_node );
	}

	void_c scene_component_sound_c::_handle_before_removed_from_scene()
	{
		assert( _scene_object != nullptr );
		assert( _scene_object->get_scene() != nullptr );
		_scene_object->get_scene()->_sound_list.remove( &_sound_list_node );
	}

	scene_component_sound_c::scene_component_sound_c()
		: scene_component_c()
		, _sound_list_node( this )
		, _sound_resource()
		, _audio_scene_source( nullptr )
		, _play( true )
		, _is_first_play( true )
		, _power_minimum( 1.0f )
		, _power_maximum( 1.0f )
		, _speed_minimum( 1.0f )
		, _speed_maximum( 1.0f )
		, _interval_minimum( 0.0f )
		, _interval_maximum( 0.0f )
	{
		_audio_scene_source = new audio2_scene_source_c();
	}

	scene_component_sound_c::~scene_component_sound_c()
	{
		_audio_scene_source->remove_reference();
		_audio_scene_source = nullptr;
	}

	resource_file_sound_c * scene_component_sound_c::get_sound_resource() const
	{
		return _sound_resource;
	}

	void_c scene_component_sound_c::set_sound_resource( resource_file_sound_c * value )
	{
		_sound_resource = value;
	}

	audio2_layer_e scene_component_sound_c::get_audio_layer() const
	{
		return _audio_scene_source->get_wave_player()->get_layer();
	}

	void_c scene_component_sound_c::set_audio_layer( audio2_layer_e value )
	{
		_audio_scene_source->get_wave_player()->set_layer( value );
	}

	boolean_c scene_component_sound_c::get_play() const
	{
		return _play;
	}

	void_c scene_component_sound_c::set_play( boolean_c value )
	{
		_play = value;
	}

	void_c scene_component_sound_c::get_power( float32_c & minimum, float32_c & maximum ) const
	{
		minimum = _power_minimum;
		maximum = _power_maximum;
	}

	void_c scene_component_sound_c::set_power( float32_c minimum, float32_c maximum )
	{
		static float32_c const limit_minimum = 0.0f;
		static float32_c const limit_maximum = 100.0f;
		_power_minimum = ops::math_clamp( minimum, limit_minimum, limit_maximum );
		_power_maximum = ops::math_clamp( maximum, _power_minimum, limit_maximum );
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

	void_c scene_component_sound_c::update_audio( float32_c time_step )
	{
		if ( _play )
		{
			_interval_counter += time_step;
			if ( _interval_counter >= _interval )
			{
				audio2_scene_listener_c * scene_listener = _scene_object->get_scene()->get_audio_scene()->get_scene_listener();
				if ( _audio_scene_source->get_wave_player()->get_appears_to_be_playing() == false && scene_listener != nullptr )
				{
					float32_c distance_squared = ops::make_float32_length_squared( vector32x3_c( _scene_object->get_world_space_position() - scene_listener->get_world_space_position() ) );
					float32_c effective_range_squared = _audio_scene_source->get_effective_range_squared();
					if ( distance_squared <= effective_range_squared )
					{
						_audio_scene_source->get_wave_player()->set_volume( ops::random_float32( _power_minimum, _power_maximum ) );
						_audio_scene_source->get_wave_player()->set_speed( ops::random_float32( _speed_minimum, _speed_maximum ) );
						_scene_object->get_scene()->get_audio_scene()->add_scene_source( _audio_scene_source, _is_first_play );
						_is_first_play = false;
					}
				}
				_interval_counter = 0.0f;
				_interval = ops::random_float32( _interval_minimum, _interval_maximum );
			}
		}
	}

}
