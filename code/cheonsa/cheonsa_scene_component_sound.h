#pragma once

#include "cheonsa_scene_component.h"
#include "cheonsa_audio2.h"
#include "cheonsa_resource_file_sound.h"

namespace cheonsa
{

	// plays looping audio for ambient sound effects.
	// randomizes upon on first play, so that upon loading into a scene for the first time everything isn't all synchronized.
	// for one-off play-once sound effects, don't use this component.
	class scene_component_sound_c
		: public scene_component_c
	{
	public:
		static inline uint8_c get_type_code_static() { return scene_component_type_e_sound; }
		virtual inline uint8_c get_type_code() const override { return get_type_code_static(); }

	public:
		core_linked_list_c< scene_component_sound_c * >::node_c _sound_list_node;

		resource_file_sound_c::reference_c _sound_resource;

		audio2_scene_source_c * _audio_scene_source;

		boolean_c _play;

		boolean_c _is_first_play;

		float32_c _power_minimum;
		float32_c _power_maximum;

		float32_c _speed_minimum;
		float32_c _speed_maximum;

		float32_c _interval_minimum;
		float32_c _interval_maximum;
		float32_c _interval;
		float32_c _interval_counter;

		virtual void_c _handle_after_added_to_scene() override;
		virtual void_c _handle_before_removed_from_scene() override;

	public:
		scene_component_sound_c(); // constructor is protected to prevent instantiation on the stack. use make_new_instance() to make a new instance on the heap. then use delete as normal to delete it.
		virtual ~scene_component_sound_c() override;

		resource_file_sound_c * get_sound_resource() const;
		void_c set_sound_resource( resource_file_sound_c * value );
		
		audio2_layer_e get_audio_layer() const;
		void_c set_audio_layer( audio2_layer_e value );

		boolean_c get_play() const;
		void_c set_play( boolean_c value );

		void_c get_power( float32_c & minimum, float32_c & maximum ) const;
		void_c set_power( float32_c minimum, float32_c maximum );

		void_c get_speed( float32_c & minimum, float32_c & maximum ) const;
		void_c set_speed( float32_c minimum, float32_c maximum );

		void_c get_interval( float32_c & minimum, float32_c & maximum ) const;
		void_c set_interval( float32_c minimum, float32_c maximum );

		void_c update_audio( float32_c time_step );

	};

}
