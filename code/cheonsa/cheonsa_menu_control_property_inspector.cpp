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
		, _property_inspector( nullptr )
	{
	}

	menu_control_property_inspector_c::property_field_c::~property_field_c()
	{
	}

	void_c menu_control_property_inspector_c::_layout_controls_for_property( property_field_c * property_field )
	{
		assert( property_field->_label_control );
		if ( property_field->_reflection_property )
		{
			if ( property_field->_reflection_property->get_type() != data_type_e_category_label )
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
						property_field->_button_control->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right, box32x2_c( styled_properties.width_for_property_control, _y, 0.0f, styled_properties.height_for_property_row ) );
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

					_y = ops::math_maximum( y, _y + styled_properties.height_for_property_list + styled_properties.height_for_property_row_padding );
				}

				if ( property_field->_property_inspector )
				{
					float32_c height = property_field->_property_inspector->get_content_bounds().get_height();
					property_field->_property_inspector->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( 8.0f, _y, 0.0f, height ) );
					_y += height + styled_properties.height_for_property_row_padding;
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

	void_c menu_control_property_inspector_c::_clear_controls()
	{
		// cancel any currently open editing operations and dialogs.
		_editing_property_field = nullptr;
		_editing_value_as_color_original = vector64x4_c( 0.0f, 0.0f, 0.0f, 0.0f );
		_editing_item_index = -1;
		if ( _message_dialog )
		{
			_message_dialog->hide_dialog( true );
			_message_dialog = nullptr;
		}
		if ( _color_picker_dialog )
		{
			_color_picker_dialog->hide_dialog( true );
			_color_picker_dialog = nullptr;
		}
		if ( _file_picker_dialog )
		{
			_file_picker_dialog->hide_dialog( true );
			_file_picker_dialog = nullptr;
		}
		if ( _text_editor_dialog )
		{
			_text_editor_dialog->hide_dialog( true );
			_text_editor_dialog = nullptr;
		}

		if ( _fixed_reflection_class == nullptr )
		{
			// remove property controls.
			_client->remove_all_daughter_controls();
			_property_field_list.remove_and_delete_all();
		}
		else
		{
			// disable property controls.
			for ( sint32_c i = 0; i < _property_field_list.get_length(); i++ )
			{
				property_field_c * property_field = _property_field_list[ i ];
				_update_ui_from_property( property_field );
			}
		}
		
		_y = 0.0f;
	}

	void_c menu_control_property_inspector_c::_add_controls( reflection_class_c const * reflection_class )
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
			_property_field_list.insert( -1, property_field );

			property_field->_reflection_property = reflection_class->get_property( i );
			if ( property_field->_reflection_property->get_type() == data_type_e_category_label )
			{
				property_field->_label_control = new menu_control_label_c();
				property_field->_label_control->set_name( string8_c( "category_label", core_list_mode_e_static ) );
				property_field->_label_control->set_user_pointer( property_field );
				property_field->_label_control->set_plain_text_value( string16_c( property_field->_reflection_property->get_name() ) );
				property_field->_label_control->set_style_map_key( string8_c( "e_label_head", core_list_mode_e_static ) );
				_client->add_daughter_control( property_field->_label_control );
			}
			else
			{
				property_field->_label_control = new menu_control_label_c();
				property_field->_label_control->set_name( string8_c( "property_label", core_list_mode_e_static ) );
				property_field->_label_control->set_user_pointer( property_field );
				property_field->_label_control->set_plain_text_value( string16_c( property_field->_reflection_property->get_name() ) );
				_client->add_daughter_control( property_field->_label_control );

				// create text control if needed.
				if ( property_field->_reflection_property->get_type() >= data_type_e_string8 && property_field->_reflection_property->get_type() <= data_type_e_float64 )
				{
					if ( property_field->_reflection_property->get_view() != data_view_e_enumeration && property_field->_reflection_property->get_view() != data_view_e_enumeration_flags )
					{
						property_field->_text_control = new menu_control_text_c();
						property_field->_text_control->set_name( string8_c( "property_value_text", core_list_mode_e_static ) );
						property_field->_text_control->set_user_pointer( property_field );
						if ( property_field->_reflection_property->get_view() == data_view_e_text )
						{
							property_field->_text_control->set_text_interact_mode( menu_text_interact_mode_e_static );
						}
						else
						{
							//property_field->_text_control->on_plain_text_value_changed_preview.subscribe( this, &menu_control_property_inspector_c::_handle_value_text_on_plain_text_value_changed_preview );
							property_field->_text_control->on_plain_text_value_changed.subscribe( this, &menu_control_property_inspector_c::_handle_value_text_on_plain_text_value_changed );
						}
						_client->add_daughter_control( property_field->_text_control );
					}
				}

				// create button control if needed.
				if ( property_field->_reflection_property->get_view() == data_view_e_color || property_field->_reflection_property->get_view() == data_view_e_text || property_field->_reflection_property->get_view() == data_view_e_file_path || property_field->_reflection_property->get_type() == data_type_e_object )
				{
					property_field->_button_control = new menu_control_button_c();
					property_field->_button_control->set_name( string8_c( "property_value_button", core_list_mode_e_static ) );
					property_field->_button_control->set_user_pointer( property_field );
					property_field->_button_control->set_plain_text_value( string16_c( L"...", core_list_mode_e_static ) );
					property_field->_button_control->on_clicked.subscribe( this, &menu_control_property_inspector_c::_handle_value_edit_on_clicked );
					_client->add_daughter_control( property_field->_button_control );
				}

				// create scroll control if needed.
				if ( property_field->_reflection_property->get_view() == data_view_e_scroll_bar || property_field->_reflection_property->get_view() == data_view_e_scrub_bar )
				{
					assert( property_field->_reflection_property->get_type() == data_type_e_float32 || property_field->_reflection_property->get_type() == data_type_e_float64 );
					assert( property_field->_reflection_property->get_type_count() == 1 );
					float64_c value_minimum = 0.0;
					float64_c value_maximum = 0.0;
					if ( property_field->_reflection_property->get_type() == data_type_e_float32 )
					{
						value_minimum = property_field->_reflection_property->_defaults_and_limits._float32_minimum;
						value_maximum = property_field->_reflection_property->_defaults_and_limits._float32_maximum;
					}
					else //if ( property->_type == data_type_e_float64 )
					{
						value_minimum = property_field->_reflection_property->_defaults_and_limits._float64_minimum;
						value_maximum = property_field->_reflection_property->_defaults_and_limits._float64_maximum;
					}
					assert( value_minimum < value_maximum );
					if ( property_field->_reflection_property->get_view() == data_view_e_scroll_bar )
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
					_client->add_daughter_control( property_field->_scroll_bar_control );
				}

				// create combo control if needed.
				if ( property_field->_reflection_property->get_view() == data_view_e_enumeration || property_field->_reflection_property->get_view() == data_view_e_enumeration_flags )
				{
					property_field->_combo_button_control = new menu_control_combo_button_c();
					property_field->_combo_button_control->set_name( string8_c( "property_value_combo_button", core_list_mode_e_static ) );

					menu_control_combo_c * combo = new menu_control_combo_c();
					combo->set_name( string8_c( "property_value_combo", core_list_mode_e_static ) );
					combo->set_user_pointer( property_field );
					combo->set_layout_simple( box32x2_c( 0.0f, 0.0f, _local_box.get_width(), 100.0f ) );
					property_field->_combo_button_control->set_combo_control( combo );
					if ( property_field->_reflection_property->get_view() == data_view_e_enumeration_flags )
					{
						combo->set_selected_item_limit( -1 );
					}

					reflection_enumeration_c const * enumeration = property_field->_reflection_property->get_type_enumeration();
					assert( enumeration );
					for ( sint32_c j = 0; j < enumeration->get_value_count(); j++ ) // populate combo control.
					{
						menu_control_combo_item_text_c * item = new menu_control_combo_item_text_c();
						item->set_name( string8_c( "list_item", core_list_mode_e_static ) );
						item->set_plain_text_value( string16_c( enumeration->get_value( j )->get_name() ) );
						if ( property_field->_reflection_property->get_view() == data_view_e_enumeration_flags )
						{
							item->set_can_be_toggled( true );
						}
						combo->add_item( item );
					}

					//combo->on_selected_item_changed_preview.subscribe( this, &menu_control_property_inspector_c::_handle_value_combo_on_selected_item_changed_preview );
					combo->on_selected_item_changed.subscribe( this, &menu_control_property_inspector_c::_handle_value_combo_on_selected_item_changed );
					_client->add_daughter_control( property_field->_combo_button_control );
				}

				// create list and button controls if needed.
				if ( property_field->_reflection_property->get_type() == data_type_e_object_list )
				{
					property_field->_item_list_control = new menu_control_list_c();
					property_field->_item_list_control->set_name( string8_c( "item_list", core_list_mode_e_static ) );
					property_field->_item_list_control->set_user_pointer( property_field );
					property_field->_item_list_control->on_selected_item_list_changed.subscribe( this, &menu_control_property_inspector_c::_handle_item_list_on_selected_item_changed );
					_client->add_daughter_control( property_field->_item_list_control );

					property_field->_item_add_button_control = new menu_control_button_c();
					property_field->_item_add_button_control->set_name( string8_c( "item_add_button", core_list_mode_e_static ) );
					property_field->_item_add_button_control->set_user_pointer( property_field );
					property_field->_item_add_button_control->set_plain_text_value( string16_c( L"+", core_list_mode_e_static ) );
					property_field->_item_add_button_control->on_clicked.subscribe( this, &menu_control_property_inspector_c::_handle_item_add_on_clicked );
					_client->add_daughter_control( property_field->_item_add_button_control );

					property_field->_item_remove_button_control = new menu_control_button_c();
					property_field->_item_remove_button_control->set_name( string8_c( "item_remove_button", core_list_mode_e_static ) );
					property_field->_item_remove_button_control->set_user_pointer( property_field );
					property_field->_item_remove_button_control->set_plain_text_value( string16_c( L"-", core_list_mode_e_static ) );
					property_field->_item_remove_button_control->on_clicked.subscribe( this, &menu_control_property_inspector_c::_handle_item_remove_on_clicked );
					_client->add_daughter_control( property_field->_item_remove_button_control );

					property_field->_item_move_up_button_control = new menu_control_button_c();
					property_field->_item_move_up_button_control->set_name( string8_c( "item_move_up_button", core_list_mode_e_static ) );
					property_field->_item_move_up_button_control->set_user_pointer( property_field );
					property_field->_item_move_up_button_control->set_plain_text_value( string16_c( L"", core_list_mode_e_static ) );
					property_field->_item_move_up_button_control->on_clicked.subscribe( this, &menu_control_property_inspector_c::_handle_item_move_up_on_clicked );
					_client->add_daughter_control( property_field->_item_move_up_button_control );

					property_field->_item_move_down_button_control = new menu_control_button_c();
					property_field->_item_move_down_button_control->set_name( string8_c( "item_move_down_button", core_list_mode_e_static ) );
					property_field->_item_move_down_button_control->set_user_pointer( property_field );
					property_field->_item_move_down_button_control->set_plain_text_value( string16_c( L"", core_list_mode_e_static ) );
					property_field->_item_move_down_button_control->on_clicked.subscribe( this, &menu_control_property_inspector_c::_handle_item_move_down_on_clicked );
					_client->add_daughter_control( property_field->_item_move_down_button_control );

					property_field->_item_sort_button_control = new menu_control_button_c();
					property_field->_item_sort_button_control->set_name( string8_c( "item_sort_button", core_list_mode_e_static ) );
					property_field->_item_sort_button_control->set_user_pointer( property_field );
					property_field->_item_sort_button_control->set_plain_text_value( string16_c( L"sort", core_list_mode_e_static ) );
					property_field->_item_sort_button_control->on_clicked.subscribe( this, &menu_control_property_inspector_c::_handle_item_sort_on_clicked );
					_client->add_daughter_control( property_field->_item_sort_button_control );
				}

				if ( property_field->_reflection_property->get_type() == data_type_e_object || property_field->_reflection_property->get_type() == data_type_e_object_list )
				{
					property_field->_property_inspector = new menu_control_property_inspector_c( this, property_field->_reflection_property->get_type_reflection_class() );
					property_field->_property_inspector->set_name( string8_c( "property_inspector", core_list_mode_e_static ) );
					property_field->_property_inspector->set_user_pointer( property_field );
					_client->add_daughter_control( property_field->_property_inspector );
				}
			}
			_layout_controls_for_property( property_field );
		}

		_is_muted = false;
	}

	void_c menu_control_property_inspector_c::_update_ui_from_property( property_field_c * property_field )
	{
		assert( _is_muted == false );

		if ( _bound_reflection_object == nullptr )
		{
			// it should only be possible to reach here if _fixed_reflection_class is set.
			assert( _fixed_reflection_class );
			_is_muted = true;
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
			_is_muted = false;
			return;
		}

		// get raw value.
		reflection_value_container_c value;
		if ( !reflection_get_object_property_value( _bound_reflection_object, property_field->_reflection_property, value ) )
		{
			return;
		}

		_is_muted = true;

		// convert value to string if needed.
		if ( property_field->_text_control )
		{
			property_field->_text_control->set_is_enabled( true );

			string16_c value_as_string16;
			value_as_string16 = reflection_convert_value_to_string16( property_field->_reflection_property, value );
			property_field->_text_control->set_plain_text_value( value_as_string16 );
		}

		// provide value to view editors.
		if ( property_field->_scroll_bar_control )
		{
			assert( property_field->_reflection_property->get_view() == data_view_e_scroll_bar || property_field->_reflection_property->get_view() == data_view_e_scrub_bar );
			assert( property_field->_reflection_property->get_type_count() == 1 );

			property_field->_scroll_bar_control->set_is_enabled( true );

			if ( property_field->_reflection_property->get_type() == data_type_e_float32 )
			{
				property_field->_scroll_bar_control->set_value( static_cast< float64_c >( value.float32[ 0 ] ) );
			}
			else if ( property_field->_reflection_property->get_type() == data_type_e_float64 )
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
			assert( property_field->_reflection_property->get_view() == data_view_e_enumeration || property_field->_reflection_property->get_view() == data_view_e_enumeration_flags );
			assert( property_field->_reflection_property->get_type_count() == 1 );
			assert( property_field->_reflection_property->get_type_enumeration() );

			property_field->_combo_button_control->set_is_enabled( true );

			sint64_c property_value = 0;
			if ( property_field->_reflection_property->get_type() == data_type_e_uint8 )
			{
				property_value = static_cast< sint64_c >( value.uint8[ 0 ] );
			}
			else if ( property_field->_reflection_property->get_type() == data_type_e_sint8 )
			{
				property_value = static_cast< sint64_c >( value.sint8[ 0 ] );
			}
			else if ( property_field->_reflection_property->get_type() == data_type_e_uint16 )
			{
				property_value = static_cast< sint64_c >( value.uint16[ 0 ] );
			}
			else if ( property_field->_reflection_property->get_type() == data_type_e_sint16 )
			{
				property_value = static_cast< sint64_c >( value.sint16[ 0 ] );
			}
			else if ( property_field->_reflection_property->get_type() == data_type_e_uint32 )
			{
				property_value = static_cast< sint64_c >( value.uint32[ 0 ] );
			}
			else if ( property_field->_reflection_property->get_type() == data_type_e_sint32 )
			{
				property_value = static_cast< sint64_c >( value.sint32[ 0 ] );
			}
			else if ( property_field->_reflection_property->get_type() == data_type_e_uint64 )
			{
				property_value = static_cast< uint64_c >( value.uint64[ 0 ] );
			}
			else if ( property_field->_reflection_property->get_type() == data_type_e_sint64 )
			{
				property_value = static_cast< sint64_c >( value.sint64[ 0 ] );
			}
			else
			{
				assert( false );
			}

			if ( property_field->_reflection_property->get_view() == data_view_e_enumeration )
			{
				sint32_c enumeration_value_index = -1;
				property_field->_reflection_property->get_type_enumeration()->find_index_with_value( enumeration_value_index, property_value ); // if this returns false then enum_index will be set to -1, which is still valid to set no selection in the combo list.
				property_field->_combo_button_control->get_combo_control()->set_selected_item_index( enumeration_value_index );
			}
			else if ( property_field->_reflection_property->get_view() == data_view_e_enumeration_flags )
			{
				for ( sint32_c i = 0; i < property_field->_combo_button_control->get_combo_control()->get_item_count(); i++ )
				{
					menu_control_combo_item_text_c * combo_item = static_cast< menu_control_combo_item_text_c * >( property_field->_combo_button_control->get_combo_control()->get_item_at_index( i ) );
					sint64_c enumeration_value = 0;
					property_field->_reflection_property->get_type_enumeration()->find_value_with_index( enumeration_value, i );
					combo_item->set_is_selected( ( property_value & enumeration_value ) != 0, true );
				}
			}
		}
		else if ( property_field->_reflection_property->get_type() == data_type_e_object )
		{
			if ( property_field->_property_inspector )
			{
				reflection_object_c * reflection_object = property_field->_reflection_property->_accessors._object_getter( _bound_reflection_object );
				property_field->_property_inspector->set_bound_reflection_object( reflection_object );
			}
		}
		else if ( property_field->_reflection_property->get_type() == data_type_e_object_list )	
		{
			assert( property_field->_item_list_control );

			property_field->_item_list_control->set_is_enabled( true );

			property_field->_item_list_control->remove_all_items();
			sint32_c item_count = property_field->_reflection_property->_accessors._object_list_item_count_getter( _bound_reflection_object );
			for ( sint32_c i = 0; i < item_count; i++ )
			{
				reflection_object_c * reflection_object = property_field->_reflection_property->_accessors._object_list_item_getter( _bound_reflection_object, i );
				menu_control_list_item_text_c * item = new menu_control_list_item_text_c();
				item->set_name( string8_c( "list_item", core_list_mode_e_static ) );
				item->set_plain_text_value( reflection_object->get_reflection_display_name() );
				property_field->_item_list_control->add_item( item );
			}

			if ( property_field->_item_add_button_control && property_field->_item_remove_button_control )
			{
				if ( property_field->_reflection_property->_accessors._object_list_item_adder && property_field->_reflection_property->_accessors._object_list_item_remover )
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
				if ( property_field->_reflection_property->_accessors._object_list_item_mover )
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
				if ( property_field->_reflection_property->_accessors._object_list_item_sorter )
				{
					property_field->_item_sort_button_control->set_is_enabled( true );
				}
				else
				{
					property_field->_item_sort_button_control->set_is_enabled( false );
				}
			}

			if ( property_field->_property_inspector )
			{
				property_field->_property_inspector->set_bound_reflection_object( nullptr );
			}
		}

		_is_muted = false;
	}

	void_c menu_control_property_inspector_c::_update_property_from_ui( property_field_c * property_field, menu_control_c * control, boolean_c and_commit )
	{
		assert( property_field && control );

		if ( _bound_reflection_object == nullptr )
		{
			// because sometimes this can get called when one of our text boxes looses keyboard focus after the selection is cleared.
			return;
		}

		if ( property_field->_text_control == nullptr && property_field->_combo_button_control == nullptr )
		{
			return;
		}

		reflection_property_c const * property = property_field->_reflection_property;

		reflection_value_container_c before_value; // old or current value.
		reflection_get_object_property_value( _bound_reflection_object, property, before_value );

		reflection_value_container_c after_value; // new value.
		if ( control == property_field->_scroll_bar_control )
		{
		}
		else if ( property_field->_combo_button_control && control == property_field->_combo_button_control->get_combo_control() )
		{
			assert( property->get_view() == data_view_e_enumeration || property->get_view() == data_view_e_enumeration_flags );
			assert( property->get_type_count() == 1 );
			assert( property->get_type_enumeration() );

			sint64_c property_value = 0;
			if ( property->get_view() == data_view_e_enumeration )
			{
				assert( property->get_type_enumeration()->find_value_with_index( property_value, property_field->_combo_button_control->get_combo_control()->get_selected_item_index() ) );
			}
			else if ( property->get_view() == data_view_e_enumeration_flags )
			{
				for ( sint32_c i = 0; i < property_field->_combo_button_control->get_combo_control()->get_item_count(); i++ )
				{
					sint64_c flag_value = 0;
					menu_control_combo_item_text_c * combo_item = static_cast< menu_control_combo_item_text_c * >( property_field->_combo_button_control->get_combo_control()->get_item_at_index( i ) );
					if ( combo_item->get_is_selected() )
					{
						property->get_type_enumeration()->find_value_with_index( flag_value, i );
						property_value  |= flag_value;
					}
				}
			}

			if ( property->get_type() == data_type_e_uint8 )
			{
				after_value.uint8[ 0 ] = static_cast< uint8_c >( property_value );
			}
			else if ( property->get_type() == data_type_e_sint8 )
			{
				after_value.sint8[ 0 ] = static_cast< sint8_c >( property_value );
			}
			else if ( property->get_type() == data_type_e_uint16 )
			{
				after_value.uint16[ 0 ] = static_cast< uint16_c >( property_value );
			}
			else if ( property->get_type() == data_type_e_sint16 )
			{
				after_value.sint16[ 0 ] = static_cast< sint16_c >( property_value );
			}
			else if ( property->get_type() == data_type_e_uint32 )
			{
				after_value.uint32[ 0 ] = static_cast< uint32_c >( property_value );
			}
			else if ( property->get_type() == data_type_e_sint32 )
			{
				after_value.sint32[ 0 ] = static_cast< sint32_c >( property_value );
			}
			else if ( property->get_type() == data_type_e_uint64 )
			{
				after_value.uint64[ 0 ] = static_cast< uint64_c >( property_value );
			}
			else if ( property->get_type() == data_type_e_sint64 )
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
			if ( property_field->_reflection_property->get_type() == data_type_e_string8 )
			{
				after_value.string8 = after_value.string16;
			}
		}
		else if ( control == _text_editor_dialog )
		{
			after_value.string16 = _text_editor_dialog->get_plain_text_value();
			if ( property_field->_reflection_property->get_type() == data_type_e_string8 )
			{
				after_value.string8 = after_value.string16;
			}
			reflection_set_object_property_value( _bound_reflection_object, property_field->_reflection_property, after_value );
		}
		else
		{
			assert( false );
		}

		if ( reflection_set_object_property_value( _bound_reflection_object, property, after_value ) )
		{
			if ( and_commit == true )
			{
				if ( !reflection_compare_values( property, before_value, after_value ) )
				{
					reflection_property_value_changed_information_c reflection_property_value_changed_information;
					reflection_property_value_changed_information.reflection_object = _bound_reflection_object;
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

	void_c menu_control_property_inspector_c::_handle_value_text_on_plain_text_value_changed_preview( menu_control_text_c * text )
	{
		if ( !_is_muted )
		{
			_update_property_from_ui( reinterpret_cast< property_field_c * >( text->get_user_pointer() ), text, false );
		}
	}

	void_c menu_control_property_inspector_c::_handle_value_text_on_plain_text_value_changed( menu_control_text_c * text )
	{
		if ( !_is_muted )
		{
			_update_property_from_ui( reinterpret_cast< property_field_c * >( text->get_user_pointer() ), text, true );
		}
	}

	void_c menu_control_property_inspector_c::_handle_value_scroll_on_value_changed_preview( menu_control_scroll_bar_i * scroll )
	{
		if ( !_is_muted )
		{
			_update_property_from_ui( reinterpret_cast< property_field_c * >( scroll->get_user_pointer() ), scroll, false );
		}
	}

	void_c menu_control_property_inspector_c::_handle_value_scroll_on_value_changed( menu_control_scroll_bar_i * scroll )
	{
		if ( !_is_muted )
		{
			_update_property_from_ui( reinterpret_cast< property_field_c * >( scroll->get_user_pointer() ), scroll, true );
		}
	}

	void_c menu_control_property_inspector_c::_handle_value_combo_on_selected_item_changed_preview( menu_control_combo_c * combo_list )
	{
		if ( !_is_muted )
		{
			_update_property_from_ui( reinterpret_cast< property_field_c * >( combo_list->get_user_pointer() ), combo_list, false );
		}
	}
	
	void_c menu_control_property_inspector_c::_handle_value_combo_on_selected_item_changed( menu_control_combo_c * combo_list )
	{
		if ( !_is_muted )
		{
			_update_property_from_ui( reinterpret_cast< property_field_c * >( combo_list->get_user_pointer() ), combo_list, true );
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
		reflection_get_object_property_value( _bound_reflection_object, _editing_property_field->_reflection_property, value );

		if ( _editing_property_field->_reflection_property->get_view() == data_view_e_color )
		{
			assert( _color_picker_dialog == nullptr );

			_color_picker_dialog = new menu_control_window_color_picker_c();
			_color_picker_dialog->set_name( string8_c( "color_picker_dialog", core_list_mode_e_static ) );
			_color_picker_dialog->set_is_showed_immediately( false );
			_editing_value_as_color_original = reflection_convert_value_to_color( _editing_property_field->_reflection_property, value );
			if ( _editing_property_field->_reflection_property->get_type() == data_type_e_uint32 || _editing_property_field->_reflection_property->get_type_count() == 4 )
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

			add_supplemental_daughter_control( _color_picker_dialog );
			_color_picker_dialog->show_dialog( nullptr );
			_color_picker_dialog->center();
			_color_picker_dialog->on_dialog_result.subscribe( this, &menu_control_property_inspector_c::_handle_dialog_on_result );
		}
		else if ( _editing_property_field->_reflection_property->get_view() == data_view_e_text )
		{
			assert( _text_editor_dialog == nullptr );

			_text_editor_dialog = new menu_control_window_text_editor_c();
			_text_editor_dialog->set_name( string8_c( "text_editor_dialog", core_list_mode_e_static ) );
			_text_editor_dialog->set_is_showed_immediately( false );

			string16_c value_as_string16;
			if ( _editing_property_field->_reflection_property->get_type() == data_type_e_string8 )
			{
				string8_c value_as_string8;
				_editing_property_field->_reflection_property->_accessors._value_getter( _bound_reflection_object, &value_as_string8, _editing_property_field->_reflection_property->_type, _editing_property_field->_reflection_property->_type_count );
				value_as_string16 = value_as_string8;
			}
			else if ( _editing_property_field->_reflection_property->get_type() == data_type_e_string16 )
			{
				_editing_property_field->_reflection_property->_accessors._value_getter( _bound_reflection_object, &value_as_string16, _editing_property_field->_reflection_property->_type, _editing_property_field->_reflection_property->_type_count );
			}
			_text_editor_dialog->set_plain_text_value( value_as_string16 );

			add_supplemental_daughter_control( _text_editor_dialog );
			_text_editor_dialog->show_dialog( nullptr );
			_text_editor_dialog->center();
			_text_editor_dialog->on_dialog_result.subscribe( this, &menu_control_property_inspector_c::_handle_dialog_on_result );
		}
		else if ( _editing_property_field->_reflection_property->get_view() == data_view_e_file_path )
		{
			assert( _file_picker_dialog == nullptr );

			string16_c value_as_string16;
			if ( _editing_property_field->_reflection_property->get_type() == data_type_e_string8 )
			{
				string8_c value_as_string8;
				_editing_property_field->_reflection_property->_accessors._value_getter( _bound_reflection_object, &value_as_string8, _editing_property_field->_reflection_property->_type, _editing_property_field->_reflection_property->_type_count );
				value_as_string16 = value_as_string8;
			}
			else if ( _editing_property_field->_reflection_property->get_type() == data_type_e_string16 )
			{
				_editing_property_field->_reflection_property->_accessors._value_getter( _bound_reflection_object, &value_as_string16, _editing_property_field->_reflection_property->_type, _editing_property_field->_reflection_property->_type_count );
			}
			string16_c folder_path = ops::path_get_folder_path( value_as_string16 );
			string16_c file_name = ops::path_get_file_or_folder_name( value_as_string16 );

			_file_picker_dialog = new menu_control_window_file_picker_c();
			_file_picker_dialog->set_name( string8_c( "file_picker_dialog", core_list_mode_e_static ) );
			_file_picker_dialog->set_is_showed_immediately( false );
			_file_picker_dialog->set_mode( menu_control_window_file_picker_c::mode_e_load );
			_file_picker_dialog->clear_history();
			_file_picker_dialog->go_to_folder_path( folder_path );
			_file_picker_dialog->set_file_name( file_name );

			add_supplemental_daughter_control( _file_picker_dialog );
			_file_picker_dialog->show_dialog( nullptr );
			_file_picker_dialog->center();
			_file_picker_dialog->on_dialog_result.subscribe( this, &menu_control_property_inspector_c::_handle_dialog_on_result );
		}
	}

	void_c menu_control_property_inspector_c::_handle_color_picker_on_value_changed( menu_control_window_color_picker_c * color_picker_panel )
	{
		assert( _editing_property_field );
		reflection_value_container_c value;
		vector64x4_c value_as_color = color_picker_panel->get_rgba();
		reflection_convert_color_to_value( _editing_property_field->_reflection_property, value, value_as_color );
		reflection_set_object_property_value( _bound_reflection_object, _editing_property_field->_reflection_property, value );
		_update_ui_from_property( _editing_property_field );
	}

	void_c menu_control_property_inspector_c::_handle_item_list_on_selected_item_changed( menu_control_list_c * list )
	{
		if ( !_is_muted )
		{
			property_field_c * property_field = static_cast< property_field_c * >( list->get_user_pointer() );
			reflection_object_c * reflection_object = nullptr;
			if ( list->get_selected_item_index() >= 0 )
			{
				reflection_object = property_field->_reflection_property->_accessors._object_list_item_getter( _bound_reflection_object, list->get_selected_item_index() );
			}
			if ( property_field->_property_inspector )
			{
				property_field->_property_inspector->set_bound_reflection_object( reflection_object );
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

		reflection_object_c * new_reflection_object = property_field->_reflection_property->_accessors._object_list_item_adder( _bound_reflection_object, at_index );
		menu_control_list_item_text_c * item = new menu_control_list_item_text_c();
		item->set_name( string8_c( "list_item", core_list_mode_e_static ) );
		item->set_user_pointer( new_reflection_object );
		item->set_plain_text_value( new_reflection_object->get_reflection_display_name() );
		property_field->_item_list_control->add_item( item, at_index );
		property_field->_item_list_control->set_selected_item( item );
	}

	void_c menu_control_property_inspector_c::_handle_item_remove_on_clicked( menu_event_information_c event_information )
	{
		assert( _message_dialog == nullptr );
		assert( _editing_property_field == nullptr );

		_editing_property_field = reinterpret_cast< property_field_c * >( event_information.control->get_user_pointer() );
		assert( _editing_property_field->_item_list_control );
		_editing_item_index = _editing_property_field->_item_list_control->get_selected_item_index();

		_message_dialog = new menu_control_window_message_c();
		_message_dialog->set_name( string8_c( "message_dialog", core_list_mode_e_static ) );
		_message_dialog->set_is_showed_immediately( false );
		_message_dialog->set_title_bar_text_value( string8_c( "delete?", core_list_mode_e_static ) );
		_message_dialog->set_message_text_value( string8_c( "are you sure you want to delete the selected item?", core_list_mode_e_static ) );

		add_supplemental_daughter_control( _message_dialog );
		_message_dialog->show_dialog( _mother_user_interface->open_modal_screen() );
		_message_dialog->center();
		_message_dialog->on_dialog_result.subscribe( this, &menu_control_property_inspector_c::_handle_dialog_on_result );
	}

	void_c menu_control_property_inspector_c::_handle_item_move_up_on_clicked( menu_event_information_c event_information )
	{
		property_field_c * property_field = static_cast< property_field_c * >( event_information.control->get_user_pointer() );
		if ( property_field->_item_list_control->get_selected_item_index() >= 1 )
		{
			int at_index = property_field->_item_list_control->get_selected_item_index();
			if ( property_field->_reflection_property->_accessors._object_list_item_mover( _bound_reflection_object, at_index, at_index - 1 ) )
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
			if ( property_field->_reflection_property->_accessors._object_list_item_mover( _bound_reflection_object, at_index, at_index + 1 ) )
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
		property_field->_reflection_property->_accessors._object_list_item_sorter( _bound_reflection_object );
		_update_ui_from_property( property_field );
	}

	void_c menu_control_property_inspector_c::_handle_dialog_on_result( menu_control_window_c * window )
	{
		assert( _editing_property_field );
		reflection_value_container_c new_value; // might be new value, or old one of canceling.
		if ( window == _message_dialog )
		{
			assert( _editing_item_index >= 0 );
			if ( window->get_dialog_result() == menu_dialog_result_e_okay )
			{
				_editing_property_field->_reflection_property->_accessors._object_list_item_remover( _bound_reflection_object, _editing_item_index );
			}
			_editing_item_index = -1;
			_message_dialog->hide_dialog( true );
			_message_dialog = nullptr;
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
				reflection_set_object_property_value( _bound_reflection_object, _editing_property_field->_reflection_property, new_value );
				_update_ui_from_property( _editing_property_field );
			}
			_color_picker_dialog->hide_dialog( true );
			_color_picker_dialog = nullptr;
		}
		else if ( window == _file_picker_dialog )
		{
			if ( window->get_dialog_result() == menu_dialog_result_e_okay )
			{
				new_value.string16 = _file_picker_dialog->get_file_path_absolute();
				if ( _editing_property_field->_reflection_property->get_type() == data_type_e_string8 )
				{
					new_value.string8 = new_value.string16;
				}
				reflection_set_object_property_value( _bound_reflection_object, _editing_property_field->_reflection_property, new_value );
			}
			_file_picker_dialog->hide_dialog( true );
			_file_picker_dialog = nullptr;
		}
		else if ( window == _text_editor_dialog )
		{
			if ( window->get_dialog_result() == menu_dialog_result_e_okay )
			{
				new_value.string16 = _text_editor_dialog->get_plain_text_value();
				if ( _editing_property_field->_reflection_property->get_type() == data_type_e_string8 )
				{
					new_value.string8 = new_value.string16;
				}
				reflection_set_object_property_value( _bound_reflection_object, _editing_property_field->_reflection_property, new_value );
			}
			_text_editor_dialog->hide_dialog( true );
			_text_editor_dialog = nullptr;
		}

		_editing_property_field = nullptr;
	}

	void_c menu_control_property_inspector_c::_on_input( input_event_c * input_event )
	{
		on_input.invoke( menu_event_information_c( this, nullptr, input_event ) );

		if ( !input_event->get_was_handled() )
		{
			if ( input_event->get_type() == input_event_c::type_e_mouse_wheel )
			{
				_vertical_scroll_bar->inject_mouse_wheel_input( input_event->get_mouse_wheel_delta() );
			}
		}
	}

	menu_control_property_inspector_c::menu_control_property_inspector_c( menu_control_property_inspector_c * mother_property_inspector, reflection_class_c const * fixed_reflection_class )
		: menu_control_panel_i()
		, _mother_property_inspector( mother_property_inspector )
		, _fixed_reflection_class( fixed_reflection_class )
		, _bound_reflection_object( nullptr )
		//, _bound_database_record( nullptr )
		, _property_field_list()
		, _editing_property_field( nullptr )
		, _editing_value_as_color_original()
		, _editing_item_index( -1 )
		, _message_dialog( nullptr )
		, _color_picker_dialog( nullptr )
		, _file_picker_dialog( nullptr )
		, _text_editor_dialog( nullptr )
		, _y( 0.0f )
		, _is_muted( false )
	{
		sint32_c depth = 0;
		menu_control_property_inspector_c * mother = _mother_property_inspector;
		while ( mother )
		{
			depth++;
			mother = mother->_mother_property_inspector;
		}
		assert( depth < 8 ); // high depth may indicate infinite recursion.

		styled_properties.height_for_class_row = 20.0f;
		styled_properties.height_for_class_row_padding = 5.0f;
		styled_properties.height_for_category_row = 20.0f;
		styled_properties.height_for_category_row_padding = 5.0f;
		styled_properties.height_for_property_row = 20.0f;
		styled_properties.height_for_property_row_padding = 5.0f;
		styled_properties.height_for_property_list = 300.0f;
		styled_properties.width_for_property_control = 20.0f;
		styled_properties.width_for_property_control_padding = 5.0f;

		_set_vertical_scroll_bar_visibility_mode( menu_visibility_mode_e_automatic );

		set_style_map_key( string8_c( "e_panel_borderless", core_list_mode_e_static ) );

		if ( _fixed_reflection_class )
		{
			_add_controls( _fixed_reflection_class );
		}
	}

	menu_control_property_inspector_c::~menu_control_property_inspector_c()
	{
		assert( _mother_user_interface == nullptr );

		_clear_controls();
	}

	void_c menu_control_property_inspector_c::update_animations( float32_c time_step )
	{
		_frame_element .set_is_selected( _is_mouse_focused || _is_text_focused );
		_update_scroll_bar_visibility();
		menu_control_c::update_animations( time_step );
	}

	void_c menu_control_property_inspector_c::set_bound_reflection_object( reflection_object_c * value )
	{
		_bound_reflection_object = value;
		if ( _fixed_reflection_class == nullptr )
		{
			_clear_controls();
		}
		if ( _bound_reflection_object && _fixed_reflection_class == nullptr )
		{
			_add_controls( _bound_reflection_object->_reflection_class );
		}
		if ( _bound_reflection_object && _fixed_reflection_class )
		{
			assert( _bound_reflection_object->get_reflection_class() == _fixed_reflection_class );
		}
		refresh_ui();
	}

	reflection_object_c * menu_control_property_inspector_c::get_bound_reflection_object() const
	{
		return _bound_reflection_object;
	}

	void_c menu_control_property_inspector_c::refresh_ui()
	{
		for ( sint32_c i = 0; i < _property_field_list.get_length(); i++ )
		{
			_update_ui_from_property( _property_field_list[ i ] );
		}
	}

}