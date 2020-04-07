#pragma once

#include "cheonsa_menu_control_list_i.h"

namespace cheonsa
{

	class menu_control_list_item_separator_c
		: public menu_control_list_item_separator_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "list_item_separator"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		menu_control_list_item_separator_c();

	};

	class menu_control_list_item_text_c
		: public menu_control_list_item_text_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "list_item_text"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		menu_control_list_item_text_c();

	};

	class menu_control_list_c
		: public menu_control_list_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "list"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		virtual void_c _on_selected_item_list_changed() override;

	public:
		menu_control_list_c();

		sint32_c get_item_count() const;
		menu_control_list_item_i const * get_item( sint32_c item_index ) const;
		menu_control_list_item_i * get_item( sint32_c item_index );

		sint32_c give_item( menu_control_list_item_i * item, sint32_c index = -1 );
		menu_control_list_item_i * take_item( sint32_c item_index );
		void_c remove_and_delete_all_items();

		sint32_c get_selected_item_index() const; // gets the most recently selected item index or -1.
		void_c set_selected_item_index( sint32_c item_index );

		menu_control_list_item_i * get_selected_item() const; // gets the most recently selected item or nullptr.
		void_c set_selected_item( menu_control_list_item_i * item );

		core_list_c< menu_control_list_item_i * > const & get_selected_item_list() const;

	public:
		core_event_c< void_c, menu_control_list_c * > on_selected_item_list_changed; // occurs when list item selection state changes.

	};

}
