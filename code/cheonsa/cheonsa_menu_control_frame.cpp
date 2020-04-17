#include "cheonsa_menu_control_frame.h"

namespace cheonsa
{

	menu_control_frame_c::menu_control_frame_c( string8_c const & name )
		: menu_control_c( name )
		, _element_frame( string8_c( core_list_mode_e_static, "frame" ) )
	{
		_element_frame.set_shared_color_class( menu_shared_color_class_e_window );
		_element_frame.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_add_element( &_element_frame );
	}

}
