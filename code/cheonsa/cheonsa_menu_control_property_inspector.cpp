#include "cheonsa_menu_control_property_inspector.h"
#include "cheonsa_user_interface.h"
#include "cheonsa_reflection_enumeration.h"
#include "cheonsa_reflection_class.h"
#include "cheonsa_reflection_property.h"
#include "cheonsa_reflection_object.h"
#include "cheonsa_database_record.h"
#include "cheonsa_database_table.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	menu_control_property_inspector_c::property_field_c * menu_control_property_inspector_c::_editing_property_field = nullptr;
	vector64x4_c menu_control_property_inspector_c::_editing_value_as_color_original = vector64x4_c();
	sint32_c menu_control_property_inspector_c::_pending_delete_list_item_index = -1;
	menu_window_dialog_c * menu_control_property_inspector_c::_message_dialog = nullptr;
	menu_control_label_c * menu_control_property_inspector_c::_message = nullptr;
	menu_window_dialog_c * menu_control_property_inspector_c::_color_picker_dialog = nullptr;
	menu_control_color_picker_c * menu_control_property_inspector_c::_color_picker = nullptr;
	menu_window_dialog_c * menu_control_property_inspector_c::_file_picker_dialog = nullptr;
	menu_control_file_picker_c * menu_control_property_inspector_c::_file_picker = nullptr;
	menu_window_dialog_c * menu_control_property_inspector_c::_text_editor_dialog = nullptr;
	menu_control_text_c * menu_control_property_inspector_c::_text_editor = nullptr;

	menu_control_property_inspector_c::property_field_c::property_field_c()
		: bound_reflection_property( nullptr )
		, label( nullptr )
		, text( nullptr )
		, combo( nullptr )
		, scroll( nullptr )
		, button( nullptr )
		, item_list( nullptr )
		, item_add( nullptr )
		, item_remove( nullptr )
		, item_move_up( nullptr )
		, item_move_down( nullptr )
		, item_sort( nullptr )
		, property_inspector( nullptr )
	{
	}

	menu_control_property_inspector_c::property_field_c::~property_field_c()
	{
		if ( label )
		{
			delete label;
			label = nullptr;
		}

		if ( text != nullptr )
		{
			delete text;
			text= nullptr;
		}

		if ( combo != nullptr )
		{
			delete combo;
			combo = nullptr;
		}

		if ( scroll != nullptr )
		{
			delete scroll;
			scroll = nullptr;
		}

		if ( button != nullptr )
		{
			delete button;
			button = nullptr;
		}

		if ( item_list )
		{
			delete item_list;
			item_list = nullptr;
			delete item_add;
			item_add = nullptr;
			delete item_remove;
			item_remove = nullptr;
			delete item_move_up;
			item_move_up = nullptr;
			delete item_move_down;
			item_move_down = nullptr;
			delete item_sort;
			item_sort = nullptr;
		}

		if ( property_inspector )
		{
			delete property_inspector;
			property_inspector = nullptr;
		}
	}

	void_c menu_control_property_inspector_c::_layout_controls_for_property( property_field_c * property_field )
	{
		assert( property_field->label );
		if ( property_field->bound_reflection_property )
		{
			if ( property_field->bound_reflection_property->_type != data_type_e_category_label )
			{
				// property label.
				property_field->label->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( 0.0f, _y, 0.0f, styled_properties.height_for_property_row ) );
				_y += styled_properties.height_for_property_row + styled_properties.height_for_property_row_padding;

				float32_c left_indent = 0.0f;
				if ( property_field->item_list == nullptr )
				{
					// property edit button.
					float32_c right = 0.0f;
					if ( property_field->button )
					{
						right = styled_properties.width_for_property_control + styled_properties.width_for_property_control_padding;
						property_field->button->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right, box32x2_c( styled_properties.width_for_property_control, 0.0f, 0.0f, styled_properties.height_for_property_row ) );
					}

					// property editor(s).
					if ( property_field->text )
					{
						property_field->text->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( left_indent, _y, right, styled_properties.height_for_property_row ) );
						_y += styled_properties.height_for_property_row + styled_properties.height_for_property_row_padding;
					}
					if ( property_field->combo )
					{
						property_field->combo->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( left_indent, _y, right, styled_properties.height_for_property_row ) );
						_y += styled_properties.height_for_property_row + styled_properties.height_for_property_row_padding;
					}
					if ( property_field->scroll )
					{
						property_field->scroll->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( left_indent, _y, right, styled_properties.height_for_property_row ) );
						_y += styled_properties.height_for_property_row + styled_properties.height_for_property_row_padding;
					}
				}
				
				if ( property_field->item_list )
				{
					assert( property_field->item_list );
					assert( property_field->item_add );
					assert( property_field->item_remove );
					assert( property_field->item_move_up );
					assert( property_field->item_move_down );
					assert( property_field->item_sort );

					float32_c right = styled_properties.width_for_property_control + styled_properties.width_for_property_control_padding;

					property_field->item_list->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( left_indent, _y, right, styled_properties.height_for_property_list ) );

					float32_c y = _y;
					property_field->item_add->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right, box32x2_c( styled_properties.width_for_property_control, y, 0.0f, styled_properties.height_for_property_row ) );
					y += styled_properties.height_for_property_row + styled_properties.height_for_property_row_padding;

					property_field->item_remove->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right, box32x2_c( styled_properties.width_for_property_control, y, 0.0f, styled_properties.height_for_property_row ) );
					y += styled_properties.height_for_property_row + styled_properties.height_for_property_row_padding;

					property_field->item_move_up->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right, box32x2_c( styled_properties.width_for_property_control, y, 0.0f, styled_properties.height_for_property_row ) );
					y += styled_properties.height_for_property_row + styled_properties.height_for_property_row_padding;

					property_field->item_move_down->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right, box32x2_c( styled_properties.width_for_property_control, y, 0.0f, styled_properties.height_for_property_row ) );
					y += styled_properties.height_for_property_row + styled_properties.height_for_property_row_padding;

					property_field->item_sort->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right, box32x2_c( styled_properties.width_for_property_control, y, 0.0f, styled_properties.height_for_property_row ) );
					y += styled_properties.height_for_property_row + styled_properties.height_for_property_row_padding;

					_y = ops::math_maximum( y, _y + styled_properties.height_for_property_list + styled_properties.height_for_property_row_padding);
				}

				if ( property_field->property_inspector )
				{
					property_field->property_inspector->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( 0.0f, _y, 0.0f, property_field->property_inspector->get_content_height() ) );
					_y += property_field->property_inspector->get_local_box().get_height();
				}
			}
			else
			{
				// category label.
				assert( property_field->label );
				property_field->label->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( 0.0f, _y, 0.0f, styled_properties.height_for_category_row ) );
				_y += styled_properties.height_for_category_row + styled_properties.height_for_category_row_padding;
			}
		}
		else
		{
			// class label.
			assert( property_field->label );
			property_field->label->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( 0.0f, _y, 0.0f, styled_properties.height_for_class_row ) );
			_y += styled_properties.height_for_class_row + styled_properties.height_for_class_row_padding;
		}
	}

	void_c menu_control_property_inspector_c::_clear()
	{
		// cancel any currently open editing operations and dialogs.
		_editing_property_field = nullptr;
		_editing_value_as_color_original = vector64x4_c( 0.0f, 0.0f, 0.0f, 0.0f );
		_pending_delete_list_item_index = -1;
		_message_dialog->hide();
		_color_picker_dialog->hide();
		_file_picker_dialog->hide();
		_text_editor_dialog->hide();

		// [remove and delete] or [disable] all property fields and controls.
		if ( _fixed_reflection_class == nullptr )
		{
			_remove_and_delete_all_controls();
			_property_field_list.remove_and_delete_all();
		}
		else
		{
			for ( sint32_c i = 0; i < _property_field_list.get_length(); i++ )
			{
				property_field_c * property_field = _property_field_list[ i ];
				_update_ui_from_property( property_field );
			}
		}
		
		_y = 0.0f;
		
	}

	void_c menu_control_property_inspector_c::_add( reflection_class_c const * reflection_class )
	{
		assert( _mute == false );
		_mute = true; // just in case the controls invoke events we should ignore them at this point, even though i don't think they would invoke events.

		//property_field_c * property_field = new property_field_c();
		//_property_field_list.insert_at_end( property_field );
		//property_field->label = new menu_control_text_c();
		//property_field->label->set_name( string8_c( mode_e_static, "class_label" ) );
		//property_field->label->set_user_pointer( property_field );
		//property_field->label->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( 0.0f, _y, 0.0f, styled_properties.height_for_class_row ) );
		//_add_private_control( property_field->label );
		//_layout_controls( property_field );

		//property_field->label->set_style_map_key( styled_properties.style_map_key_for_class_label );
		//_y += styled_properties.height_for_class_row + styled_properties.height_for_class_row_padding;

		for ( sint32_c i = 0; i < reflection_class->get_property_count(); i++ )
		{
			property_field_c * property_field = new property_field_c();
			reflection_property_c const * property = property_field->bound_reflection_property;
			_property_field_list.insert_at_end( property_field );
			property = reflection_class->get_property( i );
			property_field->label = new menu_control_text_c();
			property_field->label->set_user_pointer( property_field );
			if ( property->_type == data_type_e_category_label )
			{
				property_field->label->set_name( string8_c( mode_e_static, "category_label" ) );
				property_field->label->set_plain_text( string16_c( property->_name ) );
				_add_control( property_field->label );
			}
			else
			{
				property_field->label->set_name( string8_c( mode_e_static, "property_label" ) );
				property_field->label->set_plain_text( string16_c( property->_name ) );
				_add_control( property_field->label );

				// create text control if needed.
				if ( property->_type >= data_type_e_string8 && property->_type <= data_type_e_float64 )
				{
					property_field->text = new menu_control_text_c();
					property_field->text->set_user_pointer( property_field );
					property_field->text->set_name( string8_c( mode_e_static, "property_value_text" ) );
					if ( property->_view == data_view_e_text )
					{
						property_field->text->set_text_edit_mode( menu_text_edit_mode_e_static );
					}
					_add_control( property_field->text );
				}

				// create button control if needed.
				if ( property->_view == data_view_e_color || property->_view == data_view_e_text || property->_view == data_view_e_file_path || property->_view == data_view_e_folder_path )
				{
					property_field->button = new menu_control_button_c();
					property_field->button->set_user_pointer( property_field );
					property_field->button->set_name( string8_c( mode_e_static, "property_value_button" ) );
					property_field->button->set_plain_text( string16_c( mode_e_static, L"..." ) );
					property_field->button->on_clicked.subscribe( this, &menu_control_property_inspector_c::_handle_value_edit_on_click );
					_add_control( property_field->button );
				}

				// create scroll control if needed.
				if ( property->_view == data_view_e_scroll_bar || property->_view == data_view_e_scrub_bar )
				{
					assert( property->_type == data_type_e_float32 || property->_type == data_type_e_float64 );
					assert( property->_type_count == 1 );
					float64_c value_minimum = 0.0;
					float64_c value_maximum = 0.0;
					if ( property->_type == data_type_e_float32 )
					{
						value_minimum = property->_defaults_and_limits._float32_minimum;
						value_maximum = property->_defaults_and_limits._float32_maximum;
					}
					else //if ( property->_type == data_type_e_float64 )
					{
						value_minimum = property->_defaults_and_limits._float64_minimum;
						value_maximum = property->_defaults_and_limits._float64_maximum;
					}
					assert( value_minimum < value_maximum );
					if ( property->_view == data_view_e_scroll_bar )
					{
						property_field->scroll = new menu_control_scroll_bar_horizontal_c();
					}
					else
					{
						property_field->scroll = new menu_control_scrub_bar_horizontal_c();
					}
					property_field->scroll->set_user_pointer( property_field );
					property_field->scroll->set_name( string8_c( mode_e_static, "property_value_scroll" ) );
					property_field->scroll->set_value_minimum( value_minimum );
					property_field->scroll->set_value_maximum( value_maximum );
					property_field->scroll->on_value_changed_preview.subscribe( this, &menu_control_property_inspector_c::_handle_value_scroll_on_value_changed_preview );
					property_field->scroll->on_value_changed_commit.subscribe( this, &menu_control_property_inspector_c::_handle_value_scroll_on_value_changed_commit );
					_add_control( property_field->scroll );
				}

				// create combo control if needed.
				if ( property->_view == data_view_e_enumeration )
				{
					reflection_enumeration_c const * enumeration = property->_enumeration;
					assert( enumeration );
					property_field->combo = new menu_control_combo_c();
					property_field->combo->set_name( string8_c( mode_e_static, "property_value_combo" ) );
					//property_field->combo->get_combo_list()->on_selection_changed_preview.subscribe( this, &menu_control_property_inspector_c::_handle_value_combo_on_selection_changed_preview );
					property_field->combo->get_combo_list()->on_selection_changed_commit.subscribe( this, &menu_control_property_inspector_c::_handle_value_combo_on_selection_changed_commit );
					for ( sint32_c j = 0; j < enumeration->get_value_count(); j++ ) // populate combo control.
					{
						menu_control_combo_list_item_c * combo_list_item = new menu_control_combo_list_item_c();
						combo_list_item->set_text_plain( string16_c( enumeration->get_value( j )->get_name() ) );
						property_field->combo->get_combo_list()->add_item( combo_list_item );
					}
					_add_control( property_field->combo );
				}

				// create property inspector if needed.
				if ( property->_type == data_type_e_object )
				{
					property_field->property_inspector = new menu_control_property_inspector_c( this, property->_class );
					property_field->property_inspector->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( 0.0f, _y, 0.0f, property_field->property_inspector->get_local_box().get_height() ) );
					_add_control( property_field->property_inspector );
				}

				// create list and button controls if needed.
				if ( property->_type == data_type_e_object_list )
				{
					property_field->property_inspector = new menu_control_property_inspector_c( this, property->_class );

					property_field->item_list = new menu_control_list_c();
					property_field->item_list->set_user_pointer( property_field );
					property_field->item_list->on_selection_changed.subscribe( this, &menu_control_property_inspector_c::_handle_item_selected_index_changed );
					_add_control( property_field->item_list );

					property_field->item_add = new menu_control_button_c();
					property_field->item_add->set_user_pointer( property_field );
					property_field->item_add->set_plain_text( string16_c( mode_e_static, L"+" ) );
					property_field->item_add->on_clicked.subscribe( this, &menu_control_property_inspector_c::_handle_item_add_on_click );
					_add_control( property_field->item_add );

					property_field->item_remove = new menu_control_button_c();
					property_field->item_remove->set_user_pointer( property_field );
					property_field->item_remove->set_plain_text( string16_c( mode_e_static, L"-" ) );
					property_field->item_remove->on_clicked.subscribe( this, &menu_control_property_inspector_c::_handle_item_remove_on_click );
					_add_control( property_field->item_remove );

					property_field->item_move_up = new menu_control_button_c();
					property_field->item_move_up->set_user_pointer( property_field );
					property_field->item_move_up->set_plain_text( string16_c( mode_e_static, L"u" ) );
					property_field->item_move_up->on_clicked.subscribe( this, &menu_control_property_inspector_c::_handle_item_move_up_on_click );
					_add_control( property_field->item_move_up );

					property_field->item_move_down = new menu_control_button_c();
					property_field->item_move_down->set_user_pointer( property_field );
					property_field->item_move_down->set_plain_text( string16_c( mode_e_static, L"d" ) );
					property_field->item_move_down->on_clicked.subscribe( this, &menu_control_property_inspector_c::_handle_item_move_down_on_click );
					_add_control( property_field->item_move_down );

					property_field->item_sort = new menu_control_button_c();
					property_field->item_sort->set_user_pointer( property_field );
					property_field->item_sort->set_plain_text( string16_c( mode_e_static, L"s" ) );
					property_field->item_sort->on_clicked.subscribe( this, &menu_control_property_inspector_c::_handle_item_sort_on_click );
					_add_control( property_field->item_sort );
				}
			}
			_layout_controls_for_property( property_field );
		}

		_mute = false;
	}

	void_c menu_control_property_inspector_c::_update_ui_from_property( property_field_c * property_field )
	{
		if ( _bound_reflection_object == nullptr )
		{
			// it should only be possible to reach here if _fixed_reflection_class is set.
			assert( _fixed_reflection_class != nullptr );
			if ( property_field->label )
			{
				property_field->label->disable();
			}
			if ( property_field->text )
			{
				property_field->text->disable();
				property_field->text->set_plain_text( string16_c() );
			}
			if ( property_field->button )
			{
				property_field->button->disable();
			}
			if ( property_field->combo )
			{
				property_field->combo->disable();
				property_field->combo->get_combo_list()->set_selected_item_index( -1 );
			}
			if ( property_field->scroll )
			{
				property_field->scroll->disable();
				property_field->scroll->set_value( 0.0 );
			}
			if ( property_field->item_list )
			{
				property_field->item_list->remove_and_delete_all_items();
				property_field->item_list->disable();
				property_field->item_add->disable();
				property_field->item_remove->disable();
				property_field->item_move_up->disable();
				property_field->item_move_down->disable();
				property_field->item_sort->disable();
			}
			if ( property_field->property_inspector )
			{
				property_field->property_inspector->unbind_data();
				property_field->property_inspector->disable();
			}
			return;
		}

		_mute = true;

		// get raw value.
		reflection_property_c const * property = property_field->bound_reflection_property;
		reflection_value_container_c value;
		if ( !reflection_get_object_property_value( _bound_reflection_object, property, value ) )
		{
			return;
		}

		// convert value to string if needed.
		if ( property_field->text )
		{
			string16_c value_as_string16;
			value_as_string16 = reflection_convert_value_to_string16( property, value );
			property_field->text->set_plain_text( value_as_string16 );
		}

		// provide value to view editors.
		if ( property_field->scroll )
		{
			assert( property->_view == data_view_e_scroll_bar || property->_view == data_view_e_scrub_bar );
			assert( property->_type_count == 1 );
			if ( property->_type == data_type_e_float32 )
			{
				property_field->scroll->set_value( static_cast< float64_c >( value.float32[ 0 ] ) );
			}
			else if ( property->_type == data_type_e_float64 )
			{
				property_field->scroll->set_value( value.float64[ 0 ] );
			}
			else
			{
				assert( false );
			}
		}
		else if ( property_field->combo )
		{
			assert( property->_view == data_view_e_enumeration );
			assert( property->_type_count == 1 );
			assert( property->_enumeration );
			uint32_c enum_value = 0;
			if ( property->_type == data_type_e_uint8 )
			{
				enum_value = static_cast< uint8_c >( value.uint8[ 0 ] );
			}
			else if ( property->_type == data_type_e_uint16 )
			{
				enum_value = static_cast< uint16_c >( value.uint16[ 0 ] );
			}
			else if ( property->_type == data_type_e_uint32 )
			{
				enum_value = value.uint32[ 0 ];
			}
			else
			{
				assert( false );
			}
			sint32_c enum_index = -1;
			property->_enumeration->find_index_with_value( enum_index, enum_value ); // if this returns false then enum_index will be set to -1, which is still valid to set no selection in the combo list.
			property_field->combo->get_combo_list()->set_selected_item_index( enum_index );
		}
		else if ( property->_type == data_type_e_object )
		{
			assert( property_field->property_inspector );
			reflection_object_c * object = property->_accessors._object_getter( _bound_reflection_object );
			property_field->property_inspector->bind_reflection_object( object );
		}
		else if ( property->_type == data_type_e_object_list )	
		{
			assert( property_field->item_list );
			assert( property_field->property_inspector );

			property_field->item_list->remove_and_delete_all_items();
			sint32_c item_count = property->_accessors._object_list_item_count_getter( _bound_reflection_object );
			for ( sint32_c i = 0; i < item_count; i++ )
			{
				reflection_object_c * object = property->_accessors._object_list_item_getter( _bound_reflection_object, i );
				menu_control_list_item_c * list_item = new menu_control_list_item_c();
				list_item->set_text_plain( object->get_reflection_display_name() );
				property_field->item_list->add_item( list_item );
			}

			if ( property_field->item_add && property_field->item_remove )
			{
				if ( property->_accessors._object_list_item_adder && property->_accessors._object_list_item_remover )
				{
					property_field->item_add->enable();
					property_field->item_remove->enable();
				}
				else
				{
					property_field->item_add->disable();
					property_field->item_remove->disable();
				}
			}

			if ( property_field->item_move_up && property_field->item_move_down )
			{
				if ( property->_accessors._object_list_item_mover )
				{
					property_field->item_move_up->enable();
					property_field->item_move_down->enable();
				}
				else
				{
					property_field->item_move_up->disable();
					property_field->item_move_down->disable();
				}
			}

			if ( property_field->item_sort )
			{
				if ( property->_accessors._object_list_item_sorter )
				{
					property_field->item_sort->enable();
				}
				else
				{
					property_field->item_sort->disable();
				}
			}
		}

		_mute = false;
	}

	void_c menu_control_property_inspector_c::_update_property_from_ui( property_field_c * property_field, menu_control_c * control, boolean_c and_commit )
	{
		assert( property_field && control );

		if ( _bound_reflection_object == nullptr )
		{
			// because sometimes this can get called when one of our text boxes looses keyboard focus after the selection is cleared.
			return;
		}

		if ( property_field->text == nullptr )
		{
			return;
		}

		reflection_property_c const * property = property_field->bound_reflection_property;

		reflection_value_container_c old_value; // old or current value.
		reflection_get_object_property_value( _bound_reflection_object, property, old_value );

		reflection_value_container_c new_value; // new value.
		if ( property_field->scroll && control == property_field->scroll )
		{
		}
		else if ( property_field->combo && control == property_field->combo->get_combo_list() )
		{
			assert( property->_view == data_view_e_enumeration );
			assert( property->_type_count == 1 );
			assert( property->_enumeration );
			uint32_c enum_value = 0;
			assert( property->_enumeration->find_value_with_index( enum_value, property_field->combo->get_combo_list()->get_selected_item_index() ) );
			if ( property->_type == data_type_e_uint8 )
			{
				assert( enum_value < static_cast< uint32_c >( constants< uint8_c >::maximum() ) );
				new_value.uint8[ 0 ] = static_cast< uint8_c >( enum_value );
			}
			else if ( property->_type == data_type_e_uint16 )
			{
				assert( enum_value < static_cast< uint32_c >( constants< uint16_c >::maximum() ) );
				new_value.uint16[ 0 ] = static_cast< uint16_c >( enum_value );
			}
			else if ( property->_type == data_type_e_uint32 )
			{
				assert( enum_value < static_cast< uint32_c >( constants< uint32_c >::maximum() ) );
				new_value.uint32[ 0 ] = enum_value;
			}
			else
			{
				assert( false );
			}
		}
		else if ( property_field->text && control == property_field->text )
		{
			string16_c new_value_as_string16 = property_field->text->get_plain_text();
			reflection_convert_string16_to_value( property_field->bound_reflection_property, new_value, new_value_as_string16 );
		}
		else if ( control == _color_picker )
		{
			vector64x4_c value_as_color = _color_picker->get_rgba();
			reflection_convert_color_to_value( property_field->bound_reflection_property, new_value, value_as_color );
		}
		else if ( control == _file_picker )
		{
			new_value.string16 = _file_picker->get_folder_path();
			new_value.string16 += _file_picker->get_file_name();
			if ( property_field->bound_reflection_property->_type == data_type_e_string8 )
			{
				new_value.string8 = new_value.string16;
			}
		}
		else if ( control == _text_editor )
		{
			new_value.string16 = _text_editor->get_plain_text();
			if ( property_field->bound_reflection_property->_type == data_type_e_string8 )
			{
				new_value.string8 = new_value.string16;
			}
			reflection_set_object_property_value( _bound_reflection_object, property_field->bound_reflection_property, new_value );
		}
		else
		{
			assert( false );
		}

		if ( reflection_set_object_property_value( _bound_reflection_object, property, new_value ) )
		{
			if ( and_commit == true )
			{
				if ( !reflection_compare_values( property, old_value, new_value ) )
				{
					property_value_changed_info_c property_value_changed_info;
					property_value_changed_info.reflection_object = _bound_reflection_object;
					property_value_changed_info.property_field = property_field;
					property_value_changed_info.old_value = old_value;
					property_value_changed_info.new_value = new_value;
					menu_control_property_inspector_c * handler = this;
					while ( handler->_mother_property_inspector != nullptr ) // bubble to the top level.
					{
						handler = _mother_property_inspector;
					}
					handler->on_property_value_changed.invoke( &property_value_changed_info );
				}
				refresh_ui();
			}
		}
		else
		{
			_update_ui_from_property( property_field );
		}
	}

	void_c menu_control_property_inspector_c::_handle_value_text_on_value_changed_preview( menu_control_text_c * text )
	{
		if ( !_mute )
		{
			_update_property_from_ui( static_cast< property_field_c * >( text->get_user_pointer() ), text, false );
		}
	}

	void_c menu_control_property_inspector_c::_handle_value_text_on_value_changed_commit( menu_control_text_c * text )
	{
		if ( !_mute )
		{
			_update_property_from_ui( static_cast< property_field_c * >( text->get_user_pointer() ), text, true );
		}
	}

	void_c menu_control_property_inspector_c::_handle_value_scroll_on_value_changed_preview( menu_control_scroll_i * scroll )
	{
		if ( !_mute )
		{
			_update_property_from_ui( static_cast< property_field_c * >( scroll->get_user_pointer() ), scroll, false );
		}
	}

	void_c menu_control_property_inspector_c::_handle_value_scroll_on_value_changed_commit( menu_control_scroll_i * scroll )
	{
		if ( !_mute )
		{
			_update_property_from_ui( static_cast< property_field_c * >( scroll->get_user_pointer() ), scroll, true );
		}
	}

	void_c menu_control_property_inspector_c::_handle_value_combo_on_selection_changed_preview( menu_control_combo_list_c * combo_list )
	{
		if ( !_mute )
		{
			_update_property_from_ui( static_cast< property_field_c * >( combo_list->get_user_pointer() ), combo_list, false );
		}
	}
	
	void_c menu_control_property_inspector_c::_handle_value_combo_on_selection_changed_commit( menu_control_combo_list_c * combo_list )
	{
		if ( !_mute )
		{
			_update_property_from_ui( static_cast< property_field_c * >( combo_list->get_user_pointer() ), combo_list, true );
		}
	}

	void_c menu_control_property_inspector_c::_handle_value_edit_on_click( menu_event_info_c event_info )
	{
		if ( _editing_property_field != nullptr )
		{
			return;
		}

		_editing_property_field = reinterpret_cast< property_field_c * >( event_info.control->get_user_pointer() );

		reflection_value_container_c value;
		reflection_get_object_property_value( _bound_reflection_object, _editing_property_field->bound_reflection_property, value );

		if ( _editing_property_field->bound_reflection_property->_view == data_view_e_color )
		{
			_editing_value_as_color_original = reflection_convert_value_to_color( _editing_property_field->bound_reflection_property, value );
			if ( _editing_property_field->bound_reflection_property->_type == data_type_e_uint32 || _editing_property_field->bound_reflection_property->_type_count == 4 )
			{
				// rgba
				_color_picker->set_alpha_enable( true );
				_color_picker->set_rgba( _editing_value_as_color_original );
			}
			else
			{
				// rgb
				_color_picker->set_alpha_enable( false );
				_color_picker->set_rgb( vector64x3_c( _editing_value_as_color_original.as_array() ) );
			}
			_color_picker_dialog->show();
		}
		else if ( _editing_property_field->bound_reflection_property->_view == data_view_e_text )
		{
			string16_c value_as_string16;
			if ( _editing_property_field->bound_reflection_property->_type == data_type_e_string8 )
			{
				string8_c value_as_string8;
				_editing_property_field->bound_reflection_property->_accessors._value_getter( _bound_reflection_object, &value_as_string8 );
				value_as_string16 = value_as_string8;
			}
			else if ( _editing_property_field->bound_reflection_property->_type == data_type_e_string16 )
			{
				_editing_property_field->bound_reflection_property->_accessors._value_getter( _bound_reflection_object, &value_as_string16 );
			}
			_text_editor->set_plain_text( value_as_string16 );
			_text_editor_dialog->show();
		}
		else if ( _editing_property_field->bound_reflection_property->_view == data_view_e_file_path )
		{
			string16_c value_as_string16;
			if ( _editing_property_field->bound_reflection_property->_type == data_type_e_string8 )
			{
				string8_c value_as_string8;
				_editing_property_field->bound_reflection_property->_accessors._value_getter( _bound_reflection_object, &value_as_string8 );
				value_as_string16 = value_as_string8;
			}
			else if ( _editing_property_field->bound_reflection_property->_type == data_type_e_string16 )
			{
				_editing_property_field->bound_reflection_property->_accessors._value_getter( _bound_reflection_object, &value_as_string16 );
			}
			string16_c folder_path = ops::path_get_mother( value_as_string16 );
			string16_c file_name = ops::path_get_file_name( value_as_string16 );
			_file_picker->set_folder_path( folder_path );
			_file_picker->set_file_name( file_name );
			_file_picker_dialog->show();
		}
		else if ( _editing_property_field->bound_reflection_property->_view == data_view_e_folder_path )
		{
			string16_c value_as_string16;
			if ( _editing_property_field->bound_reflection_property->_type == data_type_e_string8 )
			{
				string8_c value_as_string8;
				_editing_property_field->bound_reflection_property->_accessors._value_getter( _bound_reflection_object, &value_as_string8 );
				value_as_string16 = value_as_string8;
			}
			else if ( _editing_property_field->bound_reflection_property->_type == data_type_e_string16 )
			{
				_editing_property_field->bound_reflection_property->_accessors._value_getter( _bound_reflection_object, &value_as_string16 );
			}
			_file_picker->set_folder_path( value_as_string16 );
			_file_picker_dialog->show();
		}
	}

	void_c menu_control_property_inspector_c::_handle_color_picker_on_value_changed( menu_control_color_picker_c * color_picker_panel )
	{
		assert( _editing_property_field );
		reflection_value_container_c value;
		vector64x4_c value_as_color = color_picker_panel->get_rgba();
		reflection_convert_color_to_value( _editing_property_field->bound_reflection_property, value, value_as_color );
		reflection_set_object_property_value( _bound_reflection_object, _editing_property_field->bound_reflection_property, value );
		_update_ui_from_property( _editing_property_field );
	}

	void_c menu_control_property_inspector_c::_handle_item_selected_index_changed( menu_control_list_c * list_box )
	{
		if ( !_mute )
		{
			property_field_c * property_field = static_cast< property_field_c * >( list_box->get_user_pointer() );
			reflection_object_c * reflection_object = property_field->bound_reflection_property->_accessors._object_list_item_getter( _bound_reflection_object, list_box->get_selected_item_index() );
			property_field->property_inspector->bind_reflection_object( reflection_object );
		}
	}

	void_c menu_control_property_inspector_c::_handle_item_add_on_click( menu_event_info_c event_info )
	{
		property_field_c * property_field = static_cast< property_field_c * >( event_info.control->get_user_pointer() );

		sint32_c at_index = property_field->item_list->get_selected_item_index();
		if ( at_index < 0 )
		{
			at_index = 0;
		}

		reflection_object_c * new_reflection_object = property_field->bound_reflection_property->_accessors._object_list_item_adder( _bound_reflection_object, at_index );
		menu_control_list_item_c * new_list_item = new menu_control_list_item_c();
		new_list_item->set_user_pointer( new_reflection_object );
		new_list_item->set_text_plain( new_reflection_object->get_reflection_display_name() );
		property_field->item_list->add_item( new_list_item, at_index );
		property_field->item_list->set_selected_item_index( at_index );
	}

	void_c menu_control_property_inspector_c::_handle_item_remove_on_click( menu_event_info_c event_info )
	{
		if ( _message_dialog->get_is_showing() || _color_picker_dialog->get_is_showing() || _file_picker_dialog->get_is_showing() || _text_editor_dialog->get_is_showing() ) // an operation is already pending.
		{
			return;
		}

		assert( _editing_property_field == nullptr );

		_editing_property_field = reinterpret_cast< property_field_c * >( event_info.control->get_user_pointer() );
		assert( _editing_property_field->item_list );
		_pending_delete_list_item_index = _editing_property_field->item_list->get_selected_item_index();

		_message->set_plain_text( string16_c( "are you sure you want to delete the selected item?" ) );
		_message_dialog->show();
	}

	void_c menu_control_property_inspector_c::_handle_item_move_up_on_click( menu_event_info_c event_info )
	{
		property_field_c * property_field = static_cast< property_field_c * >( event_info.control->get_user_pointer() );
		if ( property_field->item_list->get_selected_item_index() >= 1 )
		{
			int at_index = property_field->item_list->get_selected_item_index();
			if ( property_field->bound_reflection_property->_accessors._object_list_item_mover( _bound_reflection_object, at_index, at_index - 1 ) )
			{
				menu_control_list_item_c * list_item = property_field->item_list->get_item( at_index );
				property_field->item_list->set_selected_item_index( -1 );
				property_field->item_list->remove_item( at_index );
				property_field->item_list->add_item( list_item, at_index - 1 );
				property_field->item_list->set_selected_item_index( at_index - 1 );
			}
		}
	}

	void_c menu_control_property_inspector_c::_handle_item_move_down_on_click( menu_event_info_c event_info )
	{
		property_field_c * property_field = static_cast< property_field_c * >( event_info.control->get_user_pointer() );
		if ( property_field->item_list->get_selected_item_index() + 1 < property_field->item_list->get_item_count() )
		{
			int at_index = property_field->item_list->get_selected_item_index();
			if ( property_field->bound_reflection_property->_accessors._object_list_item_mover( _bound_reflection_object, at_index, at_index + 1 ) )
			{
				menu_control_list_item_c * list_item = property_field->item_list->get_item( at_index );
				property_field->item_list->set_selected_item_index( -1 );
				property_field->item_list->remove_item( at_index );
				property_field->item_list->add_item( list_item, at_index + 1 );
				property_field->item_list->set_selected_item_index( at_index + 1 );
			}
		}
	}

	void_c menu_control_property_inspector_c::_handle_item_sort_on_click( menu_event_info_c event_info )
	{
		property_field_c * property_field = static_cast< property_field_c * >( event_info.control->get_user_pointer() );
		property_field->bound_reflection_property->_accessors._object_list_item_sorter( _bound_reflection_object );
		_update_ui_from_property( property_field );
	}

	void_c menu_control_property_inspector_c::_handle_dialog_on_result( menu_window_dialog_c * dialog )
	{
		assert( _editing_property_field != nullptr );
		reflection_value_container_c new_value; // might be new value, or old one of canceling.
		if ( dialog == _message_dialog )
		{
			if ( dialog->get_result() == menu_window_dialog_c::result_e_okay )
			{
				_editing_property_field->bound_reflection_property->_accessors._object_list_item_remover( _bound_reflection_object, _pending_delete_list_item_index );
			}
			_pending_delete_list_item_index = -1;
		}
		else if ( dialog == _color_picker_dialog )
		{
			if ( dialog->get_result() == menu_window_dialog_c::result_e_okay )
			{
				_update_property_from_ui( _editing_property_field, _color_picker_dialog, true );
			}
			else
			{
				// handle cancel, since color picker applies changes in real time we need to revert any changes that were made.
				reflection_convert_color_to_value( _editing_property_field->bound_reflection_property, new_value, _editing_value_as_color_original );
				reflection_set_object_property_value( _bound_reflection_object, _editing_property_field->bound_reflection_property, new_value );
				_update_ui_from_property( _editing_property_field );
			}
		}
		else if ( dialog == _file_picker_dialog )
		{
			if ( dialog->get_result() == menu_window_dialog_c::result_e_okay )
			{
				new_value.string16 = _file_picker->get_folder_path();
				new_value.string16 += _file_picker->get_file_name();
				if ( _editing_property_field->bound_reflection_property->_type == data_type_e_string8 )
				{
					new_value.string8 = new_value.string16;
				}
				reflection_set_object_property_value( _bound_reflection_object, _editing_property_field->bound_reflection_property, new_value );
			}
		}
		else if ( dialog == _text_editor_dialog )
		{
			if ( dialog->get_result() == menu_window_dialog_c::result_e_okay )
			{
				new_value.string16 = _text_editor->get_plain_text();
				if ( _editing_property_field->bound_reflection_property->_type == data_type_e_string8 )
				{
					new_value.string8 = new_value.string16;
				}
				reflection_set_object_property_value( _bound_reflection_object, _editing_property_field->bound_reflection_property, new_value );
			}
		}

		_editing_property_field = nullptr;
		dialog->show();
	}

	void_c menu_control_property_inspector_c::_handle_style_map_reference_on_refreshed( menu_style_map_c::reference_c const * value )
	{
		menu_control_c::_handle_style_map_reference_on_refreshed( value );

		styled_properties.height_for_class_row = 24.0f;
		styled_properties.height_for_class_row_padding = 0.0f;
		styled_properties.height_for_category_row = 24.0f;
		styled_properties.height_for_category_row_padding = 0.0f;
		styled_properties.height_for_property_row = 24.0f;
		styled_properties.height_for_property_row_padding = 0.0f;
		styled_properties.height_for_property_list = 200.0f;
		styled_properties.width_for_property_control = 24.0f;
		styled_properties.width_for_property_control_padding = 0.0f;

		if ( _style_map_reference.get_value() )
		{
			_style_map_reference.get_value()->find_property_as_float32( string8_c( mode_e_static, "height_for_class_row" ), styled_properties.height_for_class_row );
			styled_properties.height_for_class_row = ops::math_clamp( styled_properties.height_for_class_row, 4.0f, 100.0f );

			_style_map_reference.get_value()->find_property_as_float32( string8_c( mode_e_static, "height_for_class_row_padding" ), styled_properties.height_for_class_row_padding );
			styled_properties.height_for_class_row_padding = ops::math_clamp( styled_properties.height_for_class_row_padding, 0.0f, 100.0f );

			_style_map_reference.get_value()->find_property_as_float32( string8_c( mode_e_static, "height_for_category_row" ), styled_properties.height_for_category_row );
			styled_properties.height_for_category_row = ops::math_clamp( styled_properties.height_for_category_row, 4.0f, 100.0f );

			_style_map_reference.get_value()->find_property_as_float32( string8_c( mode_e_static, "height_for_category_row_padding" ), styled_properties.height_for_category_row_padding );
			styled_properties.height_for_category_row_padding = ops::math_clamp( styled_properties.height_for_category_row_padding, 0.0f, 100.0f );

			_style_map_reference.get_value()->find_property_as_float32( string8_c( mode_e_static, "height_for_property_row" ), styled_properties.height_for_property_row );
			styled_properties.height_for_property_row = ops::math_clamp( styled_properties.height_for_property_row, 4.0f, 100.0f );

			_style_map_reference.get_value()->find_property_as_float32( string8_c( mode_e_static, "height_for_property_row_padding" ), styled_properties.height_for_property_row_padding );
			styled_properties.height_for_property_row_padding = ops::math_clamp( styled_properties.height_for_property_row_padding, 0.0f, 100.0f );

			_style_map_reference.get_value()->find_property_as_float32( string8_c( mode_e_static, "height_for_property_list" ), styled_properties.height_for_property_list );
			styled_properties.height_for_property_list = ops::math_clamp( styled_properties.height_for_property_list, 10.0f, 1000.0f );

			_style_map_reference.get_value()->find_property_as_float32( string8_c( mode_e_static, "width_for_property_control" ), styled_properties.width_for_property_control );
			styled_properties.width_for_property_control = ops::math_clamp( styled_properties.width_for_property_control, 4.0f, 100.0f );

			_style_map_reference.get_value()->find_property_as_float32( string8_c( mode_e_static, "width_for_property_control_padding" ), styled_properties.width_for_property_control_padding );
			styled_properties.width_for_property_control_padding = ops::math_clamp( styled_properties.width_for_property_control_padding, 4.0f, 100.0f );
		}

		_y = 0.0f;
		for ( sint32_c i = 0; i < _property_field_list.get_length(); i++ )
		{
			_layout_controls_for_property( _property_field_list[ i ] );
		}
	}

	menu_control_property_inspector_c::menu_control_property_inspector_c( menu_control_property_inspector_c * mother_property_inspector, reflection_class_c const * fixed_reflection_class )
		: menu_control_c()
		, _mother_property_inspector( mother_property_inspector )
		, _fixed_reflection_class( fixed_reflection_class )
		, _bound_reflection_object( nullptr )
		//, _bound_database_record( nullptr )
		, _property_field_list()
		, _y( 0.0f )
		, _mute( false )
	{
		_element_frame.set_name( string8_c( mode_e_static, "frame" ) );
		_element_frame.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_add_element( &_element_frame );

		//if ( _fixed_reflection_class )
		//{
		//	_add( _fixed_reflection_class );
		//}

		// first time initialize shared resources.
		if ( _editing_property_field == nullptr )
		{
			_message = new menu_control_label_c();
			_message->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
			_message_dialog = new menu_window_dialog_c();
			_message_dialog->add_control( _message );

			_color_picker = new menu_control_color_picker_c();
			_color_picker->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
			_color_picker->on_value_changed.subscribe( this, &menu_control_property_inspector_c::_handle_color_picker_on_value_changed );
			_color_picker_dialog = new menu_window_dialog_c();
			_color_picker_dialog->add_control( _color_picker );

			_file_picker = new menu_control_file_picker_c();
			_file_picker->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
			_file_picker->set_mode( menu_control_file_picker_c::mode_e_load );
			_file_picker_dialog = new menu_window_dialog_c();
			_file_picker_dialog->add_control( _file_picker );

			_text_editor = new menu_control_text_c();
			_text_editor->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
			_text_editor_dialog = new menu_window_dialog_c();
			_text_editor_dialog->add_control( _text_editor );
		}

		set_style_map_key( string8_c( mode_e_static, "e_property_inspector" ) );
	}

	menu_control_property_inspector_c::~menu_control_property_inspector_c()
	{
		_clear();
	}

	void_c menu_control_property_inspector_c::bind_reflection_object( reflection_object_c * value )
	{
		_bound_reflection_object = value;
		_clear();
		if ( _bound_reflection_object != nullptr )
		{
			_add( _bound_reflection_object->_reflection_class );
			refresh_ui();
		}
	}

	void_c menu_control_property_inspector_c::unbind_data()
	{
		_bound_reflection_object = nullptr;
		_clear();
	}

	void_c menu_control_property_inspector_c::refresh_ui()
	{
		for ( sint32_c i = 0; i < _property_field_list.get_length(); i++ )
		{
			_update_ui_from_property( _property_field_list[ i ] );
		}
	}

	float32_c menu_control_property_inspector_c::get_content_height() const
	{
		return _y;
	}

}
