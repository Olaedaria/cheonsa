#pragma once

#include "cheonsa__types.h"
#include "cheonsa_data_stream.h"
#include "cheonsa_core_list.h"
#include "cheonsa_core_linked_list.h"
#include "cheonsa_thread_safe_reference_counter.h"
#include "cheonsa_thread_critical_section.h"
#include "third_party/stb_vorbis.h"

#define cheonsa_audio_minimum_distance 0.01f // 1 centimeter radius, this makes point source a sphere, this prevents loudness from approaching infinity as the listener gets closer to the source.
#define cheonsa_audio_defactor_distance 0.1f // 10 centimeter radius, directional influence begins to be factored out when source and listener are less than this distance apart, this prevents the away-facing ear(s) from hearing less as the listener gets closer to the source. this would prevent a certain kind of sudden popping as the listener transits through the source.
#define cheonsa_audio_minimum_attenuation 0.0001f // if sources are attenuated to be less than this then they will not be mixed to the output.

namespace cheonsa
{

	enum audio_sample_format_e
	{
		audio_sample_format_e_none = 0,
		audio_sample_format_e_sint8,
		audio_sample_format_e_sint16,
		audio_sample_format_e_float32,
	};

	enum audio_wave_buffer_format_e
	{
		audio_wave_buffer_format_e_none, // will be set when state is unusable.
		audio_wave_buffer_format_e_raw, // data buffer format is raw wave (pulse-code modulation sample buffer portion of wav file).
		audio_wave_buffer_format_e_ogg, // data buffer format is ogg stream (entire ogg file).
	};

	enum audio_layer_e
	{
		audio_layer_e_music,
		audio_layer_e_ambient,
		audio_layer_e_effect,
		audio_layer_e_voice,
		audio_layer_e_menu,
		audio_layer_e_count_
	};

	enum audio_queue_command_e
	{
		audio_queue_command_e_add,
		audio_queue_command_e_remove,
	};

	// mixes audio from input to output.
	// input_sample_buffer and output_sample_buffer should be buffers of equivalent durations but they may be different sample rates (indicated by input_sample_count and output_sample_count), the mixer will resample automatically.
	// input_sample_format is the data type of each sample element.
	// input_channel_count is the number of channels in the input buffer.
	// input_sample_count is number of samples in input_sample_buffer. each sample has input_channel_count number of elements formatted as input_sample_format.
	// input_sample_buffer is the buffer that has sound samples to mix with output.
	// output_channel_count is the number of channels in the output buffer.
	// output_sample_count is number of samples in output_sample_buffer. each sample has output_channel_count number of elements formatted as float32_c.
	// output_sample_buffer is the in and out buffer to mix to.
	// output_channel_volumes defines volumes that mixer should apply to each channel.
	void_c audio_mix(
		audio_sample_format_e input_sample_format,
		sint32_c input_channel_count,
		sint32_c input_sample_count,
		void_c const * input_sample_buffer,
		sint32_c output_channel_count,
		sint32_c output_sample_count,
		float32_c * output_sample_buffer,
		float32_c * output_channel_volumes );

	class audio_wave_player_c;

	// holds wave data.
	// can be wav or ogg.
	// can be mono or stereo.
	// can be any sample rate.
	class audio_wave_buffer_c
	{
		friend class wave_out_implementation_c;
		friend class audio_interface_c;
		friend class audio_wave_player_c;

	private:
		// internal state.
		// internal state is decoupled from the external interface, so that new states can be created by the main thread in response to source file changes, and so that the audio thread can continue to mix with the old state without waiting for the new state to finish loading.
		// a wave buffer interface may create any number of these, but only the newest one is current, the older ones will be forgotten in time.
		class state_c
		{
			friend class wave_out_implementation_c;
			friend class audio_wave_player_c;

		private:
			thread_safe_reference_counter_c _reference_counter;

			audio_wave_buffer_format_e _format;

