#pragma once

#include "cheonsa_menu_control_button.h"
#include "cheonsa_menu_control_list.h"

namespace cheonsa
{

	// we use a list item as the combo drop down item.
	class menu_control_combo_list_item_c : public menu_control_list_item_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "combo_list_item"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		menu_control_combo_list_item_c();

	};

	// we use a list as the combo drop down selector.
	class menu_control_combo_list_c : public menu_control_list_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "combo_list"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		virtual void_c _on_deep_text_focus_lost() override;

		virtual void_c _on_selection_changed() override;

	public:
		menu_control_combo_list_c();

		sint32_c get_selected_item_index() const; // gets the index of the currently selected list item, or -1 if none or more than one is selected.
		void_c set_selected_item_index( sint32_c index ); // deselects all currently selected list items and selects the list item at index, or the index can be -1 to select none.

		menu_control_combo_list_item_c * get_selected_item() const;
		void_c set_selected_item( menu_control_combo_list_item_c * value );

	public:
		//core_event_c< void_c, menu_control_combo_list_c * > on_selection_changed_preview; // occurs as list items are moused over. not implemented at the moment.
		core_event_c< void_c, menu_control_combo_list_c * > on_selection_changed_commit; // occurs when the selected list item changes.

	public:
		sint32_c get_item_count() const;
		menu_control_combo_list_item_c * get_item_at_index( sint32_c index );

		void_c add_item( menu_control_combo_list_item_c * item, sint32_c index = -1 );
		void_c remove_item( menu_control_combo_list_item_c * item );
		void_c remove_item( sint32_c index );
		void_c remove_and_delete_all_items();

	};

	// the combo control is a button that when clicked opens up a combo list [drop down|pop up].
	// most of the functionality is managed by the combo list control, so use get_combo_list() to access it.
	class menu_control_combo_c : public menu_control_button_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "combo"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		friend class menu_control_combo_list_c;

		menu_control_combo_list_c * _combo_list; // name is "list".

		void_c _handle_on_selection_changed();

		virtual void_c _on_clicked( input_event_c * input_event ) override;

	public:
		menu_control_combo_c();

		void_c show_combo_list(); // shows/opens the combo box list box drop down.
		void_c hide_combo_list(); // hides/closes the combo box list box drop down.

		menu_control_combo_list_c * get_combo_list() const;

	};

}
