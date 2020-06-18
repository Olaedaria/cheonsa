#include "cheonsa_menu_control_window_text_editor.h"

namespace cheonsa
{

	vector32x2_c menu_control_window_text_editor_c::default_size = vector32x2_c( 800, 600 );

	void_c menu_control_window_text_editor_c::_handle_button_on_clicked( menu_event_information_c event_information )
	{
		if ( event_information.control == _okay_button )
		{
			_dialog_result = menu_dialog_result_e_okay;
			on_dialog_result.invoke( this );
		}
		else if ( event_information.control == _cancel_button )
		{
			_dialog_result = menu_dialog_result_e_cancel;
			on_dialog_result.invoke( this );
		}
	}

	void_c menu_control_window_text_editor_c::_on_input( input_event_c * input_event )
	{
		menu_control_window_c::_on_input( input_event );
		if ( !input_event->get_was_handled() )
		{
			if ( input_event->get_type() == input_event_c::type_e_keyboard_key_pressed )
			{
				if ( input_event->get_keyboard_key() == input_keyboard_key_e_escape )
				{
					_dialog_result = menu_dialog_result_e_cancel;
					on_dialog_result.invoke( this );
				}
			}
		}
	}

	menu_control_window_text_editor_c::menu_control_window_text_editor_c()
		: menu_control_window_c()
		, _text( nullptr )
		, _cancel_button( nullptr )
		, _okay_button( nullptr )
	{
		set_layout_simple( box32x2_c( 0, 0, default_size.a, default_size.b ) );

		_text = new menu_control_text_c();
		_text->set_name( string8_c( core_list_mode_e_static, "text" ) );
		_text->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0, 0, 0, 46 ) );
		_text->set_text_interact_mode( menu_text_interact_mode_e_editable );
		add_daughter_control_to_client( _text );

		_cancel_button = new menu_control_button_c();
		_cancel_button->set_name( string8_c( core_list_mode_e_static, "cancel_button" ) );
		_cancel_button->set_plain_text_value( string16_c( core_list_mode_e_static, L"cancel" ) );
		_cancel_button->set_layout_box_anchor( menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 100, 30, 116, 8 ) );
		_cancel_button->on_clicked.subscribe( this, &menu_control_window_text_editor_c::_handle_button_on_clicked );
		add_daughter_control_to_client( _cancel_button );
		
		_okay_button = new menu_control_button_c();
		_okay_button->set_name( string8_c( core_list_mode_e_static, "okay_button" ) );
		_okay_button->set_plain_text_value( string16_c( core_list_mode_e_static, L"okay" ) );
		_okay_button->set_layout_box_anchor( menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 100, 30, 8, 8 ) );
		_okay_button->on_clicked.subscribe( this, &menu_control_window_text_editor_c::_handle_button_on_clicked );
		add_daughter_control_to_client( _okay_button );
	}

	menu_control_window_text_editor_c::~menu_control_window_text_editor_c()
	{
	}

	menu_control_window_text_editor_c::mode_e menu_control_window_text_editor_c::get_mode() const
	{
		return _text->get_multi_line() ? mode_e_multi_line : mode_e_single_line;
	}

	void_c menu_control_window_text_editor_c::set_mode( mode_e value )
	{
		_text->set_multi_line( value == mode_e_multi_line );
	}

	sint32_c menu_control_window_text_editor_c::get_character_limit() const
	{
		return _text->get_character_limit();
	}

	void_c menu_control_window_text_editor_c::set_character_limit( sint32_c value )
	{
		_text->set_character_limit( value );
	}

	string16_c menu_control_window_text_editor_c::get_plain_text_value()
	{
		return _text->get_plain_text_value();
	}

	void_c menu_control_window_text_editor_c::set_plain_text_value( string16_c const & value )
	{
		_text->set_plain_text_value( value );
	}

	menu_control_text_c * menu_control_window_text_editor_c::get_text_control()
	{
		return _text;
	}

	menu_control_button_c * menu_control_window_text_editor_c::get_cancel_button()
	{
		return _cancel_button;
	}

	menu_control_button_c * menu_control_window_text_editor_c::get_okay_button()
	{
		return _okay_button;
	}

}
