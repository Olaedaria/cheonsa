#include "cheonsa__ops_file_system.h"
#include "cheonsa__ops_string.h"
#include "cheonsa__ops_time.h"

#if defined( cheonsa_platform_windows )
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX // silly windows, leave us alone.
#include <Windows.h>
#include <Shlobj.h> // needed for SHGetKnownFolderPath, used to build shortcut list to common locations on windows.
#endif

namespace cheonsa
{
	namespace ops
	{

		string16_c path_get_folder_path( string16_c const & a )
		{
			string16_c result;

			// skip the trailing slash if it's present.
			sint32_c a_length = a.get_length();
			char16_c a_character = a.character_list[ a_length - 1 ];
			if ( a_character == '/' || a_character == '\\' )
			{
				a_length--;
			}

			// scan for slashes.
			sint32_c i = a_length - 1;
			while ( i >= 0 )
			{
				a_character = a.character_list[ i ];
				if ( a_character == '/' || a_character == '\\' )
				{
					i++;
					break;
				}
				i--;
			}

			if ( i > 0 )
			{
				result.character_list.construct_mode_dynamic_from_array( a.character_list.get_internal_array(), i );
				result.character_list.insert( -1, 0 );
			}

			return result;
		}

		string16_c path_get_file_or_folder_extension( string16_c const & a )
		{
			string16_c result;

			// file paths must not end with a slash.
			sint32_c a_length = a.get_length();
			assert( a_length > 0 );
			char16_c a_character = a.character_list[ a_length - 1 ];
			if ( a_character == '/' || a_character == '\\' )
			{
				a_length--;
			}

			// walk backwards until we find a slash, but also keep note of the index of the earliest occurrence of a dot in the file name.
			sint32_c i = a_length - 1;
			sint32_c dot = -1;
			while ( i >= 0 )
			{
				char16_c a_character = a.character_list[ i ];
				if ( a_character == '.' )
				{
					dot = i;
				}
				else if ( a_character == '/' || a_character == '\\' )
				{
					break;
				}
				i--;
			}

			// create the result.
			if ( dot >= 0 )
			{
				result.character_list.construct_mode_dynamic_from_array( &a.character_list.get_internal_array()[ dot ], a_length - dot );
			}

			return result;
		}

		string16_c path_get_file_or_folder_name( string16_c const & a )
		{
			string16_c result;

			// folder paths will end in a slash and we want to exclude it from the scan.
			sint32_c a_length = a.get_length();
			assert( a_length > 0 );
			char16_c a_character = a.character_list[ a_length - 1 ];
			if ( a_character == '/' || a_character == '\\' )
			{
				a_length--;
			}

			// walk backwards until we find a slash.
			sint32_c i = a_length - 1;
			while ( i >= 0 )
			{
				char16_c a_character = a.character_list[ i ];
				if ( a_character == '/' || a_character == '\\' )
				{
					break;
				}
				i--;
			}
			i++; // + 1 because we want to exclude the slash from the result, or prevent -1.

			// create the result.
			if ( a_length - i > 0 )
			{
				result.character_list.construct_mode_dynamic_from_array( &a.character_list.get_internal_array()[ i ], a_length - i );
				result.character_list.insert( -1, 0 );
			}

			return result;
		}

		string16_c path_get_file_or_folder_name_without_extension( string16_c const & a )
		{
			string16_c result;

			// folder paths will end in a slash and we want to exclude it from the scan.
			sint32_c a_length = a.get_length();
			assert( a_length > 0 );
			char16_c a_character = a.character_list[ a_length - 1 ];
			if ( a_character == '/' || a_character == '\\' )
			{
				a_length--;
			}

			// walk backwards until we find a slash, but also keep note of the index of the earliest occurrence of a dot in the file name.
			sint32_c i = a_length - 1;
			sint32_c dot = i;
			while ( i >= 0 )
			{
				char16_c a_character = a.character_list[ i ];
				if ( a_character == '.' )
				{
					dot = i;
				}
				else if ( a_character == '/' || a_character == '\\' )
				{
					break;
				}
				i--;
			}
			i++; // + 1 because we want to exclude the slash from the result, or prevent -1.

			if ( dot > i )
			{
				result.character_list.construct_mode_dynamic_from_array( &a.character_list.get_internal_array()[ i ], dot - i );
				result.character_list.insert( -1, 0 );
			}

			return result;
		}

//		boolean_c file_system_set_file_or_folder_last_write_time( string16_c const & file_path, sint64_c milliseconds_since_epoch )
//		{
//			assert( path_is_formatted_for_cheonsa( file_path, false ) );
//#if defined( cheonsa_platform_windows )
//			string16_c file_path_windows = path_format_for_windows( file_path );
//			boolean_c result = false;
//			HANDLE file_handle = CreateFileW( file_path_windows.character_list.get_internal_array(), FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
//			if ( file_handle != INVALID_HANDLE_VALUE )
//			{
//				ULARGE_INTEGER file_time_large_integer;
//				file_time_large_integer.QuadPart = time_format_for_windows( milliseconds_since_epoch );
//				FILETIME file_time;
//				file_time.dwLowDateTime = file_time_large_integer.LowPart;
//				file_time.dwHighDateTime = file_time_large_integer.HighPart;
//				SetFileTime( file_handle, NULL, NULL, &file_time );
//				CloseHandle( file_handle );
//				result = true;
//			}
//			return result;
//#else
//#error file_system_get_file_or_folder_modified_time is not implemented.
//#endif
//		}

