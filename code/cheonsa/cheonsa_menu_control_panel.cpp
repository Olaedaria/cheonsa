#include "cheonsa_menu_control_panel.h"

namespace cheonsa
{

	menu_control_panel_c::menu_control_panel_c()
		: menu_control_panel_i()
	{
	}

	sint32_c menu_control_panel_c::give_control( menu_control_c * control, sint32_c index )
	{
		return _give_control( control, index );
	}

	menu_control_c * menu_control_panel_c::take_control( sint32_c control_index )
	{
		return _take_control( control_index );
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

}
