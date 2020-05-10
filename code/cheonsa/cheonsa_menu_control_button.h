#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_element_frame.h"
#include "cheonsa_menu_element_text.h"

namespace cheonsa
{

	// button is a compound control consisting of a frame element and a text element.
	// it's almost exactly the same as menu_control_text_field_c, but the text element is not editable.
	class menu_control_button_c
		: public menu_control_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "button"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		menu_element_frame_c _element_frame; // name is "frame".
		menu_element_text_c _element_text; // name is "text".

		//input_action_c::reference_c _input_action; // if this reference is set, then this is the action that will be invoked when this control is activated.

		menu_control_button_c( string8_c const & name );

	public:
		static menu_control_button_c * make_new_instance( string8_c const & name ); // creates a new instance on the heap with reference count of 0.

		string16_c get_plain_text_value() const;
		void_c set_plain_text_value( string8_c const & plain_text );
		void_c set_plain_text_value( string16_c const & plain_text );
		void_c set_rich_text_value( string8_c const & plain_text_with_mark_up );
		void_c set_rich_text_value( string16_c const & plain_text_with_mark_up );
		void_c clear_text_value();

		virtual void_c load_static_data_properties( data_scribe_markup_c::node_c const * node ) override;

	};

}
