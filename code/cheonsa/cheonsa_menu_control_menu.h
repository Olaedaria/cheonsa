#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_control_list.h"

namespace cheonsa
{

	class menu_control_menu_list_c;

	// a single menu item in a list of menu items, can have text and an icon.
	// can be clicked.
	// <sub_menu></sub_menu>
	// <menu_item>
	class menu_control_menu_list_item_c : public menu_control_list_item_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "menu_list_item"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		boolean_c _is_checkable; // will be true if clicking on this menu item causes it to togle its _is_checked state.
		boolean_c _is_checked; // this menu item's checked state.
		menu_control_menu_list_c * _sub_menu; // optional menu that will open when this menu item is clicked on, which is also a non-client daughter of this control.

	private:
		virtual void_c _on_mouse_over_gained() override;
		virtual void_c _on_mouse_over_lost() override;
		virtual void_c _on_clicked( input_event_c * input_event ) override;

	public:
		menu_control_menu_list_item_c();

		menu_control_menu_list_c * get_sub_menu() const;
		void_c give_sub_menu( menu_control_menu_list_c * menu );
		menu_control_menu_list_c * take_sub_menu();

	};

	// the pop up context menu that holds a list of menu items.
	class menu_control_menu_list_c : public menu_control_list_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "menu_list"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	private:
		virtual void_c _on_deep_text_focus_lost() override;

	public:
		menu_control_menu_list_c();

		void_c show_at( box32x2_c const & around, menu_popup_type_e order );

		sint32_c get_item_count() const;
		menu_control_menu_list_item_c * get_item_at_index( sint32_c index );

		void_c give_item_at_end( menu_control_menu_list_item_c * item );
		void_c give_item_at_index( menu_control_menu_list_item_c * item, sint32_c index );
		menu_control_menu_list_item_c * take_item( menu_control_menu_list_item_c * item );
		menu_control_menu_list_item_c * take_item_at_index( sint32_c index );
		void_c remove_and_delete_all_items();

	};

}
