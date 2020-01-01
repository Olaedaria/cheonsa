#include "cheonsa_menu_control_collection.h"
#include "cheonsa__ops.h"
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
		if ( _value_cache )
		{
			delete[] _value_cache;
			_value_cache = nullptr;
		}
		if ( _mother_collection )
		{
			sint32_c value_count = _mother_collection->_column_list.get_length();
			if ( value_count > 0 )
			{
				_value_cache = new value_c[ value_count ];
				for ( sint32_c i = 0; i < value_count; i++ )
				{
					get_value( _mother_collection->_column_list[ i ]->_key, _value_cache[ i ].display_value, _value_cache[ i ].absolute_value );
				}
			}
		}
	}

	menu_control_collection_item_c::menu_control_collection_item_c()
		: _mother_collection( nullptr )
		, _value_cache( nullptr )
		, _group( 0 )
		, _index( -1 )
		, _is_selected( false )
	{
	}

	sint32_c menu_control_collection_item_c::get_group() const
	{
		return _group;
	}

	void_c menu_control_collection_item_c::set_group( sint32_c value )
	{
		_group = value;
	}

	sint32_c menu_control_collection_item_c::get_index() const
	{
		return _index;
	}

	resource_file_texture_c * menu_control_collection_item_c::get_icon_texture() const
	{
		return nullptr;
	}

	boolean_c menu_control_collection_item_c::get_value( string8_c const & property_key, string16_c & display_value, sint64_c & absolute_value ) const
	{
		display_value = string16_c( mode_e_static, L"[none]" );
		absolute_value = 0;
		return false;
	}

	//menu_control_collection_item_c::value_c const * menu_control_collection_item_c::get_cached_value( string8_c const & property_key ) const
	//{
	//	assert( _mother_collection );
	//	sint32_c column_count = _mother_collection->_column_list.get_length();
	//	for ( sint32_c i = 0; i < column_count; i++ )
	//	{
	//		if ( _mother_collection->_column_list[ i ]->_key == property_key )
	//		{
	//			return &_value_cache[ i ];
	//		}
	//	}
	//	return nullptr;
	//}

	boolean_c menu_control_collection_item_c::set_value( string8_c const & property_key, string16_c const & display_value )
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

	sint32_c menu_control_collection_item_c::relative_compare( menu_control_collection_item_c * const & a, menu_control_collection_item_c * const & b )
	{
		return ops::string16_sort_compare_case_insensitive( a->_value_cache[ a->_mother_collection->_sort_index ].display_value, b->_value_cache[ a->_mother_collection->_sort_index ].display_value );
	}

	uint64_c menu_control_collection_item_c::absolute_value( menu_control_collection_item_c * const & a )
	{
		return a->_value_cache[ a->_mother_collection->_sort_index ].absolute_value;
	}

	sint32_c menu_control_collection_item_c::relative_group_compare( menu_control_collection_item_c * const & a, menu_control_collection_item_c * const & b )
	{
		if ( a->_group > b->_group )
		{
			return 1;
		}
		else if ( a->_group < b->_group )
		{
			return -1;
		}
		return 0;
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

	void_c menu_control_collection_c::_handle_on_value_changed( menu_control_scroll_i * scroll )
	{
		_item_layout_is_dirty = true;
	}

	void_c menu_control_collection_c::_update_value_cache()
	{
		for ( sint32_c i = 0; i < _item_list.get_length(); i++ )
		{
			_item_list[ i ]->_cache_values();
		}
		_value_cache_is_dirty = false;
	}

	void_c menu_control_collection_c::_update_sort()
	{
		// find column index to sort by.
		for ( sint32_c i = 0; i < _column_list.get_length(); i++ )
		{
			if ( _column_list[ i ]->_key == _sort_key )
			{
				_sort_index = i;
				
				// update value cache if needed.
				// these values need to be up to date because these are what we sort items by.
				if ( _value_cache_is_dirty )
				{
					_update_value_cache();
				}

				// primary sort, by column.
				column_c * sort_column = _column_list[ _sort_index ];
				if ( sort_column->_sort_by == sort_by_e_display_value )
				{
					_item_list.insertion_sort( &menu_control_collection_item_c::relative_compare, _sort_order == sort_order_e_descending );
				}
				else if ( sort_column->_sort_by == sort_by_e_absolute_value )
				{
					_item_list.quick_sort( &menu_control_collection_item_c::absolute_value, _sort_order == sort_order_e_descending );
				}

				// secondary sort, by group.
				// this should preserve the primary sort order.
				_item_list.insertion_sort( &menu_control_collection_item_c::relative_group_compare, false );

				// finish up.
				for ( sint32_c j = 0; j < _item_list.get_length(); j++ )
				{
					_item_list[ j ]->_index = j;
				}

				_sort_is_dirty = false;

				return;
			}
		}
	}

	void_c menu_control_collection_c::_update_item_layout()
	{
		if ( _value_cache_is_dirty )
		{
			_update_value_cache();
		}

		if ( _sort_is_dirty )
		{
			_update_sort();
		}

		if ( _element_list.get_length() == 3 )
		{
			// (re)add supplemental elements for column(s).
			// add directly, rather than with _add_element(), to avoid (re)resolving style assignment from mother control's style map.
			if ( _display_mode == display_mode_e_details )
			{
				for ( sint32_c i = 0; i < _column_list.get_length(); i++ )
				{
					column_c * column = _column_list[ i ];
					_element_list.insert_at_end( &column->_element_frame );
					_element_list.insert_at_end( &column->_element_text );
				}
			}

			// (re)add supplemental elements for items.
			// add directly, rather than with _add_element(), to avoid (re)resolving style assignment from mother control's style map.
			_element_list.insert_at_end( _item_elements.get_internal_array(), _item_elements.get_length() );
		}

		// calculate potentially visible item count.
		sint32_c content_height = 0;
		sint32_c potentially_visible_item_count = 0; // highest number of items that might be visible in a single frame.
		sint32_c potentially_visible_item_element_count = 0; // number of column and item elements needed.
		if ( _display_mode == display_mode_e_icons )
		{
			sint32_c items_per_x = ops::math_maximum( 1, static_cast< sint32_c >( _local_box.get_width() / _icons_item_width ) );
			sint32_c items_per_y = ops::math_maximum( 1, static_cast< sint32_c >( _local_box.get_height() / _icons_item_height ) );
			potentially_visible_item_count = items_per_x * items_per_y;
			sint32_c elements_per_item = 3; // item_selected_frame, item_icon_frame, item_text.
			potentially_visible_item_element_count = elements_per_item * potentially_visible_item_count;
			content_height = ( _item_list.get_length() / items_per_x + 1 ) * _icons_item_height;
			_vertical_scroll_bar->set_value_range_and_page_size( 0.0, content_height, _local_box.get_height() );
			if ( potentially_visible_item_element_count > _item_elements.get_length() )
			{
				// look up style map style assignments.
				menu_frame_style_c const * item_selected_frame_style = nullptr;
				menu_text_style_c const * item_text_style = nullptr;
				if ( _style_map_reference.get_value() )
				{
					menu_style_map_c::entry_c const * style_map_entry = nullptr;
					style_map_entry = _style_map_reference.get_value()->find_entry( string8_c( mode_e_static, "element:item_selected_frame" ) );
					if ( style_map_entry )
					{
						item_selected_frame_style = engine_c::get_instance()->get_menu_style_manager()->find_frame_style( style_map_entry->style_key );
					}
					style_map_entry = _style_map_reference.get_value()->find_entry( string8_c( mode_e_static, "element:item_text" ) );
					if ( style_map_entry )
					{
						item_text_style = engine_c::get_instance()->get_menu_style_manager()->find_text_style( style_map_entry->style_key );
					}
				}

				// create and add new element instances.
				for ( sint32_c i = _item_elements.get_length() / elements_per_item; i < potentially_visible_item_count; i++ )
				{
					menu_element_frame_c * item_selected_frame = new menu_element_frame_c();
					item_selected_frame->set_mother_control( this );
					item_selected_frame->set_name( string8_c( mode_e_static, "item_selected_frame" ) );
					item_selected_frame->set_shared_color_class( menu_shared_color_class_e_field );
					item_selected_frame->get_style_reference().set_value( item_selected_frame_style );
					_item_elements.insert_at_end( item_selected_frame );
					_element_list.insert_at_end( item_selected_frame );

					menu_element_frame_c * item_icon_frame = new menu_element_frame_c();
					item_icon_frame->set_mother_control( this );
					item_icon_frame->set_name( string8_c( mode_e_static, "item_icon_frame" ) );
					item_icon_frame->set_shared_color_class( menu_shared_color_class_e_field );
					item_icon_frame->set_override_style( &_item_icon_frame_style );
					_item_elements.insert_at_end( item_icon_frame );
					_element_list.insert_at_end( item_icon_frame );

					menu_element_text_c * item_text = new menu_element_text_c();
					item_text->set_mother_control( this );
					item_text->set_name( string8_c( mode_e_static, "item_text" ) );
					item_text->set_shared_color_class( menu_shared_color_class_e_field );
					item_text->get_style_reference().set_value( item_text_style );
					_item_elements.insert_at_end( item_text );
					_element_list.insert_at_end( item_text );
				}
			}

			// allocate and lay out elements to the currently visible set of items.
			sint32_c visible_item_index = static_cast< sint32_c >( _vertical_scroll_bar->get_value() / _icons_item_height ) * items_per_x; // index of first visible item.
			sint32_c allocated_item_count = _item_elements.get_length() / elements_per_item;
			for ( sint32_c i = 0; i < allocated_item_count; i++, visible_item_index++ )
			{
				menu_element_frame_c * item_selected_frame = dynamic_cast< menu_element_frame_c * >( _item_elements[ i * elements_per_item ] );
				menu_element_frame_c * item_icon_frame = dynamic_cast< menu_element_frame_c * >( _item_elements[ i * elements_per_item + 1 ] );
				menu_element_text_c * item_text = dynamic_cast< menu_element_text_c * >( _item_elements[ i * elements_per_item + 2 ] );
				if ( visible_item_index < _item_list.get_length() )
				{
					menu_control_collection_item_c * item = _item_list[ visible_item_index ];
					box32x2_c item_box = _get_item_box( visible_item_index );
					if ( ops::intersect_box_vs_box( item_box, _local_box ) )
					{
						item_selected_frame->set_is_showing( item->get_is_selected() );
						item_selected_frame->set_layout_simple( item_box );
						item_icon_frame->set_is_showing( true );
						resource_file_texture_c * icon_texture = item->get_icon_texture();
						item_icon_frame->set_override_texture( icon_texture );
						if ( icon_texture != nullptr )
						{
							float32_c cx = item_box.minimum.a + _icons_item_width * 0.5f;
							float32_c cy = item_box.minimum.b + _icons_icon_height * 0.5f;
							float32_c w = static_cast< float32_c >( icon_texture->get_video_texture()->get_width() );
							float32_c h = static_cast< float32_c >( icon_texture->get_video_texture()->get_height() );
							cx = cx - w * 0.5f;
							cy = cy - h * 0.5f;
							item_icon_frame->set_layout_simple( box32x2_c( cx, cy, cx + w, cy + h ) );
						}
						else
						{
							item_icon_frame->set_layout_simple( box32x2_c( item_box.minimum.a, item_box.minimum.b, item_box.maximum.a, item_box.minimum.b + _icons_icon_height ) );
						}
						item_text->set_is_showing( true );
						item_text->set_layout_simple( box32x2_c( item_box.minimum.a, item_box.minimum.b + _icons_icon_height + 1.0f, item_box.maximum.a, item_box.maximum.b ) );
						item_text->set_plain_text_value( item->_value_cache[ 0 ].display_value );
						continue;
					}
				}
				item_selected_frame->set_is_showing( false );
				item_icon_frame->set_is_showing( false );
				item_text->set_is_showing( false );
			}
		}
		else
		{
			sint32_c items_per_y = ops::math_maximum( 1, static_cast< sint32_c >( _local_box.get_height() / _details_item_height ) );
			potentially_visible_item_count = items_per_y;
			sint32_c elements_per_item = 2 + _column_list.get_length(); // item_selected_frame, item_icon_frame, item_text, ... .
			potentially_visible_item_element_count = elements_per_item * potentially_visible_item_count;
			content_height = _details_item_height * _item_list.get_length();
			_vertical_scroll_bar->set_value_range_and_page_size( 0.0, content_height, _local_box.get_height() );
			if ( potentially_visible_item_element_count > _item_elements.get_length() )
			{
				// look up style map style assignments.
				menu_frame_style_c const * item_selected_frame_style = nullptr;
				menu_text_style_c const * item_text_style = nullptr;
				if ( _style_map_reference.get_value() )
				{
					menu_style_map_c::entry_c const * style_map_entry = nullptr;
					style_map_entry = _style_map_reference.get_value()->find_entry( string8_c( mode_e_static, "element:item_selected_frame" ) );
					if ( style_map_entry )
					{
						item_selected_frame_style = engine_c::get_instance()->get_menu_style_manager()->find_frame_style( style_map_entry->style_key );
					}
					style_map_entry = _style_map_reference.get_value()->find_entry( string8_c( mode_e_static, "element:item_text" ) );
					if ( style_map_entry )
					{
						item_text_style = engine_c::get_instance()->get_menu_style_manager()->find_text_style( style_map_entry->style_key );
					}
				}

				// create and add new element instances.
				for ( sint32_c i = _item_elements.get_length() / elements_per_item; i < potentially_visible_item_count; i++ )
				{
					menu_element_frame_c * item_selected_frame = new menu_element_frame_c();
					item_selected_frame->set_mother_control( this );
					item_selected_frame->set_name( string8_c( mode_e_static, "item_selected_frame" ) );
					item_selected_frame->set_shared_color_class( menu_shared_color_class_e_field );
					item_selected_frame->get_style_reference().set_value( item_selected_frame_style );
					_item_elements.insert_at_end( item_selected_frame );
					_element_list.insert_at_end( item_selected_frame );

					menu_element_frame_c * item_icon_frame = new menu_element_frame_c();
					item_icon_frame->set_mother_control( this );
					item_icon_frame->set_name( string8_c( mode_e_static, "item_icon_frame" ) );
					item_icon_frame->set_shared_color_class( menu_shared_color_class_e_field );
					item_icon_frame->set_override_style( &_item_icon_frame_style );
					_item_elements.insert_at_end( item_icon_frame );
					_element_list.insert_at_end( item_icon_frame );

					for ( sint32_c j = 0; j < _column_list.get_length(); j++ )
					{
						column_c * column = _column_list[ j ];
						menu_element_text_c * item_text = new menu_element_text_c();
						item_text->set_mother_control( this );
						item_text->set_name( string8_c( mode_e_static, "item_text" ) );
						item_text->set_shared_color_class( menu_shared_color_class_e_field );
						item_text->get_style_reference().set_value( item_text_style );
						_item_elements.insert_at_end( item_text );
						_element_list.insert_at_end( item_text );
					}
				}
			}

			// allocate and lay out elements to the currently visible set of items.
			sint32_c visible_item_index = static_cast< sint32_c >( _vertical_scroll_bar->get_value() / _details_item_height );
			sint32_c allocated_item_count = _item_elements.get_length() / elements_per_item;
			for ( sint32_c i = 0; i < allocated_item_count; i++, visible_item_index++ )
			{
				menu_element_frame_c * item_selected_frame = dynamic_cast< menu_element_frame_c * >( _item_elements[ i * elements_per_item ] );
				menu_element_frame_c * item_icon_frame = dynamic_cast< menu_element_frame_c * >( _item_elements[ i * elements_per_item + 1 ] );
				if ( visible_item_index < _item_list.get_length() )
				{
					menu_control_collection_item_c * item = _item_list[ visible_item_index ];
					box32x2_c item_box = _get_item_box( visible_item_index );
					if ( ops::intersect_box_vs_box( item_box, _local_box ) )
					{
						item_selected_frame->set_is_showing( true );
						item_selected_frame->set_layout_simple( item_box );
						item_icon_frame->set_is_showing( true );
						item_icon_frame->set_override_texture( item->get_icon_texture() );
						item_icon_frame->set_layout_simple( box32x2_c( item_box.minimum.a, item_box.minimum.b, item_box.minimum.a + _details_item_height, item_box.maximum.b ) );
						for ( sint32_c k = 0; k < _column_list.get_length(); k++ )
						{
							column_c * column = _column_list[ k ];
							menu_element_text_c * item_text = dynamic_cast< menu_element_text_c * >( _item_elements[ i * elements_per_item + 2 + k ] );
							item_text->set_is_showing( true );
							item_text->set_layout_simple( box32x2_c( item_box.minimum.a + _details_item_height + column->_position, item_box.minimum.b, item_box.minimum.a + _details_item_height + column->_position + column->_width, item_box.maximum.b ) );
							item_text->set_plain_text_value( item->_value_cache[ k ].display_value );
						}
						continue;
					}
				}
				item_selected_frame->set_is_showing( false );
				item_icon_frame->set_is_showing( false );
				for ( sint32_c k = 0; k < _column_list.get_length(); k++ )
				{
					menu_element_text_c * item_text = dynamic_cast< menu_element_text_c * >( _item_elements[ i * elements_per_item + 2 + k ] );
					item_text->set_is_showing( false );
				}
			}
		}

		_item_layout_is_dirty = false;
	}

	box32x2_c menu_control_collection_c::_get_item_box( sint32_c item_index )
	{
		box32x2_c result;
		float32_c y = static_cast< float32_c >( -_vertical_scroll_bar->get_value() );
		if ( _display_mode == display_mode_e_icons )
		{
			sint32_c items_per_x = ops::math_maximum( 1, static_cast< sint32_c >( _local_box.get_width() / _icons_item_width ) );
			result.minimum.a = _local_box.minimum.a + static_cast< float32_c >( ( item_index % items_per_x ) * _icons_item_width );
			result.minimum.b = _local_box.minimum.b + static_cast< float32_c >( ( item_index / items_per_x ) * _icons_item_height ) + y;
			result.maximum.a = result.minimum.a + static_cast< float32_c >( _icons_item_width );
			result.maximum.b = result.minimum.b + static_cast< float32_c >( _icons_item_height );
		}
		else
		{
			assert( _column_list.get_length() > 0 );
			column_c * last_column = _column_list[ _column_list.get_length() - 1 ];
			result.minimum.a = _local_box.minimum.a;
			result.minimum.b = _local_box.minimum.b + static_cast< float32_c >( item_index * _details_item_height ) + y;
			result.maximum.a = result.minimum.a + static_cast< float32_c >( last_column->_position + last_column->_width );
			result.maximum.b = result.minimum.b + static_cast< float32_c >( _details_item_height );
		}
		return result;
	}

	menu_control_collection_item_c * menu_control_collection_c::_pick_item_at_local_point( vector32x2_c const & local_point )
	{
		for ( sint32_c i = 0; i < _item_list.get_length(); i++ )
		{
			box32x2_c item_box = _get_item_box( i );
			if ( ops::intersect_box_vs_box( item_box, _local_box ) )
			{
				if ( ops::intersect_box_vs_point( item_box, local_point ) )
				{
					return _item_list[ i ];
				}
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
		refresh();

		if ( _is_mouse_overed )
		{
		}

		if ( input_event->type == input_event_c::type_e_mouse_move || input_event->type == input_event_c::type_e_mouse_key_pressed )
		{
			vector32x2_c local_mouse_position = transform_global_point_to_local_point( input_event->menu_global_mouse_position );
			_mouse_selected_item = _pick_item_at_local_point( local_mouse_position );
		}

		_element_mouse_selected_frame.set_is_showing( false );
		if ( _mouse_selected_item )
		{
			_element_mouse_selected_frame.set_is_showing( true );
			box32x2_c item_box = _get_item_box( _mouse_selected_item->_index );
			_element_mouse_selected_frame.set_layout_simple( item_box );
		}
		_element_last_selected_frame.set_is_showing( false );
		if ( _last_selected_item )
		{
			_element_last_selected_frame.set_is_showing( true );
			box32x2_c item_box = _get_item_box( _last_selected_item->_index );
			_element_last_selected_frame.set_layout_simple( item_box );
		}
	}

	menu_control_collection_c::menu_control_collection_c()
		: menu_control_c()
		//, _element_frame()
		, _mouse_selected_item( nullptr )
		, _element_mouse_selected_frame()
		, _last_selected_item( nullptr )
		, _element_last_selected_frame()
		, _vertical_scroll_bar_visibility( menu_visibility_mode_e_automatic )
		, _vertical_scroll_bar( nullptr )
		, _display_mode( display_mode_e_icons )
		, _icons_item_width( 100 )
		, _icons_item_height( 120 )
		, _icons_icon_height( 80 )
		, _details_item_height( 20 )
		, _column_list()
		, _select_mode( select_mode_e_single )
		, _sort_key()
		, _sort_index( -1 )
		, _sort_order( sort_order_e_descending )
		, _sort_is_dirty( false )
		, _item_list()
		, _selected_item_list()
	{
		_element_frame.set_name( string8_c( mode_e_static, "frame" ) );
		_element_frame.set_shared_color_class( menu_shared_color_class_e_field );
		_add_element( &_element_frame );
		
		_element_mouse_selected_frame.set_name( string8_c( mode_e_static, "mouse_selected_frame" ) );
		_element_mouse_selected_frame.set_is_showing( false );
		_add_element( &_element_mouse_selected_frame );

		_element_last_selected_frame.set_name( string8_c( mode_e_static, "last_selected_frame" ) );
		_element_last_selected_frame.set_is_showing( false );
		_add_element( &_element_last_selected_frame );

		_vertical_scroll_bar = new menu_control_scroll_bar_vertical_c();
		_vertical_scroll_bar->set_name( string8_c( mode_e_static, "vertical_scroll_bar" ) );
		_vertical_scroll_bar->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 10.0f, 0.0f, 0.0f, 0.0f ) );
		_vertical_scroll_bar->on_value_changed_preview.subscribe( this, &menu_control_collection_c::_handle_on_value_changed );
		_give_control( _vertical_scroll_bar );
		_vertical_scroll_bar->update_visibility( _vertical_scroll_bar_visibility );

		set_style_map_key( string8_c( mode_e_static, "e_collection" ) );

		_item_icon_frame_style.texture_map_mode = menu_frame_style_c::texture_map_mode_e_scale_to_fit;
	}

	menu_control_collection_c::~menu_control_collection_c()
	{
		_column_list.remove_and_delete_all();
		_item_list.remove_and_delete_all();
		_selected_item_list.remove_all();
	}

	void_c menu_control_collection_c::update_animations( float32_c time_step )
	{
		menu_control_c::update_animations( time_step );

		boolean_c is_descendant_mouse_focused = _get_is_descendant_mouse_focused();

		_element_frame.set_is_selected( _is_mouse_focused || is_descendant_mouse_focused );
		_element_frame.set_is_pressed( _is_pressed );
	}

	void_c menu_control_collection_c::update_transform_and_layout()
	{
		menu_control_c::update_transform_and_layout();
		_update_item_layout();
	}

	void_c menu_control_collection_c::refresh()
	{
		assert( _column_list.get_length() > 0 );
		if ( _value_cache_is_dirty )
		{
			_update_value_cache();
		}
		if ( _sort_is_dirty )
		{
			_update_sort();
		}
		if ( _item_layout_is_dirty )
		{
			_update_item_layout();
		}
	}

	void_c menu_control_collection_c::load_properties( data_scribe_markup_c::node_c const * node )
	{
		menu_control_c::load_properties( node );

		data_scribe_markup_c::attribute_c const * attribute = nullptr;

		//remove_all_columns();

		/*
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
		*/

		/*
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
		*/

		/*
		data_scribe_markup_c::node_c const * tag = node->find_tag( "column_list" );
		if ( tag )
		{
			core_list_c< data_scribe_markup_c::node_c const * > sub_tags;
			tag->find_tags( "column", sub_tags );
			for ( sint32_c i = 0; i < sub_tags.get_length(); i++ )
			{
				data_scribe_markup_c::node_c const * sub_tag = sub_tags[ i ];

				string8_c column_key;
				string16_c column_display_value;
				sint32_c column_width = 0;
				sort_by_e column_sort_by = sort_by_e_display_value;
				boolean_c column_is_editable = false;

				attribute = sub_tag->find_attribute( "key" );
				if ( attribute )
				{
					column_key = attribute->get_value();
				}

				attribute = sub_tag->find_attribute( "display_value" );
				if ( attribute )
				{
					column_display_value = attribute->get_value();
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

				add_column( column_key, column_display_value, column_width, column_sort_by, column_is_editable );
			}
		}
		*/
	}

	menu_control_collection_c::display_mode_e menu_control_collection_c::get_display_mode() const
	{
		return _display_mode;
	}

	void_c menu_control_collection_c::set_display_mode( display_mode_e value )
	{
		if ( _display_mode != value )
		{
			_display_mode = value;
			_item_layout_is_dirty = true;
			_element_list.set_length( 3 );
			_item_elements.remove_and_delete_all();
		}
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
		_sort_order = sort_order;
		_sort_is_dirty = true;
		_item_layout_is_dirty = true;
	}

	void_c menu_control_collection_c::add_column( string8_c const & key, string16_c const & display_value, sint32_c width, sort_by_e sort_by, boolean_c is_editable )
	{
		for ( sint32_c i = 0; i < _column_list.get_length(); i++ )
		{
			assert( _column_list[ i ]->_key != key );
		}

		menu_frame_style_c const * column_frame_style = nullptr;
		menu_text_style_c const * column_text_style = nullptr;
		if ( _style_map_reference.get_value() )
		{
			menu_style_map_c::entry_c const * style_map_entry = nullptr;
			style_map_entry = _style_map_reference.get_value()->find_entry( string8_c( mode_e_static, "element:column_frame" ) );
			if ( style_map_entry )
			{
				column_frame_style = engine_c::get_instance()->get_menu_style_manager()->find_frame_style( style_map_entry->style_key );
			}
			style_map_entry = _style_map_reference.get_value()->find_entry( string8_c( mode_e_static, "element:column_text" ) );
			if ( style_map_entry )
			{
				column_text_style = engine_c::get_instance()->get_menu_style_manager()->find_text_style( style_map_entry->style_key );
			}
		}

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
		column->_element_frame.set_mother_control( this );
		column->_element_frame.set_name( string8_c( mode_e_static, "column_frame" ) );
		column->_element_frame.set_shared_color_class( menu_shared_color_class_e_button );
		column->_element_frame.get_style_reference().set_value( column_frame_style );
		column->_element_text.set_mother_control( this );
		column->_element_text.set_name( string8_c( mode_e_static, "column_text" ) );
		column->_element_text.set_shared_color_class( menu_shared_color_class_e_button );
		column->_element_text.get_style_reference().set_value( column_text_style );
		column->_element_text.set_plain_text_value( display_value );
		_column_list.insert_at_end( column );

		_element_list.set_length( 3 );
		_item_elements.remove_and_delete_all();
		_value_cache_is_dirty = true;
		_item_layout_is_dirty = true;
		if ( _sort_key == key )
		{
			_sort_is_dirty = true;
		}
	}

	void_c menu_control_collection_c::remove_all_columns()
	{
		_column_list.remove_and_delete_all();
		_element_list.set_length( 3 );
		_item_elements.remove_and_delete_all();
		_value_cache_is_dirty = true;
		_item_layout_is_dirty = true;
		_sort_is_dirty = true;
	}

	sint32_c menu_control_collection_c::get_item_count()
	{
		return _item_list.get_length();
	}

	menu_control_collection_item_c * menu_control_collection_c::get_item_at_index( sint32_c item_index )
	{
		if ( _sort_is_dirty )
		{
			_update_sort();
		}
		return _item_list[ item_index ];
	}

	void_c menu_control_collection_c::add_item( menu_control_collection_item_c * item )
	{
		assert( item != nullptr );
		assert( item->_mother_collection == nullptr );
		assert( _column_list.get_length() > 0 );
		item->_mother_collection = this;
		item->_index = _item_list.get_length();
		if ( _value_cache_is_dirty == false ) // if value cache is already up to date, then we can cache this new item's values. otherwise, we won't bother, entire cache will need to be updated later anyway.
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
		_item_layout_is_dirty = true;
		_sort_is_dirty = true;
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
