#include "cheonsa_menu_control_list_i.h"
#include "cheonsa_user_interface.h"
#include "cheonsa_ops.h"

namespace cheonsa
{

	void_c menu_control_list_item_i::_set_is_selected( boolean_c is_selected, boolean_c try_to_multi_select )
	{
		assert( _can_be_selected == true ); // this function should only be called if this type of list item is designed to have a toggleable selected state.
		menu_control_list_i * mother_list = dynamic_cast< menu_control_list_i * >( _mother_control->get_mother_control() );
		if ( mother_list != nullptr )
		{
			// work with mother.
			mother_list->_set_selected_item( this, is_selected, try_to_multi_select );
		}
		else
		{
			// go it alone.
			if ( _is_selected != is_selected )
			{
				_is_selected = is_selected;
				_on_is_selected_changed();
			}
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

	menu_control_list_item_i::menu_control_list_item_i( string8_c const & name )
		: menu_control_c( name )
		, _can_be_selected( false )
		, _is_selected( false )
	{
	}

	void_c menu_control_list_item_separator_i::_update_transform_and_layout()
	{
		if ( _mother_control == nullptr )
		{
			return;
		}
		menu_control_list_i * mother_list = dynamic_cast< menu_control_list_i * >( _mother_control->get_mother_control() );
		assert( mother_list != nullptr );
		box32x2_c new_local_box;
		new_local_box.minimum.a = mother_list->_client->get_local_box().minimum.a;
		new_local_box.maximum.a = mother_list->_client->get_local_box().maximum.a;
		new_local_box.minimum.b = 0.0f;
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

	menu_control_list_item_separator_i::menu_control_list_item_separator_i( string8_c const & name )
		: menu_control_list_item_i( name )
		, _element_frame( string8_c( core_list_mode_e_static, "frame" ) )
		, _height( 10.0f )
	{
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
		if ( _mother_control != nullptr )
		{
			_update_transform_and_layout();
		}
	}

	void_c menu_control_list_item_text_i::_on_is_mouse_overed_changed()
	{
		_update_element_states();
	}

	void_c menu_control_list_item_text_i::_on_is_mouse_focused_changed()
	{
		_update_element_states();
	}

	void_c menu_control_list_item_text_i::_on_is_pressed_changed()
	{
		_update_element_states();
	}

	void_c menu_control_list_item_text_i::_on_clicked( input_event_c * input_event )
	{
		assert( _mother_control );
		menu_control_list_i * mother_list = dynamic_cast< menu_control_list_i * >( _mother_control->get_mother_control() );
		assert( mother_list );
		if ( mother_list != nullptr )
		{
			_set_is_selected( true, input_event->get_modifier_flags() == input_modifier_flag_e_ctrl );
		}
		on_clicked.invoke( menu_event_information_c( this, input_event ) );
	}

	void_c menu_control_list_item_text_i::_update_element_states()
	{
		for ( sint32_c i = 0; i < _daughter_element_list.get_length(); i++ )
		{
			menu_element_c * daughter_element = _daughter_element_list[ i ];
			daughter_element->set_is_selected( _is_mouse_focused || _is_mouse_overed );
			daughter_element->set_is_pressed( _is_enabled && _is_pressed );
		}
		_element_highlighted_frame.set_is_showed( _is_enabled && _is_mouse_overed );
	}

	void_c menu_control_list_item_text_i::_update_transform_and_layout()
	{
		if ( _mother_control == nullptr )
		{
			return;
		}
		menu_control_list_i * mother_list = dynamic_cast< menu_control_list_i * >( _mother_control->get_mother_control() );
		assert( mother_list != nullptr );
		box32x2_c new_local_box;
		new_local_box.minimum.a = mother_list->_client->get_local_box().minimum.a;
		new_local_box.maximum.a = mother_list->_client->get_local_box().maximum.a;
		new_local_box.minimum.b = 0.0f; // does not really matter right now.
		new_local_box.maximum.b = 100.0f; // does not really matter right now.
		if ( new_local_box != _local_box )
		{
			_local_box = new_local_box;
			_element_text.update_layout( _local_box );
			float32_c content_height = _element_text.get_content_height(); // this will reflow text element text.
			if ( content_height < 8.0f ) // this is somewhat arbitrary.
			{
				content_height = 8.0f;
			}
			content_height = ops::math_round_up( content_height + _element_text.get_default_size() );
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

	menu_control_list_item_text_i::menu_control_list_item_text_i( string8_c const & name )
		: menu_control_list_item_i( name )
		, _element_frame( string8_c( core_list_mode_e_static, "frame" ) )
		, _element_selected_frame( string8_c( core_list_mode_e_static, "selected_frame" ) )
		, _element_highlighted_frame( string8_c( core_list_mode_e_static, "highlighted_frame" ) )
		, _element_text( string8_c( core_list_mode_e_static, "text" ) )
	{
		_can_be_selected = true;

		_layout_mode = menu_layout_mode_e_simple;
		_local_anchor = menu_anchor_e_none;

		_select_mode = menu_select_mode_e_mouse_and_directional;

		_element_frame.set_shared_color_class( menu_shared_color_class_e_field );
		_add_daughter_element( &_element_frame );

		_element_selected_frame.set_shared_color_class( menu_shared_color_class_e_field );
		_element_selected_frame.set_is_showed( false );
		_add_daughter_element( &_element_selected_frame );

		_element_highlighted_frame.set_shared_color_class( menu_shared_color_class_e_field );
		_element_highlighted_frame.set_is_showed( false );
		_add_daughter_element( &_element_highlighted_frame );

		_element_text.set_shared_color_class( menu_shared_color_class_e_field );
		_element_text.on_text_value_changed.subscribe( this, &menu_control_list_item_text_i::_handle_element_text_on_value_changed );
		_add_daughter_element( &_element_text );

		set_style_map_key( string8_c( core_list_mode_e_static, "e_list_item" ) );
	}

	void_c menu_control_list_item_text_i::set_is_selected( boolean_c value, boolean_c try_to_multi_select )
	{
		_set_is_selected( value, try_to_multi_select );
	}

	string16_c menu_control_list_item_text_i::get_plain_text_value() const
	{
		return _element_text.get_plain_text_value();
	}

	void_c menu_control_list_item_text_i::set_plain_text_value( string8_c const & plain_text )
	{
		_element_text.set_plain_text_value( plain_text );
	}

	void_c menu_control_list_item_text_i::set_plain_text_value( string16_c const & plain_text )
	{
		_element_text.set_plain_text_value( plain_text );
	}

	void_c menu_control_list_item_text_i::set_rich_text_value( string8_c const & plain_text_with_mark_up )
	{
		_element_text.set_rich_text_value( plain_text_with_mark_up );
	}

	void_c menu_control_list_item_text_i::set_rich_text_value( string16_c const & plain_text_with_mark_up )
	{
		_element_text.set_rich_text_value( plain_text_with_mark_up );
	}

	void_c menu_control_list_item_text_i::clear_text_value()
	{
		_element_text.clear_text_value();
	}

	void_c menu_control_list_i::_lay_out_item_origins()
	{
		// position list items in a stack from top to bottom, and also measure their total height.
		float32_c list_item_width = _client->get_local_box().get_width();
		float32_c list_item_top = _client->get_local_box().minimum.b;

		core_linked_list_c< menu_control_c * >::node_c const * client_daughter_control_list_node = _client->get_daughter_control_list().get_first();
		while ( client_daughter_control_list_node )
		{
			client_daughter_control_list_node = client_daughter_control_list_node->get_next();
			menu_control_list_item_i * list_item = static_cast< menu_control_list_item_i * >( client_daughter_control_list_node->get_value() );
			list_item->_local_origin.a = 0.0f;
			list_item->_local_origin.b = list_item_top + ( list_item->_local_box.get_height() * 0.5f );
			list_item->_update_global_space_properties();
			list_item_top += list_item->_local_box.get_height();
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

			_client->get_local_box().minimum.b = content_height * -0.5f;
			_client->get_local_box().maximum.b = content_height * 0.5f;
			_local_box.minimum.b = _client->get_local_box().minimum.b - _client->get_local_anchor_measures().minimum.b;
			_local_box.maximum.b = _client->get_local_box().maximum.b + _client->get_local_anchor_measures().maximum.b;

			list_item_top = _client->get_local_box().minimum.b;
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
		_vertical_scroll_bar->set_value_range_and_page_size( 0.0f, content_height, _client->get_local_box().get_height() );
		_vertical_scroll_bar->update_visibility( _vertical_scroll_bar_visibility_mode );
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

	void_c menu_control_list_i::_set_selected_item( menu_control_list_item_i * item )
	{
		assert( item->_can_be_selected );
		assert( item->get_mother_control() == _client );

		for ( sint32_c i = 0; i < _selected_item_list.get_length(); i++ )
		{
			menu_control_list_item_i * selected_item = _selected_item_list[ i ];
			selected_item->_is_selected = false;
			selected_item->_on_is_selected_changed();
		}
		_selected_item_list.remove_all();

		if ( item != nullptr )
		{
			item->_is_selected = true;
			item->_on_is_selected_changed();
			_selected_item_list.insert( -1, item );
		}

		_on_selected_item_list_changed();
	}

	void_c menu_control_list_i::_set_selected_item( menu_control_list_item_i * item, boolean_c is_selected, boolean_c try_to_multi_select )
	{
		assert( item != nullptr );
		assert( item->_can_be_selected == true );
		assert( item->get_mother_control() == _client );
		if ( _selected_item_limit != 0 && item->_is_selected != is_selected )
		{
			if ( is_selected )
			{
				// this item wants to become selected.
				assert( _selected_item_list.find_index_of( item ) == -1 );
				if ( try_to_multi_select )
				{
					_selected_item_list.insert( -1, item );
					_deselect_items_over_limit();
				}
				else
				{
					for ( sint32_c i = 0; i < _selected_item_list.get_length(); i++ )
					{
						menu_control_list_item_i * selected_item = _selected_item_list[ i ];
						selected_item->_is_selected = false;
						selected_item->_on_is_selected_changed();
					}
					_selected_item_list.remove_all();
				}
				item->_is_selected = is_selected;
				item->_on_is_selected_changed();
				_selected_item_list.insert( -1, item );
				_on_selected_item_list_changed();
			}
			else
			{
				// this item wants to become deselected.
				item->_is_selected = is_selected;
				item->_on_is_selected_changed();
				assert( _selected_item_list.remove_value( item ) );
				_on_selected_item_list_changed();
			}
		}
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
		on_is_mouse_overed_changed.invoke( menu_event_information_c( this, nullptr ) );
	}

	void_c menu_control_list_i::_on_is_deep_text_focused_changed()
	{
		if ( _is_deep_text_focused == false )
		{
			if ( _layer == menu_layer_e_popup )
			{
				set_is_showed( false );
			}
		}
		on_is_deep_text_focused_changed.invoke( menu_event_information_c( this, nullptr ) );
	}

	void_c menu_control_list_i::_on_input( input_event_c * input_event )
	{
		if ( input_event->get_type() == input_event_c::type_e_mouse_wheel )
		{
			_vertical_scroll_bar->inject_mouse_wheel_input( input_event->get_mouse_wheel_delta() );
		}
	}

	menu_control_list_i::menu_control_list_i( string8_c const & name )
		: menu_control_panel_i( name )
		, _item_origins_are_dirty( true )
		, _vertical_size_mode( menu_size_mode_e_fixed )
		, _vertical_size_maximum( 0.0f )
		, _selected_item_limit( -1 )
		, _selected_item_list()
	{
		_layer = menu_layer_e_base;
		_vertical_size_mode = menu_size_mode_e_fixed;
		_vertical_size_maximum = 0.0f;

		_set_horizontal_scroll_bar_visibility_mode( menu_visibility_mode_e_never );
		_set_vertical_scroll_bar_visibility_mode( menu_visibility_mode_e_automatic );
	}

	void_c menu_control_list_i::update_animations( float32_c time_step )
	{
		if ( _item_origins_are_dirty )
		{
			_lay_out_item_origins();
		}
		menu_control_c::update_animations( time_step );
	}

}
