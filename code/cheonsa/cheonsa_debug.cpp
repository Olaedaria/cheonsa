#include "cheonsa_debug.h"
#include "cheonsa_engine.h"
#include "cheonsa_ops.h"
#include "cheonsa___build.h"
#if defined( cheonsa_platform_windows )
#include <winuser.h>
#endif

namespace cheonsa
{

	void _cheonsa_assert( wchar_t const * file, unsigned line, wchar_t const * expression, wchar_t const * message )
	{
		string16_c file_name = ops::path_get_file_name( string16_c( mode_e_static, file ) ); // because __FILE__ macro gives an absolute path, it's too much information.
		string16_c full_message;
		full_message += "assert: \"";
		full_message += file_name;
		full_message += ":";
		string8_c line_string;
		ops::convert_uint32_to_string8( line, line_string );
		full_message += line_string;
		full_message += "\", ";
		full_message += expression;
		if ( message )
		{
			full_message += ", ";
			full_message += message;
		}
		cheonsa_log( log_type_e_error, full_message.character_list.get_internal_array() );
	#if !defined( _DEBUG )
		cheonsa_annoy( L"assert", full_message.character_list.get_internal_array() );
	#endif
#if defined( cheonsa_platform_windows )
		_CrtDbgReportW( _CRT_ASSERT, file_name.character_list.get_internal_array(), line, nullptr, expression );
#else
		abort();
#endif
	}

	void cheonsa_log( log_type_e type, wchar_t const * message )
	{
		global_engine_instance.debug._log( type, string16_c( mode_e_static_volatile, message ) ); // going in circles?
	}

	void cheonsa_annoy( wchar_t const * title, wchar_t const * message )
	{
	#if defined( cheonsa_platform_windows )
		MessageBoxW( NULL, message, title, MB_OK | MB_ICONERROR );
	#else
		#error not implemented.
	#endif
	}

}
