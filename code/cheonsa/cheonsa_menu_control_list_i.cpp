#include "cheonsa_menu_control_list_i.h"
#include "cheonsa_user_interface.h"
#include "cheonsa_engine.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	void_c menu_control_list_item_i::_set_is_selected( boolean_c value, boolean_c try_to_multi_select )
	{
		assert( _can_be_selected == true ); // this function should only be called if this type of list item is designed to have a toggleable selected state.

		menu_control_list_i * mother_list = dynamic_cast< menu_control_list_i * >( _mother_control->get_mother_control() );
		if ( mother_list )
		{
			if ( _can_be_toggled )
			{
				if ( _is_selected != value )
				{
					_is_selected = value;
					_on_is_selected_changed();
					if ( _is_selected )
					{
						mother_list->_selected_item_list.insert( -1, this );
					}
					else
					{
						assert( mother_list->_selected_item_list.remove_value( this ) );
					}
					mother_list->_on_selected_item_list_changed();
				}
			}
			else
			{
				if ( try_to_multi_select )
				{
					if ( _is_selected != value )
					{
						_is_selected = value;
						if ( _is_selected )
						{
							_is_selected = true;
							_on_is_selected_changed();
							mother_list->_selected_item_list.insert( -1, this );
							mother_list->_deselect_items_over_limit();
							mother_list->_on_selected_item_list_changed();
						}
						else
						{
							_is_selected = false;
							_on_is_selected_changed();
							mother_list->_selected_item_list.remove_value( this );
							mother_list->_on_selected_item_list_changed();
						}
					}
				}
				else
				{
					if ( value )
					{
						// deselect everything except this.
						boolean_c selected_item_list_changed = false;
						for ( sint32_c i = 0; i < mother_list->_selected_item_list.get_length(); i++ )
						{
							menu_control_list_item_i * selected_item = mother_list->_selected_item_list[ i ];
							if ( selected_item != this )
							{
								selected_item_list_changed = true;
								selected_item->_is_selected = false;
								selected_item->_on_is_selected_changed();
							}
						}
						mother_list->_selected_item_list.remove_all();
						mother_list->_selected_item_list.insert( -1, this );
						if ( !_is_selected )
						{
							_is_selected = true;
							_on_is_selected_changed();
						}
						if ( selected_item_list_changed )
						{
							mother_list->_on_selected_item_list_changed();
						}
					}
					else
					{
						// deselect everything including this.
						if ( mother_list->_selected_item_list.get_length() )
						{
							for ( sint32_c i = 0; i < mother_list->_selected_item_list.get_length(); i++ )
							{
								menu_control_list_item_i * selected_item = mother_list->_selected_item_list[ i ];
								selected_item->_is_selected = false;
								selected_item->_on_is_selected_changed();
							}
							mother_list->_selected_item_list.remove_all();
							mother_list->_on_selected_item_list_changed();
						}
					}
				}
			}
		}
		else
		{
			// go it alone.
			_is_selected = value;
			_on_is_selected_changed();
		}
	}

	void_c menu_control_list_item_i::_on_is_selected_changed()
	{
	}

	boolean_c menu_control_list_item_i::get_can_be_selected() const
	{
		return _can_be_selected;
	}

	boolean_c menu_control_list_item_i::get_is_selected() const
	{
		return _is_selected;
	}

	boolean_c menu_control_list_item_i::get_can_be_toggled() const
	{
		return _can_be_toggled;
	}

	menu_control_list_item_i::menu_control_list_item_i()
		: menu_control_c()
		, _can_be_selected( false )
		, _is_selected( false )
		, _can_be_toggled( false )
	{
	}

	void_c menu_control_list_item_separator_i::_update_transform_and_layout()
	{
		if ( _mother_control == nullptr )
		{
			return;
		}
		menu_control_list_i * mother_list = dynamic_cast< menu_control_list_i * >( _mother_control->get_mother_control() );
		assert( mother_list );
		box32x2_c new_local_box;
		new_local_box.minimum.a = mother_list->_client->get_local_box().minimum.a;
		new_local_box.maximum.a = mother_list->_client->get_local_box().maximum.a;
		new_local_box.minimum.b = 0.0f; // don't worry about correct vertical layout at this time. this will be corrected in _lay_out_item_origins().
		new_local_box.maximum.b = _height;
		if ( new_local_box != _local_box )
		{
			_local_box = new_local_box;
			for ( sint32_c i = 0; i < _daughter_element_list.get_length(); i++ )
			{
				menu_element_c * daughter_element = _daughter_element_list[ i ];
				daughter_element->update_layout( _local_box ); 
			}
			mother_list->_item_origins_are_dirty = true;
		}
	}

	menu_control_list_item_separator_i::menu_control_list_item_separator_i()
		: menu_control_list_item_i()
		, _frame_element()
		, _height( 10.0f )
	{
		_frame_element.set_name( string8_c( "frame", core_list_mode_e_static ) );
		_add_daughter_element( &_frame_element );
	}

	float32_c menu_control_list_item_separator_i::get_height() const
	{
		return _height;
	}

	void_c menu_control_list_item_separator_i::set_height( float32_c value )
	{
		_height = ops::math_clamp( value, 1.0f, 100.0f );
		_update_transform_and_layout();
	}

	void_c menu_control_list_item_text_i::_handle_element_text_on_value_changed( menu_element_text_c * element )
	{
		if ( _mother_control )
		{
			_update_transform_and_layout();
		}
	}

	void_c menu_control_list_item_text_i::_on_clicked( input_event_c * input_event )
	{
		_mother_user_interface->reset_multi_click_detection();
		if ( get_is_actually_enabled() )
		{
			assert( _mother_control );
			menu_control_list_i * mother_list = dynamic_cast< menu_control_list_i * >( _mother_control->get_mother_control() );
			assert( mother_list );
			if ( _can_be_toggled )
			{
				_set_is_selected( !_is_selected, true );
			}
			else
			{
				if ( input_event->get_modifier_flags() == input_modifier_flag_e_ctrl )
				{
					_set_is_selected( !_is_selected, true );
				}
				else
				{
					_set_is_selected( true, false );
				}
			}
			on_clicked.invoke( menu_event_information_c( this, nullptr, input_event ) );
		}
	}

	void_c menu_control_list_item_text_i::_update_daughter_element_animations( float32_c time_step )
	{
		for ( sint32_c i = 0; i < _daughter_element_list.get_length(); i++ )
		{
			menu_element_c * daughter_element = _daughter_element_list[ i ];
			daughter_element->set_is_enabled( _is_enabled );
			daughter_element->set_is_selected( _is_enabled && ( _is_mouse_focused || _is_mouse_overed ) );
			daughter_element->set_is_pressed( _is_enabled && _is_pressed );
			daughter_element->update_animations( time_step );
		}
	}

	void_c menu_control_list_item_text_i::_update_transform_and_layout()
	{
		if ( _mother_control == nullptr )
		{
			return;
		}
		menu_control_list_i * mother_list = dynamic_cast< menu_control_list_i * >( _mother_control->get_mother_control() );
		assert( mother_list );
		box32x2_c new_local_box;
		new_local_box.minimum.a = mother_list->_client->get_local_box().minimum.a;
		new_local_box.maximum.a = mother_list->_client->get_local_box().maximum.a;
		new_local_box.minimum.b = 0.0f; // don't worry about correct vertical layout at this time. this will be corrected in _lay_out_item_origins().
		new_local_box.maximum.b = 100.0f;
		if ( new_local_box != _local_box )
		{
			_local_box = new_local_box;
			_text_element.update_layout( _local_box );
			float32_c content_height = _text_element.get_content_height(); // this will reflow text element text.
			if ( content_height < 8.0f ) // this is somewhat arbitrary.
			{
				content_height = 8.0f;
			}
			content_height = ops::math_round_up( content_height + _text_element.get_default_size() );
			new_local_box.minimum.b = content_height * -0.5f;
			new_local_box.maximum.b = content_height * 0.5f;
			_local_box = new_local_box;
			for ( sint32_c i = 0; i < _daughter_element_list.get_length(); i++ )
			{
				menu_element_c * daughter_element = _daughter_element_list[ i ];
				daughter_element->update_layout( _local_box ); 
			}
			mother_list->_item_origins_are_dirty = true;
		}
	}

	menu_control_list_item_text_i::menu_control_list_item_text_i()
		: menu_control_list_item_i()
		, _frame_element()
		//, _icon_frame_element()
		//, _selected_frame_element()
		, _text_element()
	{
		_can_be_selected = true;

		_layout_mode = menu_layout_mode_e_simple;
		_local_anchor = menu_anchor_e_none;

		_select_mode = menu_select_mode_e_mouse_and_directional;

		_frame_element.set_name( string8_c( "frame", core_list_mode_e_static ) );
		_add_daughter_element( &_frame_element );

		//_selected_frame_element.set_name( string8_c( "selected_frame", core_list_mode_e_static ) );
		//_selected_frame_element.set_is_showed( false );
		//_add_daughter_element( &_selected_frame_element );

		//_icon_frame_element.set_name( string8_c( "icon_frame", core_list_mode_e_static );
		//_icon_frame_element.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 20.0f, 10.0f ) );
		//_add_daughter_element( &_icon_frame_element );

		_text_element.set_name( string8_c( "text", core_list_mode_e_static ) );
		_text_element.on_text_value_changed.subscribe( this, &menu_control_list_item_text_i::_handle_element_text_on_value_changed );
		_add_daughter_element( &_text_element );
	}

	void_c menu_control_list_item_text_i::set_is_selected( boolean_c value, boolean_c try_to_multi_select )
	{
		_set_is_selected( value, try_to_multi_select );
	}

	string16_c const & menu_control_list_item_text_i::get_internal_plain_text_value() const
	{
		return _text_element.get_internal_plain_text_value();
	}

	string16_c menu_control_list_item_text_i::get_plain_text_value() const
	{
		return _text_element.get_plain_text_value();
	}

	void_c menu_control_list_item_text_i::set_plain_text_value( string8_c const & plain_text )
	{
		_text_element.set_plain_text_value( plain_text );
	}

	void_c menu_control_list_item_text_i::set_plain_text_value( string16_c const & plain_text )
	{
		_text_element.set_plain_text_value( plain_text );
	}

	void_c menu_control_list_item_text_i::set_rich_text_value( string8_c const & plain_text_with_mark_up )
	{
		_text_element.set_rich_text_value( plain_text_with_mark_up );
	}

	void_c menu_control_list_item_text_i::set_rich_text_value( string16_c const & plain_text_with_mark_up )
	{
		_text_element.set_rich_text_value( plain_text_with_mark_up );
	}

	void_c menu_control_list_item_text_i::clear_text_value()
	{
		_text_element.clear_text_value();
	}

	void_c menu_control_list_item_text_i::set_can_be_toggled( boolean_c value )
	{
		_can_be_toggled = value;
	}

	void_c menu_control_list_i::_layout_item_origins()
	{
		// position list items in a stack from top to bottom, and also measure their total height.
		float32_c list_item_width = _client->get_local_box().get_width();
		float32_c list_item_top = _client->get_local_box().minimum.b;

		core_linked_list_c< menu_control_c * >::node_c const * client_daughter_control_list_node = _client->get_daughter_control_list().get_first();
		while ( client_daughter_control_list_node )
		{
			menu_control_list_item_i * list_item = static_cast< menu_control_list_item_i * >( client_daughter_control_list_node->get_value() );
			list_item->_local_origin.a = 0.0f;
			list_item->_local_origin.b = list_item_top + ( list_item->_local_box.get_height() * 0.5f );
			list_item->_update_global_space_properties();
			list_item_top += list_item->_local_box.get_height();
			client_daughter_control_list_node = client_daughter_control_list_node->get_next();
		}
		float32_c content_height = list_item_top - _client->get_local_box().minimum.b;

		// change vertical size of list if needed and reposition list items again to fit.
		// set the _local_box and _local_origin directly so that update_transform_and_layout() does not cascade.
		if ( _vertical_size_mode == menu_size_mode_e_fit_content )
		{
			if ( _vertical_size_maximum > 0.0f && content_height > _vertical_size_maximum )
			{
				content_height = _vertical_size_maximum;
			}

			_client->_local_origin.b = content_height * -0.5f;
			_client->_local_box.minimum.b = 0.0f; //content_height * -0.5f;
			_client->_local_box.maximum.b = content_height; //content_height * 0.5f;
			_local_box.minimum.b = content_height * -0.5f - _client_margins.minimum.b;
			_local_box.maximum.b = content_height *  0.5f + _client_margins.maximum.b;

			list_item_top = 0.0f; //_client->get_local_box().minimum.b;
			core_linked_list_c< menu_control_c * >::node_c const * list_item_list_node = _client->get_daughter_control_list().get_first();
			while ( list_item_list_node )
			{
				menu_control_list_item_i * list_item = static_cast< menu_control_list_item_i * >( list_item_list_node->get_value() );
				list_item->_local_origin.b = list_item_top + ( list_item->_local_box.get_height() * 0.5f );
				list_item->_update_global_space_properties();
				list_item_top += list_item->_local_box.get_height();
				list_item_list_node = list_item_list_node->get_next();
			}
		}

		_item_origins_are_dirty = false;

		// this might cause one more recursion (call to this function) if the scroll bar value changes, but it should not be infinite.
		_scroll_bar_y->set_value_range_and_page_size( 0.0f, content_height, _client->get_local_box().get_height() );
		_scroll_bar_y->update_visibility( _scroll_bar_y_visibility_mode );
	}

	void_c menu_control_list_i::_set_selected_item_limit( sint32_c value )
	{
		assert( value >= -1 );

		// apply new limit.
		// if needed then change current number of selected items to fit within new constraint.
		// this will also invoke the on_selected_list_item_changed event if any number of list items become deselected because of this operation.
		_selected_item_limit = value;
		_deselect_items_over_limit();
	}

	void_c menu_control_list_i::_deselect_items_over_limit()
	{
		if ( _selected_item_limit >= 0 )
		{
			sint32_c deselect_count = _selected_item_list.get_length() - _selected_item_limit;
			if ( deselect_count > 0 )
			{
				for ( sint32_c i = 0; i < deselect_count; i++ )
				{
					menu_control_list_item_i * list_item = _selected_item_list[ i ];
					list_item->_is_selected = false;
					list_item->_on_is_selected_changed();
				}
				_selected_item_list.remove( 0, deselect_count );
			}
		}
	}

	void_c menu_control_list_i::_deselect_all_items()
	{
		for ( sint32_c i = 0; i < _selected_item_list.get_length(); i++ )
		{
			menu_control_list_item_i * list_item = _selected_item_list[ i ];
			list_item->_is_selected = false;
			list_item->_on_is_selected_changed();
		}
		_selected_item_list.remove_all();
	}

	void_c menu_control_list_i::_add_item( menu_control_list_item_i * item, sint32_c index )
	{
		_client->add_daughter_control( item, index );
		if ( item->_is_selected )
		{
			// the item that is being given to this list already has its selected state set to true.
			if ( _selected_item_limit != 0 )
			{
				// try to append the selected item.
				_selected_item_list.insert( -1, item );
			}
			else
			{
				// the item that is being given to this list already had its selected state set, but this list has its selected item limit set to 0 so we have to flip its selected state to false.
				item->_is_selected = false;
				item->_on_is_selected_changed();
			}
			_on_selected_item_list_changed();
		}
	}

	void_c menu_control_list_i::_remove_item( menu_control_list_item_i * item )
	{
		_client->remove_daughter_control( item );
		if ( item->_is_selected )
		{
			sint32_c selected_item_index = _selected_item_list.find_index_of( item );
			if ( selected_item_index != -1 )
			{
				_selected_item_list.remove( selected_item_index, 1 );
				_on_selected_item_list_changed();
			}
		}
	}

	void_c menu_control_list_i::_remove_all_items()
	{
		_client->remove_all_daughter_controls();
		if ( _selected_item_list.get_length() > 0 )
		{
			_selected_item_list.remove_all();
			_on_selected_item_list_changed();
		}
	}

	sint32_c menu_control_list_i::_get_selected_item_index() const
	{
		if ( _selected_item_list.get_length() > 0 )
		{
			return _selected_item_list[ _selected_item_list.get_length() - 1 ]->get_index();
		}
		return -1;
	}

	void_c menu_control_list_i::_set_selected_item_index( sint32_c item_index )
	{
		assert( item_index >= -1 && item_index < _client->get_daughter_control_list().get_length() );

		// deselect all currently selected items.
		for ( sint32_c i = 0; i < _selected_item_list.get_length(); i++ )
		{
			menu_control_list_item_i * selected_item = _selected_item_list[ i ];
			selected_item->_is_selected = false;
			selected_item->_on_is_selected_changed();
		}
		_selected_item_list.remove_all();

		// select a new item.
		if ( item_index != -1 )
		{
			menu_control_list_item_i * item = static_cast< menu_control_list_item_i * >( _client->get_daughter_control_list().get_at_index( item_index )->get_value() );
			item->_is_selected = true;
			item->_on_is_selected_changed();
			_selected_item_list.insert( -1, item );
		}

		// invoke event.
		_on_selected_item_list_changed();
	}

	menu_control_list_item_i * menu_control_list_i::_get_selected_item() const
	{
		if ( _selected_item_list.get_length() > 0 )
		{
			return _selected_item_list[ _selected_item_list.get_length() - 1 ];
		}
		return nullptr;
	}

	void_c menu_control_list_i::_on_selected_item_list_changed()
	{
		// does nothing here, but should invoke associated public event in derived implementations.
	}

	void_c menu_control_list_i::_on_is_mouse_overed_changed()
	{
		if ( _is_mouse_overed == false )
		{
			if ( _layer == menu_layer_e_popup && _is_deep_text_focused == 0 )
			{
				set_is_showed( false );
			}
		}
		on_is_mouse_overed_changed.invoke( menu_event_information_c( this, nullptr, nullptr ) );
	}

	void_c menu_control_list_i::_on_is_deep_text_focused_changed( menu_control_c * next_control )
	{
		if ( _is_deep_text_focused == false )
		{
			if ( _layer == menu_layer_e_popup )
			{
				set_is_showed( false );
			}
		}
		on_is_deep_text_focused_changed.invoke( menu_event_information_c( this, next_control, nullptr ) );
	}

	void_c menu_control_list_i::_on_clicked( input_event_c * input_event )
	{
		if ( get_is_actually_enabled() )
		{
			_deselect_all_items();
			on_clicked.invoke( menu_event_information_c( this, nullptr, input_event ) );
		}
	}

	void_c menu_control_list_i::_on_input( input_event_c * input_event )
	{
		on_input.invoke( menu_event_information_c( this, nullptr, input_event ) );
		if ( !input_event->get_was_handled() )
		{
			if ( input_event->get_type() == input_event_c::type_e_mouse_wheel )
			{
				if ( _scroll_bar_y->inject_mouse_wheel_input( input_event->get_mouse_wheel_delta() ) )
				{
					input_event->set_was_handled( true );
				}
			}
		}
	}

	void_c menu_control_list_i::_update_transform_and_layout()
	{
		menu_control_panel_i::_update_transform_and_layout();
		_layout_item_origins();
	}

	menu_control_list_i::menu_control_list_i()
		: menu_control_panel_i()
		, _item_origins_are_dirty( true )
		, _vertical_size_mode( menu_size_mode_e_fixed )
		, _vertical_size_maximum( 0.0f )
		, _selected_item_limit( -1 )
		, _selected_item_list()
	{
		_layer = menu_layer_e_base;
		_vertical_size_mode = menu_size_mode_e_fixed;
		_vertical_size_maximum = 0.0f;

		_set_scroll_bar_x_visibility_mode( menu_visibility_mode_e_never );
		_set_scroll_bar_y_visibility_mode( menu_visibility_mode_e_automatic );
	}

	void_c menu_control_list_i::update_animations( float32_c time_step )
	{
		if ( _item_origins_are_dirty )
		{
			_layout_item_origins();
		}
		menu_control_c::update_animations( time_step );
	}

}
