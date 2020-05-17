#pragma once

#include "cheonsa_types.h"
#include "cheonsa_string16.h"
#include "cheonsa_data_scribe_ini.h"
#include "cheonsa_thread_critical_section.h"
#include "cheonsa_audio_types.h"

namespace cheonsa
{

	// holds any number of audio scenes for 3d mixing.
	// holds any number of wave players for 2d mixing.
	// manages simple music mixing (fade in, cross fade, fade out).
	// manages audio mixing.
	// manages audio playback and output.
	class audio_interface_c
	{
		friend class wave_out_implementation_c;
		friend class audio_wave_buffer_c;
		friend class audio_wave_player_c;
		friend class audio_scene_c;
		friend class engine_c;

	public:
		class settings_c
		{
		public:
			float32_c master_volume;
			float32_c music_volume;
			float32_c ambient_volume;
			float32_c effect_volume;
			float32_c voice_volume;
			float32_c menu_volume;

		public:
			settings_c();

			void_c reset_to_default(); // resets to default settings.

		};

	private:
		thread_critical_section_c _critical_section; // used to give mutually exclusive access to a single thread at a time to data that is shared between threads.

		struct queued_wave_player_c
		{
			audio_queue_command_e command;
			audio_wave_player_c * instance;
		};
		core_list_c< queued_wave_player_c > _queued_wave_player_list;
		core_linked_list_c< audio_wave_player_c * > _wave_player_list;

		struct queued_scene_c
		{
			audio_queue_command_e command;
			audio_scene_c * instance;
		};
		core_list_c< queued_scene_c > _queued_scene_list;
		core_list_c< audio_scene_c * > _scene_list;

		void_c * _implementation; // the implementation class's declaration and definition is isolated to the cpp file.

		sint32_c _output_sample_rate; // how many samples per second. a typical value that you might know of is 44100.
		sint32_c _output_channel_count; // how many channels the mixer is configured to mix output to. at the monment this is fixed to stereo and can't be changed.
		sint32_c _output_frame_rate; // how many times the mixer thread is supposed to update per second.

		float32_c _music_pause_volume;
		core_list_c< audio_wave_player_c * > _music_player_list;

		float32_c _compressor_threshold; // when a sample exceeds this threshold, then the compressor takes effect instantly. hence no attack option, and no look ahead feature.
		float32_c _compressor_fall_off_duration; // similar to release, rate of falloff of compressor effect until normalization. linear normalization, maybe just okay and not great.
		float32_c _compressor_peak; // current peak sample. minimum value is _compressor_threshold, this falls off towards _compressor_threshold at rate defined by _compressor_falloff.
		// this is intended to be used on the output buffer, after it's been mixed, and before it's submitted to the operating system.
		// updates samples and state of dynamic range compressor, applies to all channels rather than individual channels, so if one channel spikes then all of the channels will be affected.
		// sample_buffer contains input and ouput.
		// sample_count is number of samples in sample_buffer.
		// channel_count is number of sample elements per sample.
		// duration is duration in seconds of sample_buffer.
		void_c _compressor_update( float32_c * sample_buffer, sint32_c sample_count, sint32_c channel_count, float32_c duration );

		// saves audio settings to a "audio" section in the ini file.
		void_c _save_settings( data_scribe_ini_c & scribe );
		// loads audio settings from a "audio" section in the ini file.
		void_c _load_settings( data_scribe_ini_c & scribe );

	public:
		settings_c settings;

	private:
		audio_interface_c();
		~audio_interface_c();

		boolean_c start( sint32_c sample_rate = 44100, sint32_c channel_count = 2 );

	public:
		void_c refresh(); // pauses mixing thread, syncs wave players to any new wave buffer states (which restarts play back of those wave players, so it might "break" how things sound temporarily), resumes mixing thread.
		
		boolean_c add_scene( audio_scene_c * value ); // adds a 3d audio scene to this audio interface.
		boolean_c remove_scene( audio_scene_c * value ); // removes a 3d audio scene from this audio interface.

		boolean_c add_wave_player( audio_wave_player_c * value ); // adds a 2d wave player to this audio interface. the wave player will play once and then be removed.
		boolean_c remove_wave_player( audio_wave_player_c * value ); // removes a 2d wave player from this audio interface.

		void_c play_music( string16_c const & file_path, float32_c fade_duration );
		void_c stop_music( float32_c fade_duration );
		void_c pause_music(); // fades out the music really quick.
		void_c resume_music(); // fades in the music really quick.

	};

}
