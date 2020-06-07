#pragma once

#include "cheonsa_database_types.h"
#include "cheonsa_reflection_types.h"
#include "cheonsa_menu_control.h"
#include "cheonsa_menu_control_panel_i.h"
#include "cheonsa_menu_control_button.h"
#include "cheonsa_menu_control_combo.h"
#include "cheonsa_menu_control_list.h"
#include "cheonsa_menu_control_scroll_bar_x.h"
#include "cheonsa_menu_control_scroll_bar_y.h"
#include "cheonsa_menu_control_scrub_bar_x.h"
#include "cheonsa_menu_control_label.h"
#include "cheonsa_menu_control_text.h"
#include "cheonsa_menu_control_window_color_picker.h"
#include "cheonsa_menu_control_window_file_picker.h"
#include "cheonsa_menu_control_window_message.h"
#include "cheonsa_menu_control_window_text_editor.h"

#include "cheonsa_core_event.h"

namespace cheonsa
{

	// provides an interface to edit data and properties of a reflection_object_c or a database_record_c.
	// adds controls to itself to reflect the underlying bound data and to enable the user to edit it.
	class menu_control_property_inspector_c
		: public menu_control_panel_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "property_inspector"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		// keeps track of a property and the menu controls to edit that property.
		class property_field_c
		{
			friend class menu_control_property_inspector_c;

		private:
			reflection_property_c const * _reflection_property; // the reflection property associated with this field. this will not be set for the class label.
			//database_field_schema_c const * bound_database_field_schema;

			menu_control_text_c * _label; // class name, category name, or property name.

			menu_control_text_c * _text; // text box for editing string values. also used with sliders to display and edit the value of the slider.
			menu_control_combo_c * _combo; // combo box for editing enumerated values.
			menu_control_scroll_bar_i * _scroll; // might be a horizontal scroll bar or a horizontal scrub bar, for editing number values that are constrained to a certain range.
			menu_control_button_c * _button; // optional edit button that appears to the right of text box, for values that can be edited by dialog windows.

			menu_control_list_c * _item_list;
			menu_control_button_c * _item_add;
			menu_control_button_c * _item_remove;
			menu_control_button_c * _item_move_up;
			menu_control_button_c * _item_move_down;
			menu_control_button_c * _item_sort;

		public:
			property_field_c();
			~property_field_c();

		};

		struct
		{
			float32_c height_for_class_row; // taken from style map property "height_for_class_row", height of class label row.
			float32_c height_for_class_row_padding; // taken from style map property "height_for_class_row_padding", extra space to place below class label row.
			float32_c height_for_category_row; // taken from style map property "height_for_category_row", height of category label row.
			float32_c height_for_category_row_padding; // taken from style map property "height_for_category_row_padding", extra space to insert below category label row.
			float32_c height_for_property_row; // taken from style map property "height_for_property_row", height of property label and field, scroll(s), etc.
			float32_c height_for_property_row_padding; // taken from style map property "height_for_property_row_padding, extra space to insert below property row.
			float32_c height_for_property_list; // taken from style map property "height_for_property_list", height of list type property list box.
			float32_c width_for_property_control; // taken from style map property "width_for_property_control", width of scroll or column of buttons that sit to the right of a property field.
			float32_c width_for_property_control_padding; // taken from style map property "width_for_property_control_padding", extra space to insert before property control column.
		} styled_properties;

		menu_control_property_inspector_c * _mother_property_inspector; // if set then this property inspector is owned and managed by this property inspector.
		reflection_class_c const * _fixed_reflection_class; // if this is set, then the property editing interface will remain static, even if _reflection_object is set to nullptr, but this also means that _reflection_object type must correspond to _fixed_reflection_class.

		reflection_object_c * _reflection_object; // the object that we are reflecting and editing.
		//database_record_c * _bound_database_record; // the database record that we are reflecting and editing.

		core_list_c< property_field_c * > _property_field_list; // a list of the properties that we are reflecting.

		property_field_c * _editing_property_field; // if an additional dialog window is open (like a color picker), then this is a pointer to the property that that dialog window is editing.
		vector64x4_c _editing_value_as_color_original; // if the user cancels the color pick opreation then we need to revert to this value.
		sint32_c _pending_delete_list_item_index; // the index of the list item that we are in the process of asking the user if they are sure they want to delete it.

		menu_control_window_message_c * _message_dialog;
		menu_control_window_color_picker_c * _color_picker_dialog;
		menu_control_window_file_picker_c * _file_picker_dialog;
		menu_control_window_text_editor_c * _text_editor_dialog;

