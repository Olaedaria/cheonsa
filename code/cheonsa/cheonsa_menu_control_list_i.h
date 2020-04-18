#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_element_frame.h"
#include "cheonsa_menu_element_text.h"
#include "cheonsa_menu_control_scroll_bar_y.h"
#include "cheonsa_menu_control_panel.h"

namespace cheonsa
{

	class menu_control_list_i;

	// base list item interface.
	// it's kind of a tangled mess because i am trying to hide features so that they can be exposed by derived classes as needed later.
	// the height of the list item is determined by the layout of the text element.
	class menu_control_list_item_i
		: public menu_control_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "list_item_i"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		menu_control_list_item_i( string8_c const & name );

		friend class menu_control_list_i;
		friend class menu_control_list_item_text_i;

		boolean_c _can_be_selected; // will be true for list item types that can be selected, false if otherwise.
		boolean_c _is_selected; // will be true if this list item is selected, false if otherwise.

		void_c _set_is_selected( boolean_c value, boolean_c try_to_multi_select ); // sets the selected state of this list item.
		virtual void_c _on_is_selected_changed(); // is called after _is_selected value is changed.

	public:
		boolean_c get_can_be_selected() const;
		boolean_c get_is_selected() const;

	};

	// has a frame element.
	// can't be highlighted or selected.
	class menu_control_list_item_separator_i
		: public menu_control_list_item_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "list_item_separator_i"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		menu_element_frame_c _element_frame; // name is "frame".

		float32_c _height; // height of this separator in pixels.

		virtual void_c _update_transform_and_layout() override; // needs to respond to if mother resizes.

		menu_control_list_item_separator_i( string8_c const & name );

	public:
		float32_c get_height() const;
		void_c set_height( float32_c value );

	};

	// has several frame elements and a text element.
	// can be highlighted and selected.
	class menu_control_list_item_text_i
		: public menu_control_list_item_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "list_item_text_i"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		menu_element_frame_c _element_frame; // name is "frame".
		menu_element_frame_c _element_selected_frame; // name is "selected_frame". this frame's visibility is toggled when this list item is selected.
		menu_element_frame_c _element_highlighted_frame; // name is "highlighted_frame". this frame's visiblity is toggled when the mouse is over the list item or the list item is highlighted with a directional input.
		menu_element_text_c _element_text; // name is "text".

		void_c _handle_element_text_on_value_changed( menu_element_text_c * element ); // need to invalidate layout of list if text value changes to force reflow of list items.

		virtual void_c _on_is_mouse_overed_changed() override;
		virtual void_c _on_is_mouse_focused_changed() override;
		virtual void_c _on_is_pressed_changed() override;
		virtual void_c _on_clicked( input_event_c * input_event ) override;
		void_c _update_element_states();

		virtual void_c _update_transform_and_layout() override; // needs to respond to if mother resizes.

		menu_control_list_item_text_i( string8_c const & name );

	public:
		void_c set_is_selected( boolean_c value, boolean_c try_to_multi_select ); // sets the selected state of this item. if selecting this item would go over the select limit, then the oldest selected items will be deselected.

		string16_c get_plain_text_value() const;
		void_c set_plain_text_value( string8_c const & plain_text );
		void_c set_plain_text_value( string16_c const & plain_text );
		void_c set_rich_text_value( string8_c const & plain_text_with_mark_up );
		void_c set_rich_text_value( string16_c const & plain_text_with_mark_up );
		void_c clear_text_value();

	};

	// list interface and base internal implementation.
	// it is designed this way to hide features, which can be exposed selectively by specific implementations.
	// lists hold list items, and support a vertical scroll bar in order to deal with vertical overflow.
	// load_hierarchy() does not load list items from xml. list items must be added by the program.
	class menu_control_list_i
		: public menu_control_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "list_i"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		friend class menu_control_list_item_i;
		friend class menu_control_list_item_separator_i;
		friend class menu_control_list_item_text_i;

		menu_element_frame_c _element_frame; // name is "frame".
		menu_element_frame_c _element_border_frame;

		menu_control_c * _client; // holds the list items.

		boolean_c _item_origins_are_dirty; // if true then _lay_out_list_items() needs to be called.
		void_c _lay_out_item_origins(); // assumes that all items have up to date local boxes, and sets their origins so that lay out edge to edge.

		menu_size_mode_e _vertical_size_mode; // if height of list box should fit-to-height of contents or not.
		float32_c _vertical_size_maximum; // a value <= 0.0f indicates no maximum height. otherwise this list box's fit-to-content height will not be larger than this, and the scroll bar may be shown.

		menu_visibility_mode_e _vertical_scroll_bar_visibility_mode;
		menu_control_scroll_bar_y_c * _vertical_scroll_bar; // name is "vertical_scroll_bar". is always created even if it may never be shown.
		void_c _handle_vertical_scroll_bar_on_value_changed( menu_control_scroll_bar_i * scroll_bar );
		void_c _handle_vertical_scroll_bar_on_preferred_thickness_changed( menu_control_scroll_bar_i * scroll_bar );
		void_c _lay_out_vertical_scroll_bar();

		sint32_c _selected_item_limit; // at most how many list items may be selected at a time, or -1 for no limit.
		void_c _set_selected_item_limit( sint32_c value );
		core_list_c< menu_control_list_item_i * > _selected_item_list; // holds all of the items that currently have their _is_selected state set to true.
		void_c _deselect_all_items(); // deselects all selected items, does not call _on_selected_item_list_changed.
		void_c _deselect_items_over_limit(); // deselects a number of selected items, does not call _on_selected_item_list_changed.

		virtual void_c _on_selected_item_list_changed(); // is called when the selected items change. does nothing in this base class, but in derived classes it should invoke the associated event.
		virtual void_c _on_is_mouse_overed_changed() override;
		virtual void_c _on_is_deep_text_focused_changed() override;
		virtual void_c _on_input( input_event_c * input_event ) override;

		sint32_c _get_item_count() const;
		menu_control_list_item_i const * _get_item( sint32_c item_index ) const;
		menu_control_list_item_i * _get_item( sint32_c item_index );

		sint32_c _give_item( menu_control_list_item_i * item, sint32_c index ); // gives list item to client.
		menu_control_list_item_i * _take_item( sint32_c item_index ); // takes list item from client.
		void_c _remove_and_delete_all_items(); // removes and deletes all list items from client.

		sint32_c _get_selected_item_index() const;
		void_c _set_selected_item_index( sint32_c item_index );

		menu_control_list_item_i * _get_selected_item() const;
		void_c _set_selected_item( menu_control_list_item_i * item );

		menu_control_list_i( string8_c const & name );

	public:
		virtual void_c update_animations( float32_c time_step ) override;
		virtual void_c load_static_data_properties( data_scribe_markup_c::node_c const * node ) override;

		box32x2_c const & get_client_margins() const;
		void_c set_client_margins( box32x2_c const & value );

		menu_visibility_mode_e get_vertical_scroll_bar_visibility_mode() const;
		void_c set_vertical_scroll_bar_visibility_mode( menu_visibility_mode_e value );

	};

}
