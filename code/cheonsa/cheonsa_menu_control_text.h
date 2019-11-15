#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_control_scroll_bar.h"
#include "cheonsa_menu_element_frame.h"
#include "cheonsa_menu_element_text.h"

namespace cheonsa
{

	// plain text text entry field.
	// only supports text modes menu_text_mode_e_plain_static, menu_text_mode_e_plain_static_selectable, and menu_text_mode_e_plain_editable.
	class menu_control_text_c : public menu_control_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "text"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		menu_element_frame_c _element_frame; // name is "frame".
		menu_element_text_c _element_text; // name is "text".
		menu_element_text_c * _element_text_placeholder; // name is "text_placeholder". this element is only newed if a placeholder is enabled, and is deleted if it is disabled.
		string8_c _placeholder_style_key;
		string8_c _placeholder_string_key;

		menu_visibility_mode_e _horizontal_scroll_bar_visibility_mode;
		menu_control_scroll_bar_horizontal_c * _horizontal_scroll_bar;
		menu_visibility_mode_e _vertical_scroll_bar_visibility_mode;
		menu_control_scroll_bar_vertical_c * _vertical_scroll_bar;

		virtual void_c _on_text_focus_gained() override;
		virtual void_c _on_text_focus_lost() override;
		virtual void_c _on_input( input_event_c * input_event ) override;

		void_c _handle_on_value_changed_preview( menu_element_text_c * text );
		void_c _handle_on_value_changed_commit( menu_element_text_c * text );

	public:
		menu_control_text_c(); // defaults to single line, plain text, editable, no scroll bars.

		virtual void_c update_animations( float32_c time_step ) override;

		virtual void_c load_properties( data_scribe_markup_c::node_c const * node ) override;

		string16_c get_placeholder_text_plain_text() const;
		void_c set_placeholder_text_style_key( string8_c const & style_key ); // defines style to use for placeholder when it is enabled. does not enable or disable placeholder.
		void_c set_placeholder_text_plain_text( string16_c const & plain_text ); // enables placeholder and sets its value. clears the placeholder text element's string reference if it's already set.
		void_c set_placeholder_text_string_key( string8_c const & string_key ); // enables placeholder and sets its value to the string referenced by key.
		void_c clear_placeholder_text(); // disables placeholder.

		void_c set_text_style_key( string8_c const & style_key );
		string16_c get_plain_text() const; // gets the current value of text.
		void_c set_plain_text( string16_c const & plain_text );
		void_c set_text_string_key( string8_c const & string_key );
		void_c clear_text();

		menu_text_filter_mode_e get_text_filter_mode() const;
		void_c set_text_filter_mode( menu_text_filter_mode_e filter_mode, string16_c const & filter_string = string16_c() );

		menu_text_edit_mode_e get_text_edit_mode() const;
		void_c set_text_edit_mode( menu_text_edit_mode_e value );

		boolean_c get_multi_line() const;
		void_c set_multi_line( boolean_c value );

		boolean_c get_word_wrap() const;
		void_c set_word_wrap( boolean_c value );

		sint32_c get_character_limit() const;
		void_c set_character_limit( sint32_c value );

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

	public:
		core_event_c< void_c, menu_control_text_c * > on_value_changed_preview; // occurs whenever the text value changes as the user types.
		core_event_c< void_c, menu_control_text_c * > on_value_changed_commit; // occurs if the text value was modified and the user presses enter or the element loses text input focus.

	};

}
