#include "cheonsa_menu_control_scrub_bar_y.h"

namespace cheonsa
{

	menu_control_scrub_bar_y_c::menu_control_scrub_bar_y_c( string8_c const & name )
		: menu_control_scroll_bar_i( name, mode_e_scrub_bar, orientation_e_y )
	{
	}

	menu_control_scrub_bar_y_c::~menu_control_scrub_bar_y_c()
	{
	}

	menu_control_scrub_bar_y_c * menu_control_scrub_bar_y_c::make_new_instance( string8_c const & name )
	{
		return new menu_control_scrub_bar_y_c( name );
	}

}
