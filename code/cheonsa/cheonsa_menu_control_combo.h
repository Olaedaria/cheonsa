#pragma once

#include "cheonsa_menu_control_button.h"
#include "cheonsa_menu_control_list.h"

namespace cheonsa
{

	// we use a list item as the combo drop down item.
	class menu_control_combo_item_text_c
		: public menu_control_list_item_text_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "combo_item_text"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		virtual void_c _on_clicked( input_event_c * input_event ) override;
		virtual void_c _on_is_mouse_overed_changed() override;

	public:
		menu_control_combo_item_text_c();
		virtual ~menu_control_combo_item_text_c() override;

		void_c select(); // sets this item as the selected item in the combo box.

	};

	// we use a list as the combo drop down selector.
	class menu_control_combo_c
		: public menu_control_list_i
	{
		friend class menu_control_combo_button_c;

	public:
		static inline char8_c const * get_type_name_static() { return "combo"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		menu_control_c * _combo_button;
		boolean_c _was_just_hidden;

		boolean_c _use_toggle_behavior; // if true then item selected states are toggled when the items are clicked on and the list stays open until it loses focus.

		virtual void_c _on_selected_item_list_changed() override;
		virtual void_c _handle_user_interface_local_box_changed() override;
		virtual void_c _on_is_deep_text_focused_changed( menu_control_c * next_control ) override;

	public:
		menu_control_combo_c();
		virtual ~menu_control_combo_c() override;

		boolean_c get_use_toggle_behavior() const;
		void_c set_use_toggle_behavior( boolean_c value );

		void_c show_like_combo_list( menu_control_c * combo_to_spawn_pop_up_around ); // opens to bottom or top.

		sint32_c get_item_count() const;
		menu_control_list_item_i const * get_item_at_index( sint32_c item_index ) const;
		menu_control_list_item_i * get_item_at_index( sint32_c item_index );

		void_c add_item( menu_control_list_item_i * item, sint32_c index = -1 );
		void_c remove_item( menu_control_list_item_i * item );
		void_c remove_all_items();

		menu_control_list_item_i * get_selected_item() const; // gets the most recently selected item or nullptr.
		void_c set_selected_item( menu_control_list_item_i * item );

		sint32_c get_selected_item_index() const;
		void_c set_selected_item_index( sint32_c item_index );

		sint32_c get_selected_item_limit() const;
		void_c set_selected_item_limit( sint32_c value );

		core_event_c< void_c, menu_control_combo_item_text_c * > on_selected_item_changed_preview; // is invoked as list items are hovered over with the mouse or directional inputs.
		core_event_c< void_c, menu_control_combo_c * > on_selected_item_changed; // is invoked when the selected item changes.

	};

	// the combo control is a button that when clicked opens up a combo list [drop down|pop up].
	// most of the functionality is managed by the combo list control, so use get_combo_list() to access it.
	class menu_control_combo_button_c
		: public menu_control_button_c
	{
		friend class menu_control_combo_c;
		friend class menu_control_combo_item_text_c;

	public:
		static inline char8_c const * get_type_name_static() { return "combo_button"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		menu_control_combo_c * _combo_control;

		virtual void_c _on_clicked( input_event_c * input_event ) override;

		void_c _handle_on_selected_item_changed( menu_control_combo_c * combo_list );

	public:
		menu_control_combo_button_c();
		virtual ~menu_control_combo_button_c() override;

		menu_control_combo_c const * get_combo_control() const;
		menu_control_combo_c * get_combo_control();
		void_c set_combo_control( menu_control_combo_c * value );

	};

}
