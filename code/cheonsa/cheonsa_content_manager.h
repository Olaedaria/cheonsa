#pragma once

#include "cheonsa_resource_file_sprites.h"
#include "cheonsa_resource_file_strings.h"
#include "cheonsa_sprite.h"
#include "cheonsa_string16.h"
#include "cheonsa_data_scribe_ini.h"
#include "cheonsa_data_stream_file.h"

namespace cheonsa
{

	// there is one content manager instance per engine instance.
	// manages data folder paths:
	//     stores information about the engine/game executable, like what folder it's in and its file name.
	//     stores the engine data folder path.
	//     stores the game data folder path(s). the game will want to add at least one of these. if the game wants to support user made file sawp mods then it can add more of these.
	//     folders at the end of the list have higher priority than those at the start.
	//     supports modifying this list of folders at run time, which is mainly needed for the game editor, but it can also enable the game to hot-swap mods during run time rather than have a separate mod manager program.
	//     the data folder paths are used by other systems to find engine and game content files.
	// manages detecting (by scanning folder names) what locales are supported at start up, and manages changing the locale setting at run time.
	// manages resolving relative file paths to absolute file paths with resolve_absolute_file_path():
	//     it searches the locale folder first before falling back to the "_common/" folder.
	//     the resource manager uses this to determine which files to load.
	// manages loading and look up of localized text strings from "text.strings.xml" files.
	//     each engine locale folder contains a "text.strings.xml" file which holds localized engine strings.
	//     each game locale folder contains a "text.strings.xml" file which holds localized game strings.
	//     the game may also programatically define its own strings via a call back, which can be used (for example) to put names of items and characters in character dialog.
	//     text elements can insert these string values via ":$key:".
	// manages loading and look up of localized text sprites from "text.sprites.xml" files.
	//     this file is formatted the same as resource_file_sprites_c.
	//     each game locale folder contains a "text.sprites.xml" file which holds localized text-in-line-able sprites.
	//     
	//         text elements can insert these sprites via ":key:".
	// manages the engine's settings file.
	//     other engine systems save to and load from this file.
	class content_manager_c
	{
		friend class engine_c;

	public:
		// represents one of the locales that is detected as being supported by the engine and/or game.
		class locale_c
		{
		private:
			friend class content_manager_c;

			string16_c _code; // the locale code.
			string16_c _name; // the localized name of the language and locale. for example, if the code is "ko_kr" (korean, south korea), then the name would be "한국어". or if the code is "en_us" (english, united states), then the name would be "English (United States)".

		public:
			string16_c const & get_code() const;
			boolean_c get_code_language( string16_c & result ) const; // parses _code for iso language code. just returns everything before the underscore.
			boolean_c get_code_country( string16_c & result ) const; // parses _code for iso country code. just returns everything after the underscore.

			string16_c const & get_name() const;

		};

	private:
		// absolute file path where engine exe is running out of.
		string16_c _executable_folder_path;

		// file name with file extension.
		string16_c _executable_file_name;

		// absolute folder path of engine data folder on the local file system.
		string16_c _engine_data_folder_path;

		// absolute folder path of game data folder on the local file system.
		// this defaults to "[exe_folder]/data/game/".
		// except if there is a file "[exe_folder]data.txt", then the folder path contained in that file plus "game/" is used.
		core_list_c< string16_c > _game_data_folder_path_list;

		// list of locales that were detected as being supported when the data folders were scanned.
		core_list_c< locale_c * > _supported_locales;

		// the locale that is set as preferred, which will be used when it is available (detected).
		string16_c _preferred_locale_code;

		// the actual locale that is set to be in use, which may not match the preferred locale (in the case that the preferred locale is some how not available).
		locale_c * _actual_locale;

		// saves locale settings to a "locale" section in the ini file.
		void_c _save_settings( data_scribe_ini_c & scribe );

		// loads locale settings from a "locale" section in the ini file.
		void_c _load_settings( data_scribe_ini_c & scribe );

		// 0 is common engine.
		// 1 is localized engine.
		// 2 is common game.
		// 3 is localized game.
		resource_file_strings_c::reference_c _text_strings_resources[ 4 ];
		boolean_c _text_strings_users_are_dirty; // when set to true, the next update will refresh all text element instances (force re-evaluation of string entities).
		void_c _handle_text_strings_on_loaded( resource_file_c * resource_file ); // subscribed to both the on_loaded and on_unloaded events on the text strings resources. this just sets _text_strings_users_are_dirty to true.

