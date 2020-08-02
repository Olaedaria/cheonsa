#include "cheonsa_menu_control_debug_statistics.h"
#include "cheonsa_engine.h"
#include "cheonsa__ops_string.h"

namespace cheonsa
{

	menu_control_debug_statistics_c::menu_control_debug_statistics_c()
		: menu_control_c()
		, _text_element()
	{
		set_layout_simple( box32x2_c( 0.0f, 0.0f, 500.0f, 100.0f ) );

		_text_element.set_name( string8_c( "text", core_list_mode_e_static ) );
		_add_daughter_element( &_text_element );

		_select_mode = menu_select_mode_e_none;
	}

	void_c menu_control_debug_statistics_c::update_animations( float32_c time_step )
	{
		string16_c plain_text_value;

		string8_c value_string;

		ops::convert_uint32_to_string8( engine.get_user_interface()->get_canvas()->get_statistic_object_count(), value_string );
		plain_text_value += "object count:\n  ";
		plain_text_value += value_string;
		plain_text_value += "\n";

		ops::convert_uint32_to_string8( engine.get_user_interface()->get_canvas()->get_statistic_triangle_count(), value_string );
		plain_text_value += "triangle_count:\n  ";
		plain_text_value += value_string;
		plain_text_value += "\n";

		_text_element.set_plain_text_value( plain_text_value );

		menu_control_c::update_animations( time_step );
	}

}
