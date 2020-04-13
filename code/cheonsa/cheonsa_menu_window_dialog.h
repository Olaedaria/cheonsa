#pragma once

#include "cheonsa_menu_window.h"
#include "cheonsa_menu_control_label.h"
#include "cheonsa_menu_control_button.h"

namespace cheonsa
{

	// a dialog window with message text and a configurable set of button choices.
	class menu_window_dialog_c
		: public menu_window_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "window_dialog"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		enum mode_e
		{
			mode_e_okay,
			mode_e_okay_cancel,
			mode_e_yes_cancel,
			mode_e_yes_no_cancel,
		};

	private:
		menu_control_label_c * _message_label;

		menu_control_button_c * _a_button; // okay, yes.
		menu_control_button_c * _b_button; // cancel, no.
		menu_control_button_c * _c_button; // cancel.

		mode_e _mode;

		void_c _handle_button_on_clicked( menu_event_information_c event_information );

		virtual void_c _on_input( input_event_c * input_event ) override;

	public:
		menu_window_dialog_c();

		mode_e get_mode() const;
		void_c set_mode( mode_e value );

		string16_c get_message() const;
		void_c set_message( string8_c const & plain_text );
		void_c set_message( string16_c const & plain_text );
		void_c clear_message();

	};

}
