#include "cheonsa_menu_control_frame.h"

namespace cheonsa
{

	menu_control_frame_c::menu_control_frame_c( string8_c const & name )
		: menu_control_c( name )
		, _element_frame( string8_c( core_list_mode_e_static, "frame" ) )
	{
		_element_frame.set_shared_color_class( menu_shared_color_class_e_window );
		_add_daughter_element( &_element_frame );
	}

	menu_control_frame_c::~menu_control_frame_c()
	{
	}

	menu_control_frame_c * menu_control_frame_c::make_new_instance( string8_c const & name )
	{
		return new menu_control_frame_c( name );
	}

}
