#pragma once

#include "cheonsa__types.h"
#include "cheonsa_string16.h"

namespace cheonsa
{
	namespace ops
	{

		string16_c path_get_folder_path( string16_c const & path ); // removes the last node from the given path and returns it. works with forward slashes and back slashes. works with and without trailing slash.
		string16_c path_get_file_or_folder_extension( string16_c const & path ); // returns the file extension (including the dot) of the last node in the path, or an empty string if there is no file extension. works with forward slashes and back slashes. works with and without trailing slash. works with multi-dot file extensions.
		string16_c path_get_file_or_folder_name( string16_c const & path ); // returns the name of the last node in the path (including the file extension). works with forward slashes and back slashes. works with and without trailing slash.
		string16_c path_get_file_or_folder_name_without_extension( string16_c const & path ); // returns the name of the last node in the path (excluding the file extension). works with forward slashes and back slashes. works with and without trailing slash. works with multi-dot file extensions.

		/*
		these file system functions interface directly with the operating system's file system.
		all paths must be in the operating system's native path format.
		all paths given as inputs must be absolute.
		all paths given as results are absolute.
		for paths that you provide as inputs, cheonsa will be doing strict checks to make sure that they are formatted a certain way.
		for windows:
		    cheonsa expects all paths to start with "\\?\" (to disable the MAX_PATH limit) followed by a drive letter "C:\".
		    cheonsa does not care about case sensitivity.
		    cheonsa expects all folder paths to end with a trailing back slash "\".
		    cheonsa expects all file paths to not end with a trailing back slash "\".
		*/

		// records information about folders and files.
		class file_system_file_information_c
		{
		public:
			string16_c path; // absolute path to file system object in native operating system format.
			string16_c label; // drives in windows may be given a user friendly label. we may want to use this to display to the user.
			sint64_c creation_time; // milliseconds since epoch that tells us when the file was first created.
			sint64_c last_write_time; // milliseconds since epoch that tells us when the file was last modified.
			boolean_c is_folder; // if true, then the thing is a folder, otherwise it is a file.

			file_system_file_information_c()
				: path()
				, creation_time( 0 )
				, last_write_time( 0 )
				, is_folder( false )
			{
			}

			file_system_file_information_c & operator = ( file_system_file_information_c & other )
			{
				other.path.character_list.transfer_to( path.character_list );
				creation_time = other.creation_time;
				last_write_time = other.last_write_time;
				is_folder = other.is_folder;
				return *this;
			}

		};

		// for shortcut lists.
		enum file_system_quick_access_folder_e
		{
			file_system_quick_access_folder_e_desktop, // desktop folder of current user.
			file_system_quick_access_folder_e_documents, // documents folder of current user.
			file_system_quick_access_folder_e_count_,
		};

		enum file_system_path_type_e
		{
			file_system_path_type_e_dont_care, // path may or may not end with a trailing slash.
			file_system_path_type_e_file, // path must not end with a trailing slash.
			file_system_path_type_e_folder, // path must end with a trailing slash.
		};

		boolean_c file_system_is_path_formatted_for_cheonsa( string16_c const & path, file_system_path_type_e path_type );

#if defined( cheonsa_platform_windows )
		// to "proof read" and "fix" "broken" paths (correct style errors) that the user provides via manual text entry.
		// for windows:
		//     converts forward slashes to back slashes.
		//     resolves instances of "/.." by going up one node.
		//     removes instances of "/.".
		//     makes drive letters upper case.
		//     fixes casing of node names so that they match the actual casing of node names that exist in the file system.
		//     appends a trailing back slash if the path is referencing a folder.
		//     returns fixed path if successful, returns an empty string if not successful.
		string16_c file_system_fix_path( string16_c const & path );

		// used to check if path is formatted for windows.
		boolean_c file_system_is_path_formatted_for_windows( string16_c const & path, file_system_path_type_e path_type );

