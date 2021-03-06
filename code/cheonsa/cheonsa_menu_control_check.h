#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_element_frame.h"
#include "cheonsa_menu_element_text.h"

namespace cheonsa
{

	// a check box that can operate in two modes:
	// normal: any number of check box options can be selected.
	// single: also known as radio button, only at most one check box option can be selected per a group of sister check boxes that share the same mother.
	// it consists of a check box on the left side, and a text element to the right of it.
	class menu_control_check_c
		: public menu_control_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "check"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		enum mode_e
		{
			mode_e_normal, // normal style check box.
			mode_e_single, // radio button style check box, at most only one check box can be checked within the shared mother control.
		};

	protected:
		mode_e _mode;
		boolean_c _is_checked;
		virtual void_c _on_clicked( input_event_c * input_event ) override;

		menu_element_frame_c _box_element; // name is "box_frame".
		menu_element_frame_c _mark_element; // name is "mark_frame".
		menu_element_text_c _text_element; // name is "text".

		virtual void_c _update_daughter_element_animations( float32_c time_step ) override;

	public:
		menu_control_check_c();
		virtual ~menu_control_check_c() override;

		string16_c get_plain_text_value() const;
		void_c set_plain_text_value( string8_c const & plain_text );
		void_c set_plain_text_value( string16_c const & plain_text );
		void_c set_rich_text_value( string8_c const & plain_text_with_mark_up );
		void_c set_rich_text_value( string16_c const & plain_text_with_mark_up );
		void_c clear_text_value();

		mode_e get_mode() const;
		void_c set_mode( mode_e value );

		boolean_c get_is_checked() const;
		void_c set_is_checked( boolean_c value );

		core_event_c< void_c, menu_control_c * > on_checked_changed; // occurs after _checked value is changed.

		virtual void_c load_static_data_properties( data_scribe_markup_c::node_c const * node ) override;

	};

}
