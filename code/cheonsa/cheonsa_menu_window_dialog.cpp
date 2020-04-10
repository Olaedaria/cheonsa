#include "cheonsa_menu_window_dialog.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	void_c menu_window_dialog_c::_handle_button_on_clicked( menu_event_information_c event_information )
	{
		if ( event_information.control == _a_button )
		{
			if ( _mode == mode_e_okay || _mode == mode_e_okay_cancel )
			{
				_result = result_e_okay;
			}
			else if ( _mode == mode_e_yes_cancel || _mode == mode_e_yes_no_cancel )
			{
				_result = result_e_yes;
			}
		}
		else if ( event_information.control == _b_button )
		{
			if ( _mode == mode_e_okay_cancel || _mode == mode_e_yes_cancel )
			{
				_result = result_e_cancel;
			}
			else if ( _mode == mode_e_yes_no_cancel )
			{
				_result = result_e_no;
			}
		}
		else if ( event_information.control == _c_button )
		{
			if ( _mode == mode_e_yes_no_cancel )
			{
				_result = result_e_cancel;
			}
		}
		if ( _modal_screen != nullptr )
		{
			_modal_screen->set_is_showed( false, true );
			_modal_screen = nullptr;
		}
		on_result.invoke( this );
	}

	void_c menu_window_dialog_c::_on_input( input_event_c * input_event )
	{
		menu_window_c::_on_input( input_event );
		if ( !input_event->was_handled )
		{
			if ( input_event->type == input_event_c::type_e_keyboard_key_pressed && input_event->keyboard_key == input_keyboard_key_e_escape )
			{
				if ( _mode == mode_e_okay )
				{
					_result = result_e_cancel;
				}
				else if ( _mode == mode_e_okay_cancel || _mode == mode_e_yes_no_cancel )
				{
					_result = result_e_cancel;
				}
				else if ( _mode == mode_e_yes_cancel )
				{
					_result = result_e_cancel;
				}
				if ( _modal_screen != nullptr )
				{
					_modal_screen->set_is_showed( false, true );
					_modal_screen = nullptr;
				}
				on_result.invoke( this );
			}
		}
	}

	menu_window_dialog_c::menu_window_dialog_c()
		: menu_window_c()
		, _message_label( nullptr )
		, _a_button( nullptr )
		, _b_button( nullptr )
		, _c_button( nullptr )
		, _mode( mode_e_okay )
	{
		_bottom_bar_size = 30.0f + 8.0f;
		_apply_client_margins();

		_message_label = new menu_control_label_c();
		_message_label->set_name( string8_c( mode_e_static, "message_label" ) );
		_message_label->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_give_control_to_client( _message_label );

		_a_button = new menu_control_button_c();
		_a_button->set_name( string8_c( mode_e_static, "button_a" ) );
		_a_button->on_clicked.subscribe( this, &menu_window_dialog_c::_handle_button_on_clicked );
		_give_control( _a_button );

		_b_button = new menu_control_button_c();
		_b_button->set_name( string8_c( mode_e_static, "button_b" ) );
		_b_button->on_clicked.subscribe( this, &menu_window_dialog_c::_handle_button_on_clicked );
		_give_control( _b_button );

		_c_button = new menu_control_button_c();
		_c_button->set_name( string8_c( mode_e_static, "button_c" ) );
		_c_button->on_clicked.subscribe( this, &menu_window_dialog_c::_handle_button_on_clicked );
		_give_control( _c_button );
	}

	menu_window_dialog_c::mode_e menu_window_dialog_c::get_mode() const
	{
		return _mode;
	}

	void_c menu_window_dialog_c::set_mode( mode_e value )
	{
		menu_anchor_e box_anchor = menu_anchor_e_right | menu_anchor_e_bottom;
		box32x2_c box_a = box32x2_c( 100.0f, 30.0f, _edge_size + 100.0f + _edge_size + 100.0f + _edge_size, _edge_size ); // right most.
		box32x2_c box_b = box32x2_c( 100.0f, 30.0f, _edge_size + 100.0f + _edge_size, _edge_size ); // middle.
		box32x2_c box_c = box32x2_c( 100.0f, 30.0f, _edge_size, _edge_size ); // left most.

		_result = result_e_none;
		_mode = value;
		if ( _mode == mode_e_okay )
		{
			_a_button->set_layout_box_anchor( box_anchor, box_c );
			_a_button->set_plain_text_value( string16_c( mode_e_static, L"okay" ) );
			_a_button->set_is_showed_immediately( true );
			_b_button->set_is_showed_immediately( false );
			_c_button->set_is_showed_immediately( false );
		}
		else if ( _mode == mode_e_okay_cancel || _mode == mode_e_yes_cancel )
		{
			_a_button->set_layout_box_anchor( box_anchor, box_b );
			_a_button->set_is_showed_immediately( true );
			_b_button->set_layout_box_anchor( box_anchor, box_c );
			_b_button->set_is_showed_immediately( true );
			_c_button->set_is_showed_immediately( false );
			if ( _mode == mode_e_okay_cancel )
			{
				_a_button->set_plain_text_value( string16_c( mode_e_static, L"okay" ) );
				_b_button->set_plain_text_value( string16_c( mode_e_static, L"cancel" ) );
			}
			else if ( _mode == mode_e_yes_cancel )
			{
				_a_button->set_plain_text_value( string16_c( mode_e_static, L"yes" ) );
				_b_button->set_plain_text_value( string16_c( mode_e_static, L"cancel" ) );
			}
		}
		else if ( _mode == mode_e_yes_no_cancel )
		{
			_a_button->set_layout_box_anchor( box_anchor, box_a );
			_a_button->set_plain_text_value( string16_c( mode_e_static, L"yes" ) );
			_a_button->set_is_showed_immediately( true );
			_b_button->set_layout_box_anchor( box_anchor, box_b );
			_b_button->set_plain_text_value( string16_c( mode_e_static, L"no" ) );
			_b_button->set_is_showed_immediately( true );
			_c_button->set_layout_box_anchor( box_anchor, box_c );
			_c_button->set_plain_text_value( string16_c( mode_e_static, L"cancel" ) );
			_c_button->set_is_showed_immediately( true );
		}
	}

	string16_c menu_window_dialog_c::get_message() const
	{
		return _message_label->get_plain_text_value();
	}

	void_c menu_window_dialog_c::set_message( string8_c const & plain_text )
	{
		_message_label->set_plain_text_value( plain_text );
	}

	void_c menu_window_dialog_c::set_message( string16_c const & plain_text )
	{
		_message_label->set_plain_text_value( plain_text );
	}

	void_c menu_window_dialog_c::clear_message()
	{
		_message_label->clear_text_value();
	}

}