		menu_control_window_c * _property_inspector_window; // will be initialized at the moment that this property inspector needs to open up another property inspector in order to edit nestable types (data_type_e_object or data_type_e_object_list type properties).
		menu_control_property_inspector_c * _property_inspector; // is added to _property_inspector_window.
		menu_control_button_c * _property_inspector_cancel_button;
		menu_control_button_c * _property_inspector_okay_button;
		void_c _initialize_property_inspector(); // creates the window (in hidden state) and property inspector. adds it to the user interface if able.
		void_c _handle_property_inspector_button_on_clicked( menu_event_information_c event_information );

		float32_c _y; // current y offset of the cursor, when appending property menu controls (rows) with _add_row().
		void_c _layout_controls_for_property( property_field_c * property_field ); // lays out the controls for the given property field.

		boolean_c _is_muted; // if true then all the _handle_* functions will ignore events from menu controls, because the values of those controls are being updated to sync and match with the data. if false then the opposite is happening, the data is being updated to sync and match with the menu controls.
		void_c _clear(); // removes and deletes all controls from the property inspector. hides any open dialogs.
		void_c _add( reflection_class_c const * reflection_class ); // adds property field editor controls for the given reflection class.
		//void_c _add( database_record_schema_c const * database_record_schema ); // adds property field editor controls for the given database record schema.

		// updates controls to reflect the current value of the property of the bound object.
		void_c _update_ui_from_property( property_field_c * property_field );

		// control is any one of the controls in the property field, or one of the shared data editors (_color_picker, _file_picker, _text_editor).
		// we will pull the value from that control, apply it to the bound object, and apply it to any other related controls.
		// only invokes on_property_value_changed if and_commit is true, which allows value changes to be previewed without committing them to the undo stack.
		void_c _update_property_from_ui( property_field_c * property_field, menu_control_c * control, boolean_c and_commit );

		//string16_c _original_text_value;
		//void _handle_value_text_on_character_focus_gained( menu_control_c * control );
		//void _handle_value_text_on_character_focus_lost( menu_control_c * control );
		void_c _handle_value_text_on_value_changed_preview( menu_control_text_c * text );
		void_c _handle_value_text_on_value_changed( menu_control_text_c * text );
		void_c _handle_value_scroll_on_value_changed_preview( menu_control_scroll_bar_i * scroll );
		void_c _handle_value_scroll_on_value_changed( menu_control_scroll_bar_i * scroll );
		void_c _handle_value_combo_on_selected_item_changed_preview( menu_control_combo_list_c * combo_list ); // is called as user mouses over options in combo list.
		void_c _handle_value_combo_on_selected_item_changed( menu_control_combo_list_c * combo_list ); // is called when user clicks on an option in combo list.
		void_c _handle_value_edit_on_clicked( menu_event_information_c event_information );
		void_c _handle_color_picker_on_value_changed( menu_control_window_color_picker_c * color_picker_panel );
		void_c _handle_item_list_on_selected_item_changed( menu_control_list_c * list );
		void_c _handle_item_add_on_clicked( menu_event_information_c event_information );
		void_c _handle_item_remove_on_clicked( menu_event_information_c event_information );
		void_c _handle_item_move_up_on_clicked( menu_event_information_c event_information );
		void_c _handle_item_move_down_on_clicked( menu_event_information_c event_information );
		void_c _handle_item_sort_on_clicked( menu_event_information_c event_information );
		void_c _handle_dialog_on_result( menu_control_window_c * window );

	public:
		menu_control_property_inspector_c( menu_control_property_inspector_c * mother_property_inspector, reflection_class_c const * fixed_reflection_class ); // initializes a child property panel for editing nested properties such as for editing a class instance that is a part of a core_list_c or by itself.
		virtual ~menu_control_property_inspector_c() override;

		void_c bind_reflection_object( reflection_object_c * value ); // sets the object that is to be reflected by this property inspector.
		//void_c bind_database_record( database_record_c * value ); // sets the database record to be edited by this property inspector. unsubscribes the property inspector from the database events.

		void_c refresh_ui(); // refreshes the values displayed in the user interface to be up to date with the current state of the reflection object.

		float32_c get_content_height() const;

		core_event_c< void_c, reflection_property_value_changed_information_c * > on_property_value_changed; // is invoked whenever a property value is modified and committed by this property inspector interface. an undo/redo system can plug into this to record changes.

	};

}
