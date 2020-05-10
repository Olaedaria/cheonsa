#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_control_scroll_bar_y.h"
#include "cheonsa_menu_control_text.h"
#include "cheonsa_menu_element_text.h"
#include "cheonsa_menu_element_frame.h"
#include "cheonsa_menu_element_text.h"

namespace cheonsa
{

	// collections are suited for holding massive numbers of items with optimal memory overhead and constant speed cost to calculate visible item layout.
	// this is because items in the collection do not store anything related to layout.
	// contrast to cheonsa's list control, where each list items is in turn a control, and all of those controls have to update and run their layout logic even when they are not in view.
	// the collection is designed to immitate windows's file explorer icon view and details view.
	// the get_value() method that you implement on your collection item class should at bare minimum return a result for the query of "name".
	class menu_control_collection_c
		: public menu_control_c
	{
		friend class item_i;

	public:
		static inline char8_c const * get_type_name_static() { return "collection"; }
		virtual inline char8_c const * get_type_name() const { return get_type_name_static(); }

	public:
		// which mode the collection operates in.
		enum display_mode_e
		{
			// basically like the icons view in windows file explorer.
			// items are laid out in a grid from left to right, top to bottom.
			// each item has size equal to _icons_item_width and _icons_item_height.
			// each item has an icon with size equal to _icons_item_width and _icons_icon_height.
			// each item has a label beneath the icon with size equal to _icons_item_width and ( _icons_item_height - _icons_icon_height ).
			// the property key "icon" is used to resolve the display icon of each item, it should be a file path to a texture resource.
			// the property key "name" is used to resolve the display name of each item.
			// columns are not displayed in this mode but the sort column key is still used to sort items in this mode.
			display_mode_e_icons,
			
			// basically like details view in windows file explorer.
			// items are laid out in rows from top to bottom.
			// each item has width equal to the width of the container, and height defined by _details_item_height.
			// each item has an icon displayed in the left most column.
			// each item has text values displayed in each column.
			// the property key "icon" is used to resolve the display icon of each item, it should be a file path to a texture resource.
			// items are sorted by the sort column key and the sort order.
			display_mode_e_details,
		};

		// how items can be selected.
		enum select_mode_e
		{
			select_mode_e_none, // items can't be selected.
			select_mode_e_single, // at most one item can be selected by clicking or using up and down keys or page up and page down keys.
			select_mode_e_multiple, // any number of items can be selected, via mix of modifier keys ctrl/shift and clicking or dragging a box around items or using up and down keys or page up and page down keys.
		};

		// which direction items are sorted by, from top to bottom. the labels might seem backwards depending on how you think of it.
		enum sort_order_e
		{
			sort_order_e_ascending, // lowest values at top, highest values at bottom.
			sort_order_e_descending, // highest values at top, lowest values at bottom.
		};

		// property values have a display value and an absolute value, this defines which one to sort the column with.
		// this is useful for example in the case of numeric time stamps. numeric time stamps when converted to strings (of the user's locale) might sort in a different order way then if they were sorted by their numeric value.
		enum sort_by_e
		{
			sort_by_e_display_value, // sorting by display value means using natural string compare to sort the items.
			sort_by_e_absolute_value, // sorting by absolute value means using numeric compare to sort the items.
		};

		// base interface of an item instance that can be added to a menu_control_collection_c.
		// you need to implement your own version of this, and override the methods get_icon_texture(), get_value(), and set_value().
		// then add those instnaces to your menu_control_collection_c instance.
		class item_i
		{
			friend class menu_control_collection_c;

		public:
			// cached column/property values. 
			// used to sort items by column/property.
			class value_c
			{
			public:
				string16_c display_value; // treat as read only and volatile (don't save a pointer/reference to it).
				sint64_c absolute_value; // treat as read only and volatile (don't save a pointer/reference to it).

			public:
				value_c();

			};

		protected:
			menu_control_collection_c * _mother_collection; // the collection that is holding this item.
			value_c * _value_cache; // list of resolved column values, always the same length as the _column_list in the _mother_collection.
			sint32_c _group; // can be used to group together items of the same type when items are sorted. for example, when used by file collections, folders will be assigned group 0 and files will be assigned group 1, which causes folders to group together and files to group together.
			sint32_c _index; // the index of this item within the list.
			boolean_c _is_selected; // this item's selected state.

			void_c _cache_values(); // rebuilds _value_list for this item, queries and caches property column values.

			item_i();

		public:
			sint32_c get_group() const;
			void_c set_group( sint32_c value );

			sint32_c get_index() const;

			// queries the icon or thumbnail for this item.
			// if _mother_collection->get_display_mode() == menu_control_collection_c::display_mode_e_icons then you can return a thumbnail instead of an icon if you want.
			// may return nullptr.
			virtual resource_file_texture_c * get_icon_texture() const;

			// queries the values of a property with key.
			// override with your own implementation.
			// return true if the property value can be resolved, false if not.
			// key is the column/property key.
			// set sort_value to values >= 0 for things that can be sorted by absolute value, things like file size and file time modified. set it to -1 to indicate to not use the sort value.
			// set display_value to the user facing value of the item, if needed take into account locale settings when constructing this string value, when sort_value == -1 then natural string compare string16_sort_compare() will be used to sort these items.
			virtual boolean_c get_value( string8_c const & property_key, string16_c & display_value, sint64_c & absolute_value ) const;

			// attempts to set display value of a given property.
			// your implementation may reinterpret the user-provided display_value as something else, for example a date and time string could be converted to a sint64_c time in milliseconds relative to an epoch.
			// return true if the new display value is accepted, false if not.
			virtual boolean_c set_value( string8_c const & property_key, string16_c const & display_value );

			// gets the selected state of this item.
			boolean_c get_is_selected() const;
			// sets the selected state of this item.
			// if the mother collection does not support multi-selection and you try to select this item, then it will deselect whatever else is currently selected.
			void_c set_is_selected( boolean_c value );

		public:
			static boolean_c relative_compare( item_i * const & a, item_i * const & b ); // for insertion sort.
			static uint64_c absolute_value( item_i * const & a ); // for quick sort.
			static boolean_c relative_group_compare( item_i * const & a, item_i * const & b ); // for insertion sort, secondary sort.

		};

	protected:
		// which columns to display at the top of the collection when the display mode is details.
		// which property values to cache with each item in the collection.
		class column_c
		{
			friend class menu_control_collection_c;

		protected:
			string8_c _key; // unique key to identify this column by, is always the same regardless of what the language is. this key is used to look up property values in the collection items.
			sint32_c _position; // x offset in pixels of this column from the left edge of the container. this is equal to all the widths of the visible columns to the left of this column.
			sint32_c _width; // width in pixels of this column, the user can resize both fixed an non-fixed columns.
			sort_by_e _sort_by; // which value to sort this column by.
			boolean_c _is_editable; // if true, then the user is supposed to be able to edit (rename) item display value in this column.
			menu_element_frame_c _element_frame; // name is "column_frame". this is displayed at the top of the collection when display mode is display_mode_e_details.
			menu_element_text_c _element_text; // name is "column_text". this is displayed at the top of the collection when display mode is display_mode_e_details.

		public:
			column_c();

			string8_c const & get_key() const;

		};

	protected:
		// the elements in the _element_list are as follows:
		// base elements:
		//   _element_frame
		//   _element_highlighted_frame
		//   _element_last_selected_frame
		// supplemental elements:
		//   per column elements: one frame and one text:
		//     { column_frame, column_text }
		//   per item elements: one frame for selected state background, one frame for icon, [ display mode details: one text per column | display mode icons: one text ]:
		//     { item_selected_frame, item_icon_frame, item_text, ... }
		menu_element_frame_c _element_frame; // name is "frame", the background of this collection.
		menu_element_frame_c _element_last_selected_frame; // name is "last_selected_frame". is laid out and drawn behind the _last_selected_item.
		menu_element_frame_c _element_highlighted_frame; // name is "highlighted_frame". is laid out and drawn behind the _mouse_selected_item.
		menu_element_frame_c _element_border_frame; // name is "border_frame", is overlayed.

		item_i * _last_selected_item; // is set to the first item that was added to the collection, or the item that was last selected in the collection. this is the item that directional keyboard input will originate from. analogs: in modern windows this item would be outlined with a solid blue line, in legacy windows this item would be outlined with a dotted black line.
		item_i * _mouse_selected_item; // is set to the item that the mouse is currently over. analogs: in modern windows this item would be highlighted with a light blue background, in legacy windows i don't think there was an analog.

		menu_visibility_mode_e _vertical_scroll_bar_visibility_mode; // how to show or hide the vertical scroll bar.
		menu_control_scroll_bar_y_c * _vertical_scroll_bar; // name is "vertical_scroll_bar".
		void_c _handle_scroll_bar_on_value_changed( menu_control_scroll_bar_i * scroll );

		display_mode_e _display_mode; // how items are dispalyed and laid out in the container.

		sint32_c _icons_item_width; // when display mode is icons. width in pixels of each item.
		sint32_c _icons_item_height; // when display mode is icons. height in pixels of each item.
		sint32_c _icons_icon_height; // when display mode is icons. height of icon, should be less than _icons_item_height. what is left-over (of _icons_item_height - _icons_icon_height) will be the height of the item's text label.
		sint32_c _icons_item_spacing; // when display mode is icons. distance in pixels between collection edges and items, and between items and items.
		sint32_c _details_item_height; // when display mode is details. hieght in pixels of each item.

		core_list_c< column_c * > _column_list; // columns to display, when display mode is details then all columns are displayed, otherwise only the first column is used to look up the display name of each item.

		select_mode_e _select_mode; // determines how items can be selected in this collection.

		boolean_c _value_cache_is_dirty; // if true then item values need to be queried and cached.
		void_c _update_value_cache(); // caches items values based on current columns settings.

		string8_c _sort_key; // basically which column to sort by. if this can't be resolved then items will be sorted by "name", and if that can't be resolved then items will not be sorted.
		sint32_c _sort_index;
		sort_order_e _sort_order; // which direction to sort in.
		boolean_c _sort_is_dirty; // if true then items will be sorted on next call to refresh().
		void_c _update_sort(); // sorts items based on current sort settings.

		menu_frame_style_c _item_icon_frame_style; // override style applied to all icons in the collection.
		core_list_c< item_i * > _item_list; // all of the items in this collection, sorted by the _sort_key (and _sort_index) and _sort_order.
		core_list_c< item_i * > _selected_item_list; // all of the currently selected items in this collection, in the order that they were selected.
		core_list_c< menu_element_c * > _item_elements; // item elements allocated for the potentially visible set of items.
		boolean_c _item_layout_is_dirty; // if true then item elements need to be updated and laid out on next call to refresh().
		void_c _update_item_layout();

		//menu_control_collection_item_i * _editing_item; // will be set if the user is currently renaming an item.
		//string8_c _editing_item_property_key; // the property of the item that is being renamed, usually is the name but it can be something else.
		//menu_control_text_c * _editing_item_text_field; // laid out and drawn over item that is in the process of being renamed by the user. hidden otherwise.

		box32x2_c _get_item_box( sint32_c item_index );

		item_i * _pick_item_at_local_point( vector32x2_c const & local_point );

	protected:
		// handles item click selection.
		virtual void_c _on_clicked( input_event_c * input_event ) override;
		// handles item double click invocation.
		virtual void_c _on_multi_clicked( input_event_c * input_event ) override;
		// handles click and drag box selection.
		virtual void_c _on_input( input_event_c * input_event ) override;

		virtual void_c _update_transform_and_layout() override;

		menu_control_collection_c( string8_c const & name );

	public:
		virtual ~menu_control_collection_c() override;

		static menu_control_collection_c * make_new_instance( string8_c const & name ); // creates a new instance on the heap with reference count of 0.

		virtual void_c update_animations( float32_c time_step ) override;

		// call this after changing columns around or after adding or removing items.
		// queries and caches item property values if needed.
		// sorts items if needed.
		// updates scroll bar capacity.
		// updates scroll bar visibility.
		// lays out item elements.
		// recalculates _visible_items_columns_count, _visible_items_rows_count, and _visible_items_count.
		// recalculates _item_elements_list_length and reallocates _item_elements_frames and _item_elements_texts if needed.
		// this needs to be called each time _local_box is modified, or item metrics change, or scroll bar values change.
		void_c refresh();

		// only loads item layout metrics, not items content or columns or sort key or anything else.
		virtual void_c load_static_data_properties( data_scribe_markup_c::node_c const * node ) override;

		// gets display mode of collection.
		display_mode_e get_display_mode() const;
		// sets display mode of collection and refreshes.
		void_c set_display_mode( display_mode_e value );

		// sets metrics to use to lay out items when display mode is set to icons.
		void_c set_icons_metrics( sint32_c item_width, sint32_c item_height, sint32_c icon_height, sint32_c item_spacing );
		// sets metrics to use to lay out items when display mode is set to details.
		void_c set_details_metrics( sint32_c item_height );

		sint32_c get_icons_item_width() const;
		void_c set_icons_item_width( sint32_c value );

		sint32_c get_icons_item_height() const;
		void_c set_icons_item_height( sint32_c value );

		sint32_c get_icons_icon_height() const;
		void_c set_icons_icon_height( sint32_c value );

		sint32_c get_icons_item_spacing() const;
		void_c set_icons_item_spacing( sint32_c value );

		sint32_c get_details_item_height() const;
		void_c set_details_item_height( sint32_c value );

		// gets current sort column key.
		string8_c const & get_sort_key() const;
		// gets current sort order. false is low to high (ascending), true is high to low (descending).
		sort_order_e get_sort_order() const;
		// sets sort column key then sorts the items then refreshes.
		void_c set_sort( string8_c const & sort_key, sort_order_e sort_order );

		// removes and deletes all columns after the first (which is built in "name").
		// adding support for adding/removing columns during run time is not too difficult to do but i don't forsee a need for it at this time, so i'm being lazy.
		// don't forget to call refresh() after you are done changing columns and items.
		void_c remove_all_columns();
		// use this function to add columns to the collection.
		// a collection must contain at least one column in order to display items.
		// when a collection is first created, it does not contain any columns so it won't display any items.
		// key is the unique key of the column/property.
		// display_value is a localized string reference key, which references the name of the column to display to the user.
		// is_editable is true then the user can press F2 or click on the same item twice to edit its value.
		// don't forget to call refresh() after you are done changing columns and items.
		void_c add_column( string8_c const & key, string16_c const & display_value, sint32_c width, sort_by_e sort_by, boolean_c is_editable );

		// gets number of items in this collection.
		sint32_c get_item_count();
		// gets an item in this collection, after sorting has been applied.
		item_i * get_item_at_index( sint32_c item_index );

		// gives an item to this collection.
		// give, because this collection becomes responsible for managing the memory of the item and for deleting it when its time comes.
		// an item can be added to at most one collection at a time.
		// don't forget to call refresh() after you are done changing columns and items.
		void_c give_item( item_i * item );
		// removes and deletes an item from the collection.
		// don't forget to call refresh() after you are done changing columns and items.
		void_c remove_and_delete_item( sint32_c item_index );
		// don't forget to call refresh() after you are done changing columns and items.
		void_c remove_and_delete_all_items();

		// gets all of the currently selected items, they will be ordered by the order that they were selected in.
		core_list_c< item_i * > const & get_selected_items() const;
		// gets just the most recent currently selected item, or nullptr if there isn't one.
		item_i * get_last_selected_item() const;
		// clears all selected items.
		void_c clear_selected_items();

	public:
		core_event_c< void_c, menu_control_collection_c * > on_selected_items_changed; // occurs when the set of selected items changes for any reason.
		core_event_c< void_c, menu_control_collection_c * > on_selected_items_invoked; // occurs when one or more items are invoked with the enter key or when a single item is double clicked.

	};

}
