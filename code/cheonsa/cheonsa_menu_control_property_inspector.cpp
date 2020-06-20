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

	menu_control_property_inspector_c::property_field_c::property_field_c()
		: _reflection_property( nullptr )
		, _label_control( nullptr )
		, _text_control( nullptr )
		, _combo_button_control( nullptr )
		, _scroll_bar_control( nullptr )
		, _button_control( nullptr )
		, _item_list_control( nullptr )
		, _item_add_button_control( nullptr )
		, _item_remove_button_control( nullptr )
		, _item_move_up_button_control( nullptr )
		, _item_move_down_button_control( nullptr )
		, _item_sort_button_control( nullptr )
	{
	}

	menu_control_property_inspector_c::property_field_c::~property_field_c()
	{
	}

	void_c menu_control_property_inspector_c::_initialize_property_inspector()
	{
		assert( _property_inspector_window == nullptr );

		_property_inspector_window = new menu_control_window_c();
		_property_inspector_window->set_name( string8_c( "property_inspector_window", core_list_mode_e_static ) );
		_property_inspector_window->set_layout_simple( box32x2_c( 0, 0, 300, 800 ) );
		_property_inspector_window->set_is_showed_immediately( false );
		_property_inspector_cancel_button = new menu_control_button_c();
		_property_inspector_cancel_button->set_name( string8_c( "cancel_button", core_list_mode_e_static ) );
		_property_inspector_cancel_button->set_layout_box_anchor( menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 100, 30, 116, 8 ) );
		_property_inspector_cancel_button->set_plain_text_value( string16_c( L"cancel", core_list_mode_e_static ) );
		_property_inspector_cancel_button->on_clicked.subscribe( this, &menu_control_property_inspector_c::_handle_property_inspector_button_on_clicked );
		_property_inspector_window->add_daughter_control_to_client( _property_inspector_cancel_button );
		_add_supplemental_control( _property_inspector_window );

		_property_inspector_okay_button = new menu_control_button_c();
		_property_inspector_okay_button->set_name( string8_c( "okay_button", core_list_mode_e_static ) );
		_property_inspector_okay_button->set_layout_box_anchor( menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 100, 30, 8, 8 ) );
		_property_inspector_okay_button->set_plain_text_value( string16_c( L"okay", core_list_mode_e_static ) );
		_property_inspector_okay_button->on_clicked.subscribe( this, &menu_control_property_inspector_c::_handle_property_inspector_button_on_clicked );
		_property_inspector_window->add_daughter_control_to_client( _property_inspector_okay_button );

		_property_inspector = new menu_control_property_inspector_c( this, nullptr );
		_property_inspector->set_name( string8_c( "property_inspector", core_list_mode_e_static ) );
		_property_inspector->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0, 0, 0, 46 ) );
		_property_inspector_window->add_daughter_control_to_client( _property_inspector );

		if ( _mother_user_interface )
		{
			_mother_user_interface->add_daughter_control( _property_inspector_window );
			_property_inspector_window->center();
		}
	}

	void_c menu_control_property_inspector_c::_handle_property_inspector_button_on_clicked( menu_event_information_c event_information )
	{
		if ( event_information.control == _property_inspector_okay_button )
		{
		}
		else if ( event_information.control == _property_inspector_cancel_button )
		{
		}
		_property_inspector_window->hide_dialog();
	}

	void_c menu_control_property_inspector_c::_layout_controls_for_property( property_field_c * property_field )
	{
		assert( property_field->_label_control );
		if ( property_field->_reflection_property )
		{
			if ( property_field->_reflection_property->_type != data_type_e_category_label )
			{
				// property label.
				property_field->_label_control->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( 0.0f, _y, 0.0f, styled_properties.height_for_property_row ) );
				_y += styled_properties.height_for_property_row + styled_properties.height_for_property_row_padding;

				float32_c left_indent = 0.0f;
				if ( property_field->_item_list_control == nullptr )
				{
					// property edit button.
					float32_c right = 0.0f;
					if ( property_field->_button_control )
					{
						right = styled_properties.width_for_property_control + styled_properties.width_for_property_control_padding;
						property_field->_button_control->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right, box32x2_c( styled_properties.width_for_property_control, 0.0f, 0.0f, styled_properties.height_for_property_row ) );
					}

					// property editor(s).
					if ( property_field->_text_control )
					{
						property_field->_text_control->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( left_indent, _y, right, styled_properties.height_for_property_row ) );
						_y += styled_properties.height_for_property_row + styled_properties.height_for_property_row_padding;
					}
					if ( property_field->_combo_button_control )
					{
						property_field->_combo_button_control->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( left_indent, _y, right, styled_properties.height_for_property_row ) );
						_y += styled_properties.height_for_property_row + styled_properties.height_for_property_row_padding;
					}
					if ( property_field->_scroll_bar_control )
					{
						property_field->_scroll_bar_control->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( left_indent, _y, right, styled_properties.height_for_property_row ) );
						_y += styled_properties.height_for_property_row + styled_properties.height_for_property_row_padding;
					}
				}
				
				if ( property_field->_item_list_control )
				{
					assert( property_field->_item_list_control );
					assert( property_field->_item_add_button_control );
					assert( property_field->_item_remove_button_control );
					assert( property_field->_item_move_up_button_control );
					assert( property_field->_item_move_down_button_control );
					assert( property_field->_item_sort_button_control );

					float32_c right = styled_properties.width_for_property_control + styled_properties.width_for_property_control_padding;

					property_field->_item_list_control->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( left_indent, _y, right, styled_properties.height_for_property_list ) );

					float32_c y = _y;
					property_field->_item_add_button_control->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right, box32x2_c( styled_properties.width_for_property_control, y, 0.0f, styled_properties.height_for_property_row ) );
					y += styled_properties.height_for_property_row + styled_properties.height_for_property_row_padding;

					property_field->_item_remove_button_control->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right, box32x2_c( styled_properties.width_for_property_control, y, 0.0f, styled_properties.height_for_property_row ) );
					y += styled_properties.height_for_property_row + styled_properties.height_for_property_row_padding;

					property_field->_item_move_up_button_control->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right, box32x2_c( styled_properties.width_for_property_control, y, 0.0f, styled_properties.height_for_property_row ) );
					y += styled_properties.height_for_property_row + styled_properties.height_for_property_row_padding;

					property_field->_item_move_down_button_control->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right, box32x2_c( styled_properties.width_for_property_control, y, 0.0f, styled_properties.height_for_property_row ) );
					y += styled_properties.height_for_property_row + styled_properties.height_for_property_row_padding;

					property_field->_item_sort_button_control->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right, box32x2_c( styled_properties.width_for_property_control, y, 0.0f, styled_properties.height_for_property_row ) );
					y += styled_properties.height_for_property_row + styled_properties.height_for_property_row_padding;

					_y = ops::math_maximum( y, _y + styled_properties.height_for_property_list + styled_properties.height_for_property_row_padding);
				}
			}
			else
			{
				// category label.
				assert( property_field->_label_control );
				property_field->_label_control->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( 0.0f, _y, 0.0f, styled_properties.height_for_category_row ) );
				_y += styled_properties.height_for_category_row + styled_properties.height_for_category_row_padding;
			}
		}
		else
		{
			// class label.
			assert( property_field->_label_control );
			property_field->_label_control->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( 0.0f, _y, 0.0f, styled_properties.height_for_class_row ) );
			_y += styled_properties.height_for_class_row + styled_properties.height_for_class_row_padding;
		}
	}

	void_c menu_control_property_inspector_c::_clear()
	{
		// cancel any currently open editing operations and dialogs.
		_editing_property_field = nullptr;
		_editing_value_as_color_original = vector64x4_c( 0.0f, 0.0f, 0.0f, 0.0f );
		_pending_delete_list_item_index = -1;
		_message_dialog->set_is_showed( false );
		_color_picker_dialog->set_is_showed( false );
		_file_picker_dialog->set_is_showed( false );
		_text_editor_dialog->set_is_showed( false );

		// [remove and delete] or [disable] all property fields and controls.
		if ( _fixed_reflection_class == nullptr )
		{
			remove_all_daughter_controls();
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
		assert( _is_muted == false );
		_is_muted = true; // just in case the controls invoke events we should ignore them at this point, even though i don't think they would invoke events.

		//property_field_c * property_field = new property_field_c();
		//_property_field_list.insert_at_end( property_field );
		//property_field->label = new menu_control_text_c();
		//property_field->label->set_name( string8_c( core_list_mode_e_static, "class_label" ) );
		//property_field->label->set_user_pointer( property_field );
		//property_field->label->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( 0.0f, _y, 0.0f, styled_properties.height_for_class_row ) );
		//_add_private_control( property_field->label );
		//_layout_controls( property_field );

		//property_field->label->set_style_map_key( styled_properties.style_map_key_for_class_label );
		//_y += styled_properties.height_for_class_row + styled_properties.height_for_class_row_padding;

		for ( sint32_c i = 0; i < reflection_class->get_property_count(); i++ )
		{
			property_field_c * property_field = new property_field_c();
			reflection_property_c const * property = property_field->_reflection_property;
			_property_field_list.insert( -1, property_field );
			property = reflection_class->get_property( i );
			if ( property->_type == data_type_e_category_label )
			{
				property_field->_label_control = new menu_control_text_c();
				property_field->_label_control->set_name( string8_c( "category_label", core_list_mode_e_static ) );
				property_field->_label_control->set_user_pointer( property_field );
				property_field->_label_control->set_plain_text_value( string16_c( property->_name ) );
				add_daughter_control( property_field->_label_control );
			}
			else
			{
				property_field->_label_control = new menu_control_text_c();
				property_field->_label_control->set_name( string8_c( "property_label", core_list_mode_e_static ) );
				property_field->_label_control->set_user_pointer( property_field );
				property_field->_label_control->set_plain_text_value( string16_c( property->_name ) );
				add_daughter_control( property_field->_label_control );

				// create text control if needed.
				if ( property->_type >= data_type_e_string8 && property->_type <= data_type_e_float64 )
				{
					property_field->_text_control = new menu_control_text_c();
					property_field->_text_control->set_name( string8_c( "property_value_text", core_list_mode_e_static ) );
					property_field->_text_control->set_user_pointer( property_field );
					if ( property->_view == data_view_e_text )
					{
						property_field->_text_control->set_text_interact_mode( menu_text_interact_mode_e_static );
					}
					add_daughter_control( property_field->_text_control );
				}

				// create button control if needed.
				if ( property->_view == data_view_e_color || property->_view == data_view_e_text || property->_view == data_view_e_file_path || property->_type == data_type_e_object )
				{
					property_field->_button_control = new menu_control_button_c();
					property_field->_button_control->set_name( string8_c( "property_value_button", core_list_mode_e_static ) );
					property_field->_button_control->set_user_pointer( property_field );
					property_field->_button_control->set_plain_text_value( string16_c( L"...", core_list_mode_e_static ) );
					property_field->_button_control->on_clicked.subscribe( this, &menu_control_property_inspector_c::_handle_value_edit_on_clicked );
					add_daughter_control( property_field->_button_control );
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
						property_field->_scroll_bar_control = new menu_control_scroll_bar_x_c();
						property_field->_scroll_bar_control->set_name( string8_c( "property_value_scroll_bar", core_list_mode_e_static ) );
					}
					else
					{
						property_field->_scroll_bar_control = new menu_control_scrub_bar_x_c();
						property_field->_scroll_bar_control->set_name( string8_c( "property_value_scrub_bar", core_list_mode_e_static ) );
					}
					property_field->_scroll_bar_control->set_user_pointer( property_field );
					property_field->_scroll_bar_control->set_value_minimum( value_minimum );
					property_field->_scroll_bar_control->set_value_maximum( value_maximum );
					property_field->_scroll_bar_control->on_value_changed_preview.subscribe( this, &menu_control_property_inspector_c::_handle_value_scroll_on_value_changed_preview );
					property_field->_scroll_bar_control->on_value_changed.subscribe( this, &menu_control_property_inspector_c::_handle_value_scroll_on_value_changed );
					add_daughter_control( property_field->_scroll_bar_control );
				}

				// create combo control if needed.
				if ( property->_view == data_view_e_enumeration || property->_view == data_view_e_enumeration_flags )
				{
					property_field->_combo_button_control = new menu_control_combo_button_c();
					property_field->_combo_button_control->set_name( string8_c( "property_value_combo_button", core_list_mode_e_static ) );

					menu_control_combo_c * combo = new menu_control_combo_c();
					combo->set_name( string8_c( "property_value_combo", core_list_mode_e_static ) );
					property_field->_combo_button_control->set_combo_control( combo );
					if ( property->_view == data_view_e_enumeration_flags )
					{
						combo->set_use_toggle_behavior( true );
						combo->set_selected_item_limit( -1 );
					}

					reflection_enumeration_c const * enumeration = property->_enumeration;
					assert( enumeration );
					for ( sint32_c j = 0; j < enumeration->get_value_count(); j++ ) // populate combo control.
					{
						menu_control_combo_item_text_c * item = new menu_control_combo_item_text_c();
						item->set_name( string8_c( "list_item", core_list_mode_e_static ) );
						item->set_plain_text_value( string16_c( enumeration->get_value( j )->get_name() ) );
						combo->add_item( item );
					}

					//combo->on_selected_item_changed_preview.subscribe( this, &menu_control_property_inspector_c::_handle_value_combo_on_selected_item_changed_preview );
					combo->on_selected_item_changed.subscribe( this, &menu_control_property_inspector_c::_handle_value_combo_on_selected_item_changed );
					add_daughter_control( property_field->_combo_button_control );
				}

				// create list and button controls if needed.
				if ( property->_type == data_type_e_object_list )
				{
					property_field->_item_list_control = new menu_control_list_c();
					property_field->_item_list_control->set_name( string8_c( "item_list", core_list_mode_e_static ) );
					property_field->_item_list_control->set_user_pointer( property_field );
					property_field->_item_list_control->on_selected_item_list_changed.subscribe( this, &menu_control_property_inspector_c::_handle_item_list_on_selected_item_changed );
					add_daughter_control( property_field->_item_list_control );

					property_field->_item_add_button_control = new menu_control_button_c();
					property_field->_item_add_button_control->set_name( string8_c( "item_add_button", core_list_mode_e_static ) );
					property_field->_item_add_button_control->set_user_pointer( property_field );
					property_field->_item_add_button_control->set_plain_text_value( string16_c( L"+", core_list_mode_e_static ) );
					property_field->_item_add_button_control->on_clicked.subscribe( this, &menu_control_property_inspector_c::_handle_item_add_on_clicked );
					add_daughter_control( property_field->_item_add_button_control );

					property_field->_item_remove_button_control = new menu_control_button_c();
					property_field->_item_remove_button_control->set_name( string8_c( "item_remove_button", core_list_mode_e_static ) );
					property_field->_item_remove_button_control->set_user_pointer( property_field );
					property_field->_item_remove_button_control->set_plain_text_value( string16_c( L"-", core_list_mode_e_static ) );
					property_field->_item_remove_button_control->on_clicked.subscribe( this, &menu_control_property_inspector_c::_handle_item_remove_on_clicked );
					add_daughter_control( property_field->_item_remove_button_control );

					property_field->_item_move_up_button_control = new menu_control_button_c();
					property_field->_item_move_up_button_control->set_name( string8_c( "item_move_up_button", core_list_mode_e_static ) );
					property_field->_item_move_up_button_control->set_user_pointer( property_field );
					property_field->_item_move_up_button_control->set_plain_text_value( string16_c( L"", core_list_mode_e_static ) );
					property_field->_item_move_up_button_control->on_clicked.subscribe( this, &menu_control_property_inspector_c::_handle_item_move_up_on_clicked );
					add_daughter_control( property_field->_item_move_up_button_control );

					property_field->_item_move_down_button_control = new menu_control_button_c();
					property_field->_item_move_down_button_control->set_name( string8_c( "item_move_down_button", core_list_mode_e_static ) );
					property_field->_item_move_down_button_control->set_user_pointer( property_field );
					property_field->_item_move_down_button_control->set_plain_text_value( string16_c( L"", core_list_mode_e_static ) );
					property_field->_item_move_down_button_control->on_clicked.subscribe( this, &menu_control_property_inspector_c::_handle_item_move_down_on_clicked );
					add_daughter_control( property_field->_item_move_down_button_control );

					property_field->_item_sort_button_control = new menu_control_button_c();
					property_field->_item_sort_button_control->set_name( string8_c( "item_sort_button", core_list_mode_e_static ) );
					property_field->_item_sort_button_control->set_user_pointer( property_field );
					property_field->_item_sort_button_control->set_plain_text_value( string16_c( L"sort", core_list_mode_e_static ) );
					property_field->_item_sort_button_control->on_clicked.subscribe( this, &menu_control_property_inspector_c::_handle_item_sort_on_clicked );
					add_daughter_control( property_field->_item_sort_button_control );
				}
			}
			_layout_controls_for_property( property_field );
		}

		_is_muted = false;
	}

	void_c menu_control_property_inspector_c::_update_ui_from_property( property_field_c * property_field )
	{
		if ( _reflection_object == nullptr )
		{
			// it should only be possible to reach here if _fixed_reflection_class is set.
			assert( _fixed_reflection_class );
			if ( property_field->_label_control )
			{
				property_field->_label_control->set_is_enabled( false );
			}
			if ( property_field->_text_control )
			{
				property_field->_text_control->set_is_enabled( false );
				property_field->_text_control->clear_text_value();
			}
			if ( property_field->_button_control )
			{
				property_field->_button_control->set_is_enabled( false );
			}
			if ( property_field->_combo_button_control )
			{
				property_field->_combo_button_control->set_is_enabled( false );
				property_field->_combo_button_control->get_combo_control()->set_selected_item( nullptr );
			}
			if ( property_field->_scroll_bar_control )
			{
				property_field->_scroll_bar_control->set_is_enabled( false );
				property_field->_scroll_bar_control->set_value( 0.0 );
			}
			if ( property_field->_item_list_control )
			{
				property_field->_item_list_control->remove_all_items();
				property_field->_item_list_control->set_is_enabled( false );
				property_field->_item_add_button_control->set_is_enabled( false );
				property_field->_item_remove_button_control->set_is_enabled( false );
				property_field->_item_move_up_button_control->set_is_enabled( false );
				property_field->_item_move_down_button_control->set_is_enabled( false );
				property_field->_item_sort_button_control->set_is_enabled( false );
			}
			return;
		}

		_is_muted = true;

		// get raw value.
		reflection_property_c const * property = property_field->_reflection_property;
		reflection_value_container_c value;
		if ( !reflection_get_object_property_value( _reflection_object, property, value ) )
		{
			return;
		}

		// convert value to string if needed.
		if ( property_field->_text_control )
		{
			string16_c value_as_string16;
			value_as_string16 = reflection_convert_value_to_string16( property, value );
			property_field->_text_control->set_plain_text_value( value_as_string16 );
		}

		// provide value to view editors.
		if ( property_field->_scroll_bar_control )
		{
			assert( property->_view == data_view_e_scroll_bar || property->_view == data_view_e_scrub_bar );
			assert( property->_type_count == 1 );
			if ( property->_type == data_type_e_float32 )
			{
				property_field->_scroll_bar_control->set_value( static_cast< float64_c >( value.float32[ 0 ] ) );
			}
			else if ( property->_type == data_type_e_float64 )
			{
				property_field->_scroll_bar_control->set_value( value.float64[ 0 ] );
			}
			else
			{
				assert( false );
			}
		}
		else if ( property_field->_combo_button_control )
		{
			assert( property->_view == data_view_e_enumeration || property->_view == data_view_e_enumeration_flags );
			assert( property->_type_count == 1 );
			assert( property->_enumeration );

			sint64_c property_value = 0;
			if ( property->_type == data_type_e_uint8 )
			{
				property_value = static_cast< sint64_c >( value.uint8[ 0 ] );
			}
			else if ( property->_type == data_type_e_sint8 )
			{
				property_value = static_cast< sint64_c >( value.sint8[ 0 ] );
			}
			else if ( property->_type == data_type_e_uint16 )
			{
				property_value = static_cast< sint64_c >( value.uint16[ 0 ] );
			}
			else if ( property->_type == data_type_e_sint16 )
			{
				property_value = static_cast< sint64_c >( value.sint16[ 0 ] );
			}
			else if ( property->_type == data_type_e_uint32 )
			{
				property_value = static_cast< sint64_c >( value.uint32[ 0 ] );
			}
			else if ( property->_type == data_type_e_sint32 )
			{
				property_value = static_cast< sint64_c >( value.sint32[ 0 ] );
			}
			else if ( property->_type == data_type_e_uint64 )
			{
				property_value = static_cast< uint64_c >( value.uint64[ 0 ] );
			}
			else if ( property->_type == data_type_e_sint64 )
			{
				property_value = static_cast< sint64_c >( value.sint64[ 0 ] );
			}
			else
			{
				assert( false );
			}

			if ( property->_view == data_view_e_enumeration )
			{
				sint32_c enumeration_value_index = -1;
				property->_enumeration->find_index_with_value( enumeration_value_index, property_value ); // if this returns false then enum_index will be set to -1, which is still valid to set no selection in the combo list.
				property_field->_combo_button_control->get_combo_control()->set_selected_item_index( enumeration_value_index );
			}
			else if ( property->_view == data_view_e_enumeration_flags )
			{
				for ( sint32_c i = 0; i < property_field->_combo_button_control->get_combo_control()->get_item_count(); i++ )
				{
					menu_control_combo_item_text_c * combo_item = static_cast< menu_control_combo_item_text_c * >( property_field->_combo_button_control->get_combo_control()->get_item_at_index( i ) );
					sint64_c enumeration_value = 0;
					property->_enumeration->find_value_with_index( enumeration_value, i );
					combo_item->set_is_selected( ( property_value & enumeration_value ) != 0, true );
				}
			}
		}
		else if ( property->_type == data_type_e_object )
		{
		}
		else if ( property->_type == data_type_e_object_list )	
		{
			assert( property_field->_item_list_control );

			property_field->_item_list_control->remove_all_items();
			sint32_c item_count = property->_accessors._object_list_item_count_getter( _reflection_object );
			for ( sint32_c i = 0; i < item_count; i++ )
			{
				reflection_object_c * object = property->_accessors._object_list_item_getter( _reflection_object, i );
				menu_control_list_item_text_c * item = new menu_control_list_item_text_c();
				item->set_name( string8_c( "list_item", core_list_mode_e_static ) );
				item->set_plain_text_value( object->get_reflection_display_name() );
				property_field->_item_list_control->add_item( item );
			}

			if ( property_field->_item_add_button_control && property_field->_item_remove_button_control )
			{
				if ( property->_accessors._object_list_item_adder && property->_accessors._object_list_item_remover )
				{
					property_field->_item_add_button_control->set_is_enabled( true );
					property_field->_item_remove_button_control->set_is_enabled( true );
				}
				else
				{
					property_field->_item_add_button_control->set_is_enabled( false );
					property_field->_item_remove_button_control->set_is_enabled( false );
				}
			}

			if ( property_field->_item_move_up_button_control && property_field->_item_move_down_button_control )
			{
				if ( property->_accessors._object_list_item_mover )
				{
					property_field->_item_move_up_button_control->set_is_enabled( true );
					property_field->_item_move_down_button_control->set_is_enabled( true );
				}
				else
				{
					property_field->_item_move_up_button_control->set_is_enabled( false );
					property_field->_item_move_down_button_control->set_is_enabled( false );
				}
			}

			if ( property_field->_item_sort_button_control )
			{
				if ( property->_accessors._object_list_item_sorter )
				{
					property_field->_item_sort_button_control->set_is_enabled( true );
				}
				else
				{
					property_field->_item_sort_button_control->set_is_enabled( false );
				}
			}
		}

		_is_muted = false;
	}

	void_c menu_control_property_inspector_c::_update_property_from_ui( property_field_c * property_field, menu_control_c * control, boolean_c and_commit )
	{
		assert( property_field && control );

		if ( _reflection_object == nullptr )
		{
			// because sometimes this can get called when one of our text boxes looses keyboard focus after the selection is cleared.
			return;
		}

		if ( property_field->_text_control == nullptr )
		{
			return;
		}

		reflection_property_c const * property = property_field->_reflection_property;

		reflection_value_container_c before_value; // old or current value.
		reflection_get_object_property_value( _reflection_object, property, before_value );

		reflection_value_container_c after_value; // new value.
		if ( control == property_field->_scroll_bar_control )
		{
		}
		else if ( property_field->_combo_button_control && control == property_field->_combo_button_control->get_combo_control() )
		{
			assert( property->_view == data_view_e_enumeration || property->_view == data_view_e_enumeration_flags );
			assert( property->_type_count == 1 );
			assert( property->_enumeration );

			sint64_c property_value = 0;
			if ( property->_view == data_view_e_enumeration )
			{
				assert( property->_enumeration->find_value_with_index( property_value, property_field->_combo_button_control->get_combo_control()->get_selected_item_index() ) );
			}
			else if ( property->_view == data_view_e_enumeration_flags )
			{
				for ( sint32_c i = 0; i < property_field->_combo_button_control->get_combo_control()->get_item_count(); i++ )
				{
					sint64_c flag_value = 0;
					menu_control_combo_item_text_c * combo_item = static_cast< menu_control_combo_item_text_c * >( property_field->_combo_button_control->get_combo_control()->get_item_at_index( i ) );
					if ( combo_item->get_is_selected() )
					{
						property->_enumeration->find_value_with_index( flag_value, i );
						property_value  |= flag_value;
					}
				}
			}

			if ( property->_type == data_type_e_uint8 )
			{
				after_value.uint8[ 0 ] = static_cast< uint8_c >( property_value );
			}
			else if ( property->_type == data_type_e_sint8 )
			{
				after_value.sint8[ 0 ] = static_cast< sint8_c >( property_value );
			}
			else if ( property->_type == data_type_e_uint16 )
			{
				after_value.uint16[ 0 ] = static_cast< uint16_c >( property_value );
			}
			else if ( property->_type == data_type_e_sint16 )
			{
				after_value.sint16[ 0 ] = static_cast< sint16_c >( property_value );
			}
			else if ( property->_type == data_type_e_uint32 )
			{
				after_value.uint32[ 0 ] = static_cast< uint32_c >( property_value );
			}
			else if ( property->_type == data_type_e_sint32 )
			{
				after_value.sint32[ 0 ] = static_cast< sint32_c >( property_value );
			}
			else if ( property->_type == data_type_e_uint64 )
			{
				after_value.uint64[ 0 ] = static_cast< uint64_c >( property_value );
			}
			else if ( property->_type == data_type_e_sint64 )
			{
				after_value.sint64[ 0 ] = static_cast< sint64_c >( property_value );
			}
			else
			{
				assert( false );
			}
		}
		else if ( control == property_field->_text_control )
		{
			string16_c new_value_as_string16 = property_field->_text_control->get_plain_text_value();
			reflection_convert_string16_to_value( property_field->_reflection_property, after_value, new_value_as_string16 );
		}
		else if ( control == _color_picker_dialog )
		{
			vector64x4_c value_as_color = _color_picker_dialog->get_rgba();
			reflection_convert_color_to_value( property_field->_reflection_property, after_value, value_as_color );
		}
		else if ( control == _file_picker_dialog )
		{
			after_value.string16 = _file_picker_dialog->get_file_path_absolute();
			if ( property_field->_reflection_property->_type == data_type_e_string8 )
			{
				after_value.string8 = after_value.string16;
			}
		}
		else if ( control == _text_editor_dialog )
		{
			after_value.string16 = _text_editor_dialog->get_plain_text_value();
			if ( property_field->_reflection_property->_type == data_type_e_string8 )
			{
				after_value.string8 = after_value.string16;
			}
			reflection_set_object_property_value( _reflection_object, property_field->_reflection_property, after_value );
		}
		else
		{
			assert( false );
		}

		if ( reflection_set_object_property_value( _reflection_object, property, after_value ) )
		{
			if ( and_commit == true )
			{
				if ( !reflection_compare_values( property, before_value, after_value ) )
				{
					reflection_property_value_changed_information_c reflection_property_value_changed_information;
					reflection_property_value_changed_information.reflection_object = _reflection_object;
					reflection_property_value_changed_information.reflection_property = property_field->_reflection_property;
					reflection_property_value_changed_information.before_value = before_value;
					reflection_property_value_changed_information.after_value = after_value;
					menu_control_property_inspector_c * handler = this;
					while ( handler->_mother_property_inspector ) // bubble to the top level so subscribers can respond to nested changes.
					{
						handler = _mother_property_inspector;
					}
					handler->on_property_value_changed.invoke( &reflection_property_value_changed_information );
				}
			}
		}

		refresh_ui();
	}

	void_c menu_control_property_inspector_c::_handle_value_text_on_value_changed_preview( menu_control_text_c * text )
	{
		if ( !_is_muted )
		{
			_update_property_from_ui( static_cast< property_field_c * >( text->get_user_pointer() ), text, false );
		}
	}

	void_c menu_control_property_inspector_c::_handle_value_text_on_value_changed( menu_control_text_c * text )
	{
		if ( !_is_muted )
		{
			_update_property_from_ui( static_cast< property_field_c * >( text->get_user_pointer() ), text, true );
		}
	}

	void_c menu_control_property_inspector_c::_handle_value_scroll_on_value_changed_preview( menu_control_scroll_bar_i * scroll )
	{
		if ( !_is_muted )
		{
			_update_property_from_ui( static_cast< property_field_c * >( scroll->get_user_pointer() ), scroll, false );
		}
	}

	void_c menu_control_property_inspector_c::_handle_value_scroll_on_value_changed( menu_control_scroll_bar_i * scroll )
	{
		if ( !_is_muted )
		{
			_update_property_from_ui( static_cast< property_field_c * >( scroll->get_user_pointer() ), scroll, true );
		}
	}

	void_c menu_control_property_inspector_c::_handle_value_combo_on_selected_item_changed_preview( menu_control_combo_c * combo_list )
	{
		if ( !_is_muted )
		{
			_update_property_from_ui( static_cast< property_field_c * >( combo_list->get_user_pointer() ), combo_list, false );
		}
	}
	
	void_c menu_control_property_inspector_c::_handle_value_combo_on_selected_item_changed( menu_control_combo_c * combo_list )
	{
		if ( !_is_muted )
		{
			_update_property_from_ui( static_cast< property_field_c * >( combo_list->get_user_pointer() ), combo_list, true );
		}
	}

	void_c menu_control_property_inspector_c::_handle_value_edit_on_clicked( menu_event_information_c event_information )
	{
		if ( _editing_property_field )
		{
			return;
		}

		_editing_property_field = reinterpret_cast< property_field_c * >( event_information.control->get_user_pointer() );

		reflection_value_container_c value;
		reflection_get_object_property_value( _reflection_object, _editing_property_field->_reflection_property, value );

		if ( _editing_property_field->_reflection_property->_view == data_view_e_color )
		{
			_editing_value_as_color_original = reflection_convert_value_to_color( _editing_property_field->_reflection_property, value );
			if ( _editing_property_field->_reflection_property->_type == data_type_e_uint32 || _editing_property_field->_reflection_property->_type_count == 4 )
			{
				// rgba
				_color_picker_dialog->set_alpha_is_enabled( true );
				_color_picker_dialog->set_rgba( _editing_value_as_color_original );
			}
			else
			{
				// rgb
				_color_picker_dialog->set_alpha_is_enabled( false );
				_color_picker_dialog->set_rgb( vector64x3_c( _editing_value_as_color_original.as_array() ) );
			}
			_color_picker_dialog->show_dialog( nullptr );
		}
		else if ( _editing_property_field->_reflection_property->_view == data_view_e_text )
		{
			string16_c value_as_string16;
			if ( _editing_property_field->_reflection_property->_type == data_type_e_string8 )
			{
				string8_c value_as_string8;
				_editing_property_field->_reflection_property->_accessors._value_getter( _reflection_object, &value_as_string8, _editing_property_field->_reflection_property->_type, _editing_property_field->_reflection_property->_type_count );
				value_as_string16 = value_as_string8;
			}
			else if ( _editing_property_field->_reflection_property->_type == data_type_e_string16 )
			{
				_editing_property_field->_reflection_property->_accessors._value_getter( _reflection_object, &value_as_string16, _editing_property_field->_reflection_property->_type, _editing_property_field->_reflection_property->_type_count );
			}
			_text_editor_dialog->set_plain_text_value( value_as_string16 );
			_text_editor_dialog->show_dialog( nullptr );
		}
		else if ( _editing_property_field->_reflection_property->_view == data_view_e_file_path )
		{
			string16_c value_as_string16;
			if ( _editing_property_field->_reflection_property->_type == data_type_e_string8 )
			{
				string8_c value_as_string8;
				_editing_property_field->_reflection_property->_accessors._value_getter( _reflection_object, &value_as_string8, _editing_property_field->_reflection_property->_type, _editing_property_field->_reflection_property->_type_count );
				value_as_string16 = value_as_string8;
			}
			else if ( _editing_property_field->_reflection_property->_type == data_type_e_string16 )
			{
				_editing_property_field->_reflection_property->_accessors._value_getter( _reflection_object, &value_as_string16, _editing_property_field->_reflection_property->_type, _editing_property_field->_reflection_property->_type_count );
			}
			string16_c folder_path = ops::path_get_folder_path( value_as_string16 );
			string16_c file_name = ops::path_get_file_or_folder_name( value_as_string16 );
			_file_picker_dialog->set_mode( menu_control_window_file_picker_c::mode_e_load );
			_file_picker_dialog->clear_history();
			_file_picker_dialog->go_to_folder_path( folder_path );
			_file_picker_dialog->set_file_name( file_name );
			_file_picker_dialog->show_dialog( nullptr );
		}
	}

	void_c menu_control_property_inspector_c::_handle_color_picker_on_value_changed( menu_control_window_color_picker_c * color_picker_panel )
	{
		assert( _editing_property_field );
		reflection_value_container_c value;
		vector64x4_c value_as_color = color_picker_panel->get_rgba();
		reflection_convert_color_to_value( _editing_property_field->_reflection_property, value, value_as_color );
		reflection_set_object_property_value( _reflection_object, _editing_property_field->_reflection_property, value );
		_update_ui_from_property( _editing_property_field );
	}

	void_c menu_control_property_inspector_c::_handle_item_list_on_selected_item_changed( menu_control_list_c * list )
	{
		if ( !_is_muted )
		{
			property_field_c * property_field = static_cast< property_field_c * >( list->get_user_pointer() );
			reflection_object_c * reflection_object = property_field->_reflection_property->_accessors._object_list_item_getter( _reflection_object, list->get_selected_item_index() );
			if ( _property_inspector_window )
			{
				_property_inspector_window->show_dialog( nullptr );
				_property_inspector->bind_reflection_object( reflection_object );
			}
		}
	}

	void_c menu_control_property_inspector_c::_handle_item_add_on_clicked( menu_event_information_c event_information )
	{
		property_field_c * property_field = static_cast< property_field_c * >( event_information.control->get_user_pointer() );

		sint32_c at_index = property_field->_item_list_control->get_selected_item_index();
		if ( at_index < 0 )
		{
			at_index = 0;
		}

		reflection_object_c * new_reflection_object = property_field->_reflection_property->_accessors._object_list_item_adder( _reflection_object, at_index );
		menu_control_list_item_text_c * item = new menu_control_list_item_text_c();
		item->set_name( string8_c( "list_item", core_list_mode_e_static ) );
		item->set_user_pointer( new_reflection_object );
		item->set_plain_text_value( new_reflection_object->get_reflection_display_name() );
		property_field->_item_list_control->add_item( item, at_index );
		property_field->_item_list_control->set_selected_item( item );
	}

	void_c menu_control_property_inspector_c::_handle_item_remove_on_clicked( menu_event_information_c event_information )
	{
		if ( _message_dialog->get_is_showed() || _color_picker_dialog->get_is_showed() || _file_picker_dialog->get_is_showed() || _text_editor_dialog->get_is_showed() ) // an operation is already pending.
		{
			return;
		}

		assert( _editing_property_field == nullptr );

		_editing_property_field = reinterpret_cast< property_field_c * >( event_information.control->get_user_pointer() );
		assert( _editing_property_field->_item_list_control );
		_pending_delete_list_item_index = _editing_property_field->_item_list_control->get_selected_item_index();

		_message_dialog->set_message_text_value( string16_c( L"are you sure you want to delete the selected item?", core_list_mode_e_static ) );
		_message_dialog->set_is_showed( true );
	}

	void_c menu_control_property_inspector_c::_handle_item_move_up_on_clicked( menu_event_information_c event_information )
	{
		property_field_c * property_field = static_cast< property_field_c * >( event_information.control->get_user_pointer() );
		if ( property_field->_item_list_control->get_selected_item_index() >= 1 )
		{
			int at_index = property_field->_item_list_control->get_selected_item_index();
			if ( property_field->_reflection_property->_accessors._object_list_item_mover( _reflection_object, at_index, at_index - 1 ) )
			{
				menu_control_list_item_i * item = property_field->_item_list_control->get_item_at_index( at_index );
				item->add_reference(); // we don't want it to die on us when we remove it from the list.
				property_field->_item_list_control->remove_item( item );
				property_field->_item_list_control->add_item( item, at_index - 1 );
				item->remove_reference();
			}
		}
	}

	void_c menu_control_property_inspector_c::_handle_item_move_down_on_clicked( menu_event_information_c event_information )
	{
		property_field_c * property_field = static_cast< property_field_c * >( event_information.control->get_user_pointer() );
		if ( property_field->_item_list_control->get_selected_item_index() + 1 < property_field->_item_list_control->get_item_count() )
		{
			int at_index = property_field->_item_list_control->get_selected_item_index();
			if ( property_field->_reflection_property->_accessors._object_list_item_mover( _reflection_object, at_index, at_index + 1 ) )
			{
				menu_control_list_item_i * item = property_field->_item_list_control->get_item_at_index( at_index );
				item->add_reference(); // we don't want it to die on us when we remove it from the list.
				property_field->_item_list_control->remove_item( item );
				property_field->_item_list_control->add_item( item, at_index + 1 );
				item->remove_reference();
			}
		}
	}

	void_c menu_control_property_inspector_c::_handle_item_sort_on_clicked( menu_event_information_c event_information )
	{
		property_field_c * property_field = static_cast< property_field_c * >( event_information.control->get_user_pointer() );
		property_field->_reflection_property->_accessors._object_list_item_sorter( _reflection_object );
		_update_ui_from_property( property_field );
	}

	void_c menu_control_property_inspector_c::_handle_dialog_on_result( menu_control_window_c * window )
	{
		assert( _editing_property_field );
		reflection_value_container_c new_value; // might be new value, or old one of canceling.
		if ( window == _message_dialog )
		{
			assert( _pending_delete_list_item_index >= 0 );
			if ( window->get_dialog_result() == menu_dialog_result_e_okay )
			{
				_editing_property_field->_reflection_property->_accessors._object_list_item_remover( _reflection_object, _pending_delete_list_item_index );
			}
			_pending_delete_list_item_index = -1;
		}
		else if ( window == _color_picker_dialog )
		{
			if ( window->get_dialog_result() == menu_dialog_result_e_okay )
			{
				_update_property_from_ui( _editing_property_field, _color_picker_dialog, true );
			}
			else
			{
				// handle cancel, since color picker applies changes in real time we need to revert any changes that were made.
				reflection_convert_color_to_value( _editing_property_field->_reflection_property, new_value, _editing_value_as_color_original );
				reflection_set_object_property_value( _reflection_object, _editing_property_field->_reflection_property, new_value );
				_update_ui_from_property( _editing_property_field );
			}
		}
		else if ( window == _file_picker_dialog )
		{
			if ( window->get_dialog_result() == menu_dialog_result_e_okay )
			{
				new_value.string16 = _file_picker_dialog->get_file_path_absolute();
				if ( _editing_property_field->_reflection_property->_type == data_type_e_string8 )
				{
					new_value.string8 = new_value.string16;
				}
				reflection_set_object_property_value( _reflection_object, _editing_property_field->_reflection_property, new_value );
			}
		}
		else if ( window == _text_editor_dialog )
		{
			if ( window->get_dialog_result() == menu_dialog_result_e_okay )
			{
				new_value.string16 = _text_editor_dialog->get_plain_text_value();
				if ( _editing_property_field->_reflection_property->_type == data_type_e_string8 )
				{
					new_value.string8 = new_value.string16;
				}
				reflection_set_object_property_value( _reflection_object, _editing_property_field->_reflection_property, new_value );
			}
		}

		_editing_property_field = nullptr;

		window->hide_dialog();
	}

	menu_control_property_inspector_c::menu_control_property_inspector_c( menu_control_property_inspector_c * mother_property_inspector, reflection_class_c const * fixed_reflection_class )
		: menu_control_panel_i()
		, _mother_property_inspector( mother_property_inspector )
		, _fixed_reflection_class( fixed_reflection_class )
		, _reflection_object( nullptr )
		//, _bound_database_record( nullptr )
		, _property_field_list()
		, _editing_property_field( nullptr )
		, _editing_value_as_color_original()
		, _pending_delete_list_item_index( -1 )
		, _message_dialog( nullptr )
		, _color_picker_dialog( nullptr )
		, _file_picker_dialog( nullptr )
		, _text_editor_dialog( nullptr )
		, _property_inspector_window( nullptr )
		, _property_inspector( nullptr )
		, _y( 0.0f )
		, _is_muted( false )
	{
		_message_dialog = new menu_control_window_message_c();
		_message_dialog->set_name( string8_c( "message_dialog", core_list_mode_e_static ) );
		_message_dialog->add_reference();
		_message_dialog->set_is_showed_immediately( false );
		_add_supplemental_control( _message_dialog );

		_color_picker_dialog = new menu_control_window_color_picker_c();
		_color_picker_dialog->set_name( string8_c( "color_picker_dialog", core_list_mode_e_static ) );
		_color_picker_dialog->add_reference();
		_color_picker_dialog->set_is_showed_immediately( false );
		_add_supplemental_control( _color_picker_dialog );
		
		_file_picker_dialog = new menu_control_window_file_picker_c();
		_file_picker_dialog->set_name( string8_c( "file_picker_dialog", core_list_mode_e_static ) );
		_file_picker_dialog->add_reference();
		_file_picker_dialog->set_is_showed_immediately( false );
		_add_supplemental_control( _file_picker_dialog );

		_text_editor_dialog = new menu_control_window_text_editor_c();
		_text_editor_dialog->set_name( string8_c( "text_editor_dialog", core_list_mode_e_static ) );
		_text_editor_dialog->add_reference();
		_text_editor_dialog->set_is_showed_immediately( false );
		_add_supplemental_control( _text_editor_dialog );

		//if ( _fixed_reflection_class )
		//{
		//	_add( _fixed_reflection_class );
		//}

		set_style_map_key( string8_c( "e_property_inspector", core_list_mode_e_static ) );
	}

	menu_control_property_inspector_c::~menu_control_property_inspector_c()
	{
		assert( _mother_user_interface == nullptr );

		_clear();

		_message_dialog->remove_reference();
		_message_dialog = nullptr;

		_color_picker_dialog->remove_reference();
		_color_picker_dialog = nullptr;

		_file_picker_dialog->remove_reference();
		_file_picker_dialog = nullptr;

		_text_editor_dialog->remove_reference();
		_text_editor_dialog = nullptr;

		if ( _property_inspector_window )
		{
			_property_inspector_window->remove_reference(); // this will also remove the reference on _property_inspector.
			_property_inspector_window = nullptr;
			_property_inspector = nullptr;
		}
	}

	void_c menu_control_property_inspector_c::bind_reflection_object( reflection_object_c * value )
	{
		_reflection_object = value;
		_clear();
		if ( _reflection_object )
		{
			_add( _reflection_object->_reflection_class );
			refresh_ui();
		}
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
