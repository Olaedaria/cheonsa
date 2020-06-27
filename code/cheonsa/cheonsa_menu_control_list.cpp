#include "cheonsa_menu_control_list.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	menu_control_list_item_separator_c::menu_control_list_item_separator_c()
		: menu_control_list_item_separator_i()
	{
	}

	menu_control_list_item_separator_c::~menu_control_list_item_separator_c()
	{
	}

	menu_control_list_item_text_c::menu_control_list_item_text_c()
		: menu_control_list_item_text_i()
	{
		_name = string8_c( "list_item", core_list_mode_e_static );
	}

	menu_control_list_item_text_c::~menu_control_list_item_text_c()
	{
	}

	void_c menu_control_list_item_text_c::set_is_selected( boolean_c is_selected, boolean_c try_to_multi_select )
	{
		_set_is_selected( is_selected, try_to_multi_select );
	}

	void_c menu_control_list_c::_on_selected_item_list_changed()
	{
		on_selected_item_list_changed.invoke( this );
	}

	menu_control_list_c::menu_control_list_c()
		: menu_control_list_i()
	{
		_name = string8_c( "list", core_list_mode_e_static );
		set_style_map_key( string8_c( "e_list", core_list_mode_e_static ) );
	}

	menu_control_list_c::~menu_control_list_c()
	{
	}

	sint32_c menu_control_list_c::get_selected_item_limit() const
	{
		return _selected_item_limit;
	}

	void_c menu_control_list_c::set_selected_item_limit( sint32_c value )
	{
		_set_selected_item_limit( value );
	}

	sint32_c menu_control_list_c::get_item_count() const
	{
		return _client->get_daughter_control_list().get_length();
	}

	menu_control_list_item_i const * menu_control_list_c::get_item_at_index( sint32_c item_index ) const
	{
		return static_cast< menu_control_list_item_i const * >( _client->get_daughter_control_list().get_at_index( item_index )->get_value() );
	}

	menu_control_list_item_i * menu_control_list_c::get_item_at_index( sint32_c item_index )
	{
		return static_cast< menu_control_list_item_i * >( _client->get_daughter_control_list().get_at_index( item_index )->get_value() );
	}

	void_c menu_control_list_c::add_item( menu_control_list_item_i * item, sint32_c index )
	{
		assert( dynamic_cast< menu_control_list_item_separator_c * >( item ) || dynamic_cast< menu_control_list_item_text_c * >( item ) );
		_add_item( item, index );
	}

	void_c menu_control_list_c::remove_item( menu_control_list_item_i * item )
	{
		_remove_item( item );
	}

	void_c menu_control_list_c::remove_all_items()
	{
		_remove_all_items();
	}

	menu_control_list_item_i * menu_control_list_c::get_selected_item() const
	{
		return _get_selected_item();
	}

	void_c menu_control_list_c::set_selected_item( menu_control_list_item_i * item )
	{
		menu_control_list_item_text_c * list_item = dynamic_cast< menu_control_list_item_text_c * >( item );
		if ( list_item )
		{
			list_item->set_is_selected( true, false );
		}
	}

	sint32_c menu_control_list_c::get_selected_item_index() const
	{
		return _get_selected_item_index();
	}

	void_c menu_control_list_c::set_selected_item_index( sint32_c item_index )
	{
		_set_selected_item_index( item_index );
	}

	core_list_c< menu_control_list_item_i * > const & menu_control_list_c::get_selected_item_list() const
	{
		return _selected_item_list;
	}

	sint32_c list_item_compare_function( menu_control_list_item_text_c * const & a, menu_control_list_item_text_c * const & b )
	{
		return ops::string16_sort_compare( a->get_internal_plain_text_value(), b->get_internal_plain_text_value() );
	}

	void_c menu_control_list_c::sort_items_by_plain_text_value( boolean_c invert )
	{
		core_list_c< menu_control_list_item_text_c * > list_item_list;
		core_linked_list_c< menu_control_c * >::node_c * daughter_node = _client->get_daughter_control_list().get_first();
		while ( daughter_node )
		{
			list_item_list.insert( -1, static_cast< menu_control_list_item_text_c * >( daughter_node->get_value() ) );
			daughter_node = daughter_node->get_next();
		}
		list_item_list.quick_sort_2( &list_item_compare_function, false );

		_client->_get_daughter_control_list().remove_all();
		for ( sint32_c i = 0; i < list_item_list.get_length(); i++ )
		{
			_client->_get_daughter_control_list().insert_at_end( &list_item_list[ i ]->_get_daughter_control_list_node() );
		}
		_item_origins_are_dirty = true;
		_layout_item_origins();
	}

}

