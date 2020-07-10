#include "cheonsa_menu_control_debug_console.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	void_c menu_control_debug_console_c::_handle_message_logged( debug_log_message_c const & log_message )
	{
		_output_text->append_plain_text_as_new_paragraph( log_message.get_message() );
	}

	menu_control_debug_console_c::menu_control_debug_console_c()
		: menu_control_window_c()
		, _output_text( nullptr )
		, _input_text( nullptr )
		, _close_button( nullptr )
	{
		set_size( vector32x2_c( 800.0f, 600.0f ) );
		set_title_plain_text_value( string8_c( "console", core_list_mode_e_static ) );

		_output_text = new menu_control_text_c();
		_output_text->set_name( string8_c( "output_text", core_list_mode_e_static ) );
		_output_text->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 38.0f ) );
		_output_text->set_text_interact_mode( menu_text_interact_mode_e_static_selectable );
		_output_text->set_text_align_y( menu_text_align_y_e_bottom );
		_output_text->set_vertical_scroll_visibility_mode( menu_visibility_mode_e_always );
		add_daughter_control_to_client( _output_text );

		_input_text = new menu_control_text_c();
		_input_text->set_name( string8_c( "input_text", core_list_mode_e_static ) );
		_input_text->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 30.0f, 0.0f, 0.0f ) );
		_input_text->set_text_interact_mode( menu_text_interact_mode_e_editable );
		add_daughter_control_to_client( _input_text );

		set_close_button_is_enabled( true );

		engine.get_debug_manager()->on_message_logged.subscribe( this, &menu_control_debug_console_c::_handle_message_logged );
	}

	menu_control_debug_console_c::~menu_control_debug_console_c()
	{
		engine.get_debug_manager()->on_message_logged.unsubscribe( this, &menu_control_debug_console_c::_handle_message_logged );
	}

}
