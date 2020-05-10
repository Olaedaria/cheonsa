#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_element_frame.h"
#include "cheonsa_menu_element_text.h"

namespace cheonsa
{

	// static plain or rich text label.
	// does not support scroll bars.
	// only supports text modes menu_text_mode_e_plain_static and menu_text_mode_e_rich_static.
	class menu_control_label_c
		: public menu_control_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "label"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		menu_element_frame_c _element_frame; // name is "frame".
		menu_element_text_c _element_text; // name is "text".

		menu_control_label_c( string8_c const & name );

	public:
		virtual ~menu_control_label_c() override;

		static menu_control_label_c * make_new_instance( string8_c const & name ); // creates a new instance on the heap with reference count of 0.

		virtual void_c load_static_data_properties( data_scribe_markup_c::node_c const * node ) override;

		string16_c get_plain_text_value() const;
		void_c set_plain_text_value( string8_c const & plain_text );
		void_c set_plain_text_value( string16_c const & plain_text );
		void_c set_rich_text_value( string8_c const & plain_text_with_mark_up );
		void_c set_rich_text_value( string16_c const & plain_text_with_mark_up );
		void_c clear_text_value();

		menu_text_format_mode_e get_text_format_mode() const;
		void_c set_text_format_mode( menu_text_format_mode_e value );

		boolean_c get_word_wrap() const;
		void_c set_word_wrap( boolean_c value );

		menu_text_align_horizontal_e get_text_align_horizontal() const;
		void_c set_text_align_horizontal( menu_text_align_horizontal_e value );

		menu_text_align_vertical_e get_text_align_vertical() const;
		void_c set_text_align_vertical( menu_text_align_vertical_e value );

	};

}
