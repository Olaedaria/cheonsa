#include "cheonsa_menu_control_collection.h"
#include "cheonsa_ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	menu_control_collection_item_c::value_c::value_c()
		: display_value( mode_e_static, L"[none]" )
		, absolute_value( 0 )
	{
	}

	void_c menu_control_collection_item_c::_cache_values()
	{
		if ( _values )
		{
			delete[] _values;
			_values = nullptr;
		}
		if ( _mother_collection )
		{
			sint32_c value_count = _mother_collection->_column_list.get_length();
			assert( value_count > 0 );
			_values = new value_c[ value_count ];
			for ( sint32_c i = 0; i < value_count; i++ )
			{
				query_value( _mother_collection->_column_list[ i ]->_key, _values[ i ].display_value, _values[ i ].absolute_value );
			}
		}
	}

	menu_control_collection_item_c::menu_control_collection_item_c()
		: _mother_collection( nullptr )
		, _values( nullptr )
		, _index( -1 )
		, _is_selected( false )
	{
	}

	sint32_c menu_control_collection_item_c::get_index() const
	{
		return _index;
	}

	boolean_c menu_control_collection_item_c::query_icon( resource_file_texture_c * & result ) const
	{
		result = nullptr;
		return false;
	}

	boolean_c menu_control_collection_item_c::query_value( string8_c const & key, string16_c & display_value, sint64_c & absolute_value ) const
	{
		display_value = string16_c( mode_e_static, L"[none]" );
		absolute_value = 0;
		return false;
	}

	menu_control_collection_item_c::value_c const * menu_control_collection_item_c::get_value( string8_c const & key ) const
	{
		assert( _mother_collection );
		sint32_c column_count = _mother_collection->_column_list.get_length();
		for ( sint32_c i = 0; i < column_count; i++ )
		{
			if ( _mother_collection->_column_list[ i ]->_key == key )
			{
				return &_values[ i ];
			}
		}
		return nullptr;
	}

	boolean_c menu_control_collection_item_c::set_value( string8_c const & key, string16_c const & display_value )
	{
		return false;
	}

	boolean_c menu_control_collection_item_c::get_is_selected() const
	{
		return _is_selected;
	}

	void_c menu_control_collection_item_c::set_is_selected( boolean_c value )
	{
		if ( _is_selected != value )
		{
			_is_selected = value;
			if ( _mother_collection )
			{
				if ( _is_selected )
				{
					if ( _mother_collection->_select_mode == menu_control_collection_c::select_mode_e_single )
					{
						for ( sint32_c i = 0; i < _mother_collection->_selected_item_list.get_length(); i++ )
						{
							_mother_collection->_selected_item_list[ i ]->_is_selected = false;
						}
						_mother_collection->_selected_item_list.remove_all();
						_mother_collection->_selected_item_list.insert_at_end( this );
					}
					else if ( _mother_collection->_select_mode == menu_control_collection_c::select_mode_e_multiple )
					{
						_mother_collection->_selected_item_list.insert_at_end( this );
					}
					else //if ( _mother_collection->_select_mode == menu_control_collection_c::select_mode_e_none )
					{
						_is_selected = false;
					}
				}
				else
				{
					_mother_collection->_selected_item_list.remove( this );
				}
			}
		}
	}

	sint32_c menu_control_collection_item_c::relative_value_function( menu_control_collection_item_c * const & a, menu_control_collection_item_c * const & b )
	{
		sint32_c sort_index = a->_mother_collection->_sort_index;
		return ops::string16_sort_compare( a->_values[ sort_index ].display_value, b->_values[ sort_index ].display_value );
	}

	uint64_c menu_control_collection_item_c::absolute_value_function( menu_control_collection_item_c * const & a )
	{
		return a->_values[ a->_mother_collection->_sort_index ].absolute_value;
	}

	menu_control_collection_c::column_c::column_c()
		: _key()
		, _position( 0 )
		, _width( 0 )
		, _sort_by( sort_by_e_display_value )
		, _is_editable( false )
		, _element_frame()
		, _element_text()
	{
	}

	void_c menu_control_collection_c::_handle_on_value_changed( menu_control_scroll_c * scroll )
	{
		_item_layout_is_dirty = true;
	}

	void_c menu_control_collection_c::_cache_items_values()
	{
		for ( sint32_c i = 0; i < _item_list.get_length(); i++ )
		{
			_item_list[ i ]->_cache_values();
		}
		_value_cache_is_dirty = false;
	}

	void_c menu_control_collection_c::_sort_items()
	{
		assert( _sort_index >= 0 && _sort_index < _column_list.get_length() );
		if ( _value_cache_is_dirty ) // update and cache values if needed so that we can sort them.
		{
			_cache_items_values();
		}
		column_c * sort_column = _column_list[ _sort_index ];
		if ( sort_column->_sort_by == sort_by_e_display_value )
		{
			_item_list.insertion_sort( &menu_control_collection_item_c::relative_value_function, _sort_order == sort_order_e_descending );
		}
		else if ( sort_column->_sort_by == sort_by_e_absolute_value )
		{
			_item_list.quick_sort( &menu_control_collection_item_c::absolute_value_function, _sort_order == sort_order_e_descending );
		}
		_sort_is_dirty = false;
	}

	box32x2_c menu_control_collection_c::_get_item_rectangle( sint32_c item_index, float32_c x, float32_c y )
	{
		box32x2_c item_rectangle;
		if ( _display_mode == display_mode_e_icons )
		{
			sint32_c visible_column_count = ops::math_minimum( 1, static_cast< sint32_c >( _local_box.get_width() / _icons_item_width ) );
			item_rectangle.minimum.a = x + ( ( item_index % visible_column_count ) * _icons_item_width );
			item_rectangle.minimum.b = y + ( ( item_index / visible_column_count ) * _icons_item_height );
			item_rectangle.maximum.a = item_rectangle.minimum.a + _icons_item_width;
			item_rectangle.maximum.b = item_rectangle.minimum.b + _icons_item_height;
		}
		else
		{
			assert( _column_list.get_length() > 0 );
			column_c * last_column = _column_list[ _column_list.get_length() - 1 ];
			item_rectangle.minimum.a = x + _local_box.minimum.a;
			item_rectangle.minimum.b = y + ( item_index * _details_item_height );
			item_rectangle.maximum.a = item_rectangle.minimum.a + last_column->_position + last_column->_width;
			item_rectangle.maximum.b = item_rectangle.minimum.b + _details_item_height;
		}
		return item_rectangle;
	}

	menu_control_collection_item_c * menu_control_collection_c::_pick_item_at_local_point( vector32x2_c const & local_point )
	{
		float32_c x = 0.0f;
		float32_c y = static_cast< float32_c >( -_vertical_scroll->get_value() );
		sint32_c next_visible_item_index = 0;
		if ( _display_mode == display_mode_e_icons )
		{
			sint32_c visible_column_count = ops::math_minimum( 1, static_cast< sint32_c >( _local_box.get_width() / _icons_item_width ) );
			next_visible_item_index = visible_column_count * static_cast< sint32_c >( -y / _icons_item_height );
		}
		else
		{
			next_visible_item_index = static_cast< sint32_c >( -y ) / _details_item_height;
		}
		for ( sint32_c i = next_visible_item_index; i < _item_list.get_length(); i++ )
		{
			box32x2_c item_rectangle = _get_item_rectangle( i, x, y );
			if ( ops::intersect_rectangle_vs_rectangle( _local_box, item_rectangle ) )
			{
				if ( ops::intersect_rectangle_vs_point( item_rectangle, local_point ) )
				{
					return _item_list[ i ];
				}
			}
			else
			{
				return nullptr;
			}
		}
		return nullptr;
	}

	void_c menu_control_collection_c::_on_clicked( input_event_c * input_event )
	{
		vector32x2_c local_mouse_position = transform_global_point_to_local_point( input_event->menu_global_mouse_position );
		menu_control_collection_item_c * picked_item = _pick_item_at_local_point( local_mouse_position );
		if ( ( ( input_event->modifier_keys_state[ input_modifier_flag_e_ctrl ] & input_key_state_bit_e_on ) != 0 ) && ( _select_mode == select_mode_e_multiple ) )
		{
			if ( picked_item )
			{
				picked_item->set_is_selected( !picked_item->get_is_selected() );
			}
		}
		else
		{
			if ( picked_item )
			{
				picked_item->set_is_selected( true );
			}
			else
			{
				
			}
		}
	}

	void_c menu_control_collection_c::_on_multi_clicked( input_event_c * input_event )
	{
		if ( input_event->mouse_key_multi_click_count == 2 )
		{
			on_selected_items_invoked.invoke( this );
		}
	}

	void_c menu_control_collection_c::_on_input( input_event_c * input_event )
	{
		if ( _is_mouse_overed )
		{
		}

		if ( input_event->type == input_event_c::type_e_mouse_move || input_event->type == input_event_c::type_e_mouse_key_pressed )
		{
			vector32x2_c local_mouse_position = transform_global_point_to_local_point( input_event->menu_global_mouse_position );
			_mouse_selected_item = _pick_item_at_local_point( local_mouse_position );
		}

		float32_c x = 0.0f;
		float32_c y = static_cast< float32_c >( -_vertical_scroll->get_value() );
		_element_mouse_selected_frame.set_is_showing( false );
		if ( _mouse_selected_item )
		{
			_element_mouse_selected_frame.set_is_showing( true );
			box32x2_c item_rectangle = _get_item_rectangle( _mouse_selected_item->_index, x, y );
			_element_mouse_selected_frame.set_layout_simple( item_rectangle );
		}
		_element_last_selected_frame.set_is_showing( false );
		if ( _last_selected_item )
		{
			_element_last_selected_frame.set_is_showing( true );
			box32x2_c item_rectangle = _get_item_rectangle( _last_selected_item->_index, x, y );
			_element_last_selected_frame.set_layout_simple( item_rectangle );
		}
	}

	menu_control_collection_c::menu_control_collection_c()
		: menu_control_c()
		, _element_frame()
		, _mouse_selected_item( nullptr )
		, _element_mouse_selected_frame()
		, _last_selected_item( nullptr )
		, _element_last_selected_frame()
		, _vertical_scroll_visibility( menu_visibility_mode_e_automatic )
		, _vertical_scroll( nullptr )
		, _display_mode( display_mode_e_icons )
		, _icons_item_width( 60 )
		, _icons_item_height( 100 )
		, _icons_icon_height( 60 )
		, _details_item_height( 20 )
		, _currently_visible_item_start( 0 )
		, _currently_visible_item_count( 0 )
		, _column_list()
		, _select_mode( select_mode_e_single )
		, _sort_key()
		, _sort_order( sort_order_e_descending )
		, _sort_is_dirty( false )
		, _item_list()
		, _selected_item_list()
		, _item_renaming( nullptr )
		, _item_renaming_property_key()
		, _item_renaming_text( nullptr )
	{
		_element_frame.set_name( string8_c( mode_e_static, "frame" ) );
		_add_element( &_element_frame );
		
		_element_mouse_selected_frame.set_name( string8_c( mode_e_static, "mouse_selected_frame" ) );
		_add_element( &_element_mouse_selected_frame );

		_element_last_selected_frame.set_name( string8_c( mode_e_static, "last_selected_frame" ) );
		_add_element( &_element_last_selected_frame );

		_vertical_scroll = new menu_control_scroll_c();
		_vertical_scroll->set_name( string8_c( mode_e_static, "vertical_scroll" ) );
		_vertical_scroll->set_orientation( menu_control_scroll_c::orientation_e_vertical );
		_vertical_scroll->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 10.0f, 0.0f, 0.0f, 0.0f ) );
		_vertical_scroll->on_value_changed_preview.subscribe( this, &menu_control_collection_c::_handle_on_value_changed );
		_add_control( _vertical_scroll );
		_vertical_scroll->update_visibility( _vertical_scroll_visibility );

		set_style_map_key( string8_c( mode_e_static, "e_collection" ) );
	}

	void_c menu_control_collection_c::refresh()
	{
		assert( _column_list.get_length() > 0 );

		_currently_visible_item_start = 0;
		_currently_visible_item_count = 0;

		// query and cache item values if needed.
		if ( _value_cache_is_dirty )
		{
			_cache_items_values();
		}

		// sort items if needed.
		if ( _sort_is_dirty )
		{
			_sort_items();
		}

		// reallocate elements if needed.
		sint32_c content_height = 0;
		sint32_c potentially_visible_item_count = 0; // highest number of items that might be visible in a single frame.
		sint32_c potentially_visible_element_count = 0; // number of column and item elements needed.
		if ( _display_mode == display_mode_e_icons )
		{
			potentially_visible_item_count = ops::math_maximum( 1, static_cast< sint32_c >( _local_box.get_width() / _icons_item_width ) ) * ops::math_maximum( 1, static_cast< sint32_c >( _local_box.get_height() / _icons_item_height  ) );
			potentially_visible_element_count = potentially_visible_item_count * 2;
			content_height = _item_list.get_length() / ops::math_maximum( 1, static_cast< sint32_c >( _local_box.get_width() / _icons_item_width ) ) * _icons_item_height;
		}
		else
		{
			potentially_visible_item_count = static_cast< sint32_c >( _local_box.get_height() / _icons_icon_height ) + 1;
			potentially_visible_element_count = ( _column_list.get_length() * 2 ) + ( ( 2 + _column_list.get_length() ) * potentially_visible_item_count );
			content_height = _details_item_height * _item_list.get_length();
		}
		if ( potentially_visible_element_count > _element_list.get_length() - 3 )
		{
			// delete existing column and item elements.
			for ( sint32_c i = 3; i < _element_list.get_length(); i++ )
			{
				delete _element_list[ i ];
				_element_list[ i ] = nullptr;
			}

			// truncate element list.
			_element_list.set_length( 3 );

			// look up styles for application later.
			menu_style_for_frame_c const * column_frame_style = nullptr;
			menu_style_for_text_c const * column_text_style = nullptr;
			menu_style_for_frame_c const * item_selected_frame_style = nullptr;
			menu_style_for_frame_c const * item_icon_frame_style = nullptr;
			menu_style_for_text_c const * item_text_style = nullptr;
			if ( _style_map_reference.get_value() )
			{
				menu_style_map_c::entry_c const * style_map_entry = nullptr;
				style_map_entry = _style_map_reference.get_value()->find_entry( string8_c( mode_e_static, "column_frame" ) );
				if ( style_map_entry && style_map_entry->get_target_type() == "element" )
				{
					column_frame_style = global_engine_instance.interfaces.menu_style_manager->find_style_for_frame( style_map_entry->style_key );
				}
				style_map_entry = _style_map_reference.get_value()->find_entry( string8_c( mode_e_static, "column_text" ) );
				if ( style_map_entry && style_map_entry->get_target_type() == "element" )
				{
					column_text_style = global_engine_instance.interfaces.menu_style_manager->find_style_for_text( style_map_entry->style_key );
				}
				style_map_entry = _style_map_reference.get_value()->find_entry( string8_c( mode_e_static, "item_selected_frame" ) );
				if ( style_map_entry && style_map_entry->get_target_type() == "element" )
				{
					item_selected_frame_style = global_engine_instance.interfaces.menu_style_manager->find_style_for_frame( style_map_entry->style_key );
				}
				style_map_entry = _style_map_reference.get_value()->find_entry( string8_c( mode_e_static, "item_icon_frame" ) );
				if ( style_map_entry && style_map_entry->get_target_type() == "element" )
				{
					item_icon_frame_style = global_engine_instance.interfaces.menu_style_manager->find_style_for_frame( style_map_entry->style_key );
				}
				style_map_entry = _style_map_reference.get_value()->find_entry( string8_c( mode_e_static, "item_text" ) );
				if ( style_map_entry && style_map_entry->get_target_type() == "element" )
				{
					item_text_style = global_engine_instance.interfaces.menu_style_manager->find_style_for_text( style_map_entry->style_key );
				}
			}

			// create new column and item elements.
			// we aren't adding them via the proper _add_element() method because doing so would incur style map lookups each timee.
			// we already looked up which styles to apply from the style maps, so we can just set them directly.
			if ( _display_mode == display_mode_e_icons )
			{
				for ( sint32_c i = 0; i < potentially_visible_item_count; i++ )
				{
					menu_element_frame_c * item_element_frame = new menu_element_frame_c();
					item_element_frame->set_name( string8_c( mode_e_static, "item_icon_frame" ) );
					item_element_frame->get_style_reference().set_value( item_icon_frame_style );
					_element_list.insert_at_end( item_element_frame );
					menu_element_text_c * item_element_text = new menu_element_text_c();
					item_element_text->set_name( string8_c( mode_e_static, "item_text" ) );
					item_element_text->get_style_reference().set_value( item_text_style );
					_element_list.insert_at_end( item_element_text );
				}
			}
			else
			{
				for ( sint32_c i = 0; i < _column_list.get_length(); i++ )
				{
					menu_element_frame_c * column_frame = new menu_element_frame_c();
					column_frame->set_name( string8_c( mode_e_static, "column_frame" ) );
					column_frame->get_style_reference().set_value( column_frame_style );
					_element_list.insert_at_end( column_frame );
					menu_element_text_c * column_text = new menu_element_text_c();
					column_text->set_name( string8_c( mode_e_static, "column_text" ) );
					column_text->get_style_reference().set_value( column_text_style );
					_element_list.insert_at_end( column_text );
				}
				for ( sint32_c i = 0; i < potentially_visible_item_count; i++ )
				{
					menu_element_frame_c * item_selected_frame = new menu_element_frame_c();
					item_selected_frame->set_name( string8_c( mode_e_static, "item_selected_frame" ) );
					item_selected_frame->get_style_reference().set_value( item_selected_frame_style );
					_element_list.insert_at_end( item_selected_frame );
					menu_element_frame_c * item_icon_frame = new menu_element_frame_c();
					item_icon_frame->set_name( string8_c( mode_e_static, "item_icon_frame" ) );
					item_icon_frame->get_style_reference().set_value( item_icon_frame_style );
					_element_list.insert_at_end( item_icon_frame );
					for ( sint32_c j = 0; j < _column_list.get_length(); j++ )
					{
						menu_element_text_c * item_text = new menu_element_text_c();
						item_text->set_name( string8_c( mode_e_static, "item_text" ) );
						item_text->get_style_reference().set_value( item_text_style );
						_element_list.insert_at_end( item_text );
					}
				}
			}
		}

		// resize vertical scroll bar.
		_vertical_scroll->set_value_range_and_page_size( 0.0, static_cast< float64_c >( content_height ), static_cast< float64_c >( _local_box.get_height() ) );

		// layout and update visible item elements if needed.
		if ( _item_layout_is_dirty )
		{
			_item_layout_is_dirty = false;
			float32_c x = 0.0f;
			float32_c y = static_cast< float32_c >( -_vertical_scroll->get_value() );
			if ( _display_mode == display_mode_e_icons )
			{
				sint32_c visible_column_count = ops::math_minimum( 1, static_cast< sint32_c >( _local_box.get_width() / _icons_item_width ) );
				_currently_visible_item_start = static_cast< sint32_c >( -y ) / _icons_item_height * visible_column_count;
				_currently_visible_item_count = 0;
				sint32_c item_element_per_set_count = 3; // selected frame, icon frame, text.
				sint32_c item_element_start = 3 + ( _column_list.get_length() * 2 ); // three for built in frames. then for each column, one frame and one text.
				sint32_c item_element_flat_count = _element_list.get_length() - item_element_start; // total number of elements allocated for items.
				sint32_c item_element_set_count = item_element_flat_count / item_element_per_set_count; // number of elements allocated per item.
				sint32_c i = item_element_start;
				for ( ; i < _element_list.get_length(); i += item_element_per_set_count )
				{
					box32x2_c item_rectangle = _get_item_rectangle( _currently_visible_item_start + _currently_visible_item_count, x, y );
					if ( item_rectangle.minimum.b > _local_box.maximum.b )
					{
						break;
					}
					_currently_visible_item_count++;
					menu_control_collection_item_c * item = _item_list[ _currently_visible_item_start + _currently_visible_item_count ];
					menu_element_frame_c * item_selected_frame = static_cast< menu_element_frame_c * >( _element_list[ i ] );
					item_selected_frame->set_is_showing( item->_is_selected );
					item_selected_frame->set_layout_simple( item_rectangle );
					menu_element_frame_c * item_icon_frame = static_cast< menu_element_frame_c * >( _element_list[ i + 1 ] );
					item_icon_frame->set_is_showing( true );
					item_icon_frame->set_layout_simple( box32x2_c( item_rectangle.minimum.a, item_rectangle.minimum.b, item_rectangle.minimum.a + _icons_item_width, item_rectangle.minimum.b + _icons_icon_height ) );
					menu_element_text_c * item_text = static_cast< menu_element_text_c * >( _element_list[ i + 2 ] );
					item_text->set_is_showing( true );
					item_text->set_layout_simple( box32x2_c( item_rectangle.minimum.a, item_rectangle.minimum.b + _icons_icon_height, item_rectangle.maximum.a, item_rectangle.maximum.b ) );
				}
				for ( ; i < _element_list.get_length(); i++ )
				{
					menu_element_c * element = _element_list[ i ];
					element->set_is_showing( false );
				}
			}
			else
			{
				_currently_visible_item_start = static_cast< sint32_c >( -y ) / _details_item_height;
				_currently_visible_item_count = 0;
				sint32_c item_element_per_set_count = ( 2 + _column_list.get_length() );
				sint32_c item_element_start = 3 + ( _column_list.get_length() * 2 );
				sint32_c item_element_flat_count = _element_list.get_length() - item_element_start;
				sint32_c item_element_set_count = item_element_flat_count / item_element_per_set_count;
				sint32_c i = item_element_start;
				for ( ; i < _element_list.get_length(); i += item_element_per_set_count )
				{
					box32x2_c item_rectangle = _get_item_rectangle( _currently_visible_item_start + _currently_visible_item_count, x, y );
					if ( item_rectangle.minimum.b > _local_box.maximum.b )
					{
						break;
					}
					_currently_visible_item_count++;
					menu_control_collection_item_c * item = _item_list[ _currently_visible_item_start + _currently_visible_item_count ];
					menu_element_frame_c * item_selected_frame = static_cast< menu_element_frame_c * >( _element_list[ i ] );
					item_selected_frame->set_is_showing( true );
					item_selected_frame->set_layout_simple( item_rectangle );
					menu_element_frame_c * item_icon_frame = static_cast< menu_element_frame_c * >( _element_list[ i + 1 ] );
					item_icon_frame->set_is_showing( true );
					item_icon_frame->set_layout_simple( box32x2_c( item_rectangle.minimum.a, item_rectangle.minimum.b, item_rectangle.minimum.a + _details_item_height, item_rectangle.maximum.b ) );
					for ( sint32_c j = 0; j < _column_list.get_length(); j++ )
					{
						column_c const * column = _column_list[ j ];
						menu_element_text_c * item_text = static_cast< menu_element_text_c * >( _element_list[ i + 2 + j ] );
						item_text->set_is_showing( true );
						item_text->set_layout_simple( box32x2_c( item_rectangle.minimum.a + column->_position, item_rectangle.minimum.b, item_rectangle.minimum.a + column->_position + column->_width, item_rectangle.maximum.b ) );
					}
				}
				for ( ; i < _element_list.get_length(); i++ )
				{
					menu_element_c * element = _element_list[ i ];
					element->set_is_showing( false );
				}
			}
		}
	}

	void_c menu_control_collection_c::load_properties( data_scribe_markup_c::node_c const * node )
	{
		menu_control_c::load_properties( node );

		data_scribe_markup_c::attribute_c const * attribute = nullptr;

		remove_all_columns();

		attribute = node->find_attribute( "display_mode" );
		display_mode_e display_mode = display_mode_e_icons;
		if ( attribute )
		{
			if ( attribute->get_value() == "icons" )
			{
				display_mode = display_mode_e_icons;
			}
			else if ( attribute->get_value() == "details" )
			{
				display_mode = display_mode_e_details;
			}
		}
		set_display_mode( display_mode );

		sint32_c icons_item_width = 60;
		sint32_c icons_item_height = 100;
		sint32_c icons_icon_height = 50;
		attribute = node->find_attribute( "icons_item_width" );
		if ( attribute )
		{
			ops::convert_string8_to_sint32( attribute->get_value(), icons_item_width );
		}
		attribute = node->find_attribute( "icons_item_height" );
		if ( attribute )
		{
			ops::convert_string8_to_sint32( attribute->get_value(), icons_item_height );
		}
		attribute = node->find_attribute( "icons_icon_height" );
		if ( attribute )
		{
			ops::convert_string8_to_sint32( attribute->get_value(), icons_icon_height );
		}
		set_icons_metrics( icons_item_width, icons_item_height, icons_icon_height );

		sint32_c details_item_height = 20;
		attribute = node->find_attribute( "details_item_height" );
		if ( attribute )
		{
			ops::convert_string8_to_sint32( attribute->get_value(), details_item_height );
		}
		set_details_metrics( details_item_height );

		data_scribe_markup_c::node_c const * tag = node->find_tag( "column_list" );
		if ( tag )
		{
			core_list_c< data_scribe_markup_c::node_c const * > sub_tags;
			tag->find_tags( "column", sub_tags );
			for ( sint32_c i = 0; i < sub_tags.get_length(); i++ )
			{
				data_scribe_markup_c::node_c const * sub_tag = sub_tags[ i ];

				string8_c column_key;
				string8_c column_display_value_key;
				sint32_c column_width = 0;
				sort_by_e column_sort_by = sort_by_e_display_value;
				boolean_c column_is_editable = false;

				attribute = sub_tag->find_attribute( "key" );
				if ( attribute )
				{
					column_key = attribute->get_value();
				}

				attribute = sub_tag->find_attribute( "display_value_key" );
				if ( attribute )
				{
					column_display_value_key = attribute->get_value();
				}

				attribute = sub_tag->find_attribute( "width" );
				if ( attribute )
				{
					ops::convert_string8_to_sint32( attribute->get_value(), column_width );
				}

				attribute = sub_tag->find_attribute( "sort_by" );
				if ( attribute )
				{
					if ( attribute->get_value() == "display_value" )
					{
						column_sort_by = sort_by_e_display_value;
					}
					else if ( attribute->get_value() == "absolute_value" )
					{
						column_sort_by = sort_by_e_absolute_value;
					}
				}

				attribute = sub_tag->find_attribute( "is_editable" );
				if ( attribute )
				{
					ops::convert_string8_to_boolean( attribute->get_value(), column_is_editable );
				}

				add_column( column_key, column_display_value_key, column_width, column_sort_by, column_is_editable );
			}
		}
	}

	menu_control_collection_c::display_mode_e menu_control_collection_c::get_display_mode() const
	{
		return _display_mode;
	}

	void_c menu_control_collection_c::set_display_mode( display_mode_e value )
	{
		_display_mode = value;
		_item_layout_is_dirty = true;
		for ( sint32_c i = 3; i < _element_list.get_length(); i++ )
		{
			delete _element_list[ i ];
			_element_list[ i ] = nullptr;
		}
		_element_list.set_length( 3 );
	}

	void_c menu_control_collection_c::set_icons_metrics( sint32_c item_width, sint32_c item_height, sint32_c icon_height )
	{
		_icons_item_width = ops::math_clamp( item_width, 10, 1000 );
		_icons_item_height = ops::math_clamp( item_height, 10, 1000 );
		_icons_icon_height = ops::math_clamp( icon_height, 10, 1000 );
		_item_layout_is_dirty = true;
	}

	//void_c menu_control_collection_c::set_list_metrics( sint32_c item_height )
	//{
	//	_list_item_height = ops::math_clamp( item_height, 10, 1000 );
	//}

	void_c menu_control_collection_c::set_details_metrics( sint32_c item_height )
	{
		_details_item_height = ops::math_clamp( item_height, 10, 1000 );
		_item_layout_is_dirty = true;
	}

	string8_c const & menu_control_collection_c::get_sort_key() const
	{
		return _sort_key;
	}

	menu_control_collection_c::sort_order_e menu_control_collection_c::get_sort_order() const
	{
		return _sort_order;
	}

	void_c menu_control_collection_c::set_sort( string8_c const & sort_key, sort_order_e sort_order )
	{
		_sort_key = sort_key;
		_sort_index = -1;
		for ( sint32_c i = 0; i < _column_list.get_length(); i++ )
		{
			if ( _column_list[ i ]->_key == sort_key )
			{
				_sort_index = i;
				break;
			}
		}
		_sort_order = sort_order;
		_sort_is_dirty = true;
		_item_layout_is_dirty = true;
	}

	void_c menu_control_collection_c::add_column( string8_c const & key, string8_c display_value_key, sint32_c width, sort_by_e sort_by, boolean_c is_editable )
	{
		for ( sint32_c i = 0; i < _column_list.get_length(); i++ )
		{
			assert( _column_list[ i ]->_key != key );
		}

		sint32_c index = _column_list.get_length();

		column_c * column = new column_c();
		column->_key = key;
		column->_position = 0;
		if ( _column_list.get_length() > 0 )
		{
			column_c * last_column = _column_list[ _column_list.get_length() - 1 ];
			column->_position = last_column->_position + last_column->_width;
		}
		column->_width = width;
		column->_sort_by = sort_by;
		column->_is_editable = is_editable;
		column->_element_frame.set_name( string8_c( mode_e_static, "column_frame" ) );
		_add_element( &column->_element_frame );
		column->_element_text.set_name( string8_c( mode_e_static, "column_text" ) );
		_add_element( &column->_element_text );
		column->_element_text.get_string_reference().set_key( display_value_key );
		_column_list.insert_at_end( column );

		if ( _sort_index == -1 && _sort_key == key )
		{
			_sort_index = index;
			_sort_is_dirty = true;
		}

		_value_cache_is_dirty = true;
	}

	void_c menu_control_collection_c::remove_all_columns()
	{
		for ( sint32_c i = 0; i < _column_list.get_length(); i++ )
		{
			column_c * column = _column_list[ i ];
			_remove_element( &column->_element_frame );
			_remove_element( &column->_element_text );
		}
		_column_list.remove_all();
		_sort_index = -1;
		_value_cache_is_dirty = true;
	}

	sint32_c menu_control_collection_c::get_item_count()
	{
		return _item_list.get_length();
	}

	menu_control_collection_item_c * menu_control_collection_c::get_item_at_index( sint32_c item_index )
	{
		if ( _value_cache_is_dirty )
		{
			_cache_items_values();
		}
		if ( _sort_is_dirty )
		{
			_sort_items();
		}
		return _item_list[ item_index ];
	}

	void_c menu_control_collection_c::add_item( menu_control_collection_item_c * item )
	{
		assert( item != nullptr );
		assert( item->_mother_collection == nullptr );
		assert( _column_list.get_length() > 0 );
		_sort_is_dirty = true;
		item->_mother_collection = this;
		item->_index = _item_list.get_length();
		if ( _value_cache_is_dirty == false ) // cache now if rest of cache is already up to date since this will be cheap, otherwise we can skip caching now because cache will be rebuilt later.
		{
			item->_cache_values();
		}
		_item_list.insert_at_end( item );
		if ( item->_is_selected )
		{
			if ( _select_mode == select_mode_e_multiple )
			{
				_selected_item_list.insert_at_end( item );
			}
			else if ( _select_mode == select_mode_e_single )
			{
				for ( sint32_c i = 0; i < _selected_item_list.get_length(); i++ )
				{
					_selected_item_list[ i ]->_is_selected = false;
				}
				_selected_item_list.remove_all();
				_selected_item_list.insert_at_end( item );
			}
			else //if ( _select_mode == select_mode_e_none )
			{
				item->_is_selected = false;
			}
		}
	}

	void_c menu_control_collection_c::remove_and_delete_item( sint32_c item_index )
	{
		delete _item_list[ item_index ];
		_item_list.remove_at_index( item_index );
	}

	void_c menu_control_collection_c::remove_and_delete_all_items()
	{
		_item_list.remove_and_delete_all();
		_value_cache_is_dirty = false;
		_sort_is_dirty = false;
	}

	core_list_c< menu_control_collection_item_c * > const & menu_control_collection_c::get_selected_items() const
	{
		return _selected_item_list;
	}

	void_c menu_control_collection_c::set_selected_item( menu_control_collection_item_c * item )
	{
		assert( item->_mother_collection == this );
		for ( sint32_c i = 0; i < _selected_item_list.get_length(); i++ )
		{
			_selected_item_list[ i ]->_is_selected = false;
		}
		_selected_item_list.remove_all();
		if ( item )
		{
			item->_is_selected = true;
			_selected_item_list.insert_at_end( item );
		}
	}

}
