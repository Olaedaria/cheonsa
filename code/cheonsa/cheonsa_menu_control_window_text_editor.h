#pragma once

#include "cheonsa_menu_control_window.h"
#include "cheonsa_menu_control_text.h"

namespace cheonsa
{

	class menu_control_window_text_editor_c
		: public menu_control_window_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "window_text_editor"; }
		virtual inline char8_c const * get_type_name() const { return get_type_name_static(); }

		static vector32x2_c default_size; // new text editor windows will be set to this size by default.

	private:
		menu_control_text_c * _text;

		menu_control_button_c * _cancel_button;
		menu_control_button_c * _okay_button;

		void_c _handle_button_on_clicked( menu_event_information_c event_information );

		menu_control_window_text_editor_c( string8_c const & name );

	public:
		virtual ~menu_control_window_text_editor_c() override;

		static menu_control_window_text_editor_c * make_new_instance( string8_c const & name ); // creates a new instance on the heap with reference count of 0.

		string16_c get_text_value();
		void_c set_text_value( string16_c const & value );

	};

}
