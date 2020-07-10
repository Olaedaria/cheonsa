#pragma once

#include "cheonsa_menu_control_list_i.h"

namespace cheonsa
{

	class menu_control_list_c;

	class menu_control_list_item_separator_c
		: public menu_control_list_item_separator_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "list_item_separator"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		menu_control_list_item_separator_c();
		virtual ~menu_control_list_item_separator_c() override;

		menu_control_list_c * get_list() const; // gets the list that this list item is added to, if any.

	};

	class menu_control_list_item_text_c
		: public menu_control_list_item_text_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "list_item_text"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		virtual void_c _on_clicked( input_event_c * input_event ) override;
		virtual void_c _on_multi_clicked( input_event_c * input_event ) override;

	public:
		menu_control_list_item_text_c();
		virtual ~menu_control_list_item_text_c() override;

		menu_control_list_c * get_list() const; // gets the list that this list item is added to, if any.

		void_c set_is_selected( boolean_c is_selected, boolean_c try_to_multi_select );

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
		virtual ~menu_control_list_c() override;

		sint32_c get_selected_item_limit() const;
		void_c set_selected_item_limit( sint32_c value );

		sint32_c get_item_count() const;
		menu_control_list_item_i const * get_item_at_index( sint32_c item_index ) const;
		menu_control_list_item_i * get_item_at_index( sint32_c item_index );

		void_c add_item( menu_control_list_item_i * item, sint32_c index = -1 );
		void_c remove_item( menu_control_list_item_i * item );
		void_c remove_all_items();

		menu_control_list_item_i * get_selected_item() const; // gets the most recently selected item, or nullptr if no item is selected.
		void_c set_selected_item( menu_control_list_item_i * item ); // selects a single item, or deselects everything if nullptr.

		sint32_c get_selected_item_index() const; // gets the index of the most recently selected item, or -1 if no item is selected.
		void_c set_selected_item_index( sint32_c item_index ); // selects a single item, or deselects everything if -1.

		core_list_c< menu_control_list_item_i * > const & get_selected_item_list() const;

		void_c sort_items_by_plain_text_value( boolean_c invert = false );

		core_event_c< void_c, menu_control_list_c * > on_selected_item_list_changed; // occurs when list item selection state changes.

	};

}
