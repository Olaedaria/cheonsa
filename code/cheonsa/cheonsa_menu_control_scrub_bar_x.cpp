#include "cheonsa_menu_control_scrub_bar_x.h"

namespace cheonsa
{

	menu_control_scrub_bar_x_c::menu_control_scrub_bar_x_c( string8_c const & name )
		: menu_control_scroll_bar_i( name, mode_e_scrub_bar, orientation_e_x )
	{
	}

	menu_control_scrub_bar_x_c::~menu_control_scrub_bar_x_c()
	{
	}

	menu_control_scrub_bar_x_c * menu_control_scrub_bar_x_c::make_new_instance( string8_c const & name )
	{
		return new menu_control_scrub_bar_x_c( name );
	}

}
