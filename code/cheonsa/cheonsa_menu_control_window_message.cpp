#include "cheonsa_menu_control_window_message.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	vector32x2_c menu_control_window_message_c::default_size = vector32x2_c( 600, 300 );

	void_c menu_control_window_message_c::_handle_button_on_clicked( menu_event_information_c event_information )
	{
		if ( event_information.control == _a_button )
		{
			if ( _mode == mode_e_okay || _mode == mode_e_cancel_okay )
			{
				_dialog_result = menu_dialog_result_e_okay;
			}
			else if ( _mode == mode_e_no_yes || _mode == mode_e_cancel_no_yes )
			{
				_dialog_result = menu_dialog_result_e_yes;
			}
		}
		else if ( event_information.control == _b_button )
		{
			if ( _mode == mode_e_cancel_okay )
			{
				_dialog_result = menu_dialog_result_e_cancel;
			}
			else if ( _mode == mode_e_no_yes || _mode == mode_e_cancel_no_yes )
			{
				_dialog_result = menu_dialog_result_e_no;
			}
		}
		else if ( event_information.control == _c_button )
		{
			if ( _mode == mode_e_cancel_no_yes )
			{
				_dialog_result = menu_dialog_result_e_cancel;
			}
		}
		on_dialog_result.invoke( this );
	}

	void_c menu_control_window_message_c::_on_input( input_event_c * input_event )
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

	menu_control_window_message_c::menu_control_window_message_c()
		: menu_control_window_c()
		, _message_label( nullptr )
		, _a_button( nullptr )
		, _b_button( nullptr )
		, _c_button( nullptr )
		, _mode( mode_e_okay )
	{
		set_size( vector32x2_c( default_size.a, default_size.b ) );

		_bottom_bar_size = 30.0f + 8.0f;
		_apply_client_margins();

		_message_label = new menu_control_label_c();
		_message_label->set_name( string8_c( core_list_mode_e_static, "message_label" ) );
		_message_label->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_message_label->set_text_align_horizontal( menu_text_align_horizontal_e_center );
		add_daughter_control_to_client( _message_label );

		_a_button = new menu_control_button_c();
		_a_button->set_name( string8_c( core_list_mode_e_static, "button_a" ) );
		_a_button->set_layout_box_anchor( menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 100, 30, 8, 8 ) );
		_a_button->on_clicked.subscribe( this, &menu_control_window_message_c::_handle_button_on_clicked );
		add_daughter_control( _a_button );

		_b_button = new menu_control_button_c();
		_b_button->set_name( string8_c( core_list_mode_e_static, "button_b" ) );
		_b_button->set_layout_box_anchor( menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 100, 30, 116, 8 ) );
		_b_button->on_clicked.subscribe( this, &menu_control_window_message_c::_handle_button_on_clicked );
		add_daughter_control( _b_button );

		_c_button = new menu_control_button_c();
		_c_button->set_name( string8_c( core_list_mode_e_static, "button_c" ) );
		_c_button->set_layout_box_anchor( menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 100, 30, 224, 8 ) );
		_c_button->on_clicked.subscribe( this, &menu_control_window_message_c::_handle_button_on_clicked );
		add_daughter_control( _c_button );
	}

	menu_control_window_message_c::~menu_control_window_message_c()
	{
	}

	menu_control_window_message_c::mode_e menu_control_window_message_c::get_mode() const
	{
		return _mode;
	}

	void_c menu_control_window_message_c::set_mode( mode_e value )
	{
		_dialog_result = menu_dialog_result_e_none;
		_mode = value;
		if ( _mode == mode_e_okay )
		{
			_c_button->set_is_showed_immediately( false );
			_b_button->set_is_showed_immediately( false );
			_a_button->set_is_showed_immediately( true );
			_a_button->set_plain_text_value( string16_c( core_list_mode_e_static, L"okay" ) );
		}
		else if ( _mode == mode_e_cancel_okay )
		{
			_c_button->set_is_showed_immediately( false );
			_b_button->set_is_showed_immediately( true );
			_b_button->set_plain_text_value( string16_c( core_list_mode_e_static, L"cancel" ) );
			_a_button->set_is_showed_immediately( true );
			_a_button->set_plain_text_value( string16_c( core_list_mode_e_static, L"okay" ) );
		}
		else if ( _mode == mode_e_no_yes )
		{
			_c_button->set_is_showed_immediately( false );
			_b_button->set_is_showed_immediately( true );
			_b_button->set_plain_text_value( string16_c( core_list_mode_e_static, L"no" ) );
			_a_button->set_is_showed_immediately( true );
			_a_button->set_plain_text_value( string16_c( core_list_mode_e_static, L"yes" ) );
		}
		else if ( _mode == mode_e_cancel_no_yes )
		{
			_c_button->set_is_showed_immediately( true );
			_c_button->set_plain_text_value( string16_c( core_list_mode_e_static, L"cancel" ) );
			_b_button->set_is_showed_immediately( true );
			_b_button->set_plain_text_value( string16_c( core_list_mode_e_static, L"no" ) );
			_c_button->set_is_showed_immediately( true );
			_c_button->set_plain_text_value( string16_c( core_list_mode_e_static, L"yes") );
		}
	}

	string16_c menu_control_window_message_c::get_message_text_value() const
	{
		return _message_label->get_plain_text_value();
	}

	void_c menu_control_window_message_c::set_message_text_value( string8_c const & plain_text )
	{
		_message_label->set_plain_text_value( plain_text );
	}

	void_c menu_control_window_message_c::set_message_text_value( string16_c const & plain_text )
	{
		_message_label->set_plain_text_value( plain_text );
	}

	void_c menu_control_window_message_c::clear_message_text_value()
	{
		_message_label->clear_text_value();
	}

}
