#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_control_scroll_bar_x.h"
#include "cheonsa_menu_control_scroll_bar_y.h"
#include "cheonsa_menu_element_frame.h"
#include "cheonsa_menu_element_text.h"

namespace cheonsa
{

	// plain text text box text entry field.
	// only supports text modes menu_text_mode_e_plain_static, menu_text_mode_e_plain_static_selectable, and menu_text_mode_e_plain_editable.
	class menu_control_text_c
		: public menu_control_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "text"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		menu_element_frame_c _frame_element; // name is "frame".
		menu_element_text_c _text_element; // name is "text".
		menu_element_text_c * _place_holder_text_element; // name is "place_holder_text". this element is only newed if a placeholder is enabled, and is deleted if it is disabled.
		menu_element_frame_c _border_frame_element; // name is "border_frame".

		menu_visibility_mode_e _horizontal_scroll_bar_visibility_mode;
		menu_control_scroll_bar_x_c * _horizontal_scroll_bar;
		menu_visibility_mode_e _vertical_scroll_bar_visibility_mode;
		menu_control_scroll_bar_y_c * _vertical_scroll_bar;

		virtual void_c _on_is_text_focused_changed( menu_control_c * next_control ) override;
		virtual void_c _on_clicked( input_event_c * input_event ) override;
		virtual void_c _on_multi_clicked( input_event_c * input_event ) override;
		virtual void_c _on_input( input_event_c * input_event ) override;

		void_c _handle_on_plain_text_value_changed_preview( menu_element_text_c * text );
		void_c _handle_on_plain_text_value_changed( menu_element_text_c * text );

	public:
		menu_control_text_c(); // defaults to single line, plain text, editable, no scroll bars.
		virtual ~menu_control_text_c() override;

		virtual void_c update_animations( float32_c time_step ) override;

		string16_c get_place_holder_text_value() const;
		void_c set_place_holder_plain_text_value( string8_c const & plain_text );
		void_c set_place_holder_plain_text_value( string16_c const & plain_text );
		void_c clear_place_holder_text_value();

		string16_c get_plain_text_value() const;
		void_c set_plain_text_value( string8_c const & plain_text );
		void_c set_plain_text_value( string16_c const & plain_text );
		void_c set_rich_text_value( string8_c const & plain_text_with_mark_up );
		void_c set_rich_text_value( string16_c const & plain_text_with_mark_up );
		void_c clear_text_value();

		menu_text_format_mode_e get_text_format_mode() const;
		void_c set_text_format_mode( menu_text_format_mode_e value );

		menu_text_interact_mode_e get_text_interact_mode() const;
		void_c set_text_interact_mode( menu_text_interact_mode_e value );

		menu_text_filter_mode_e get_text_filter_mode() const;
		void_c set_text_filter_mode( menu_text_filter_mode_e value );
		void_c set_text_filter_string( string16_c const & value );

		sint32_c get_character_limit() const;
		void_c set_character_limit( sint32_c value );

		boolean_c get_multi_line() const;
		void_c set_multi_line( boolean_c value );

		boolean_c get_word_wrap() const;
		void_c set_word_wrap( boolean_c value );

		sint32_c get_cursor_index() const;
		void_c set_cursor_index( sint32_c value );

		menu_text_align_horizontal_e get_text_align_horizontal() const;
		void_c set_text_align_horizontal( menu_text_align_horizontal_e value );

		menu_text_align_vertical_e get_text_align_vertical() const;
		void_c set_text_align_vertical( menu_text_align_vertical_e value );

		menu_visibility_mode_e get_horizontal_scroll_visibility_mode() const;
		void_c set_horizontal_scroll_visibility_mode( menu_visibility_mode_e value );

		menu_visibility_mode_e get_vertical_scroll_visibility_mode() const;
		void_c set_vertical_scroll_visibility_mode( menu_visibility_mode_e value );

		core_event_c< void_c, menu_control_text_c * > on_plain_text_value_changed_preview; // occurs whenever the text value changes as the user types.
		core_event_c< void_c, menu_control_text_c * > on_plain_text_value_changed; // occurs if the text value was modified and the user presses enter or the element loses text input focus.

	};

}
