#include "cheonsa___build.h"
#include "cheonsa__debug.h"
#include "cheonsa_engine.h"
#include "cheonsa__ops.h"
#if defined( cheonsa_platform_windows )
#include <windows.h>
#endif

namespace cheonsa
{

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
