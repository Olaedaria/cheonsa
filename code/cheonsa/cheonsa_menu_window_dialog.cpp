#include "cheonsa_menu_window_dialog.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	void_c menu_window_dialog_c::_handle_button_on_click( menu_event_information_c event_information )
	{
		if ( event_information.control == _control_button_a )
		{
			if ( _mode == mode_e_okay || _mode == mode_e_okay_cancel )
			{
				_result = result_e_okay;
			}
			else if ( _mode == mode_e_yes_no || _mode == mode_e_yes_no_cancel )
			{
				_result = result_e_yes;
			}
		}
		else if ( event_information.control == _control_button_b )
		{
			if ( _mode == mode_e_okay_cancel )
			{
				_result = result_e_cancel;
			}
			else if ( _mode == mode_e_yes_no || _mode == mode_e_yes_no_cancel )
			{
				_result = result_e_no;
			}
		}
		else if ( event_information.control == _control_button_c )
		{
			if ( _mode == mode_e_yes_no_cancel )
			{
				_result = result_e_cancel;
			}
		}
		set_is_showed( false );
		on_result.invoke( this );
	}

	void_c menu_window_dialog_c::_on_input( input_event_c * input_event )
	{
		menu_window_c::_on_input( input_event );
		if ( !input_event->processed )
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
				else if ( _mode == mode_e_yes_no )
				{
					_result = result_e_no;
				}
				set_is_showed( false );
				on_result.invoke( this );
			}
		}
	}

	menu_window_dialog_c::menu_window_dialog_c()
		: menu_window_c()
		, _control_button_a( nullptr )
		, _control_button_b( nullptr )
		, _control_button_c( nullptr )
		, _mode( mode_e_okay )
		, _result( result_e_okay )
	{
		_bottom_bar_size = 30.0f + 8.0f;
		_apply_client_margins();

		_control_button_a = new menu_control_button_c();
		_control_button_a->set_name( string8_c( mode_e_static, "button_a" ) );
		_control_button_a->on_clicked.subscribe( this, &menu_window_dialog_c::_handle_button_on_click );
		_give_control( _control_button_a );

		_control_button_b = new menu_control_button_c();
		_control_button_b->set_name( string8_c( mode_e_static, "button_b" ) );
		_control_button_b->on_clicked.subscribe( this, &menu_window_dialog_c::_handle_button_on_click );
		_give_control( _control_button_b );

		_control_button_c = new menu_control_button_c();
		_control_button_c->set_name( string8_c( mode_e_static, "button_c" ) );
		_control_button_c->on_clicked.subscribe( this, &menu_window_dialog_c::_handle_button_on_click );
		_give_control( _control_button_c );
	}

	void menu_window_dialog_c::load_static_data_properties( data_scribe_markup_c::node_c const * node )
	{
		menu_window_c::load_static_data_properties( node );

		data_scribe_markup_c::attribute_c const * attribute = nullptr;

		attribute = node->find_attribute( "mode" );
		if ( attribute )
		{
			_mode = mode_e_okay;
			if ( attribute->get_value() == "okay" )
			{
				_mode = mode_e_okay;
			}
			else if ( attribute->get_value() == "okay_cancel" )
			{
				_mode = mode_e_okay_cancel;
			}
			else if ( attribute->get_value() == "yes_no" )
			{
				_mode = mode_e_yes_no;
			}
			else if ( attribute->get_value() == "yes_no_cancel" )
			{
				_mode = mode_e_yes_no_cancel;
			}
			set_mode( _mode );
		}
	}

	menu_window_dialog_c::mode_e menu_window_dialog_c::get_mode() const
	{
		return _mode;
	}

	void_c menu_window_dialog_c::set_mode( mode_e value )
	{
		menu_anchor_e box_anchor = menu_anchor_e_right | menu_anchor_e_bottom;
		box32x2_c box_a = box32x2_c( 100.0f, 30.0f, _edge_size + 100.0f + _edge_size + 100.0f + _edge_size, _edge_size );
		box32x2_c box_b = box32x2_c( 100.0f, 30.0f, _edge_size + 100.0f + _edge_size, _edge_size );
		box32x2_c box_c = box32x2_c( 100.0f, 30.0f, _edge_size, _edge_size );

		_mode = value;
		if ( _mode == mode_e_okay )
		{
			if ( _control_button_a )
			{
				_control_button_a->set_layout_box_anchor( box_anchor, box_c );
				_control_button_a->set_plain_text_value( string16_c( mode_e_static, L"okay" ) );
				_control_button_a->set_is_showed_immediately( true );
			}
			if ( _control_button_b )
			{
				_control_button_b->set_is_showed_immediately( false );
			}
			if ( _control_button_c )
			{
				_control_button_c->set_is_showed_immediately( false );
			}
		}
		else if ( _mode == mode_e_okay_cancel || _mode == mode_e_yes_no )
		{
			if ( _control_button_a )
			{
				_control_button_a->set_layout_box_anchor( box_anchor, box_b );
				_control_button_a->set_is_showed_immediately( true );
				
			}
			if ( _control_button_b )
			{
				_control_button_b->set_layout_box_anchor( box_anchor, box_c );
				_control_button_b->set_is_showed_immediately( false );
			}
			if ( _control_button_c )
			{
				_control_button_c->set_is_showed_immediately( false );
			}
			if ( _mode == mode_e_okay_cancel )
			{
				_control_button_a->set_plain_text_value( string16_c( mode_e_static, L"okay" ) );
				_control_button_b->set_plain_text_value( string16_c( mode_e_static, L"cancel" ) );
			}
			else if ( _mode == mode_e_yes_no )
			{
				_control_button_a->set_plain_text_value( string16_c( mode_e_static, L"yes" ) );
				_control_button_b->set_plain_text_value( string16_c( mode_e_static, L"no" ) );
			}
		}
		else if ( _mode == mode_e_yes_no_cancel )
		{
			if ( _control_button_a )
			{
				_control_button_a->set_layout_box_anchor( box_anchor, box_a );
				_control_button_a->set_plain_text_value( string16_c( mode_e_static, L"yes" ) );
				_control_button_a->set_is_showed_immediately( true );
			}
			if ( _control_button_b )
			{
				_control_button_b->set_layout_box_anchor( box_anchor, box_b );
				_control_button_b->set_plain_text_value( string16_c( mode_e_static, L"no" ) );
				_control_button_b->set_is_showed_immediately( true );
			}
			if ( _control_button_c )
			{
				_control_button_c->set_layout_box_anchor( box_anchor, box_c );
				_control_button_c->set_plain_text_value( string16_c( mode_e_static, L"cancel" ) );
				_control_button_c->set_is_showed_immediately( true );
			}
		}
	}

	menu_window_dialog_c::result_e menu_window_dialog_c::get_result() const
	{
		return _result;
	}

	void_c menu_window_dialog_c::set_is_showed( boolean_c value, boolean_c and_wants_to_be_deleted )
	{
		if ( value )
		{
			_result = result_e_none;
		}
		menu_control_c::set_is_showed( value, and_wants_to_be_deleted );
	}

	void_c menu_window_dialog_c::set_is_showed_immediately( boolean_c value )
	{
		if ( value )
		{
			_result = result_e_none;
		}
		menu_control_c::set_is_showed_immediately( value );
	}

}
