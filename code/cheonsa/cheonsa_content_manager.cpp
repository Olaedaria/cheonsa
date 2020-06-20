#include "cheonsa_content_manager.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"
#include "cheonsa_menu_element_text.h"

#define defined_user_data_file_name "user_data_folder_path_list.txt"

#if defined( cheonsa_platform_windows )
#include <shlobj.h> // for SHGetKnownFolderPath and CoTaskMemFree.
#endif

namespace cheonsa
{

	string16_c const & content_manager_c::locale_c::get_code() const
	{
		return _code;
	}

	boolean_c content_manager_c::locale_c::get_code_language( string16_c & result ) const
	{
		sint32_c index = 0;
		if ( ops::string16_find_index_of( _code, string16_c( L"_", core_list_mode_e_static ), index ) )
		{
			if ( index > 0 )
			{
				result = ops::string16_sub_string( _code, 0, index );
				return true;
			}
		}
		return false;
	}

	boolean_c content_manager_c::locale_c::get_code_country( string16_c & result ) const
	{
		sint32_c index = 0;
		if ( ops::string16_find_index_of( _code, string16_c( L"_", core_list_mode_e_static ), index ) )
		{
			index++;
			if ( index < _code.get_length() )
			{
				result = ops::string16_sub_string( _code, index, _code.get_length() - index );
				return true;
			}
		}
		return false;
	}

	string16_c const & content_manager_c::locale_c::get_name() const
	{
		return _name;
	}

	void_c content_manager_c::_save_settings( data_scribe_ini_c & scribe )
	{
		scribe.add_section( string8_c( "locale", core_list_mode_e_static ) );
		scribe.add_property( string8_c( "code", core_list_mode_e_static ), string8_c( _preferred_locale_code ) );
	}

	void_c content_manager_c::_load_settings( data_scribe_ini_c & scribe )
	{
		string8_c value;
		if ( scribe.find_property_value( string8_c( "locale", core_list_mode_e_static ), string8_c( "code", core_list_mode_e_static ), value ) )
		{
			set_preferred_locale_code( string16_c( value ) );
		}
		else
		{
			set_preferred_locale_code( string16_c( L"en", core_list_mode_e_static ) );
		}
	}

	void_c content_manager_c::_handle_text_strings_on_loaded( resource_file_c * resource_file )
	{
		_text_strings_users_are_dirty = true;
	}

	void_c content_manager_c::_handle_text_sprites_on_loaded( resource_file_c * resource_file )
	{
		_text_sprites_users_are_dirty = true;
	}

	content_manager_c::content_manager_c()
		: _executable_folder_path()
		, _executable_file_name()
		, _engine_data_folder_path()
		, _game_data_folder_path_list()
		, _supported_locales()
		, _preferred_locale_code()
		, _actual_locale( nullptr )
		, _text_strings_resources()
		, _text_strings_users_are_dirty( false )
		, _text_sprites_resources()
		, _text_sprites_users_are_dirty( false )
		, _settings_file()
	{
	}

	content_manager_c::~content_manager_c()
	{
	}

	boolean_c content_manager_c::start( string16_c const & engine_data_folder_path, string16_c const & executable_folder_path, string16_c const & executable_file_name )
	{
		_executable_folder_path = executable_folder_path;
		_executable_file_name = executable_file_name;
		_engine_data_folder_path = engine_data_folder_path;
		return ops::file_system_does_folder_exist( _engine_data_folder_path );
		load_settings_file();
		_load_settings( _settings_file );
	}

	void_c content_manager_c::update()
	{
		if ( _wants_to_refresh )
		{
			engine.get_video_renderer_shader_manager()->refresh();
			engine.get_resource_manager()->refresh();
			engine.get_menu_style_manager()->refresh();
			engine.get_game()->refresh();
			_wants_to_refresh = false;
		}
		if ( _text_strings_users_are_dirty || _text_sprites_users_are_dirty )
		{
			menu_element_text_c::invalidate_glyph_layout_of_all_instances();
			_text_strings_users_are_dirty = false;
			_text_sprites_users_are_dirty = false;
		}
	}