		boolean_c file_system_is_path_formatted_for_cheonsa( string16_c const & path, file_system_path_type_e path_type )
		{
			// all cheonsa paths should be relative.
			// should not start with a drive letter, but we check this indirectly by checking that it does not contain a colon ':'.
			// check that only valid characters are in the path.
			char16_c const * c = path.character_list.get_internal_array();
			char16_c character;
			while ( *c )
			{
				character = *c++;
				if ( !char16_is_printable( character ) || ( character == '<' ) || ( character == '>' ) || ( character == '"' ) || ( character == '|' ) || ( character == '?' ) || ( character == '*' ) || ( character == '\\' ) )
				{
					return false;
				}
			}
			// check for presence or absence of trailing slash.
			if ( path_type == file_system_path_type_e_file && character == '/' )
			{
				return false;
			}
			if ( path_type == file_system_path_type_e_folder && character != '/' )
			{
				return false;
			}
			return true;
		}

		string16_c file_system_fix_path( string16_c const & path )
		{
			string16_c result;
			string16_c t;
			string16_c node;
			sint32_c i = 0;
			WCHAR t2[ MAX_PATH ] = {};
			DWORD t2r = 0;
			//if ( string16_starts_with( path.character_list.get_internal_array(), "\\\\?\\" ) )
			//{
			//	i = 4;
			//}
			// check for drive letter.
			if ( path.get_length() < i + 2 )
			{
				goto done;
			}
			// check for colon.
			if ( path.character_list[ i + 1 ] == ':' )
			{
				if ( !char16_is_latin_letter( path.character_list[ i ] ) )
				{
					goto done;
				}
				t += char16_to_upper( path.character_list[ i ] );
				t += ':';
			}
			// if path is just drive letter and colon, then add the trailing slash and return.
			if ( path.get_length() == 2 )
			{
				t += '\\';
				result = t;
				goto done;
			}
			i = 2;

		process_next_node:
			while ( i < path.get_length() )
			{
				char16_c c = path.character_list[ i ];
				if ( node.get_length() == 0 )
				{
					if ( c == '/' || c == '\\' )
					{
						node += '\\';
						i++;
					}
					else
					{
						goto done;
					}
				}
				else
				{
					if ( c == '/' || c == '\\' )
					{
						break;
					}
					else
					{
						node += c;
						i++;
					}
				}
			}

			if ( node == "\\." )
			{
				// ignore this node.
			}
			else if( node == "\\.." )
			{
				// get mother node.
				t = path_get_folder_path( t );
				if ( t.get_length() == 0 )
				{
					// if collapsed into nothing, return nothing.
					goto done;
				}
			}
			else
			{
				if ( t.character_list[ t.get_length() - 1 ] != '\\' )
				{
					t += node;
				}
			}
			if ( i < path.get_length() )
			{
				node = string16_c();
				goto process_next_node;
			}

			// fix casing.
			t2r = GetLongPathName( t.character_list.get_internal_array(), t2, MAX_PATH );
			if ( t2r == 0 )
			{
				goto done;
			}
			else if ( t2r == 3 )
			{
				// just drive letter.
				result = t2;
				goto done;
			}

			// determine if trailing slash needs to be added or removed.
			DWORD file_attributes = GetFileAttributes( t2 );
			if ( file_attributes == INVALID_FILE_ATTRIBUTES )
			{
				goto done;
			}
			result = t2;
			if ( file_attributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				if ( result.character_list[ result.get_length() - 1 ] != '\\' )
				{
					result += '\\';
				}
			}
			else
			{
				if ( result.character_list[ result.get_length() - 1 ] == '\\' )
				{
					result.character_list.remove( -1, 2 );
					result.character_list.insert( -1, 0 );
				}
			}

		done:
			return result;
		}

