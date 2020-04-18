#include "cheonsa_menu_control_list_i.h"
#include "cheonsa_user_interface.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	menu_control_list_item_i::menu_control_list_item_i( string8_c const & name )
		: menu_control_c( name )
		, _can_be_selected( false )
		, _is_selected( false )
	{
	}

	void_c menu_control_list_item_i::_set_is_selected( boolean_c value, boolean_c try_to_multi_select )
	{
		assert( _can_be_selected == true ); // this function should only be called if this type of list item is designed to have a toggleable selected state.
		menu_control_list_i * mother_list = dynamic_cast< menu_control_list_i * >( _mother_control->get_mother_control() );
		assert( mother_list != nullptr );
		if ( mother_list->_selected_item_limit != 0 && _is_selected != value )
		{
			if ( value )
			{
				// this item wants to become selected.
				if ( mother_list != nullptr )
				{
					assert( mother_list->_selected_item_list.find_index_of( this ) == -1 );
					if ( try_to_multi_select )
					{
						// work with list to update selected item state.
						mother_list->_selected_item_list.insert_at_end( this ); // append new selection.
						mother_list->_deselect_items_over_limit();
					}
					else
					{
						mother_list->_deselect_all_items();
					}
				}
				_is_selected = value;
				//_on_is_selected_changed();
				if ( mother_list != nullptr )
				{
					mother_list->_on_selected_item_list_changed();
				}
			}
			else
			{
				// this item wants to become deselected.
				_is_selected = value;
				//_on_is_selected_changed();
				if ( mother_list != nullptr )
				{
					assert( mother_list->_selected_item_list.remove( this ) );
					mother_list->_on_selected_item_list_changed();
				}
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
			for ( sint32_c i = 0; i < _element_list.get_length(); i++ )
			{
				menu_element_c * element = _element_list[ i ];
				element->update_layout( _local_box ); 
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
		_element_highlighted_frame.set_is_showed( _is_mouse_overed );
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
		assert( _mother_control->get_mother_control() );
		menu_control_list_i * mother_list = dynamic_cast< menu_control_list_i * >( _mother_control->get_mother_control() );
		assert( mother_list );
		if ( mother_list != nullptr )
		{
			_set_is_selected( true, input_event->check_modifier_key_states( false, true, false ) );
		}
		on_clicked.invoke( menu_event_information_c( this, input_event ) );
	}

	void_c menu_control_list_item_text_i::_update_element_states()
	{
		for ( sint32_c i = 0; i < _element_list.get_length(); i++ )
		{
			menu_element_c * element = _element_list[ i ];
			element->set_is_selected( _is_mouse_focused || _is_mouse_overed );
			element->set_is_pressed( _is_pressed );
		}
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
			for ( sint32_c i = 0; i < _element_list.get_length(); i++ )
			{
				menu_element_c * element = _element_list[ i ];
				element->update_layout( _local_box ); 
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
		_add_element( &_element_frame );

		_element_selected_frame.set_shared_color_class( menu_shared_color_class_e_field );
		_element_selected_frame.set_is_showed( false );
		_add_element( &_element_selected_frame );

		_element_highlighted_frame.set_shared_color_class( menu_shared_color_class_e_field );
		_element_highlighted_frame.set_is_showed( false );
		_add_element( &_element_highlighted_frame );

		_element_text.set_shared_color_class( menu_shared_color_class_e_field );
		_element_text.on_text_value_changed.subscribe( this, &menu_control_list_item_text_i::_handle_element_text_on_value_changed );
		_add_element( &_element_text );

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
		float32_c list_item_width = _client->_local_box.get_width();
		float32_c list_item_top = _client->_local_box.minimum.b;
		for ( sint32_c i = 0; i < _client->_control_list.get_length(); i++ )
		{
			menu_control_list_item_i * list_item = static_cast< menu_control_list_item_i * >( _client->_control_list[ i ] );
			list_item->_local_origin.a = 0.0f;
			list_item->_local_origin.b = list_item_top + ( list_item->_local_box.get_height() * 0.5f );
			list_item->_update_global_space_properties();
			list_item_top += list_item->_local_box.get_height();
		}
		float32_c content_height = list_item_top - _client->_local_box.minimum.b;

		// change vertical size of list if needed and reposition list items again to fit.
		// set the _local_box and _local_origin directly so that update_transform_and_layout() does not cascade.
		if ( _vertical_size_mode == menu_size_mode_e_fit_content )
		{
			if ( _vertical_size_maximum > 0.0f && content_height > _vertical_size_maximum )
			{
				content_height = _vertical_size_maximum;
			}

			_client->_local_box.minimum.b = content_height * -0.5f;
			_client->_local_box.maximum.b = content_height * 0.5f;
			_local_box.minimum.b = _client->_local_box.minimum.b - _client->get_local_anchor_measures().minimum.b;
			_local_box.maximum.b = _client->_local_box.maximum.b + _client->get_local_anchor_measures().maximum.b;

			list_item_top = _client->_local_box.minimum.b;
			for ( sint32_c i = 0; i < _client->_control_list.get_length(); i++ )
			{
				menu_control_list_item_i * list_item = static_cast< menu_control_list_item_i * >( _client->_control_list[ i ] );
				list_item->_local_origin.b = list_item_top + ( list_item->_local_box.get_height() * 0.5f );
				list_item->_update_global_space_properties();
				list_item_top += list_item->_local_box.get_height();
			}
		}

		_item_origins_are_dirty = false;

		// this might cause one more recursion (call to this function) if the scroll bar value changes, but it should not be infinite.
		_vertical_scroll_bar->set_value_range_and_page_size( 0.0f, content_height, _client->_local_box.get_height() );
		_vertical_scroll_bar->update_visibility( _vertical_scroll_bar_visibility_mode );
	}

	void_c menu_control_list_i::_handle_vertical_scroll_bar_on_value_changed( menu_control_scroll_bar_i * scroll_bar )
	{
		_client->set_content_offset_vertical( static_cast< float32_c >( -_vertical_scroll_bar->get_value() ) );
	}

	void_c menu_control_list_i::_handle_vertical_scroll_bar_on_preferred_thickness_changed( menu_control_scroll_bar_i * scroll_bar )
	{
		_lay_out_vertical_scroll_bar();
	}

	void_c menu_control_list_i::_lay_out_vertical_scroll_bar()
	{
		if ( _vertical_scroll_bar != nullptr )
		{
			_vertical_scroll_bar->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( _vertical_scroll_bar->get_preferred_thickness(), _client->get_local_anchor_measures().minimum.b, _client->get_local_anchor_measures().maximum.a, _client->get_local_anchor_measures().maximum.b ) );
		}
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

	void_c menu_control_list_i::_deselect_all_items()
	{
		for ( sint32_c i = 0; i < _selected_item_list.get_length(); i++ )
		{
			menu_control_list_item_i * list_item = _selected_item_list[ i ];
			list_item->_is_selected = false;
			list_item->_on_is_selected_changed();
		}
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
				_selected_item_list.remove_at_index( 0, deselect_count );
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
		if ( input_event->type == input_event_c::type_e_mouse_wheel )
		{
			_vertical_scroll_bar->inject_mouse_wheel_input( input_event->mouse_wheel_delta );
		}
	}

	sint32_c menu_control_list_i::_get_item_count() const
	{
		return _client->get_control_count();
	}

	menu_control_list_item_i const * menu_control_list_i::_get_item( sint32_c list_item_index ) const
	{
		return static_cast< menu_control_list_item_i const * >( _client->get_control( list_item_index ) );
	}

	menu_control_list_item_i * menu_control_list_i::_get_item( sint32_c list_item_index )
	{
		return static_cast< menu_control_list_item_i * >( _client->get_control( list_item_index ) );
	}

	sint32_c menu_control_list_i::_give_item( menu_control_list_item_i * list_item, sint32_c index )
	{
		sint32_c result = _client->_give_control( list_item, index ); // this should call list_item->update_transform_and_layout(), which in turn should call list_item->_lay_out_local_box(), which in turn should set _list_item_origins_are_dirty to true.
		if ( list_item->_is_selected )
		{
			// the item that is being given to this list already has its selected state set to true.
			if ( _selected_item_limit != 0 )
			{
				// try to append the selected item.
				_selected_item_list.insert_at_end( list_item );
				// deselect any currently 
			}
			else
			{
				// the item that is being given to this list already had its selected state set, but this list has its selected item limit set to 0 so we have to flip its selected state to false.
				list_item->_is_selected = false;
				list_item->_on_is_selected_changed();
			}
			_on_selected_item_list_changed();
		}
		return result;
	}

	menu_control_list_item_i * menu_control_list_i::_take_item( sint32_c list_item_index )
	{
		menu_control_list_item_i * result = static_cast< menu_control_list_item_i * >( _client->_take_control( list_item_index ) );
		if ( result->_is_selected )
		{
			assert( _selected_item_list.remove( result ) );
			_on_selected_item_list_changed();
		}
		return result;
	}

	void_c menu_control_list_i::_remove_and_delete_all_items()
	{
		_client->_remove_and_delete_all_controls();
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
		assert( item_index >= -1 && item_index < _client->get_control_count() );
		for ( sint32_c i = 0; i < _selected_item_list.get_length(); i++ )
		{
			_selected_item_list[ i ]->_is_selected = false;
		}
		_selected_item_list.remove_all();
		if ( item_index != -1 )
		{
			menu_control_list_item_i * item = static_cast< menu_control_list_item_i * >( _client->get_control( item_index ) );
			item->_is_selected = true;
			_selected_item_list.insert_at_end( item );
		}
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
		for ( sint32_c i = 0; i < _selected_item_list.get_length(); i++ )
		{
			_selected_item_list[ i ]->_is_selected = false;
		}
		_selected_item_list.remove_all();
		if ( item != nullptr )
		{
			assert( item->_mother_control == _client );
			item->_is_selected = true;
			_selected_item_list.insert_at_end( item );
		}
		_on_selected_item_list_changed();
	}

	menu_control_list_i::menu_control_list_i( string8_c const & name )
		: menu_control_c( name )
		, _element_frame( string8_c( core_list_mode_e_static, "frame" ) )
		, _element_border_frame( string8_c( core_list_mode_e_static, "border_frame" ) )
		, _client( nullptr )
		, _item_origins_are_dirty( false )
		, _vertical_size_mode( menu_size_mode_e_fixed )
		, _vertical_size_maximum( 0.0f )
		, _vertical_scroll_bar_visibility_mode( menu_visibility_mode_e_automatic )
		, _vertical_scroll_bar( nullptr )
		, _selected_item_limit( -1 )
		, _selected_item_list()
	{
		_layer = menu_layer_e_base;
		_vertical_size_mode = menu_size_mode_e_fixed;
		_vertical_size_maximum = 0.0f;

		_element_frame.set_shared_color_class( menu_shared_color_class_e_field );
		_add_element( &_element_frame );

		_element_border_frame.set_is_overlay( true );
		_add_element( &_element_border_frame );

		_vertical_scroll_bar = new menu_control_scroll_bar_y_c( string8_c( core_list_mode_e_static, "vertical_scroll_bar" ) );
		_vertical_scroll_bar->on_value_changed_preview.subscribe( this, &menu_control_list_i::_handle_vertical_scroll_bar_on_value_changed );
		_vertical_scroll_bar->on_preferred_thickness_changed.subscribe( this, &menu_control_list_i::_handle_vertical_scroll_bar_on_preferred_thickness_changed );
		_give_control( _vertical_scroll_bar );

		_client = new menu_control_c( string8_c( core_list_mode_e_static, "client" ) );
		_client->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 8.0f, 8.0f, 8.0f, 8.0f ) );
		_give_control( _client );

		_lay_out_vertical_scroll_bar();

		set_style_map_key( string8_c( core_list_mode_e_static, "e_list" ) );
	}

	void_c menu_control_list_i::update_animations( float32_c time_step )
	{
		if ( _item_origins_are_dirty )
		{
			_lay_out_item_origins();
		}
		menu_control_c::update_animations( time_step );
	}

	void_c menu_control_list_i::load_static_data_properties( data_scribe_markup_c::node_c const * node )
	{
		menu_control_c::load_static_data_properties( node );

		_vertical_size_mode = menu_size_mode_e_fixed;
		data_scribe_markup_c::attribute_c const * attribute = node->find_attribute( "vertical_size_mode" );
		if ( attribute )
		{
			if ( attribute->get_value() == "fixed" )
			{
				_vertical_size_mode = menu_size_mode_e_fixed;
			}
			else if ( attribute->get_value() == "fit_content" )
			{
				_vertical_size_mode = menu_size_mode_e_fit_content;
			}
		}

		_vertical_size_maximum = 0.0f;
		attribute = node->find_attribute( "vertical_size_maximum" );
		if ( attribute )
		{
			ops::convert_string8_to_float32( attribute->get_value(), _vertical_size_maximum );
		}

		menu_visibility_mode_e vertical_scroll_visibility_mode = menu_visibility_mode_e_automatic;
		attribute = node->find_attribute( "vertical_scroll_visibility_mode" );
		if ( attribute )
		{
			if ( attribute->get_value() == "never" )
			{
				vertical_scroll_visibility_mode = menu_visibility_mode_e_never;
			}
			else if ( attribute->get_value() == "always" )
			{
				vertical_scroll_visibility_mode = menu_visibility_mode_e_always;
			}
		}
		set_vertical_scroll_bar_visibility_mode( vertical_scroll_visibility_mode );
	}

	box32x2_c const & menu_control_list_i::get_client_margins() const
	{
		return _client->get_local_anchor_measures();
	}

	void_c menu_control_list_i::set_client_margins( box32x2_c const & value )
	{
		_client->set_local_anchor_measures( value );
	}

	menu_visibility_mode_e menu_control_list_i::get_vertical_scroll_bar_visibility_mode() const
	{
		return _vertical_scroll_bar_visibility_mode;
	}

	void_c menu_control_list_i::set_vertical_scroll_bar_visibility_mode( menu_visibility_mode_e value )
	{
		_vertical_scroll_bar_visibility_mode = value;
		_vertical_scroll_bar->update_visibility( value );
	}

}
