#pragma once

#include "cheonsa_scene_component.h"
#include "cheonsa_resource_object_sound.h"

namespace cheonsa
{

	class scene_component_sound_c : public scene_component_c
	{
	public:
		static inline uint8_c get_type_code_static() { return scene_component_type_e_sound; }
		virtual inline uint8_c get_type_code() const override { return get_type_code_static(); }

	public:
		// the underlying audio interface audio source, which also manages its own audio player.
		// we hold a reference to this so that we can check on it and restart it when it finishes playing.
		// we are responsible for looping because the audio thread 
		audio_source_c * _audio_source;
		resource_object_sound_c::reference_c _sound_resource;
		audio_layer_e _audio_layer;
		float32_c _volume_minimum;
		float32_c _volume_maximum;
		float32_c _speed_minimum;
		float32_c _speed_maximum;
		float32_c _interval_minimum;
		float32_c _interval_maximum;

		virtual void_c _handle_after_added_to_scene() override;
		virtual void_c _handle_before_removed_from_scene() override;

		void_c _handle_sound_resource_on_load( resource_object_c * resource );
		void_c _handle_sound_resource_on_un_load( resource_object_c * resource );

	public:
		scene_component_sound_c(); // constructor is protected to prevent instantiation on the stack. use make_new_instance() to make a new instance on the heap. then use delete as normal to delete it.
		virtual ~scene_component_sound_c() override;

		resource_object_sound_c * get_sound_resource();
		void_c set_sound_resource( resource_object_sound_c * value );

		audio_layer_e get_audio_layer() const;
		void_c set_audio_layer( audio_layer_e value );

		void_c get_volume( float32_c & minimum, float32_c & maximum ) const;
		void_c set_volume( float32_c minimum, float32_c maximum );

		void_c get_speed( float32_c & minimum, float32_c & maximum ) const;
		void_c set_speed( float32_c minimum, float32_c maximum );

		void_c get_interval( float32_c & minimum, float32_c & maximum ) const;
		void_c set_interval( float32_c minimum, float32_c maximum );

	};

}