	string16_c content_manager_c::get_user_data_folder_path() const
	{
		string16_c result;
#if defined( cheonsa_platform_windows )
		PWSTR system_folder_path = nullptr;
		SHGetKnownFolderPath( FOLDERID_LocalAppData, 0, NULL, &system_folder_path );
		result += system_folder_path;
		CoTaskMemFree( system_folder_path );
		system_folder_path = nullptr;
		for ( sint32_c i = 0; i < result.character_list.get_length(); i++ )
		{
			if ( result.character_list[ i ] == '\\' )
			{
				result.character_list[ i ] = '/';
			}
		}
		result += '/';
		result += engine.get_game()->get_user_data_folder_name();
		result += '/';
#else
	#error get_user_data_folder_path() is not implemented.
#endif
		return result;
	}

	string16_c const & content_manager_c::get_executable_folder_path() const
	{
		return _executable_folder_path;
	}

	string16_c const & content_manager_c::get_executable_file_name() const
	{
		return _executable_file_name;
	}

	string16_c const & content_manager_c::get_engine_data_folder_path() const
	{
		return _engine_data_folder_path;
	}

	core_list_c< string16_c > const & content_manager_c::get_game_data_folder_path_list() const
	{
		return _game_data_folder_path_list;
	}

	void_c content_manager_c::remove_all_game_data_folder_paths()
	{
		_game_data_folder_path_list.remove_all();
	}

	void_c content_manager_c::add_game_data_folder_path( string16_c const & game_data_folder_path )
	{
		_game_data_folder_path_list.insert( -1, game_data_folder_path );
	}

	core_list_c< content_manager_c::locale_c * > const & content_manager_c::get_supported_locales() const
	{
		return _supported_locales;
	}

	void_c content_manager_c::set_preferred_locale_code( string16_c const & locale_code )
	{
		_preferred_locale_code = locale_code;
	}

	string16_c const & content_manager_c::get_preferred_locale_code()
	{
		return _preferred_locale_code;
	}

	content_manager_c::locale_c const * content_manager_c::get_actual_locale() const
	{
		return _actual_locale;
	}

	void_c content_manager_c::apply_changes()
	{
		assert( engine.get_resource_manager() );

		// detect supported locales.
		// scan sub folders, each sub folder should be another supported locale.
		_supported_locales.remove_and_delete_all();
		core_list_c< string16_c > sub_folder_path_list;
		if ( _engine_data_folder_path.get_length() > 0 )
		{
			ops::file_system_get_folder_path_list( sub_folder_path_list, _engine_data_folder_path, false );
		}
		for ( sint32_c i = 0; i < _game_data_folder_path_list.get_length(); i++ )
		{
			ops::file_system_get_folder_path_list( sub_folder_path_list, _game_data_folder_path_list[ i ], false );
		}
		for ( sint32_c j = 0; j < sub_folder_path_list.get_length(); j++ )
		{
			string16_c locale_code = ops::path_get_file_or_folder_name( sub_folder_path_list[ j ] ); // the folder name is the locale code.

			// first two characters are for language code.
			// third character should be an underscore.
			// next two characters are for country code but are optional.
			if ( locale_code != "_common" )
			{
				// look for a file called "strings.xml" which contains information about the locale.
				string16_c language_file_path = sub_folder_path_list[ j ];
				language_file_path += "strings.xml";
				data_stream_file_c language_file;
				if ( !language_file.open( language_file_path, data_stream_mode_e_read ) )
				{
					continue;
				}

				// parse the language meta data file.
				data_scribe_markup_c markup( '<' );
				if ( !markup.parse_first_tag( &language_file ) )
				{
					continue;
				}

				// create the locale entry if needed.
				locale_c * locale = nullptr;
				for ( sint32_c i = 0; i < _supported_locales.get_length(); i++ )
				{
					if ( _supported_locales[ i ]->_code == locale_code )
					{
						locale = _supported_locales[ i ];
						break;
					}
				}
				if ( locale == nullptr )
				{
					locale = new locale_c();
					locale->_code = locale_code;
					locale->_name = locale_code;
					_supported_locales.insert( -1, locale );
				}
				
				// find the name from the markup.
				data_scribe_markup_c::attribute_c const * attribute = markup.get_node( 0 )->find_attribute( "language" );
				if ( attribute )
				{
					locale->_name = attribute->get_value();
				}
			}
		}

		// try to find the preferred locale in the supported locale list.
		_actual_locale = nullptr;
		for ( sint32_c i = 0; i < _supported_locales.get_length(); i++ )
		{
			if ( _supported_locales[ i ]->_code == _preferred_locale_code )
			{
				_actual_locale = _supported_locales[ i ];
				break;
			}
		}

		// propagate changes.
		engine.get_resource_manager()->refresh();
		engine.get_menu_style_manager()->refresh();
	}

