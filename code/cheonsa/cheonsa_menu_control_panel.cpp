#include "cheonsa_menu_control_panel.h"

namespace cheonsa
{

	menu_control_panel_c::menu_control_panel_c( string8_c const & name )
		: menu_control_panel_i( name )
	{
	}

	menu_control_panel_c::~menu_control_panel_c()
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

	void_c menu_control_panel_c::update_scroll_bar_visibility()
	{
		_update_scroll_bar_visibility();
	}

	core_linked_list_c< menu_control_c * > const & menu_control_panel_c::get_client_daughter_control_list() const
	{
		return _client->get_daughter_control_list();
	}

	void_c menu_control_panel_c::add_daughter_control_to_client( menu_control_c * control, sint32_c index )
	{
		_client->add_daughter_control( control, index );
	}

	void_c menu_control_panel_c::remove_daughter_control_from_client( menu_control_c * control )
	{
		_client->remove_daughter_control( control );
	}

	void_c menu_control_panel_c::remove_all_daughter_controls_from_client()
	{
		_client->remove_all_daughter_controls();
	}

}
