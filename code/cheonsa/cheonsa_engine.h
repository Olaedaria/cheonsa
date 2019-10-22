#pragma once

// this file has two underscores in its name so that it appears at the top when the files are sorted alphabetically.

#include "cheonsa_base_types.h"
#include "cheonsa_math_types.h"

#include "cheonsa_user_interface.h"
#include "cheonsa_content_manager.h"
#include "cheonsa_reflection_manager.h"
#include "cheonsa_script_manager.h"
#include "cheonsa_resource_manager.h"
#include "cheonsa_glyph_manager.h"
#include "cheonsa_input_manager.h"
#include "cheonsa_menu_style_manager.h"

#if defined( cheonsa_video_d3d11 )
#include "cheonsa_video_interface_d3d11.h"
#endif

#if defined( cheonsa_platform_windows )
#include "cheonsa_audio_interface_wave_out.h"
#endif

#include "cheonsa_video_renderer_interface.h"
#include "cheonsa_video_renderer_shader_manager.h"
#include "cheonsa_platform_pointer.h"
#include "cheonsa_platform_icon.h"

#include "cheonsa_core_event.h"
#include "cheonsa_core_circle_buffer.h"

#include "cheonsa_game.h"

#define debug_key_toggle_console input_keyboard_key_e_grave
#define debug_key_toggle_stats input_keyboard_key_e_f1
#define debug_key_toggle_menu_bounds input_keyboard_key_e_f2
#define debug_key_refresh_resources input_keyboard_key_e_f5

namespace cheonsa
{

	//enum run_time_mode_e
	//{
	//	run_time_mode_e_editor,
	//	run_time_mode_e_player,
	//};

	enum window_state_e
	{
		window_state_e_minimized,
		window_state_e_normaled,
		window_state_e_maximized,
	};

	// the engine class.
	//
	// this is essentially the entry point to everything else.
	// some terms:
	// "engine", this class, of which there is only one global instance shared per application process, which can be accessed from any source file by including "cheonsa_engine.h" and referencing cheonsa::engine_instance.
	// "client", the application process instance that runs on the user's computer, which executes the engine and game. this is going to be "dream_player.exe".
	// "client window", the window of the application that runs on the user's computer, of which the user can interact with the engine and game and to which the engine and game outputs video to.
	// 
	// data_folder redirect override:
	// data_folder defines the folder that contains "engine_data" and "game_data" sub-folders.
	// data_folder defaults to the executable's folder.
	// at engine start up, the engine scans the executable's folder for file with the same name as the executable but with ".txt" extension.
	// if this file is found, then it is expected to contain one line of text that defines an absolute file path to set data_folder to.
	//
	// example folder structure, for default operation:
	// "[executable folder]/dream_player.exe" (the game client)
	// "[executable folder]/engine_data" (contains files that are required for the engine to run, for all locales)
	// "[executable folder]/game_data" (contains all of the game's content, for all locales)
	//
	// override folder structure, for debug operation:
	// "[executable folder]/dream_player.exe" (the game client)
	// "[executable folder]/dream_player.txt" (text file that contains an absolute file path to the real data folder to use, which we will call data_folder)
	// "[data_folder]/engine_data"
	// "[data_folder]/game_data"
	// localization:
	// at start up, the engine attempts to load locale_code from the settings file, but if it can't then it defaults to "en-US".
	// when apply_locale_code() is called, which happens once automatically after the engine loads its settings...
	// the engine looks for core_engine_data_folder_path + locale_code + "strings.xml", which defines strings that are used by the engine.
	// the engine looks for core_game_data_folder_path + locale_code + "strings.xml", which defines strings that are used by the game.
	// then appropriate systems and objects are notified of the locale_code change so that they can re-acquire their strings and resources.
	// however when this occurs during engine start up, there won't be any systems or objects instantiated yet that will be affected by this;
	// they will acquire their strings and resources at the time that they are initialized, which happens later.
	//
	// user input evemts are routed through menu_context first, then if it isn't consumed then it is processed by game.
	// then game updates, which updates animations:
	//   models with animations can be updated on worker threads.
	//   models with physically based animations i'm not sure yet.
	//
	// to do:
	//   create our own physics engine that is integrated directly into our scene system, so that we can stop using bullet which simulates its own copy of our scene which is redundant.
	class engine_c
	{
	public:
		// the main interfaces that the game interacts with to do stuff.
		class interfaces_c
		{
		public:
			game_c * const game; // the game instance.
			content_manager_c * const content_manager; // defines data folders, locale setting, and manages game strings.
			resource_manager_c * const resource_manager; // pointer to global resource manager instance, which manages loading of localized fonts, models, model materials, sounds, and textures.
			audio_interface_c * const audio_interface; // platform specific audio interface for the game to use.
			video_interface_c * const video_interface; // platform specific video interface for the game to use.
			video_renderer_interface_c * const video_renderer_interface; // renderer interface for the game to use.
			video_renderer_shader_manager_c * const video_renderer_shader_manager; // manages shader loading, compiling, and caching.
			glyph_manager_c * const glyph_manager; // manages cpu rendering and caching of glyph images to textures, which can then be uploaded to the gpu. also manages loading cache from disk at start, and saving cache to disk at stop.
			menu_style_manager_c * const menu_style_manager; // pointer to the global menu style index instance, which manages loading of "menu_styles.xml" files for engine and game.
			input_manager_c * const input_manager; // pointer to global input interface instance, which gathers input events for the engine and game to process each frame.
			script_manager_c * const script_manager; // pointer to global script index instance, holds all game script scopes and game script functions so that scripts can reference them.
			reflection_manager_c * const reflection_manager; // pointer to global reflection index instance, enables reflection feature and property inspector.
			user_interface_c * const user_interface; // the user interface, the game manages this closely to control the user's experience. it controls the 2d menus and 3d scene/menus that the user interacts with, and manages routing input events into those things, and manages mixing audio from those things, and rendering those things.

