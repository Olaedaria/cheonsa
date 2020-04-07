#include "cheonsa_menu_control_list.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	menu_control_list_item_separator_c::menu_control_list_item_separator_c()
		: menu_control_list_item_separator_i()
	{
	}

	menu_control_list_item_text_c::menu_control_list_item_text_c()
		: menu_control_list_item_text_i()
	{
		_name = string8_c( mode_e_static, "list_item" );
	}

	void_c menu_control_list_c::_on_selected_item_list_changed()
	{
		on_selected_item_list_changed.invoke( this );
	}

	menu_control_list_c::menu_control_list_c()
		: menu_control_list_i()
	{
		_name = string8_c( mode_e_static, "list" );
		set_style_map_key( string8_c( mode_e_static, "e_list" ) );
	}

	sint32_c menu_control_list_c::get_item_count() const
	{
		return _get_item_count();
	}

	menu_control_list_item_i const * menu_control_list_c::get_item( sint32_c item_index ) const
	{
		return _get_item( item_index );
	}

	menu_control_list_item_i * menu_control_list_c::get_item( sint32_c item_index )
	{
		return _get_item( item_index );
	}

	sint32_c menu_control_list_c::give_item( menu_control_list_item_i * item, sint32_c index )
	{
		assert( dynamic_cast< menu_control_list_item_separator_c * >( item ) != nullptr || dynamic_cast< menu_control_list_item_text_c * >( item ) != nullptr );
		return _give_item( item, index );
	}

	menu_control_list_item_i * menu_control_list_c::take_item( sint32_c item_index )
	{
		return _take_item( item_index );
	}

	void_c menu_control_list_c::remove_and_delete_all_items()
	{
		_remove_and_delete_all_items();
	}

	sint32_c menu_control_list_c::get_selected_item_index() const
	{
		return _get_selected_item_index();
	}

	void_c menu_control_list_c::set_selected_item_index( sint32_c item_index )
	{
		_set_selected_item_index( item_index );
	}

	menu_control_list_item_i * menu_control_list_c::get_selected_item() const
	{
		return _get_selected_item();
	}

	void_c menu_control_list_c::set_selected_item( menu_control_list_item_i * item )
	{
		_set_selected_item( item );
	}

	core_list_c< menu_control_list_item_i * > const & menu_control_list_c::get_selected_item_list() const
	{
		return _selected_item_list;
	}

}

