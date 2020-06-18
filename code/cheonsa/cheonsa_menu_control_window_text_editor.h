#pragma once

#include "cheonsa_menu_control_window.h"
#include "cheonsa_menu_control_text.h"

namespace cheonsa
{

	// plain text editor.
	// can be configured as single-line or multi-line.
	class menu_control_window_text_editor_c
		: public menu_control_window_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "window_text_editor"; }
		virtual inline char8_c const * get_type_name() const { return get_type_name_static(); }

		static vector32x2_c default_size; // new text editor windows will be set to this size by default.

	public:
		enum mode_e
		{
			mode_e_single_line,
			mode_e_multi_line,
		};

	private:
		menu_control_text_c * _text;

		menu_control_button_c * _cancel_button;
		menu_control_button_c * _okay_button;

		void_c _handle_button_on_clicked( menu_event_information_c event_information );

		virtual void_c _on_input( input_event_c * input_event ) override;

	public:
		menu_control_window_text_editor_c();
		virtual ~menu_control_window_text_editor_c() override;

		mode_e get_mode() const;
		void_c set_mode( mode_e value );

		sint32_c get_character_limit() const;
		void_c set_character_limit( sint32_c value );

		string16_c get_plain_text_value();
		void_c set_plain_text_value( string16_c const & value );

		menu_control_text_c * get_text_control();
		menu_control_button_c * get_cancel_button();
		menu_control_button_c * get_okay_button();

	};

}
