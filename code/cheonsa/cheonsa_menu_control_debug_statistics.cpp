#include "cheonsa_menu_control_debug_statistics.h"
#include "cheonsa_engine.h"
#include "cheonsa__ops_string.h"

namespace cheonsa
{

	menu_control_debug_statistics_c::menu_control_debug_statistics_c()
		: menu_control_c()
		, _text_element()
	{
		set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( 0.0f, 0.0f, 0.0f, 60.0f ) );

		_text_element.set_name( string8_c( "text", core_list_mode_e_static ) );
		_add_daughter_element( &_text_element );

		_select_mode = menu_select_mode_e_none;

		set_color_theme( engine.get_menu_style_manager()->get_internal_window_color_theme() );
	}

	void_c menu_control_debug_statistics_c::update_animations( float32_c time_step )
	{
		string16_c plain_text_value;

		string8_c value_string;

		ops::convert_uint32_to_string8( engine.get_user_interface()->get_canvas()->get_statistic_object_count(), value_string );
		plain_text_value += "objects: ";
		plain_text_value += value_string;
		plain_text_value += "\n";

		ops::convert_uint32_to_string8( engine.get_user_interface()->get_canvas()->get_statistic_triangle_count(), value_string );
		plain_text_value += "triangles: ";
		plain_text_value += value_string;
		plain_text_value += "\n";

		_text_element.set_plain_text_value( plain_text_value );

		menu_control_c::update_animations( time_step );
	}

}