			interfaces_c();

		} interfaces;

		// tracks current client window state and client environment state.
		// also provides functions to get and set the window state during run time.
		class environment_c
		{
		private:
			friend class engine_c;
			friend class engine_members_c;

			//boolean_c _is_running; // set to false at any time, and the engine will terminate itself at the next update.

			window_state_e _window_state; // treat as read only, but use set_window_state() to change.
			boolean_c _window_is_focused; // will be set to true as long as the client's window has focus.
			sint32_c _window_edge_thickness; // when the client window is borderless mode, this tells the non client hit detection logic the thickness of each resizable edge handle in pxiels.
			sint32_c _window_title_bar_thickness; // when the client window is in borderless mode, this tells the non client hit detection logic the size of the title bar in pixels.

			string16_c _executable_folder_path; // folder path of _executable_file_path.
			string16_c _executable_file_name; // file name of _executable_file_path.

			sint32_c _logical_processor_count; // used to determine a reasonable maximum number of worker threads to spawn for doing things.

			environment_c();

		public:
			//boolean_c get_is_running() const;
			//void_c set_is_running( boolean_c value ); // set to false to make the engine exit at its convenience.

			void_c * get_window_handle(); // gets the client window handle, but casted to a void_c * type, so that it is platform neutral.
			void_c set_window_title( string16_c const & value ); // called by client to set title of the window.
			void_c set_window_icon_big( platform_icon_c * value ); // called by client to set icon of the window. the big icon is shown when alt+tabbing.
			void_c set_window_icon_small( platform_icon_c * value ); // called by client to set icon of the window. the small icon is shown in the title bar of the window.

			window_state_e get_window_state() const;
			void_c set_window_state( window_state_e value );

			sint32_c get_window_edge_thickness() const;
			sint32_c get_window_title_bar_thickness() const;

		} environment;

		// tracks current debug state.
		class debug_c
		{
		public:
			vector32x4_c grid_color;
			vector32x4_c axis_a_color;
			vector32x4_c axis_b_color;
			vector32x4_c axis_c_color;
			vector32x4_c axis_d_color;
			vector32x4_c bone_color;

			boolean_c draw_menu_bounds; // draws boxes around menu controls.
			boolean_c draw_world_axes; // draws axes at center of scene.
			boolean_c draw_world_grid; // draws grid around center of scene.
			boolean_c draw_object_light_probes; // draws spheres for light probes in scene.
			boolean_c draw_object_icons; // draws icons of objects of objects in scene.
			boolean_c draw_object_axes; // draws axes of objects in scene.
			boolean_c draw_object_bounds; // draws bounds of objects in scene.
			boolean_c draw_object_bones; // draws bones of model objects in scene.
			boolean_c draw_object_physics; // draws physics debug.

		private:
			friend class engine_c;

			boolean_c _log_file_is_open;
			data_stream_file_c _log_file; // log file stream located at environment.engine_data_folder + "log.txt".

			boolean_c _statistics_is_showing;
			string16_c _statistics_text;

			struct console_line_c
			{
				log_type_e type;
				string16_c message;
				console_line_c & operator = ( console_line_c const & other );
			};

			boolean_c _console_is_showing;
			core_circle_buffer_c< console_line_c > _console_lines;

			debug_c();

		public:
			void_c _start();
			void_c _stop();

			void_c _log( log_type_e type, string16_c const & message ); // outputs the message to the debugger log (if _DEBUG), and writes the message to the log file, and to the console.

			boolean_c get_statistics_is_showing();
			void_c set_statistics_is_showing( boolean_c value );

			boolean_c get_console_is_showing();
			void_c set_console_is_showing( boolean_c value );

		} debug;

	private:
		friend class engine_members_c;

		engine_members_c * _members; // a pointer to an instance of cheonsa::engine_members_c that is allocated on the heap, who's declaration is isolated to the cpp source file because it contains all that platform cruft that we want to isolate our headers from.

		string16_c _game_name;

		boolean_c _is_running;

		data_scribe_ini_c _settings_ini;

		// engine settings are saved to the file "[engine_data_folder_path]/settings".
		// this is only called once at engine start up.
		// if the settings file does not exist, then appropriate settings will be chosen and a settings file will be saved.
		boolean_c _load_settings_ini();

		// engine settings are saved to the file "[engine_data_folder_path]/settings".
		// this can be called at any time during run time.
		boolean_c _save_settings_ini();

		void_c _open_client_window(); // opens the client window to the size specified in engine_settings_c, and then centers it.
		void_c _center_client_window(); // centers the window again after exiting full screen.
		void_c _close_client_window(); // closes the client window.

		void_c _update( float32_c time_step ); // updates the engine and game.
		void_c _render( float32_c time_step ); // renders the engine and game.

	public:
		engine_c();
		~engine_c();

		boolean_c start( char16_c const * const * arguments, sint32_c arguments_count, game_c * game );
		void_c stop(); // call at any time while the main loop is running to tell the engine to exit at its convenience.

		string16_c const & get_game_name() const; // gets the text that is used for the window's title and the app data folder name.
		void_c set_game_name( string16_c const & value ); // sets the text that is used for the window's title and the app data folder name.

	};

	// singleton global engine instance, only one per process.
	extern engine_c global_engine_instance;

}
