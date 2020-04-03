#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_control_list.h"

namespace cheonsa
{

	class menu_control_menu_list_c;

	class menu_control_menu_list_item_separator_c
		: public menu_control_list_item_separator_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "menu_list_item_separator"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		menu_control_menu_list_item_separator_c();

	};

	// a single menu item in a list of menu items, can have text and an icon.
	// can be clicked.
	class menu_control_menu_list_item_text_c
		: public menu_control_list_item_text_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "menu_list_item_text"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		boolean_c _can_be_checked; // will be true if this menu item should behave like a check box.
		boolean_c _is_checked; // this menu item's checked state.
		menu_control_menu_list_c * _sub_menu; // optional menu that will open when this menu item is clicked on, which is also a non-client daughter of this control.

	private:
		virtual void_c _on_is_mouse_overed_changed() override;
		virtual void_c _on_clicked( input_event_c * input_event ) override;

	public:
		menu_control_menu_list_item_text_c();

		boolean_c get_can_be_checked() const;
		void_c set_can_be_checked( boolean_c value );

		boolean_c get_is_checked() const;
		void_c set_is_checked( boolean_c value );

		menu_control_menu_list_c * get_sub_menu() const;
		void_c give_sub_menu( menu_control_menu_list_c * menu );
		menu_control_menu_list_c * take_sub_menu();

		core_event_c< void_c, menu_control_menu_list_item_text_c * > on_is_checked_changed;

	};

	// the pop up context menu that holds a list of menu items.
	class menu_control_menu_list_c : public menu_control_list_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "menu_list"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	private:
		virtual void_c _on_is_deep_text_focused_changed() override;

	public:
		menu_control_menu_list_c();

		void_c show_like_context_menu( vector32x2_c const & screen_space_point_to_spawn_pop_up_around );
		void_c show_like_sub_menu( menu_control_c * menu_item_to_spawn_pop_up_around );
		void_c show_like_combo_list( menu_control_c * combo_to_spawn_pop_up_around );

		sint32_c get_item_count() const;
		menu_control_list_item_i * get_item( sint32_c item_index );

		sint32_c give_item( menu_control_list_item_i * item, sint32_c index = -1 );
		menu_control_list_item_i * take_item( sint32_c item_index );
		void_c remove_and_delete_all_items();

	};

}