			sint32_c _channel_count; // how many channels are in the source data.
			audio_sample_format_e _sample_format; // format of each sample in the source data. not really used for ogg, since stb vorbis always samples it as float32.
			sint32_c _sample_size; // size in bytes of each sample of sample format. not really used for ogg.
			sint32_c _sample_rate; // number of samples per second of the wave, typically something like 44100.
			sint32_c _sample_count; // total number of samples in the wave. duration can be calculated from ( (float64)_sample_count / (float64)_sample_rate )

			// for wav, this is the raw wave data (conventionally known as pulse-code modulation) portion of the wav file.
			// for ogg, this is the whole ogg file.
			uint8_c * _data_buffer;
			// this is the size in bytes of _load_buffer.
			sint32_c _data_buffer_size;

			state_c( data_stream_c * stream ); // no return value, if the load fails then it does so silently, and this state will not have its _format set.

		public:
			~state_c();

			static state_c * make_new_instance( data_stream_c * stream );

			sint32_c add_reference(); // returns the new reference count. if it returns 0, then it means that there was a race condition and another thread deleted this thing, so the calling thread should gracefully forget its reference to this thing.
			sint32_c remove_reference(); // returns the new reference count.

		};

		static core_linked_list_c< audio_wave_buffer_c * > _instance_list;
		core_linked_list_c< audio_wave_buffer_c * >::node_c _instance_list_node;
		state_c * _state; // the state of this wave buffer that the mixer is mixing. the main thread may replace this at any time with a new instance. the mixing thread needs to detect when it changes so it can resituate.
		thread_safe_reference_counter_c _reference_count;

		audio_wave_buffer_c();

	public:
		~audio_wave_buffer_c();

		static audio_wave_buffer_c * make_new_instance(); // creates a new instance on the heap with reference count of 0.

		sint32_c add_reference(); // returns the new reference count. if it returns 0, then it means that there was a race condition and another thread deleted this thing, so the calling thread should gracefully forget its reference to this thing.
		sint32_c remove_reference(); // returns the new reference count.

		void_c load_new_state( data_stream_c * stream ); // creates a new state from the given stream, which may contain a riff wave (.wav) or vorbis ogg (.ogg). wave players that are still playing this wave buffer will continue to play the old state.
		void_c release_state(); // forgets state. wave players that are still playing this wave buffer will continue to play the old state.

	};

	// manages play back state of a wave buffer.
	class audio_wave_player_c
	{
		friend class wave_out_implementation_c;
		friend class audio_interface_c;
		friend class audio_scene_source_c;
		friend class audio_scene_c;

	private:
		static core_linked_list_c< audio_wave_player_c * > _instance_list;
		core_linked_list_c< audio_wave_player_c * >::node_c _instance_list_node;

		core_linked_list_c< audio_wave_player_c * >::node_c _wave_player_list_node; // for use by audio_interface_c and mixing thread.

		audio_wave_buffer_c * _wave_buffer; // the wave buffer that this wave player is currently playing. set by main thread.
		audio_wave_buffer_c::state_c * _wave_buffer_state; // the wave buffer state that this wave player is currently playing. the mixing thread uses this to detect when the wave buffer state was changed.

		boolean_c _randomize_seek;
		boolean_c _is_queued; // will be set to true if this wave player is waiting in queue for the mixing thread to formally add it. this is mainly to prevent the game thread from queuing the same wave player more than once, which isn't necessarily harmful (the mixing thread will ignore redudnant commands), but in the case that (as an example) the game's updates are running at 100 fps and the mixing thread is running at 1 fps, then the game's audio loop logic (which thinks that the wave player is not playing) will queue the wave player 100 times before the mixing thread gets around to starting _is_playing on it, and this would create some wasted overhead.
		boolean_c _is_playing; // will be set to true when the mixer thread starts play back of this wave player.

		audio_layer_e _layer; // which audio layer this wave player mixes to.
		float32_c _volume; // for 2d this is just volume, for 3d this is watts. for 2d this is capped between 0 and 1, for 3d this is capped between 0 and something high.
		float32_c _speed; // playback speed scale, and pitch. 1.0 is default.

		stb_vorbis * _ogg_stb_vorbis; // play state for ogg format.
		sint32_c _sample; // play state for raw and ogg formats.