		boolean_c file_system_is_path_formatted_for_windows( string16_c const & path, file_system_path_type_e path_type )
		{
			// all windows paths should be absolute.
			// check for max path prefix.
			sint32_c i = 0;
			//if ( string16_starts_with( path.character_list.get_internal_array(), "\\\\?\\" ) )
			//{
			//	i = 4;
			//}
			// check for drive letter.
			if ( path.get_length() < i + 3 )
			{
				return false;
			}
			if ( char16_is_latin_letter( path.character_list[ i ] ) )
			{
				// check for colon.
				if ( path.character_list[ i + 1 ] == ':' )
				{
					// check for back slash.
					if ( path.character_list[ i + 2 ] == '\\' )
					{
						// check that only valid characters are in the path.
						char16_c const * c = &path.character_list.get_internal_array()[ i ];
						char16_c character;
						while ( *c )
						{
							character = *c++;
							if ( !char16_is_printable( character ) || ( character == 127 ) || ( character == '<' ) || ( character == '>' ) || ( character == '"' ) || ( character == '|' ) || ( character == '?' ) || ( character == '*' ) || ( character == '/' ) )
							{
								return false;
							}
						}
						// check for presence or absence of trailing slash.
						if ( path_type == file_system_path_type_e_file && character == '\\' )
						{
							return false;
						}
						if ( path_type == file_system_path_type_e_folder && character != '\\' )
						{
							return false;
						}
						return true;
					}
				}
			}
			return false;
		}

		string16_c file_system_convert_path_format_from_cheonsa_to_windows( string16_c const & path )
		{
			string16_c result( core_list_mode_e_dynamic, path.character_list.get_internal_array(), path.character_list.get_length() );
			char16_c * character_pointer = result.character_list.get_internal_array();
			while ( *character_pointer )
			{
				if ( *character_pointer == '/' )
				{
					*character_pointer = '\\';
				}
				character_pointer++;
			}
			return result;
		}

		string16_c file_system_convert_path_format_from_windows_to_cheonsa( string16_c const & path )
		{
			string16_c result;
			result = path;
			char16_c * c = result.character_list.get_internal_array();
			while ( *c )
			{
				if ( *c == '\\' )
				{
					*c = '/';
				}
				c++;
			}
			return result;
		}

