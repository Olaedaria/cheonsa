#pragma once

#include "cheonsa_menu_control_button.h"
#include "cheonsa_menu_control_list.h"

namespace cheonsa
{

	// we use a list item as the combo drop down item.
	class menu_control_combo_list_item_text_c
		: public menu_control_list_item_text_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "combo_list_item"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		virtual void_c _on_is_mouse_overed_changed() override;

		menu_control_combo_list_item_text_c( string8_c const & name );

	public:
		virtual ~menu_control_combo_list_item_text_c() override;

		static menu_control_combo_list_item_text_c * make_new_instance( string8_c const & name ); // creates a new instance on the heap with reference count of 0.

		void_c select(); // sets this item as the selected item in the combo box.

	};

	// we use a list as the combo drop down selector.
	class menu_control_combo_list_c
		: public menu_control_list_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "combo_list"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		virtual void_c _on_selected_item_list_changed() override;
		virtual void_c _on_is_deep_text_focused_changed() override;

		menu_control_combo_list_c( string8_c const & name );

	public:
		virtual ~menu_control_combo_list_c() override;

		static menu_control_combo_list_c * make_new_instance( string8_c const & name ); // creates a new instance on the heap with reference count of 0.

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

		core_event_c< void_c, menu_control_combo_list_item_text_c * > on_selected_item_changed_preview; // is invoked as list items are hovered over with the mouse or directional inputs.
		core_event_c< void_c, menu_control_combo_list_c * > on_selected_item_changed; // is invoked when the selected item changes.

	};

	// the combo control is a button that when clicked opens up a combo list [drop down|pop up].
	// most of the functionality is managed by the combo list control, so use get_combo_list() to access it.
	class menu_control_combo_c
		: public menu_control_button_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "combo"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		friend class menu_control_combo_list_c;
		friend class menu_control_combo_list_item_text_c;

		menu_control_combo_list_c * _combo_list; // name is "combo_list".

		virtual void_c _on_clicked( input_event_c * input_event ) override;

		void_c _handle_on_selected_item_changed( menu_control_combo_list_c * combo_list );

		menu_control_combo_c( string8_c const & name );

	public:
		virtual ~menu_control_combo_c() override;

		static menu_control_combo_c * make_new_instance( string8_c const & name ); // creates a new instance on the heap with reference count of 0.

		void_c show_combo_list(); // shows/opens the combo box list box drop down.
		void_c hide_combo_list(); // hides/closes the combo box list box drop down.

		menu_control_combo_list_c const * get_combo_list() const;
		menu_control_combo_list_c * get_combo_list();

	};

}
