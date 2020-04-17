#include "cheonsa_menu_control_panel.h"

namespace cheonsa
{

	menu_control_panel_c::menu_control_panel_c( string8_c const & name )
		: menu_control_panel_i( name )
	{
	}

	menu_visibility_mode_e menu_control_panel_c::get_horizontal_scroll_bar_visibility_mode() const
	{
		return _get_horizontal_scroll_bar_visibility_mode();
	}

	void_c menu_control_panel_c::set_horizontal_scroll_bar_visibility_mode( menu_visibility_mode_e value )
	{
		_set_horizontal_scroll_bar_visibility_mode( value );
	}

	menu_visibility_mode_e menu_control_panel_c::get_vertical_scroll_bar_visibility_mode() const
	{
		return _get_vertical_scroll_bar_visibility_mode();
	}

	void_c menu_control_panel_c::set_vertical_scroll_bar_visibility_mode( menu_visibility_mode_e value )
	{
		_set_vertical_scroll_bar_visibility_mode( value );
	}

	sint32_c menu_control_panel_c::get_controls_in_client_count() const
	{
		return _get_controls_in_client_count();
	}

	menu_control_c const * menu_control_panel_c::get_control_in_client( sint32_c control_index ) const
	{
		return _get_control_in_client( control_index );
	}
	
	menu_control_c * menu_control_panel_c::get_control_in_client( sint32_c control_index )
	{
		return _get_control_in_client( control_index );
	}

	sint32_c menu_control_panel_c::give_control_to_client( menu_control_c * control, sint32_c index )
	{
		return _give_control_to_client( control, index );
	}

	menu_control_c * menu_control_panel_c::take_control_from_client( sint32_c control_index )
	{
		return _take_control_from_client( control_index );
	}

	void_c menu_control_panel_c::remove_and_delete_all_controls_from_client()
	{
		_remove_and_delete_all_controls_from_client();
	}

}
