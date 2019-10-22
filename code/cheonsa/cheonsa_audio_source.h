#pragma once

#include "cheonsa_audio_types.h"

namespace cheonsa
{

	// audio source that can be used for 3d and 2d sound play back.
	// it will play once and then the audio thread will drop its reference.
	// defines a bunch of mixing properties and spatial properties which defines how the audio thread will mix with the final audio output.
	// uses reference counting because this instance may be shared between multiple threads.
	// only create instances on the heap, don't create instances on the stack.
	// once you create a new instance, the constuctor will initialize its reference count to 1.
	// when you are done with it, just call remove_reference() and forget about it, don't delete it yourself.
	class audio_source_c
	{
	protected:
		friend class audio_scene_c;
		friend class wave_out_process_c;
		friend class audio_interface_c;

		//enum flag_e
		//{
		//	flag_e_none = 0,
		//	flag_e_kill = 0x01 // if true then the audio thread should forget its reference to this source as soon as it can.
		//};

		audio_layer_e _layer; // what layer this source plays on.
		float32_c _volume; // current playback volume.
		float32_c _speed; // play back speed scale, causes pitch shift.
		boolean_c _is_3d; // if true then this source will be mixed into the output as though it is in 3D space and relative to the observer.
		//vector64x3_c _world_space_position; // the position of this source in the scene.
		space_transform_c const * _world_space_transform; // 

		audio_scene_c * _audio_scene; // the audio scene that this audio source lives in.
		audio_buffer_c const * _audio_buffer;
		audio_player_c * _audio_player;
		//uint32_c _flags; // see flag_e.
		float32_c _interval_timer; // current value of down time, increments by time step each audio update. once this value reaches _interval_timer_target then the sound will begin playing again.
		float32_c _interval_timer_target; // target value of down time.
		sint32_c _reference_count;

	public:
		audio_source_c( audio_buffer_c const * buffer ); // initializes this audio source and starts play back, so that it will begin to play back once it is added to the audio interface, or added to an audio scene which is added to the audio interface.
		~audio_source_c();

		boolean_c get_is_playing() const; // returns true if the underlying audio player is still playing, false if it finished. this lets the game restart the source again.

		boolean_c start( boolean_c randomize_seek ); // starts or restarts play back of this source. randomize seek is needed to prevent multiple instances of the same sound from playing all at the same time (usually upon loading in to the scene for the first time) and creating a sound experience that feels really fake.

		void_c add_reference() const;
		void_c remove_reference() const;

		audio_layer_e get_layer() const;
		void_c set_layer( audio_layer_e value );

		float32_c get_volume() const;
		void_c set_volume( float32_c value );

		float32_c get_speed() const;
		void_c set_speed( float32_c value );

		boolean_c get_is_3d() const;
		void_c set_is_3d( boolean_c value );

		//vector64x3_c const & get_world_space_position() const;
		//void_c set_world_space_position( vector64x3_c const & value );

		void_c set_world_space_transform( space_transform_c const * value );

	};

}
