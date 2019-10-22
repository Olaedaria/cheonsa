#pragma once

#undef NDEBUG // force assert to be defined in both debug and release.
#include <assert.h>

//#define a_to_w_(s) L ## s
//#define a_to_w(s) a_to_w_(s) // looks really stupid but it's needed to macro expand s, in case s is a macro.
//#define cheonsa_assert(expression, ...) (void)( (!!(expression)) || ( cheonsa::_cheonsa_assert( a_to_w(__FILE__), (unsigned)(__LINE__), a_to_w(#expression), __VA_ARGS__ ), 0 ) )

#if defined( _DEBUG )
// enabled in debug builds.
#define cheonsa_assert( expression, ... ) assert( expression )
#else
// disabled in release builds.
#define cheonsa_assert( expression, ... ) ((void)0)
#endif

// enabled in debug and release builds.
#define cheonsa_assert_release( expression, ... ) assert( expression )

namespace cheonsa
{

	// this will write a message to the debug output, write a message to the engine's log file, and write a message to the engine's console.
	// it will also open a message box pop up with the message in release builds.
	// then it will abort execution of the program.
	void _cheonsa_assert( wchar_t const * file, unsigned line, wchar_t const * expression, wchar_t const * message = nullptr );

	enum log_type_e
	{
		log_type_e_information, // white.
		log_type_e_okay, // green-yellow.
		log_type_e_warning, // yellow.
		log_type_e_error, // red-orange.
	};

	void cheonsa_log( log_type_e type, wchar_t const * message ); // this will write a message to the debug output, and if the engine is started then it will also write a message to the engine's log file and to the engine's console interface.
	void cheonsa_annoy( wchar_t const * title, wchar_t const * message ); // this will open up an operating system message box pop up window with an OK button, it blocks execution until the user dismisses it.

}