		// 0 is common engine.
		// 1 is localized engine.
		// 2 is common game.
		// 3 is localized game.
		resource_file_sprites_c::reference_c _text_sprites_resources[ 4 ];
		boolean_c _text_sprites_users_are_dirty; // when set to true, the next update will refresh all text element instances (force re-evaluation of sprite entities).
		void_c _handle_text_sprites_on_loaded( resource_file_c * resource_file ); // subscribed to both the on_loaded an on_unloaded events on the text sprites resources. this just sets _text_sprites_users_are_dirty to true.

		data_scribe_ini_c _settings_file;

		boolean_c _wants_to_refresh;

		content_manager_c();
		~content_manager_c();

		boolean_c start( string16_c const & engine_data_folder_path, string16_c const & executable_folder_path, string16_c const & executable_file_name );

		void_c update(); // called by engine after it updates resource manager.

	public:
		// gets platform specific absolute folder path of where to save user data, which can be used to save the user's settings, saved games, screen shots, etc.
		// on windows this is "%LocalAppData%\[game_name]\".
		string16_c get_user_data_folder_path() const;

		// gets the folder that the engine executable is running out of.
		string16_c const & get_executable_folder_path() const;

		// gets the file name with file extension of the executable.
		string16_c const & get_executable_file_name() const;

		// gets the engine data folder path.
		// this is the folder path that contains files that are vital for the basic operation of the engine.
		string16_c const & get_engine_data_folder_path() const;

		// gets all game data folder paths. usually the first is the original|master, and additional ones are mods|addons.
		// these are folder paths that contain files that are vital for the operation of the game or application.
		core_list_c< string16_c > const & get_game_data_folder_path_list() const;

		// removes all game data folder paths.
		// you will need to call apply_changes() when you are done making all your updates.
		void_c remove_all_game_data_folder_paths();
		// adds a game data folder path to the end of the game data folder path list.
		// you will need to call apply_changes() when you are done making all your updates.
		void_c add_game_data_folder_path( string16_c const & game_data_folder_path );

		// gets the list of detected supported locales.
		// this can be used to show a locale selection list to the user.
		core_list_c< locale_c * > const & get_supported_locales() const;
		// sets the preferred locale code.
		// you will have to call apply_changes() when you are ready to try to switch to it.
		void_c set_preferred_locale_code( string16_c const & locale_code );
		// gets the preferred locale code.
		string16_c const & get_preferred_locale_code();
		// gets the currently applied locale, which might be different from the preferred locale.
		locale_c const * get_actual_locale() const;

		// apply any data folder and/or locale changes made.
		// detects supported by scanning each engine data folder and each game data folder for sub folders, and the name of each sub folder defines a supported locale code (with the exception of the folder named "_common" which contains files common to all locales).
		// this incurs a reload of resources and strings.
		void_c apply_changes();

		// takes a relative file path and scans data folders for the first file that matches.
		// relative_file_path is the file to search the data folders for.
		// absolute_file_path will hold the result if the function returns true.
		// search_game_data if true means that the game data folders will be searched. game data folders have higher priority and so are searched before engine data folders.
		// search_engine_data if true means that the engine data folders will be searched.
		boolean_c resolve_absolute_file_path( string16_c const & relative_file_path, string16_c & absolute_file_path, boolean_c search_game_data, boolean_c search_engine_data );

		// looks up a text string.
		// key may start with '$', if it does then it will be skipped.
		// returns true if match was found, false if not.
		boolean_c find_text_string( string8_c const & key, string16_c & result ) const;

		// looks up a text sprite.
		// returns true if match was found, false if not.
		boolean_c find_text_sprite( string8_c const & key, sprite_c & result ) const;

		// queues a full refresh at next update, before the next game logic update.
		// this will scan resource source files for changes and reload as needed.
		// this is done outside of the game logic update to avoid complications with dependencies being deleted while in use.
		void_c queue_refresh();


		// loads the settings file state from the settings file on disk.
		boolean_c load_settings_file();
		// saves the settings file state to the settings file on disk.
		boolean_c save_settings_file();
		// other systems use this system file instance to read or write their settings.
		data_scribe_ini_c & get_settings_file();

	};

}
