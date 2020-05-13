#pragma once

#include "cheonsa_localized_string_file.h"
#include "cheonsa_string16.h"
#include "cheonsa_data_scribe_ini.h"
#include "cheonsa_data_stream_file.h"

namespace cheonsa
{

	// there is one content manager instance per engine instance.
	// tells other systems what folders to load content from and what locale setting to use.
	// resolves relative file paths (of game content) to absolute file paths (which can be used to open actual files).
	// it's done this way in part to support file modding as a feature.
	// folders at the end of the list have higher priority than those at the start.
	// the resource manager reinterprets paths that start with "engine/" to mean [engine_data_folder_path], this kind of compartmentalizes internal engine data from game data.
	// all built-in internal use data files will have paths that start with "engine/".
	// all game data will start with anything else.
	// the engine and game needs to support loading game content after start up, so that the game editor tool can open, modify, save, and close game projects at will.
	// so the game is responsible for working with the engine to load and unload the game data folder appropriately.
	class content_manager_c
	{
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
		friend class engine_c;

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

		// the actual locale that is set to be in use, which may not match the preferred locale.
		locale_c * _actual_locale;

		// saves locale settings to a "locale" section in the ini file.
		void_c _save_settings( data_scribe_ini_c & scribe );

		// loads locale settings from a "locale" section in the ini file.
		void_c _load_settings( data_scribe_ini_c & scribe );

		// one string file is loaded for each data folder that contains a "strings.xml" file.
		core_list_c< localized_string_file_c * > _string_file_list;

		data_scribe_ini_c _settings_file;

	public:
		// base_data_folder_path is by default the folder that the exe is running out of.
		// but it can be overridden by -data="[base_data_folder_path]" command line argument.
		content_manager_c();
		~content_manager_c();

		boolean_c start( string16_c const & engine_data_folder_path, string16_c const & executable_folder_path, string16_c const & executable_file_name );

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

		// takes a relative file path and scans data folders for the first file that matches.
		// relative_file_path is the file to search the data folders for.
		// absolute_file_path will hold the result if the function returns true.
		// search_game_data if true means that the game data folders will be searched. game data folders have higher priority and so are searched before engine data folders.
		// search_engine_data if true means that the engine data folders will be searched.
		boolean_c resolve_absolute_file_path( string16_c const & relative_file_path, string16_c & absolute_file_path, boolean_c search_game_data, boolean_c search_engine_data );

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

		// apply any data or locale changes made.
		// detects supported by scanning each engine data folder and each game data folder for sub folders, and the name of each sub folder defines a supported locale code (with the exception of the folder named "_common" which contains files common to all locales).
		// this incurs a reload of resources and strings.
		void_c apply_changes();

		// looks up a string for the current locale.
		localized_string_c const * find_string( string8_c const & key ) const; // looks up a string in this string file.

		// loads the settings file state from the settings file on disk.
		boolean_c load_settings_file();
		// saves the settings file state to the settings file on disk.
		boolean_c save_settings_file();
		// other systems use this system file instance to read or write their settings.
		data_scribe_ini_c & get_settings_file();

	};

}