		boolean_c _start(); // called from mixing thread to begin play back after wave player is added from queue. returns true if play back started successfully, false if wave buffer state is not ready.
		boolean_c _advance( float32_c time_step ); // called from mixing thread to advance the stream without sampling from it.
		boolean_c _sample_and_mix( sint32_c output_channel_count, sint32_c output_sample_rate, sint32_c output_sample_count, float32_c * output_sample_buffer, float32_c * output_channel_volumes ); // called from mixing thread to sample and mix this stream.

		thread_safe_reference_counter_c _reference_count;

		audio_wave_player_c();

	public:
		~audio_wave_player_c();

		static audio_wave_player_c * make_new_instance(); // creates a new instance on the heap with reference count of 0.

		sint32_c add_reference(); // returns the new reference count. if it returns 0, then it means that there was a race condition and another thread deleted this thing, so the calling thread should gracefully forget its reference to this thing.
		sint32_c remove_reference(); // returns the new reference count. if it returns 0, then it means that the calling thread should delete this thing.

		audio_wave_buffer_c * get_wave_buffer() const; // gets the most recent wave buffer.
		void_c set_wave_buffer( audio_wave_buffer_c * value ); // sets the wave buffer to play the next time this wave player is added to the audio interface or its audio scene source is added to an audio scene. if this wave player is currently playing an older state, it will finish playing that state before it changes over to this new state.

		audio_layer_e get_layer() const;
		void_c set_layer( audio_layer_e value );

		float32_c get_volume() const;
		void_c set_volume( float32_c value );

		float32_c get_speed() const;
		void_c set_speed( float32_c value );

		// returns true if the player has been queued to play or is currently playing.
		boolean_c get_appears_to_be_playing() const;

	};

	// 3d audio source.
	// wraps the functionality of a wave player, and provides spatial properties that the mixing thread uses to mix it to the output.
	// sources only play once and then they are removed from the audio scene.
	// if looping functionality is desired, then needs to be implemented externally by just re-adding the source after it finishes playing.
	// to determine if a source is not playing, check if get_wave_player()->get_appears_to_be_playing() returns false.
	class audio_scene_source_c
	{
		friend class wave_out_implementation_c;
		friend class audio_scene_c;

	private:
		core_linked_list_c< audio_scene_source_c * >::node_c _scene_source_list_node; // used by audio_scene_c and mixing thread.
		vector64x3_c _world_space_position;
		matrix32x3x3_c _world_space_basis;
		vector32x3_c _world_space_velocity;
		audio_wave_player_c * _wave_player; // this wave player is created by and owned by this audio source.

		thread_safe_reference_counter_c _reference_count;

		audio_scene_source_c();

	public:
		~audio_scene_source_c();

		static audio_scene_source_c * make_new_instance(); // creates a new instance on the heap with reference count of 0.

		sint32_c add_reference(); // returns the new reference count. if it returns 0, then it means that there was a race condition and another thread deleted this thing, so the calling thread should gracefully forget its reference to this thing.
		sint32_c remove_reference(); // returns the new reference count. if it returns 0, then it means that the calling thread should delete this thing.

		void_c set_world_space_transform( transform3d_c const & value );
		vector64x3_c const & get_world_space_position() const;
		matrix32x3x3_c const & get_world_space_basis() const;
		vector32x3_c const & get_world_space_velocity() const;

		audio_wave_player_c * get_wave_player() const; // gets the wave player that this source owns. will never return nullptr. does not change the reference count of the returned instance. also you should not change the reference count on the returned instance.

		float32_c get_effective_range_squared() const; // evaluates power, and returns the maximum distance that this source could be heard from. the game and/or scene can use this to determine when to add this audio scene source to the audio scene.

	};

	// defines how audio in a 3d scene is mixed to the output.
	class audio_scene_listener_c
	{
		friend class wave_out_implementation_c;
		friend class audio_scene_c;

	private:
		vector64x3_c _world_space_position;
		matrix32x3x3_c _world_space_basis;
		vector32x3_c _world_space_velocity;
		float32_c _damping; // factor that affects the loss of energy of sound waves as they propagate through a medium.
		float32_c _sensitivity; // effectively same thing as volume, but using more natural terminology.