		// just replaces forward slashes with back slashes.
		// does not validate input for correctness.
		// should only be used on relative paths.
		string16_c file_system_convert_path_format_from_cheonsa_to_windows( string16_c const & path );

		// just replaces back slashes with forward slashes.
		// does not validate input for correctness.
		// should only be used on relative paths.
		string16_c file_system_convert_path_format_from_windows_to_cheonsa( string16_c const & path );
#endif

		// returns true if a file exists at file_path, false if not.
		boolean_c file_system_does_file_exist( string16_c const & file_path );
		// creates a new and empty file at file_path, returns true if successful, false if not.
		// if the file already exists, then it overwrites it.
		// the folder must already exist in order for this to work.
		boolean_c file_system_create_file( string16_c const & file_path );

		// returns true if a folder exists at folder_path, false if not.
		boolean_c file_system_does_folder_exist( string16_c const & folder_path );
		// creates a new and empty folder at folder_path.
		// if the given folder_path references a folder that lies within a range of non-existent sub-folders, then each non-existent sub-folder will also be created.
		boolean_c file_system_create_folder( string16_c const & folder_path );

		// moves or renames an existing file or folder.
		// please be super careful with this.
		// when trying to rename files, if only the case changes, maybe nothing will happen, i might take a look at it again later.
		boolean_c file_system_move_file_or_folder( string16_c const & from_file_path, string16_c const & to_file_path );

		// deletes an existing file or folder.
		// please be super careful with this.
		boolean_c file_system_delete_file_or_folder( string16_c const & file_path );

		// gets the last time that the file was written to, in milliseconds since epoch.
		boolean_c file_system_get_file_or_folder_last_write_time( string16_c const & file_path, sint64_c & last_write_time );

		// gets file information of all files and folders in the given folder path. it excludes system and hidden files and folders from the result.
		// result will hold the result after the function returns.
		// folder_path must be an absolute folder path (in the windows path format).
		// file_extension_filter is optional, but if set then it will be used to filter the results to only include files that have the given file extension(s). it should be formatted as a vertical slash separated list of file extensions, like: ".jpg|.png".
		void_c file_system_get_file_information_list( core_list_c< file_system_file_information_c > & result, string16_c const & folder_path, boolean_c get_files, boolean_c get_folders, char8_c const * file_extension_filter = nullptr );

		// gets a list of absolute file paths (in the windows path format) in the given folder path.
		// result will hold absolute file paths (in the windows path format) after the function returns.
		// folder_path must be an absolute folder path (in the windows path format).
		// search_sub_folders if true searches all sub folders recursively.
		// file_extension_filter is optional, but if set then it will be used to filter the results to only include files that have the given file extension(s). it should be formatted as a vertical slash separated list of file extensions, like: ".jpg|.png".
		void_c file_system_get_file_path_list( core_list_c< string16_c > & result, string16_c const & folder_path, boolean_c const search_sub_folders, char8_c const * file_extension_filter );

		// gets a list of absolute folder paths (in the windows path format) in the given folder path.
		// result will hold absolute folder paths (in the windows path format) after the function returns.
		// folder_path must be an absolute folder path (in the windows path format).
		// seach_sub_folders if true searches all sub folders recursively.
		void_c file_system_get_folder_path_list( core_list_c< string16_c > & result, string16_c const & folder_path, boolean_c const search_sub_folders );

		// sets the last write time of a file or folder.
		//boolean_c file_system_set_file_or_folder_last_write_time( string16_c const & file_path, sint64_c milliseconds_since_epoch );

		// returns true if the current operating system and user supports the given type of quick access folder.	
		boolean_c file_system_get_quick_access_folder_path( file_system_quick_access_folder_e folder, string16_c & result_folder_path );

		// gets a list of folder paths, one for each drives that is mounted in the operating system.
		// only sets the path and label members of each result.
		void_c file_system_get_drive_path_list( core_list_c< file_system_file_information_c > & result );

	}
}
