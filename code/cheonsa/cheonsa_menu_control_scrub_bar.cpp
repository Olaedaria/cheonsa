#include "cheonsa_menu_control_scrub_bar.h"

namespace cheonsa
{

	menu_control_scrub_bar_horizontal_c::menu_control_scrub_bar_horizontal_c()
		: menu_control_scroll_i( mode_e_scrub_bar, orientation_e_horizontal )
	{
	}

	menu_control_scrub_bar_vertical_c::menu_control_scrub_bar_vertical_c()
		: menu_control_scroll_i( mode_e_scrub_bar, orientation_e_vertical )
	{
	}

}