	boolean_c content_manager_c::resolve_absolute_file_path( string16_c const & relative_file_path, string16_c & absolute_file_path, boolean_c search_engine_data, boolean_c search_game_data )
	{
		absolute_file_path = string16_c();

		if ( relative_file_path.get_length() == 0 )
		{
			return false;
		}

		assert( ops::file_system_is_path_formatted_for_cheonsa( relative_file_path, ops::file_system_path_type_e_file ) );

		locale_c const * locale = engine.get_content_manager()->get_actual_locale();

		// search game data folders.
		if ( search_game_data )
		{
			core_list_c< string16_c > const & game_data_folder_path_list = engine.get_content_manager()->get_game_data_folder_path_list();
			for ( sint32_c i = game_data_folder_path_list.get_length() - 1; i >= 0; i-- )
			{
				for ( sint32_c j = locale ? 1 : 0; j >= 0; j-- )
				{
					string16_c scan_path = game_data_folder_path_list[ i ];
					if ( j != 0 )
					{
						assert( locale );
						scan_path += locale->get_code();
#if defined( cheonsa_platform_windows )
						scan_path += "\\";
#else
#error
#endif
					}
					else
					{
#if defined( cheonsa_platform_windows )
						scan_path += "_common\\";
#else
#error
#endif				
					}
#if defined( cheonsa_platform_windows )
					scan_path += ops::file_system_convert_path_format_from_cheonsa_to_windows( relative_file_path );
#else
#error
#endif
					if ( ops::file_system_does_file_exist( scan_path ) )
					{
						absolute_file_path = scan_path;
						return true;
					}
				}
			}
		}

		// search engine data folders.
		if ( search_engine_data )
		{
			for ( sint32_c j = locale ? 1 : 0; j >= 0; j-- )
			{
				string16_c scan_path = engine.get_content_manager()->get_engine_data_folder_path();
				if ( j != 0 )
				{
					assert( locale );
					scan_path += locale->get_code();
#if defined( cheonsa_platform_windows )
					scan_path += "\\";
#else
#error
#endif
				}
				else
				{
#if defined( cheonsa_platform_windows )
					scan_path += "_common\\";
#else
#error
#endif
				}
#if defined( cheonsa_platform_windows )
				scan_path += ops::file_system_convert_path_format_from_cheonsa_to_windows( relative_file_path );
#else
#error
#endif
				if ( ops::file_system_does_file_exist( scan_path ) )
				{
					absolute_file_path = scan_path;
					return true;
				}
			}
		}

		return false;
	}

	boolean_c content_manager_c::find_text_string( string8_c const & key, string16_c & result ) const
	{
		for ( sint32_c i = 3; i >= 0; i-- )
		{
			if ( _text_strings_resources[ i ].get_is_value_set_and_loaded() )
			{
				if ( _text_strings_resources[ i ]->find_string( key, result ) )
				{
					return true;
				}
			}
		}
		return false;
	}

	boolean_c content_manager_c::find_text_sprite( string8_c const & key, sprite_c & result ) const
	{
		for ( sint32_c i = 3; i >= 0; i-- )
		{
			if ( _text_sprites_resources[ i ].get_is_value_set_and_loaded() )
			{
				if ( result.set_sprites_resource_and_sprite_with_name( _text_sprites_resources[ i ], key ) )
				{
					return true;
				}
			}
		}
		return false;
	}

	void_c content_manager_c::queue_refresh()
	{
		_wants_to_refresh = true;
	}

#define cheonsa_settings_file_name "settings.ini"

	boolean_c content_manager_c::load_settings_file()
	{
		string16_c file_path;
		file_path += _engine_data_folder_path;
		file_path += cheonsa_settings_file_name;
		data_stream_file_c stream;
		if ( !stream.open( file_path, data_stream_mode_e_read ) )
		{
			return false;
		}
		if ( !_settings_file.load( &stream ) )
		{
			return false;
		}
		return true;
	}

	boolean_c content_manager_c::save_settings_file()
	{
		string16_c file_path;
		file_path += _engine_data_folder_path;
		file_path += cheonsa_settings_file_name;
		data_stream_file_c stream;
		if ( !stream.open( file_path, data_stream_mode_e_write ) )
		{
			return false;
		}
		if ( !_settings_file.save( &stream ) )
		{
			return false;
		}  
		return true;
	}

	data_scribe_ini_c & content_manager_c::get_settings_file()
	{
		return _settings_file;
	}

}