		boolean_c file_system_does_file_exist( string16_c const & file_path )
		{
#if defined( cheonsa_platform_windows )
			//assert( file_system_is_path_formatted_for_windows( file_path, file_system_path_type_e_file ) );
			DWORD file_attributes = GetFileAttributes( file_path.character_list.get_internal_array() );
			return ( ( file_attributes != INVALID_FILE_ATTRIBUTES ) && ( ( file_attributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 ) );
#else
#error ops::file_system_does_file_exist() is not implemented.
#endif
		}

		boolean_c file_system_create_file( string16_c const & file_path )
		{
#if defined( cheonsa_platform_windows )
			assert( file_system_is_path_formatted_for_windows( file_path, file_system_path_type_e_file ) );
			FILE * file = nullptr;
			errno_t asdkgjfhawe = _wfopen_s( &file, file_path.character_list.get_internal_array(), L"wb" );
			if ( asdkgjfhawe != 0 )
			{
				return false;
			}
			if ( file == 0 )
			{
				return false;
			}
			fclose( file );
			return true;
#else
#error ops::file_system_create_file() is not implemented.
#endif
		}

		boolean_c file_system_does_folder_exist( string16_c const & folder_path )
		{
#if defined( cheonsa_platform_windows )
			assert( file_system_is_path_formatted_for_windows( folder_path, file_system_path_type_e_folder ) );
			DWORD file_attributes = GetFileAttributes( folder_path.character_list.get_internal_array() );
			return ( ( file_attributes != INVALID_FILE_ATTRIBUTES ) && ( ( file_attributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 ) );
#else
#error ops::file_system_does_folder_exist() is not implemented.
#endif
		}

		boolean_c file_system_create_folder( string16_c const & folder_path )
		{
#if defined( cheonsa_platform_windows )
			assert( file_system_is_path_formatted_for_windows( folder_path, file_system_path_type_e_folder ) );
			// windows doesn't let us create a set of nested folders in one call, so we have to create folders at each level one by one.
			BOOL result = FALSE;
			sint32_c final_end = folder_path.get_length();
			sint32_c end = 7; // skip the "\\?\C:\".
			while ( end < final_end )
			{
				while ( end < final_end )
				{
					if ( folder_path.character_list[ end ] == '\\' )
					{
						end++;
						break;
					}
					end++;
				}
				string16_c sub_folder_path = ops::string16_sub_string( folder_path, 0, end );
				DWORD file_attributes = GetFileAttributes( sub_folder_path.character_list.get_internal_array() );
				BOOL folder_exists = ( ( file_attributes != INVALID_FILE_ATTRIBUTES ) && ( ( file_attributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 ) );
				if ( folder_exists == false )
				{
					result = CreateDirectoryW( sub_folder_path.character_list.get_internal_array(), NULL );
				}
			}
			return result != 0;
#else
#error ops::file_system_create_folder() is not implemented.
#endif
		}

		boolean_c file_system_move_file_or_folder( string16_c const & from_file_path, string16_c const & to_file_path )
		{
			assert( false );
#if defined( cheonsa_platform_windows )
			assert( file_system_is_path_formatted_for_windows( from_file_path, file_system_path_type_e_dont_care ) );
			assert( file_system_is_path_formatted_for_windows( to_file_path, file_system_path_type_e_dont_care ) );
			BOOL result = MoveFileW( from_file_path.character_list.get_internal_array(), to_file_path.character_list.get_internal_array() );
			return result != 0;
#else
#error ops::file_system_move_file_or_folder() is not implemented.
#endif
		}

		boolean_c file_system_delete_file_or_folder( string16_c const & file_path )
		{
			assert( false );
#if defined( cheonsa_platform_windows )
			assert( file_system_is_path_formatted_for_windows( file_path, file_system_path_type_e_dont_care ) );
			BOOL result = DeleteFileW( file_path.character_list.get_internal_array() );
			return result != 0;
#else
#error ops::file_system_delete_file_or_folder() is not implemented.
#endif
		}

		boolean_c file_system_get_file_or_folder_last_write_time( string16_c const & file_path, sint64_c & last_write_time )
		{
#if defined( cheonsa_platform_windows )
			assert( file_system_is_path_formatted_for_windows( file_path, file_system_path_type_e_dont_care ) );
			boolean_c result = false;
			HANDLE file_handle = CreateFileW( file_path.character_list.get_internal_array(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
			if ( file_handle != INVALID_HANDLE_VALUE )
			{
				FILETIME file_time;
				if ( GetFileTime( file_handle, NULL, NULL, &file_time ) )
				{
					ULARGE_INTEGER file_time_large_integer;
					file_time_large_integer.LowPart = file_time.dwLowDateTime;
					file_time_large_integer.HighPart = file_time.dwHighDateTime;
					last_write_time = time_convert_time_format_from_windows_to_cheonsa( file_time_large_integer.QuadPart );
					result = true;
				}
				CloseHandle( file_handle );
			}
			return result;
#else
#error ops::file_system_get_file_or_folder_last_write_time() is not implemented.
#endif
		}

		void_c file_system_get_file_information_list( core_list_c< file_system_file_information_c > & result, string16_c const & folder_path, boolean_c get_files, boolean_c get_folders, char8_c const * file_extension_filter )
		{
			result.remove_all();
#if defined( cheonsa_platform_windows )
			assert( file_system_is_path_formatted_for_windows( folder_path, file_system_path_type_e_folder ) );

			string16_c folder_path_with_wild_card = folder_path;
			folder_path_with_wild_card += "*";

			HANDLE find_handle = INVALID_HANDLE_VALUE;
			WIN32_FIND_DATAW find_file_data;
			find_handle = FindFirstFileW( folder_path_with_wild_card.character_list.get_internal_array(), &find_file_data );
			if ( find_handle == INVALID_HANDLE_VALUE )
			{
				return;
			}

			core_list_c< string16_c > file_extension_filter_list;
			if ( file_extension_filter )
			{
				ops::string16_split_at_delimiter( string16_c( file_extension_filter ), string16_c( core_list_mode_e_static, L"|" ), file_extension_filter_list );
			}

			do
			{
				if ( ( find_file_data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) != 0 || ( find_file_data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM ) != 0 )
				{
					continue;
				}
				if ( ops::string16_compare( find_file_data.cFileName, L"." ) || ops::string16_compare( find_file_data.cFileName, L".." ) )
				{
					continue;
				}

				ULARGE_INTEGER windows_time;
				if ( get_folders && ( find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
				{
					file_system_file_information_c * result_item = result.emplace( -1, 1 );
					result_item->path = folder_path;
					result_item->path += string16_c( core_list_mode_e_static, find_file_data.cFileName );
					result_item->path += L'\\';
					windows_time.LowPart = find_file_data.ftCreationTime.dwLowDateTime;
					windows_time.HighPart = find_file_data.ftCreationTime.dwHighDateTime;
					result_item->creation_time = time_convert_time_format_from_windows_to_cheonsa( windows_time.QuadPart );
					windows_time.LowPart = find_file_data.ftLastWriteTime.dwLowDateTime;
					windows_time.HighPart = find_file_data.ftLastWriteTime.dwHighDateTime;
					result_item->last_write_time = time_convert_time_format_from_windows_to_cheonsa( windows_time.QuadPart );
					result_item->is_folder = true;
				}
				else if ( get_files && ( find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
				{
					boolean_c add = true;
					if ( file_extension_filter_list.get_length() > 0 )
					{
						add = false;
						for ( sint32_c i = 0; i < file_extension_filter_list.get_length(); i++ )
						{
							if ( string16_ends_with( find_file_data.cFileName, file_extension_filter_list[ i ].character_list.get_internal_array() ) )
							{
								add = true;
								break;
							}
						}
					}
					if ( add )
					{
						file_system_file_information_c * result_item = result.emplace( -1, 1 );
						result_item->path = folder_path;
						result_item->path += string16_c( core_list_mode_e_static, find_file_data.cFileName );
						windows_time.LowPart = find_file_data.ftCreationTime.dwLowDateTime;
						windows_time.HighPart = find_file_data.ftCreationTime.dwHighDateTime;
						result_item->creation_time = time_convert_time_format_from_windows_to_cheonsa( windows_time.QuadPart );
						windows_time.LowPart = find_file_data.ftLastWriteTime.dwLowDateTime;
						windows_time.HighPart = find_file_data.ftLastWriteTime.dwHighDateTime;
						result_item->last_write_time = time_convert_time_format_from_windows_to_cheonsa( windows_time.QuadPart );
						result_item->is_folder = false;
					}
				}
			} while ( FindNextFileW( find_handle, &find_file_data ) != 0 );
			FindClose( find_handle );
#else
#error ops::file_system_get_file_information_list() is not implemented.
#endif
		}

		void_c _file_system_get_files_recursive( core_list_c< string16_c >& result, string16_c const & folder_path, boolean_c const search_sub_folders, char8_c const * extension_filter )
		{
#if defined( cheonsa_platform_windows )
			assert( file_system_is_path_formatted_for_windows( folder_path, file_system_path_type_e_folder ) );

			string16_c folder_path_with_wild_card = folder_path;
			folder_path_with_wild_card += "*";

			HANDLE find_handle = INVALID_HANDLE_VALUE;
			WIN32_FIND_DATAW find_file_data;
			find_handle = FindFirstFileW( folder_path_with_wild_card.character_list.get_internal_array(), &find_file_data );
			if ( find_handle == INVALID_HANDLE_VALUE )
			{
				return;
			}

			do
			{
				if ( ( find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
				{
					string16_c sub_file_name = string16_c( core_list_mode_e_static, find_file_data.cFileName );
					if ( ( extension_filter == nullptr ) || ( string16_ends_with( sub_file_name.character_list.get_internal_array(), extension_filter ) ) )
					{
						string16_c sub_file_path;
						sub_file_path += folder_path;
						sub_file_path += sub_file_name;
						result.insert( -1, sub_file_path );
					}
				}
				else if ( search_sub_folders )
				{
					string16_c sub_folder_name = string16_c( core_list_mode_e_static, find_file_data.cFileName );
					string16_c sub_folder_path;
					sub_folder_path += folder_path;
					sub_folder_path += sub_folder_name;
					sub_folder_path += "\\";
					_file_system_get_files_recursive( result, sub_folder_path, search_sub_folders, extension_filter );
				}
			} while ( FindNextFileW( find_handle, &find_file_data ) != 0 );
			FindClose( find_handle );
#else
#error ops::_file_system_get_files_recursive() is not implemented.
#endif
		}

		void_c file_system_get_file_path_list( core_list_c< string16_c > & result, string16_c const & folder_path, boolean_c const search_sub_folders, char8_c const * file_extension_filter )
		{
			_file_system_get_files_recursive( result, folder_path, search_sub_folders, file_extension_filter );
		}

		void_c _file_system_get_folders_recursive( core_list_c< string16_c > & result, string16_c const & folder_path, boolean_c const search_sub_folders )
		{
#if defined( cheonsa_platform_windows )
			assert( file_system_is_path_formatted_for_windows( folder_path, file_system_path_type_e_folder ) );

			string16_c folder_path_with_wild_card = folder_path;
			folder_path_with_wild_card += "*";

			HANDLE find_handle = INVALID_HANDLE_VALUE;
			WIN32_FIND_DATAW find_file_data;
			find_handle = FindFirstFileW( folder_path_with_wild_card.character_list.get_internal_array(), &find_file_data );
			if ( find_handle == INVALID_HANDLE_VALUE )
			{
				return;
			}

			do
			{
				if ( ( find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
				{
					string16_c sub_folder_name = string16_c( core_list_mode_e_static, find_file_data.cFileName );
					if ( sub_folder_name != "." && sub_folder_name != ".." )
					{
						string16_c sub_folder_path;
						sub_folder_path += folder_path;
						sub_folder_path += sub_folder_name;
						sub_folder_path += '\\';
						result.insert( -1, sub_folder_path );
					}
					if ( search_sub_folders )
					{
						string16_c sub_folder_path;
						sub_folder_path += folder_path;
						sub_folder_path += sub_folder_name;
						sub_folder_path += "\\";
						_file_system_get_folders_recursive( result, sub_folder_path, search_sub_folders );
					}
				}
			} while ( FindNextFileW( find_handle, &find_file_data ) != 0 );
			FindClose( find_handle );
#else
#error ops::_file_system_get_folders_recursive() is not implemented.
#endif
		}

		void_c file_system_get_folder_path_list( core_list_c< string16_c > & result, string16_c const & folder_path, boolean_c const search_sub_folders )
		{
			_file_system_get_folders_recursive( result, folder_path, search_sub_folders );
		}

		boolean_c file_system_get_quick_access_folder_path( file_system_quick_access_folder_e folder, string16_c & result_folder_path )
		{
#if defined( cheonsa_platform_windows )
			PWSTR ppszPath = NULL;
			if ( folder == file_system_quick_access_folder_e_desktop )
			{
				if ( S_OK == SHGetKnownFolderPath( FOLDERID_Desktop, 0, NULL, &ppszPath ) )
				{
					result_folder_path = ppszPath;
					CoTaskMemFree( ppszPath );
					return true;
				}
			}
			else if ( folder == file_system_quick_access_folder_e_documents )
			{
				if ( S_OK == SHGetKnownFolderPath( FOLDERID_Documents, 0, NULL, &ppszPath ) )
				{
					result_folder_path  = ppszPath;
					CoTaskMemFree( ppszPath );
					return true;
				}
			}
#else
#error ops::file_system_get_quick_access_folder_path() is not implemented.
#endif
			return false;
		}

		void_c file_system_get_drive_path_list( core_list_c< file_system_file_information_c > & result )
		{
			result.remove_all();
#if defined( cheonsa_platform_windows )
			// scan all 26 letters.
			// what a dumb design in my opinion but it's set in stone i guess.
			DWORD logical_drives = GetLogicalDrives();
			for ( sint32_c i = 0; i < 26; i++ )
			{
				uint32_c bit = static_cast< uint32_c >( 0x0001 ) << i;
				if ( logical_drives & bit )
				{
					string16_c drive_path;
					drive_path += static_cast< char16_c >( 'A' + i );
					drive_path += ":\\";
					UINT drive_type = GetDriveTypeW( drive_path.character_list.get_internal_array() );
					if ( drive_type >= 2 )
					{
						file_system_file_information_c * information = result.emplace( -1, 1 );
						information->label = string16_c();
						information->path = drive_path;
						WCHAR drive_label[ MAX_PATH + 1 ];
						if ( GetVolumeInformationW( drive_path.character_list.get_internal_array(), drive_label, MAX_PATH + 1, NULL, 0, 0, NULL, 0 ) )
						{
							information->label = drive_label;
						}
					}
				}
			}
#else
#error ops::file_system_get_drive_path_list() is not implemented.
#endif
		}

	}
}
