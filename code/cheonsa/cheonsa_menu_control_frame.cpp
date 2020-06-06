#include "cheonsa_menu_control_frame.h"

namespace cheonsa
{

	menu_control_frame_c::menu_control_frame_c( string8_c const & name )
		: menu_control_c( name )
		, _frame_element( string8_c( core_list_mode_e_static, "frame" ) )
	{
		_frame_element.set_shared_color_class( menu_shared_color_class_e_window );
		_add_daughter_element( &_frame_element );
	}

	menu_control_frame_c::~menu_control_frame_c()
	{
	}

}
