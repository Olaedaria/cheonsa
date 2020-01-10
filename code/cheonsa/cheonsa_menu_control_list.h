#pragma once

#include "cheonsa_menu_control_list_i.h"

namespace cheonsa
{

	// usable list item implementation.
	class menu_control_list_item_c : public menu_control_list_item_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "list_item"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		menu_control_list_item_c();

	};

	// usable list implementation.
	class menu_control_list_c : public menu_control_list_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "list"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		virtual void_c _on_selection_changed() override;

	public:
		menu_control_list_c();

		sint32_c get_item_count() const;
		menu_control_list_item_c * get_item( sint32_c index ) const;

		void_c add_item( menu_control_list_item_c * item, sint32_c index = -1 );
		void_c remove_item( menu_control_list_item_c * item );
		void_c remove_item( sint32_c index );
		void_c remove_and_delete_all_items();

		sint32_c get_selected_item_index() const; // gets the index of the currently selected list item, or -1 if none or more than one is selected.
		void_c set_selected_item_index( sint32_c index ); // deselects all currently selected list items and selects the list item at index, or the index can be -1 to select none.

		menu_control_list_item_c * get_selected_item() const;
		void_c set_selected_item( menu_control_list_item_c * value );

		core_list_c< menu_control_list_item_c * > get_selected_item_list() const; // be mindful that this creates a new list instance so it's slow.

	public:
		core_event_c< void_c, menu_control_list_c * > on_selection_changed; // occurs when list item selection state changes.

	};

}
