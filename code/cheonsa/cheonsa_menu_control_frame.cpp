#include "cheonsa_menu_control_frame.h"

namespace cheonsa
{

	menu_control_frame_c::menu_control_frame_c()
		: menu_control_c()
		, _frame_element( string8_c( "frame", core_list_mode_e_static ) )
	{
		_frame_element.set_shared_color_class( menu_shared_color_class_e_window );
		_add_daughter_element( &_frame_element );
	}

	menu_control_frame_c::~menu_control_frame_c()
	{
	}

}
