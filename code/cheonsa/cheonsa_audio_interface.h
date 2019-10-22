#pragma once

#include "cheonsa_audio_types.h"
#include "cheonsa_math_types.h"
#include "cheonsa_core_list.h"
#include "cheonsa_data_scribe_ini.h"
#include "cheonsa_platform_critical_section.h"

namespace cheonsa
{

	// main audio mixing interface.
	class audio_interface_c
	{
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

	protected:
		friend class engine_c;
		friend class wave_out_process_c;

		// mixer settings. these should remain [static|constant] once the mixer is start()ed.
		sint32_c _channel_count; // how many channels the mixer is configured to mix output to. at the monment this is fixed to stereo and can't be changed.
		sint32_c _sample_rate; // how many samples per second. a typical value that you might know of is 44100.
		sint32_c _frames_per_second; // how many times the mixer thread is supposed to update per second.
		sint32_c _samples_per_frame; // how many samples there are in one mixing frame. this is equal to _frame_frequency / _frames_per_second, hopefully it is evenly divisible.

		core_list_c< audio_source_c * > _audio_sources; // 2d audio sources, most likely these will all be play-once sources spawned by menus and such.
		core_list_c< audio_scene_c * > _audio_scenes; // all the 3d audio scenes to mix into the output. each scene contains one listener and any number of sources.
		platform_critical_section_c _critical_section; // critical section for mutually exclusive access to _audio_sources and _audio_scenes.

		// saves audio settings to a "audio" section in the ini file.
		void_c _save_settings( data_scribe_ini_c & scribe );
		// loads audio settings from a "audio" section in the ini file.
		void_c _load_settings( data_scribe_ini_c & scribe );

	public:
		// it is safe to set the values in here at any time since it will be an atomic operation, the audio mixing thread reads these values in real time.
		settings_c settings;

	public:
		audio_interface_c();
		virtual ~audio_interface_c() = 0;

		virtual boolean_c start() = 0;

		// creates and adds an audio scene to the audio interface.
		void_c add_audio_scene( audio_scene_c * audio_scene );
		// removes an audio scene from the audio interface.
		void_c remove_audio_scene( audio_scene_c * audio_scene );

		// plays a 2d sound once.
		// this is mainly for use by menus.
		void_c play_2d( audio_buffer_c * buffer, audio_layer_e layer, float32_c volume, float32_c speed );

		// begins play back of the given music file.
		// file_path_relative references a music file in any of the data folders of the content manager (preferably .ogg format).
		// fade_speed is time in seconds to cross fade to new song.
		void_c play_music( string16_c const & file_path_relative );
		// stops play back of any music that is currently playing.
		// fade_speed is time in seconds to fade out.
		void_c stop_music();

	};

}
