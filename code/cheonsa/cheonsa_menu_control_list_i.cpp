#include "cheonsa_menu_control_list_i.h"
#include "cheonsa_user_interface.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	void_c menu_control_list_item_i::_handle_element_text_on_value_changed_commit( menu_element_text_c * element )
	{
		if ( _mother_control )
		{
			menu_control_list_i * list = dynamic_cast< menu_control_list_i * >( _mother_control );
			assert( list );
			list->_list_item_holder->_item_layout_is_dirty = true;
		}
	}

	void_c menu_control_list_item_i::_on_clicked( input_event_c * input_event )
	{
		assert( _mother_control );
		assert( _mother_control->get_mother_control() );
		menu_control_list_i * list = dynamic_cast< menu_control_list_i * >( _mother_control->get_mother_control() );
		assert( list );
		if ( list != nullptr )
		{
			if ( list->_selected_item_limit != 0 )
			{
				set_is_selected( !_is_selected );
			}
		}
		menu_control_c::_on_clicked( input_event );
	}

	menu_control_list_item_i::menu_control_list_item_i()
		: menu_control_c()
		, _is_selected( false )
		, _element_frame()
		, _element_frame_selected()
		, _element_text()
	{
		_layout_mode = menu_layout_mode_e_simple;
		_local_anchor = menu_anchor_e_none;

		_select_mode = menu_select_mode_e_mouse_and_directional;

		_element_frame.set_name( string8_c( mode_e_static, "frame" ) );
		_add_element( &_element_frame );

		_element_frame_selected.set_name( string8_c( mode_e_static, "frame_selected" ) );
		_add_element( &_element_frame_selected );

		_element_text.set_name( string8_c( mode_e_static, "text" ) );
		_element_text.on_value_changed_commit.subscribe( this, &menu_control_list_item_i::_handle_element_text_on_value_changed_commit );
		_add_element( &_element_text );

		set_style_map_key( string8_c( mode_e_static, "e_list_item" ) );
	}

	void_c menu_control_list_item_i::update_animations( float32_c time_step )
	{
		for ( sint32_c i = 0; i < _element_list.get_length(); i++ )
		{
			menu_element_c * element = _element_list[ i ];
			element->set_is_selected( _is_mouse_focused );
			element->set_is_pressed( _is_pressed );
		}
		menu_control_c::update_animations( time_step );
	}

	void_c menu_control_list_item_i::update_item_layout()
	{
		assert( _mother_control != nullptr );
		box32x2_c new_local_box;
		new_local_box.minimum.a = _mother_control->get_local_box().minimum.a;
		new_local_box.maximum.a = _mother_control->get_local_box().maximum.a;
		new_local_box.minimum.b = 0.0f;
		new_local_box.maximum.b = 100.0f;
		set_layout_simple( new_local_box );
		_element_text.update_text_layout();
		float32_c content_height = _element_text.get_content_height();
		if ( content_height < 8.0f )
		{
			content_height = 8.0f;
		}
		else
		{
			content_height = static_cast< float32_c >( static_cast< sint32_c >( content_height + 10.5f ) );
		}
		new_local_box = _local_box;
		new_local_box.minimum.b = content_height * -0.5f;
		new_local_box.maximum.b = content_height * 0.5f;
		set_layout_simple( new_local_box );
	}

	boolean_c menu_control_list_item_i::get_is_selected() const
	{
		return _is_selected;
	}

	void_c menu_control_list_item_i::set_is_selected( boolean_c value )
	{
		menu_control_list_i * list = static_cast< menu_control_list_i * >( _mother_control );
		if ( _is_selected != value )
		{
			_is_selected = value;
			_element_frame_selected.set_is_showing( value );
			if ( value )
			{
				if ( list != nullptr )
				{
					assert( list->_selected_item_list.find_index_of( this ) == -1 );
					list->_selected_item_list.insert_at_end( this );
					if ( list->_selected_item_limit >= 0 )
					{
						sint32_c deselect_count = list->_selected_item_list.get_length() - list->_selected_item_limit;
						if ( deselect_count > 0 )
						{
							for ( sint32_c i = 0; i < deselect_count; i++ )
							{
								menu_control_list_item_i * list_item = list->_selected_item_list[ i ];
								list_item->_is_selected = false;
								list_item->_element_frame_selected.set_is_showing( false );
							}
							list->_selected_item_list.remove_range_at_index( deselect_count, 0 );
							list->_on_selection_changed();
						}
					}
				}
			}
			else
			{
				if ( list != nullptr )
				{
					assert( list->_selected_item_list.find_index_of( this ) != -1 );
					list->_on_selection_changed();
				}
			}
		}
	}

	string_c::reference_c & menu_control_list_item_i::get_string_reference()
	{
		return _element_text.get_string_reference();
	}

	string16_c menu_control_list_item_i::get_text_plain() const
	{
		return _element_text.get_plain_text();
	}

	void_c menu_control_list_item_i::set_text_plain( string16_c const & plain_text )
	{
		_element_text.set_plain_text( plain_text );
		if ( _mother_control )
		{
			update_transform_and_layout();
			menu_control_list_i * list = static_cast< menu_control_list_i * >( _mother_control );
			list->_list_item_holder->_item_layout_is_dirty = true;
		}
	}

	void_c menu_control_list_item_i::set_text_rich( string8_c const & plain_text_with_markup )
	{
		_element_text.set_rich_text( plain_text_with_markup );
		if ( _mother_control )
		{
			update_transform_and_layout();
			menu_control_list_i * list = static_cast< menu_control_list_i * >( _mother_control );
			list->_list_item_holder->_item_layout_is_dirty = true;
		}
	}

	void_c menu_control_list_i::_vertical_scroll_handle_on_value_changed( menu_control_scroll_c * control )
	{
		_list_item_holder->_content_offset.b = static_cast< float32_c >( -_vertical_scroll->get_value() );
	}

	void_c menu_control_list_i::_set_selected_item_limit( sint32_c selected_item_limit )
	{
		assert( selected_item_limit >= -1 );

		// set new limit.
		_selected_item_limit = selected_item_limit;

		// apply new limit.
		// if needed then change current number of selected items to fit within new constraint.
		// this will also invoke the on_selected_list_item_changed event.
		if ( _selected_item_limit >= 0 )
		{
			sint32_c deselect_count = _selected_item_list.get_length() - selected_item_limit;
			if ( deselect_count > 0 )
			{
				for ( sint32_c i = 0; i < deselect_count; i++ )
				{
					menu_control_list_item_i * list_item = _selected_item_list[ i ];
					list_item->_is_selected = false;
					list_item->_element_frame_selected.set_is_showing( false );
				}
				_selected_item_list.remove_range_at_index( deselect_count, 0 );
				_on_selection_changed();
			}
		}
	}

	sint32_c menu_control_list_i::_get_selected_item_index() const
	{
		if ( _selected_item_list.get_length() > 0 )
		{
			return _selected_item_list[ _selected_item_list.get_length() - 1 ]->_index;
		}
		return -1;
	}

	void_c menu_control_list_i::_set_selected_item_index( sint32_c item_index )
	{
		assert( item_index >= -1 && item_index < _control_list.get_length() );

		boolean_c selection_changed = false;

		if ( _selected_item_list.get_length() > 0 )
		{
			for ( sint32_c i = 0; i < _selected_item_list.get_length(); i++ )
			{
				menu_control_list_item_i * list_item = _selected_item_list[ i ];
				list_item->_is_selected = false;
				list_item->_element_frame_selected.set_is_showing( false );
			}
			_selected_item_list.remove_all();
			selection_changed = true;
		}

		if ( item_index >= 0 )
		{
			menu_control_list_item_i * list_item = dynamic_cast< menu_control_list_item_i * >( _control_list[ item_index ] );
			assert( list_item != nullptr );
			assert( list_item->_index == item_index );
			assert( list_item->_is_selected == false );
			list_item->_is_selected = true;
			list_item->_element_frame_selected.set_is_showing( true );
			_selected_item_list.insert_at_end( list_item );
			selection_changed = true;
		}

		if ( selection_changed )
		{
			_on_selection_changed();
		}
	}

	void_c menu_control_list_i::_on_selection_changed()
	{
	}

	void_c menu_control_list_i::_on_mouse_over_lost()
	{
		if ( _layer == menu_layer_e_popup && _is_deep_text_focused == 0 )
		{
			hide( false );
		}
	}

	void_c menu_control_list_i::_on_deep_text_focus_lost()
	{
		if ( _layer == menu_layer_e_popup )
		{
			hide( false );
			menu_control_c::_on_deep_text_focus_lost();
		}
	}

	void_c menu_control_list_i::_on_input( input_event_c * input_event )
	{
		if ( input_event->type == input_event_c::type_e_mouse_wheel )
		{
			_vertical_scroll->inject_mouse_wheel_input( input_event->mouse_wheel_delta );
		}
	}

	menu_control_list_item_holder_i::menu_control_list_item_holder_i()
		: menu_control_c()
		, _item_layout_is_dirty( false )
	{
	}

	void_c menu_control_list_item_holder_i::update_transform_and_layout()
	{
		if ( _mother_control != nullptr )
		{
			_local_origin = vector32x2_c( 0.0f, 0.0f );
			float32_c last_width = _local_box.get_width();
			_local_box = _mother_control->get_local_box();
			if ( _local_box.get_width() != last_width || _item_layout_is_dirty )
			{
				_item_layout_is_dirty = true;
				_layout_items();
				assert( _item_layout_is_dirty == false );
			}
		}
		menu_control_c::update_transform_and_layout();
	}

	void_c menu_control_list_item_holder_i::update_animations( float32_c time_step )
	{
		if ( _item_layout_is_dirty )
		{
			update_transform_and_layout();
			assert( _item_layout_is_dirty == false );
		}
		menu_control_c::update_animations( time_step );
	}

	void_c menu_control_list_item_holder_i::_layout_items()
	{
		if ( _mother_control == nullptr || _item_layout_is_dirty == false )
		{
			return;
		}

		// layout list items in a stack from top to bottom.
		menu_control_list_i * mother = static_cast< menu_control_list_i * >( _mother_control );
		float32_c item_width = _local_box.get_width();
		float32_c item_left = _local_box.minimum.a;
		float32_c item_right = _local_box.maximum.a;
		float32_c item_y = _local_box.minimum.b;
		for ( sint32_c i = 0; i < _control_list.get_length(); i++ )
		{
			menu_control_list_item_i * list_item = static_cast< menu_control_list_item_i * >( _control_list[ i ] );
			//if ( list_item->_local_box.get_width() != item_width )
			{
				list_item->update_item_layout();
			}
			list_item->_local_origin.a = 0.0f;
			list_item->_local_origin.b = item_y + list_item->_local_box.get_height() * 0.5f;
			item_y += list_item->_local_box.get_height();
		}
		float32_c content_height = item_y - _local_box.minimum.b;
		if ( content_height < 10.0f)
		{
			content_height = 10.0f;
		}

		// change vertical size of this and mother if needed.
		if ( mother->_vertical_size_mode == menu_size_mode_e_fit_content )
		{
			if ( mother->_vertical_size_maximum >= 1.0f && content_height > mother->_vertical_size_maximum )
			{
				content_height = mother->_vertical_size_maximum;
			}
			_local_box.minimum.b = content_height * -0.5f;
			_local_box.maximum.b = content_height * 0.5f;
			mother->_local_box = _local_box;
			// position list items vertically again.
			// this is needed because origin is in center of _local_box, and the vertical extents of _local_box changed.
			float32_c item_y = _local_box.minimum.b;
			for ( sint32_c i = 0; i < _control_list.get_length(); i++ )
			{
				menu_control_list_item_i * list_item = static_cast< menu_control_list_item_i * >( _control_list[ i ] );
				list_item->_local_origin.a = 0.0f;
				list_item->_local_origin.b = item_y + list_item->_local_box.get_height() * 0.5f;
				item_y += list_item->_local_box.get_height();
			}
		}
		_item_layout_is_dirty = false;

		// this might cause one more recursion if the scroll bar value changes, but it should not be infinite.
		mother->_vertical_scroll->set_value_range_and_page_size( 0.0f, content_height, _local_box.get_height() );
		mother->_vertical_scroll->update_visibility( mother->_vertical_scroll_visibility );
	}

	void_c menu_control_list_item_holder_i::_add_control( menu_control_c * control, sint32_c index )
	{
		menu_control_list_i * mother = static_cast< menu_control_list_i * >( _mother_control );
		menu_control_list_item_i * list_item = dynamic_cast< menu_control_list_item_i * >( control );
		assert( list_item != nullptr );
		menu_control_c::_add_control( control, index );
		if ( list_item->_is_selected )
		{
			mother->_selected_item_list.insert_at_end( list_item );
			mother->_on_selection_changed();
		}
		_item_layout_is_dirty = true;
	}

	void_c menu_control_list_item_holder_i::_remove_control( sint32_c index )
	{
		menu_control_list_i * mother = static_cast< menu_control_list_i * >( _mother_control );
		menu_control_list_item_i * list_item = dynamic_cast< menu_control_list_item_i * >( _control_list[ index ] );
		assert( list_item != nullptr );
		menu_control_c::_remove_control( index );
		if ( list_item->_is_selected )
		{
			assert( mother->_selected_item_list.remove( list_item ) );
			mother->_on_selection_changed();
		}
		_item_layout_is_dirty = true;
	}

	void_c menu_control_list_item_holder_i::_remove_and_delete_all_controls()
	{
		menu_control_list_i * mother = static_cast< menu_control_list_i * >( _mother_control );
		menu_control_c::_remove_and_delete_all_controls();
		if ( mother->_selected_item_list.get_length() > 0 )
		{
			mother->_selected_item_list.remove_all();
			mother->_on_selection_changed();
		}
		_item_layout_is_dirty = true;
	}

	void_c menu_control_list_i::_show_at( box32x2_c const & around, menu_popup_type_e order )
	{
		box32x2_c new_local_box;
		if ( order == menu_popup_type_e_bottom )
		{
			// try to open towards bottom right.
			new_local_box.minimum.a = around.minimum.a;
			new_local_box.minimum.b = around.maximum.b;
			new_local_box.maximum.a = new_local_box.minimum.a + _local_box.get_width();
			new_local_box.maximum.b = new_local_box.minimum.b + _local_box.get_height();

			// try to open towards buttom left.

			// try to open towards top right.

			// try to open towards top left.
		}
		else if ( order == menu_popup_type_e_right )
		{
			// try to open towards right bottom.
			new_local_box.minimum.a = around.maximum.a;
			new_local_box.minimum.b = around.minimum.b;
			new_local_box.maximum.a = new_local_box.minimum.a + _local_box.get_width();
			new_local_box.maximum.b = new_local_box.minimum.b + _local_box.get_height();

			// try to open towards right top.

			// try to open towards left bottom.

			// try to open towards left top.
		}
		set_layout_simple( new_local_box );
		show_immediately();
		give_text_focus();
	}

	menu_control_list_i::menu_control_list_i()
		: menu_control_c()
		, _element_frame()
		, _list_item_holder( nullptr )
		, _vertical_size_mode( menu_size_mode_e_fixed )
		, _vertical_size_maximum( 0.0f )
		, _vertical_scroll_visibility( menu_visibility_mode_e_automatic )
		, _vertical_scroll( nullptr )
		, _selected_item_limit( 0 )
		, _selected_item_list()
	{
		_layer = menu_layer_e_base;
		_vertical_size_mode = menu_size_mode_e_fixed;
		_vertical_size_maximum = 0.0f;

		_element_frame.set_name( string8_c( mode_e_static, "frame" ) );
		_element_frame.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_add_element( &_element_frame );

		_vertical_scroll = new menu_control_scroll_c();
		_vertical_scroll->set_name( string8_c( mode_e_static, "vertical_scroll" ) );
		_vertical_scroll->set_orientation( menu_control_scroll_c::orientation_e_vertical );
		_vertical_scroll->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 8.0f, 0.0f, 0.0f, 0.0f ) );
		_vertical_scroll->on_value_changed_preview.subscribe( this, &menu_control_list_i::_vertical_scroll_handle_on_value_changed );
		_add_control( _vertical_scroll );

		_list_item_holder = new menu_control_list_item_holder_i();
		_list_item_holder->set_name( string8_c( mode_e_static, "list_item_holder" ) );
		_add_control( _list_item_holder );

		set_style_map_key( string8_c( mode_e_static, "e_list" ) );
	}

	void_c menu_control_list_i::update_animations( float32_c time_step )
	{
		boolean_c is_descendant_mouse_focused = _get_is_descendant_mouse_focused();

		_element_frame.set_is_selected( _is_mouse_focused || is_descendant_mouse_focused );

		menu_control_c::update_animations( time_step );
	}

	void_c menu_control_list_i::load_properties( data_scribe_markup_c::node_c const * node )
	{
		menu_control_c::load_properties( node );

		data_scribe_markup_c::attribute_c const * attribute = nullptr;
		
		_vertical_size_mode = menu_size_mode_e_fixed;
		attribute = node->find_attribute( "vertical_size_mode" );
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
		set_vertical_scroll_visibility_mode( vertical_scroll_visibility_mode );
	}

	//void_c menu_control_list_i::set_mode( mode_e value, float32_c vertical_size_maximum )
	//{
	//	if ( value == menu_list_mode_e_list )
	//	{
	//		_layer = menu_layer_e_base;
	//		set_vertical_size_mode( menu_size_mode_e_fixed, vertical_size_maximum );
	//		set_scroll_bar_y_mode( menu_scroll_bar_mode_e_automatic );
	//		set_list_item_select_mode( menu_list_item_select_mode_e_single );
	//	}
	//	else if ( value == menu_list_mode_e_popup_list )
	//	{
	//		_layer = menu_layer_e_popup;
	//		set_vertical_size_mode( menu_size_mode_e_fit_to_content, vertical_size_maximum );
	//		set_scroll_bar_y_mode( menu_scroll_bar_mode_e_automatic );
	//		set_list_item_select_mode( menu_list_item_select_mode_e_single );
	//	}
	//	else if ( value == menu_list_mode_e_popup_menu )
	//	{
	//		_layer = menu_layer_e_popup;
	//		set_vertical_size_mode( menu_size_mode_e_fit_to_content, vertical_size_maximum );
	//		set_scroll_bar_y_mode( menu_scroll_bar_mode_e_automatic );
	//		set_list_item_select_mode( menu_list_item_select_mode_e_none );
	//	}
	//}

	menu_visibility_mode_e menu_control_list_i::get_vertical_scroll_visibility_mode() const
	{
		return _vertical_scroll_visibility;
	}

	void_c menu_control_list_i::set_vertical_scroll_visibility_mode( menu_visibility_mode_e value )
	{
		_vertical_scroll_visibility = value;
		_vertical_scroll->update_visibility( value );
	}

	menu_control_scroll_c * menu_control_list_i::get_vertical_scroll()
	{
		return _vertical_scroll;
	}

}
