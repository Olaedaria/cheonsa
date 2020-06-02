#include "cheonsa_menu_control_list.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	menu_control_list_item_separator_c::menu_control_list_item_separator_c( string8_c const & name )
		: menu_control_list_item_separator_i( name )
	{
	}

	menu_control_list_item_separator_c::~menu_control_list_item_separator_c()
	{
	}

	menu_control_list_item_separator_c * menu_control_list_item_separator_c::make_new_instance( string8_c const & name )
	{
		return new menu_control_list_item_separator_c( name );
	}

	menu_control_list_item_text_c::menu_control_list_item_text_c( string8_c const & name )
		: menu_control_list_item_text_i( name )
	{
		_name = string8_c( core_list_mode_e_static, "list_item" );
	}

	menu_control_list_item_text_c::~menu_control_list_item_text_c()
	{
	}

	menu_control_list_item_text_c * menu_control_list_item_text_c::make_new_instance( string8_c const & name )
	{
		return new menu_control_list_item_text_c( name );
	}

	void_c menu_control_list_item_text_c::set_is_selected( boolean_c is_selected, boolean_c try_to_multi_select )
	{
		_set_is_selected( is_selected, try_to_multi_select );
	}

	void_c menu_control_list_c::_on_selected_item_list_changed()
	{
		on_selected_item_list_changed.invoke( this );
	}

	menu_control_list_c::menu_control_list_c( string8_c const & name )
		: menu_control_list_i( name )
	{
		_name = string8_c( core_list_mode_e_static, "list" );
		set_style_map_key( string8_c( core_list_mode_e_static, "e_list" ) );
	}

	menu_control_list_c::~menu_control_list_c()
	{
	}

	menu_control_list_c * menu_control_list_c::make_new_instance( string8_c const & name )
	{
		return new menu_control_list_c( name );
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
		_set_selected_item( item );
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

}

