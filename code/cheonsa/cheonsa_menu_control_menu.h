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
		virtual ~menu_control_menu_list_item_separator_c() override;

	};

	// a single menu item in a list of menu items, can have text and an icon.
	// can be clicked.
	class menu_control_menu_list_item_text_c
		: public menu_control_list_item_text_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "menu_list_item_text"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		boolean_c _can_be_checked; // will be true if this menu item should behave like a check box.
		boolean_c _is_checked; // this menu item's checked state.
		menu_control_menu_list_c * _sub_menu; // optional menu that will open when this menu item is clicked on, which is also a non-client daughter of this control.

		virtual void_c _on_clicked( input_event_c * input_event ) override;

	public:
		menu_control_menu_list_item_text_c();
		virtual ~menu_control_menu_list_item_text_c() override;

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
	// has a contrived hack mechanism to get around an issue of menu opening each time a button is clicked.
	// it's more complicated than i'd like it to be, but it's whatever (it's hard to program certain behaviors around strictly linear events, but it's easier with some non-linearity through near-future-sight).
	// if you have a button that is programmed to show the menu in its on clicked handler, then it will open the menu each time it is clicked.
	// the menu is also programmed to hide itself it loses deep text focus.
	// the losing of the menu's deep text focus happens before the button gets the on pressed changed and on clicked events, so the button has no way to know if it is going to be clicked from the same chain of events that caused the menu to lose deep text focus and hide.
	// so we have in the menu a couple of properties:
	// _button and _was_just_hidden.
	// _button can be set to the button that is supposed to open and close the menu when it's clicked.
	// _on_is_deep_text_focused_changed() receives a pointer to the control that is going to recieve text focus next.
	// so _on_is_deep_text_focused_changed() checks if the control that is going to receive text focus next is equal to the _button, and if so then it sets _was_just_hidden to true.
	// in the _button's on_clicked event handler, it should check if the menu's get_was_just_hidden() is true, then set it to false, else it can show the menu.
	class menu_control_menu_list_c : public menu_control_list_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "menu_list"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	private:
		menu_control_c * _tab_button; // can be set to the button that is programmed to open menu in its on clicked handler. used to detect if the menu is being closed by the button (when losing deep text focus) so that the program has a way to detect if it shouldn't open the menu again.
		boolean_c _was_just_hidden; // flip flop switch, to prevent (for example) a button that opens the menu, to open it on the same click that closes it.

		virtual void_c _handle_user_interface_local_box_changed() override;
		virtual void_c _on_is_deep_text_focused_changed( menu_control_c * next_control ) override;

	public:
		menu_control_menu_list_c();
		virtual ~menu_control_menu_list_c() override;

		menu_control_c * get_tab_button() const;
		void_c set_tab_button( menu_control_c * value ); // does not hold a reference to the given control (to avoid circular reference counts).

		boolean_c get_was_just_hidden() const;
		void_c set_was_just_hidden( boolean_c value );

		void_c show_like_context_menu( vector32x2_c const & screen_space_point_to_spawn_pop_up_around );
		void_c show_like_sub_menu( menu_control_c * menu_item_to_spawn_pop_up_around ); // opens to right or left.
		void_c show_like_combo_list( menu_control_c * combo_to_spawn_pop_up_around ); // opens to bottom or top.

		sint32_c get_item_count() const;
		menu_control_list_item_i const * get_item_at_index( sint32_c item_index ) const;
		menu_control_list_item_i * get_item_at_index( sint32_c item_index );

		void_c add_item( menu_control_list_item_i * item, sint32_c index = -1 );
		void_c remove_item( menu_control_list_item_i * item );
		void_c remove_all_items();

	};

}