		//thread_safe_reference_counter_c _reference_count;

		audio_scene_listener_c();

	public:
		//static audio_scene_listener_c * make_new_instance(); // creates a new instance on the heap with reference count of 0.
		  
		//sint32_c add_reference(); // returns the new reference count. if it returns 0, then it means that there was a race condition and another thread deleted this thing, so the calling thread should gracefully forget its reference to this thing.
		//sint32_c remove_reference(); // returns the new reference count. if it returns 0, then it means that the calling thread should delete this thing.

		void_c set_world_space_transform( transform3d_c const & value );
		vector64x3_c const & get_world_space_position() const;
		matrix32x3x3_c const & get_world_space_basis() const;
		vector32x3_c const & get_world_space_velocity() const;

		float32_c get_damping() const;
		void_c set_damping( float32_c value );

		float32_c get_sensitivity() const;
		void_c set_sensitivity( float32_c value );

	};

	// holds and mixes 3d sounds relative to a 3d listener.
	// scene sources that are added to the scene begin play back as soon as possible, they play once, and then they are removed.
	// to play them again, they must be added again.
	// the scene is intended to hold only the finite set of scene sources that are for sure in range of the listener or have potential to come in range of the listener.
	// some things we can do with this later:
	// plug this in to the physics engine to do ray casts, to detect occluders, material properties, transmission mediums, reflections/echo/reverb, etc.
	// simulate speed of sound, to create delay and doppler effects.
	// simulate low pass filtering for occluded and distant sources.
	// simulate distortions that become apparent at high energies and distances, like how lightning up close is a crack, but from kilometers away its a very low and drawn out rumble. the sound designer would only have to author the crack sound effect, the simulation model would do the rest.
	class audio_scene_c
	{
		friend class wave_out_implementation_c;
		friend class audio_interface_c;

	private:
		thread_critical_section_c _critical_section; // used to give mutually exclusive access to a single thread at a time to data that is shared between threads. the audio mixing thread runs continuously, along with any game thread(s).

		audio_scene_listener_c _scene_listener;

		struct queued_scene_source_c
		{
			audio_queue_command_e command;
			audio_scene_source_c * instance;
		};
		core_list_c< queued_scene_source_c > _queued_scene_source_list;
		core_linked_list_c< audio_scene_source_c * > _scene_source_list; // sources that are currently mixing.

		boolean_c _is_added;

		thread_safe_reference_counter_c _reference_count;

		audio_scene_c();

	public:
		~audio_scene_c();

		static audio_scene_c * make_new_instance(); // creates a new instance on the heap with reference count of 0.

		sint32_c add_reference(); // returns the new reference count. if it returns 0, then it means that there was a race condition and another thread deleted this thing, so the calling thread should gracefully forget its reference to this thing.
		sint32_c remove_reference(); // returns the new reference count. if it returns 0, then it means that the calling thread should delete this thing.

		audio_scene_listener_c const & get_scene_listener() const;
		audio_scene_listener_c & get_scene_listener();

		//vector64x3_c get_scene_listener_world_space_position() const; // used by scene to determine if a source would be in range of the listener, to determine if it should play that source or not.

		// sets the scene listener, may block the mixing thread for a brief moment.
		// may return false if there was a race condition and the passed in value was deleted by another thread (should be impossible if you're doing the right thing and holding a persistent reference to it).
		//void_c set_scene_listener( audio_scene_listener_c * value );

		// queues the given source for play back in this scene.
		// asserts if the source is already queued or mixing.
		// may return false if there was a race condition and the passed in value was deleted by another thread (should be impossible if you're doing the right thing and holding a persistent reference to it).
		boolean_c add_scene_source( audio_scene_source_c * value, boolean_c randomize_seek );

		// queues the given source for removal from this scene.
		// may return false if there was a race condition and the passed in value was deleted by another thread (should be impossible if you're doing the right thing and holding a persistent reference to it).
		boolean_c remove_scene_source( audio_scene_source_c * value );

	};

}
