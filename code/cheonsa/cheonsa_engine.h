#pragma once

// this file has two underscores in its name so that it appears at the top when the files are sorted alphabetically.

#include "cheonsa__types.h"

#include "cheonsa_user_interface.h"
#include "cheonsa_window_manager.h"
#include "cheonsa_content_manager.h"
#include "cheonsa_debug_manager.h"
#include "cheonsa_reflection_manager.h"
#include "cheonsa_script_manager.h"
#include "cheonsa_resource_manager.h"
#include "cheonsa_glyph_manager.h"
#include "cheonsa_input_manager.h"
#include "cheonsa_menu_style_manager.h"

#if defined( cheonsa_video_d3d11 )
#include "cheonsa_video_interface_d3d11.h"
#endif

#include "cheonsa_audio_interface.h"
#include "cheonsa_video_renderer_interface.h"
#include "cheonsa_video_renderer_shader_manager.h"
#include "cheonsa_platform_pointer.h"
#include "cheonsa_platform_icon.h"

#include "cheonsa_core_event.h"
#include "cheonsa_core_circle_buffer.h"

#include "cheonsa_game_i.h"

#define debug_key_toggle_console input_keyboard_key_e_grave
#define debug_key_toggle_stats input_keyboard_key_e_f1
#define debug_key_toggle_menu_bounds input_keyboard_key_e_f2
#define debug_key_refresh_resources input_keyboard_key_e_f5

namespace cheonsa
{

	/*

	the engine class.
	
	this is essentially the entry point to everything else.
	some terms:
	"engine", this class, of which there is only one global instance shared per application process, which can be accessed from any source file by including "cheonsa_engine.h" and referencing cheonsa::global_engine_instance.
	"client", the application process instance that runs on the user's computer, which executes the engine and game. this is going to be "dream_player.exe".
	"client window", the window of the application that runs on the user's computer, of which the user can interact with the engine and game and to which the engine and game outputs video to.
	
	data_folder redirect override:
	data_folder defines the folder that contains "engine_data" and "game_data" sub-folders.
	data_folder defaults to the executable's folder.
	at engine start up, the engine scans the executable's folder for file with the same name as the executable but with ".txt" extension.
	if this file is found, then it is expected to contain one line of text that defines an absolute file path to set data_folder to.
	
	example folder structure, for default operation:
	"[executable folder]/dream_player.exe" (the game client)
	"[executable folder]/engine_data" (contains files that are required for the engine to run, for all locales)
	"[executable folder]/game_data" (contains all of the game's content, for all locales)
	
	override folder structure, for debug operation:
	"[executable folder]/dream_player.exe" (the game client)
	"[executable folder]/dream_player.txt" (text file that contains an absolute file path to the real data folder to use, which we will call data_folder)
	"[data_folder]/engine_data"
	"[data_folder]/game_data"
	localization:
	at start up, the engine attempts to load locale_code from the settings file, but if it can't then it defaults to "en-US".
	when apply_locale_code() is called, which happens once automatically after the engine loads its settings...
	the engine looks for core_engine_data_folder_path + locale_code + "strings.xml", which defines strings that are used by the engine.
	the engine looks for core_game_data_folder_path + locale_code + "strings.xml", which defines strings that are used by the game.
	then appropriate systems and objects are notified of the locale_code change so that they can re-acquire their strings and resources.
	however when this occurs during engine start up, there won't be any systems or objects instantiated yet that will be affected by this;
	they will acquire their strings and resources at the time that they are initialized, which happens later.
	
	user input evemts are routed through menu_context first, then if it isn't consumed then it is processed by game.
	then game updates, which updates animations:
	  models with animations can be updated on worker threads.
	  models with physically based animations i'm not sure yet.
	
	to do:
	  create our own physics engine that is integrated directly into our scene system, so that we can stop using bullet which simulates its own copy of our scene which is redundant.

	  */
	class engine_c
	{
		friend class engine_members_c;

	private:
		game_i * _game; // the game instance.
		debug_manager_c * _debug_manager;
		window_manager_c * _window_manager;
		content_manager_c * _content_manager;
		resource_manager_c * _resource_manager;
		audio_interface_c * _audio_interface;
		video_interface_c * _video_interface;
		video_renderer_interface_c * _video_renderer_interface;
		video_renderer_shader_manager_c * _video_renderer_shader_manager;
		glyph_manager_c * _glyph_manager;
		menu_style_manager_c * _menu_style_manager;
		input_manager_c * _input_manager;
		script_manager_c * _script_manager;
		reflection_manager_c * _reflection_manager;
		user_interface_c * _user_interface;

		

		boolean_c _is_running;

		void_c _update( float32_c time_step ); // updates the engine and game.
		void_c _render( float32_c time_step ); // renders the engine and game.

	public:
		engine_c();
		~engine_c();

		boolean_c start( game_i * game, char16_c const * const * arguments, sint32_c arguments_count );
		void_c stop(); // call at any time while the main loop is running to tell the engine to exit at its convenience.

		boolean_c get_is_running() const;

		// the game instance.
		inline game_i * get_game() const { return _game; }
		// manages log and console.
		inline debug_manager_c * get_debug_manager() const { return _debug_manager; }
		// manages creation of window.
		inline window_manager_c * get_window_manager() const { return _window_manager; }
		// manages ini settings file, data folders, locale setting, and game strings.
		inline content_manager_c * get_content_manager() const { return _content_manager; }
		// manages loading of assets.
		inline resource_manager_c * get_resource_manager() const { return _resource_manager; }
		// manages audio mixing.
		inline audio_interface_c * get_audio_interface() const { return _audio_interface; }
		// forms a layer to the underlying video api.
		inline video_interface_c * get_video_interface() const { return _video_interface; }
		// manages rendering of scenes and menus.
		inline video_renderer_interface_c * get_video_renderer_interface() const { return _video_renderer_interface; }
		// manages loading, compiling, and caching of shaders.
		inline video_renderer_shader_manager_c * get_video_renderer_shader_manager() const { return _video_renderer_shader_manager; }
		// manages loading, rendering (by cpu to gpu texture), and caching, of font glyphs.
		inline glyph_manager_c * get_glyph_manager() const { return _glyph_manager; }
		// manages shared menu styles (themes, skins).
		inline menu_style_manager_c * get_menu_style_manager() const { return _menu_style_manager; }
		// manages recording of user input.
		inline input_manager_c * get_input_manager() const { return _input_manager; }
		// manages script instances and executions.
		inline script_manager_c * get_script_manager() const { return _script_manager; }
		// manages reflection types, which lets object instances share their properties with other interfaces.
		inline reflection_manager_c * get_reflection_manager() const { return _reflection_manager; }
		// mediates everything that goes between the user and the computer.
		inline user_interface_c * get_user_interface() const { return _user_interface; }

	};

	extern engine_c engine;

}
