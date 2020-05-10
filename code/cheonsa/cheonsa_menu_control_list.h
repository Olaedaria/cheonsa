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

	protected:
		menu_control_list_item_separator_c( string8_c const & name );

	public:
		virtual ~menu_control_list_item_separator_c() override;

		static menu_control_list_item_separator_c * make_new_instance( string8_c const & name ); // creates a new instance on the heap with reference count of 0.

	};

	class menu_control_list_item_text_c
		: public menu_control_list_item_text_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "list_item_text"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		menu_control_list_item_text_c( string8_c const & name );

	public:
		virtual ~menu_control_list_item_text_c() override;

		static menu_control_list_item_text_c * make_new_instance( string8_c const & name ); // creates a new instance on the heap with reference count of 0.

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

		menu_control_list_c( string8_c const & name );

	public:
		virtual ~menu_control_list_c() override;

		static menu_control_list_c * make_new_instance( string8_c const & name ); // creates a new instance on the heap with reference count of 0.

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

		core_event_c< void_c, menu_control_list_c * > on_selected_item_list_changed; // occurs when list item selection state changes.

	};

}
