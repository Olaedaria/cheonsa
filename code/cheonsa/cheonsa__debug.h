#pragma once

namespace cheonsa
{

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
