#include "cheonsa_menu_control_list.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	menu_control_list_item_c::menu_control_list_item_c()
		: menu_control_list_item_i()
	{
		_name = string8_c( mode_e_static, "list_item" );
	}

	boolean_c menu_control_list_item_c::get_is_selected() const
	{
		return _is_selected;
	}

	void_c menu_control_list_item_c::set_is_selected( boolean_c value )
	{
		if ( _is_selected != value && _mother_control != nullptr )
		{
			assert( _mother_control->get_type_name() == menu_control_list_c::get_type_name_static() );
			menu_control_list_i * list_box = dynamic_cast< menu_control_list_i * >( _mother_control );
			assert( list_box );
			assert( list_box->_selected_item_limit >= -1 );

			_is_selected = value;
			_element_frame_selected.set_is_showing( value );

			if ( value )
			{
				// append this selection.
				list_box->_selected_item_list.insert_at_end( this );

				// if over the select limit then deselect old selection(s).
				sint32_c deselect_count = list_box->_selected_item_list.get_length() - list_box->_selected_item_limit;
				if ( deselect_count > 0 )
				{
					for ( sint32_c i = 0; i < deselect_count; i++ )
					{
						menu_control_list_item_i * list_item = list_box->_selected_item_list[ i ];
						list_item->_is_selected = false;
						list_item->_element_frame_selected.set_is_showing( false );
					}
					list_box->_selected_item_list.remove_range_at_index( deselect_count, 0 );
					list_box->_on_selection_changed();
				}
			}
			else
			{
				// remove this selection.
				list_box->_selected_item_list.remove( this );
			}

			// trigger event.
			list_box->_on_selection_changed();
		}
	}

	void_c menu_control_list_c::_on_selection_changed()
	{
		on_selection_changed.invoke( this );
	}

	menu_control_list_c::menu_control_list_c()
		: menu_control_list_i()
	{
		_name = string8_c( mode_e_static, "list" );
		set_style_map_key( string8_c( mode_e_static, "e_list" ) );
	}

	void_c menu_control_list_c::load_hierarchy_and_properties( data_scribe_markup_c::node_c const * node )
	{
		remove_and_delete_all_items();

		menu_control_c::load_hierarchy_and_properties( node );

		// list items.
		data_scribe_markup_c::node_c const * sub_node = sub_node = node->find_tag( "list_items" );
		if ( sub_node )
		{
			data_scribe_markup_c::node_c const * sub_sub_node = sub_node->get_first_daughter();
			while ( sub_sub_node )
			{
				if ( sub_sub_node->get_type() == data_scribe_markup_c::node_c::type_e_tag )
				{
					if ( sub_sub_node->get_value()== menu_control_list_item_c::get_type_name_static() )
					{
						menu_control_list_item_c * list_item = new menu_control_list_item_c();
						data_scribe_markup_c::attribute_c const * name_attribute = sub_sub_node->find_attribute( "name" );
						if ( name_attribute != nullptr && name_attribute->get_value().get_length() > 0 )
						{
							list_item->set_name( name_attribute->get_value() );
						}
						list_item->load_hierarchy_and_properties( sub_sub_node );
						add_item( list_item );
					}
				}
				sub_sub_node = sub_sub_node->get_next_sister();
			}
		}
	}

	void_c menu_control_list_c::load_properties( data_scribe_markup_c::node_c const * node )
	{
		menu_control_c::load_properties( node );

		data_scribe_markup_c::attribute_c const * attribute = node->find_attribute( "selected_item_limit" );
		if ( attribute )
		{
			sint32_c value = 0;
			if ( ops::convert_string8_to_sint32( attribute->get_value(), value ) )
			{
				if ( value < -1 )
				{
					value = -1;
				}
				set_selected_item_limit( value );
			}
		}
	}

	sint32_c menu_control_list_c::get_item_count() const
	{
		return _control_list.get_length();
	}

	menu_control_list_item_c * menu_control_list_c::get_item( sint32_c index ) const
	{
		assert( index >= 0 && index < _control_list.get_length() );
		menu_control_list_item_c * result = dynamic_cast< menu_control_list_item_c * >( _control_list[ index ] );
		assert( result != nullptr );
		return result;
	}

	void_c menu_control_list_c::add_item( menu_control_list_item_c * item, sint32_c index )
	{
		_list_item_holder->_add_control( item, index );
	}

	void_c menu_control_list_c::remove_item( menu_control_list_item_c * item )
	{
		_list_item_holder->_remove_control( item->get_index() );
	}

	void_c menu_control_list_c::remove_item( sint32_c index )
	{
		_list_item_holder->_remove_control( index );
	}

	void_c menu_control_list_c::remove_and_delete_all_items()
	{
		_list_item_holder->_remove_and_delete_all_controls();
	}

	sint32_c menu_control_list_c::get_selected_item_limit() const
	{
		return _selected_item_limit;
	}

	void_c menu_control_list_c::set_selected_item_limit( sint32_c value )
	{
		_set_selected_item_limit( value );
	}

	sint32_c menu_control_list_c::get_selected_item_index() const
	{
		return _get_selected_item_index();
	}

	void_c menu_control_list_c::set_selected_item_index( sint32_c index )
	{
		_set_selected_item_index( index );
	}

	menu_control_list_item_c * menu_control_list_c::get_selected_item() const
	{
		sint32_c selected_item_index = _get_selected_item_index();
		if ( selected_item_index >= 0 )
		{
			menu_control_list_item_c * result = dynamic_cast< menu_control_list_item_c * >( _control_list[ selected_item_index ] );
			assert( result != nullptr );
			assert( result->get_index() == selected_item_index );
			return result;
		}
		return nullptr;
	}

	void_c menu_control_list_c::set_selected_item( menu_control_list_item_c * value )
	{
		set_selected_item_index( value != nullptr ? value->get_index() : -1 );
	}

	core_list_c< menu_control_list_item_c * > menu_control_list_c::get_selected_item_list() const
	{
		core_list_c< menu_control_list_item_c * > result;
		for ( sint32_c i = 0; i < _selected_item_list.get_length(); i++ )
		{
			menu_control_list_item_c * list_item = dynamic_cast< menu_control_list_item_c * >( _selected_item_list[ i ] );
			assert( list_item != nullptr );
			result.insert_at_end( list_item );
		}
		return result;
	}

}

