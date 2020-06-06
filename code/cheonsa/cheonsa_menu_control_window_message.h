#pragma once

#include "cheonsa_menu_control_window.h"
#include "cheonsa_menu_control_label.h"
#include "cheonsa_menu_control_button.h"

namespace cheonsa
{

	// a dialog window with a message label and a configurable set of button choices.
	class menu_control_window_message_c
		: public menu_control_window_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "window_message"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

		static vector32x2_c default_size; // new file picker windows will be set to this size by default.

	public:
		enum mode_e
		{
			mode_e_okay,
			mode_e_cancel_okay,
			mode_e_no_yes,
			mode_e_cancel_no_yes,
		};

	protected:
		menu_control_label_c * _message_label;

		menu_control_button_c * _a_button; // first from right.
		menu_control_button_c * _b_button; // second from right.
		menu_control_button_c * _c_button; // third from right.

		mode_e _mode;

		void_c _handle_button_on_clicked( menu_event_information_c event_information );

		virtual void_c _on_input( input_event_c * input_event ) override;

	public:
		menu_control_window_message_c( string8_c const & name );
		virtual ~menu_control_window_message_c() override;

		mode_e get_mode() const;
		void_c set_mode( mode_e value );

		string16_c get_message_text_value() const;
		void_c set_message_text_value( string8_c const & plain_text );
		void_c set_message_text_value( string16_c const & plain_text );
		void_c clear_message_text_value();

	};

}
