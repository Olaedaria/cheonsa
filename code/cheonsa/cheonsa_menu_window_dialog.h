#pragma once

#include "cheonsa_menu_window.h"
#include "cheonsa_menu_control_text.h"
#include "cheonsa_menu_control_button.h"

namespace cheonsa
{

	// a dialog window or panel, that offers a configurable set of button choices (okay, okay cancel, yes no, yes no cancel).
	class menu_window_dialog_c : public menu_window_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "window_dialog"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		enum mode_e
		{
			mode_e_okay,
			mode_e_okay_cancel,
			mode_e_yes_no,
			mode_e_yes_no_cancel,
		};

		enum result_e
		{
			result_e_none, // is set when the window is initially shown, and also is set if cancel or close button is clicked. indicates no result.
			result_e_cancel,
			result_e_okay,
			result_e_yes,
			result_e_no,
		};

	private:
		menu_control_button_c * _control_button_a; // okay, yes.
		menu_control_button_c * _control_button_b; // cancel, no.
		menu_control_button_c * _control_button_c; // cancel.

		mode_e _mode;
		result_e _result;

		void_c _handle_button_on_click( menu_event_information_c event_information );

		virtual void_c _on_input( input_event_c * input_event ) override;

	public:
		menu_window_dialog_c();

		virtual void_c load_static_data_properties( data_scribe_markup_c::node_c const * node ) override;

		mode_e get_mode() const;
		void_c set_mode( mode_e value );

		result_e get_result() const; // gets the last result of the dialog.

		virtual void_c set_is_showed( boolean_c value, boolean_c and_wants_to_be_deleted = false ) override; // sets _result to result_e_none and then calls base implementation.
		virtual void_c set_is_showed_immediately( boolean_c value ) override; // sets _result to result_e_none and then calls base implementation.

		core_event_c< void_c, menu_window_dialog_c * > on_result; // subscribe to on_hide instead and then call get_result().

	};

}
