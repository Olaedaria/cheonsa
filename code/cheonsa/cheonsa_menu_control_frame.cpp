#include "cheonsa_menu_control_frame.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	menu_control_frame_c::menu_control_frame_c()
		: menu_control_c()
		, _frame_element()
	{
		_frame_element.set_name( string8_c( "frame", core_list_mode_e_static ) );
		_frame_element.set_color_theme( engine.get_menu_style_manager()->get_internal_window_color_theme() );
		_add_daughter_element( &_frame_element );
	}

	menu_control_frame_c::~menu_control_frame_c()
	{
	}

}
