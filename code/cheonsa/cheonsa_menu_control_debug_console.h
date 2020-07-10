#pragma once

#include "cheonsa_menu_control_window.h"
#include "cheonsa_menu_control_text.h"
#include "cheonsa_menu_control_button.h"
#include "cheonsa_debug_manager.h"

namespace cheonsa
{

	// typical game engine debug console, as a window.
	// messages that are logged with debug_log() and debug_annoy() will show up here.
	// multi line text log displayed at top, single line text input at bottom.
	class menu_control_debug_console_c
		: public menu_control_window_c
	{
	private:
		menu_control_text_c * _output_text;
		menu_control_text_c * _input_text;
		menu_control_button_c * _close_button;

		void_c _handle_message_logged( debug_log_message_c const & log_message );

	public:
		menu_control_debug_console_c();
		~menu_control_debug_console_c();

	};

}
