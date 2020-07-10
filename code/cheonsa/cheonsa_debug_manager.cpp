#include "cheonsa_debug_manager.h"
#include "cheonsa_engine.h"

#if defined( cheonsa_platform_windows )
#include <windows.h>
#endif

namespace cheonsa
{

	debug_log_message_c::debug_log_message_c()
		: _type( debug_log_type_e_information )
		, _message()
	{
	}

	debug_log_type_e debug_log_message_c::get_type() const
	{
		return _type;
	}

	string16_c const & debug_log_message_c::get_message() const
	{
		return _message;
	}

	debug_manager_c::debug_manager_c()
		: _grid_color( 1.0f, 1.0f, 1.0f, 1.0f )
		, _axis_a_color( 1.000f, 0.279f, 0.000f, 1.0f )
		, _axis_b_color( 0.671f, 1.000f, 0.000f, 1.0f )
		, _axis_c_color( 0.000f, 0.893f, 1.000f, 1.0f )
		, _axis_d_color( 1.0f, 1.0f, 1.0f, 1.0f )
		, _bone_color( 1.0f, 1.0f, 1.0f, 1.0f )
		, _menu_bounds_color( 0.5f, 1.0f, 0.0f, 1.0f )
		, _draw_menu_bounds( false )
		, _draw_world_axes( false )
		, _draw_world_grid( false )
		, _draw_object_light_probes( false )
		, _draw_object_icons( false )
		, _draw_object_axes( false )
		, _draw_object_bounds( false )
		, _draw_object_bones( false )
		, _draw_object_physics( false )
		, _log_file_is_open( false )
		, _log_file()
		, _statistics_is_showing( false )
		, _statistics_text()
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

	void_c debug_manager_c::_log( debug_log_type_e type, string16_c const & message )
	{
		if ( _log_file_is_open && type != debug_log_type_e_information )
		{
			core_list_c< char8_c > utf8;
			ops::append_string16_to_string8( message.character_list, utf8 );
			_log_file.save( utf8.get_internal_array(), utf8.get_length() - 1 );
			_log_file.save( "\n", 1 );
		}
		debug_log_message_c log_message;
		log_message._type = type;
		log_message._message = message;
		on_message_logged.invoke( log_message );
	}

	void_c debug_log( debug_log_type_e type, char8_c const * message )
	{
#if defined( _DEBUG ) && defined( cheonsa_platform_windows )
		OutputDebugStringA( message );
#else
#error not implemented.
#endif
		if ( engine.get_debug_manager() )
		{
			engine.get_debug_manager()->_log( type, string16_c( message ) );
		}
	}

	void_c debug_log( debug_log_type_e type, string8_c const & message )
	{
		#if defined( _DEBUG ) && defined( cheonsa_platform_windows )
		OutputDebugStringA( message.character_list.get_internal_array() );
#else
#error not implemented.
#endif
		if ( engine.get_debug_manager() )
		{
			engine.get_debug_manager()->_log( type, string16_c( message ) );
		}
	}

	void_c debug_log( debug_log_type_e type, char16_c const * message )
	{
#if defined( _DEBUG ) && defined( cheonsa_platform_windows )
		OutputDebugStringW( message );
#else
#error not implemented.
#endif
		if ( engine.get_debug_manager() )
		{
			engine.get_debug_manager()->_log( type, string16_c( message, core_list_mode_e_static_volatile ) );
		}
	}

	void_c debug_log( debug_log_type_e type, string16_c const & message )
	{
		#if defined( _DEBUG ) && defined( cheonsa_platform_windows )
		OutputDebugStringW( message.character_list.get_internal_array() );
#else
#error not implemented.
#endif
		if ( engine.get_debug_manager() )
		{
			engine.get_debug_manager()->_log( type, message );
		}
	}

	void_c debug_annoy( char8_c const * title, char8_c const * message )
	{
#if defined( cheonsa_platform_windows )
		MessageBoxA( NULL, message, title, MB_OK | MB_ICONERROR );
#else
#error not implemented.
#endif
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
