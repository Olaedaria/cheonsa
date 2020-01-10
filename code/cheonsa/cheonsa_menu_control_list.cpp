#include "cheonsa_menu_control_list.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	menu_control_list_item_c::menu_control_list_item_c()
		: menu_control_list_item_i()
	{
		_name = string8_c( mode_e_static, "list_item" );
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
		_list_item_holder->_give_control( item, index );
	}

	void_c menu_control_list_c::remove_item( menu_control_list_item_c * item )
	{
		_list_item_holder->_take_control( item->get_index() );
	}

	void_c menu_control_list_c::remove_item( sint32_c index )
	{
		_list_item_holder->_take_control( index );
	}

	void_c menu_control_list_c::remove_and_delete_all_items()
	{
		_list_item_holder->_remove_and_delete_all_controls();
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

