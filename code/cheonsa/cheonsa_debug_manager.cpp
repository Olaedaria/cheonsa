#include "cheonsa_debug_manager.h"
#include "cheonsa_engine.h"

#if defined( cheonsa_platform_windows )
#include <windows.h>
#endif

namespace cheonsa
{

	debug_manager_c::console_line_c & debug_manager_c::console_line_c::operator = ( console_line_c const & other )
	{
		type = other.type;
		message = other.message;
		return *this;
	}

	debug_manager_c::debug_manager_c()
		: _log_file_is_open( false )
		, _log_file()
		, _statistics_is_showing( false )
		, _statistics_text()
		, _console_is_showing( false )
		, _console_lines( 4096 )
	{
	}

	debug_manager_c::~debug_manager_c()
	{
		if ( _log_file_is_open )
		{
			_log_file.close();
		}
	}

	boolean_c debug_manager_c::start( string16_c const & executable_file_path )
	{
		string16_c log_file_path;
		log_file_path += executable_file_path;
		log_file_path += ".log";
		_log_file_is_open = _log_file.open( log_file_path, data_stream_mode_e_write );
		return true;
	}

	boolean_c debug_manager_c::get_statistics_is_showing()
	{
		return _statistics_is_showing;
	}

	void_c debug_manager_c::set_statistics_is_showing( boolean_c value )
	{
		_statistics_is_showing = false;
	}

	boolean_c debug_manager_c::get_console_is_showing()
	{
		return _console_is_showing;
	}

	void_c debug_manager_c::set_console_is_showing( boolean_c value )
	{
		_console_is_showing = value;
	}

	void_c debug_manager_c::log( log_type_e type, string16_c const & message )
	{
#if defined( _DEBUG ) && defined( cheonsa_platform_windows )
		OutputDebugStringW( message.character_list.get_internal_array() );
#endif
		if ( _log_file_is_open && type != log_type_e_information )
		{
			core_list_c< char8_c > utf8;
			ops::convert_string16_to_string8( message.character_list, utf8 );
			_log_file.save( utf8.get_internal_array(), utf8.get_length() - 1 );
			_log_file.save( "\n", 1 );
		}

		if ( _console_lines.get_length() == _console_lines.get_capacity() )
		{
			_console_lines.pop_front();
		}
		console_line_c & console_line = _console_lines.emplace_back();
		console_line.type = type;
		console_line.message = message;
	}

	void_c debug_log( log_type_e type, char16_c const * message )
	{
		if ( engine_c::get_instance()->get_debug_manager() )
		{
			engine_c::get_instance()->get_debug_manager()->log( type, string16_c( core_list_mode_e_static_volatile, message ) );
		}
	}

	void_c debug_annoy( char16_c const * title, char16_c const * message )
	{
#if defined( cheonsa_platform_windows )
		MessageBoxW( NULL, message, title, MB_OK | MB_ICONERROR );
#else
#error not implemented.
#endif
	}

}
