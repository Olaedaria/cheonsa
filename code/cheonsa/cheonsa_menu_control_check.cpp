#include "cheonsa_menu_control_check.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	void_c menu_control_check_c::_on_clicked( input_event_c * input_event )
	{
		set_is_checked( !_is_checked );
	}

	menu_control_check_c::menu_control_check_c()
		: menu_control_c()
		, _mode( mode_e_normal )
		, _is_checked( false )
		, _element_box()
		, _element_mark()
		, _element_text()
	{
		_select_mode = menu_select_mode_e_mouse_and_directional;

		_element_box.set_name( string8_c( mode_e_static, "box_frame" ) );
		_element_box.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 16.0f ) );
		_add_element( &_element_box );

		_element_mark.set_name( string8_c( mode_e_static, "mark_frame" ) );
		_element_mark.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 16.0f ) );
		_element_mark.set_is_showing( _is_checked );
		_add_element( &_element_mark );

		_element_text.set_name( string8_c( mode_e_static, "text" ) );
		_element_text.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 18.0f, 0.0f, 0.0f, 0.0f ) );
		_add_element( &_element_text );

		set_style_map_key( string8_c( mode_e_static, "e_check" ) );
	}

	void_c menu_control_check_c::update_animations( float32_c time_step )
	{
		boolean_c is_descendant_mouse_focused = _get_is_descendant_mouse_focused();
		_element_box.set_is_selected( _is_mouse_focused || is_descendant_mouse_focused );
		_element_box.set_is_pressed( _is_pressed );
		_element_mark.set_is_selected( _is_mouse_focused || is_descendant_mouse_focused );
		_element_mark.set_is_pressed( _is_pressed );
		_element_text.set_is_selected( _is_mouse_focused || is_descendant_mouse_focused );
		_element_text.set_is_pressed( _is_pressed );
		menu_control_c::update_animations( time_step );
	}

	void_c menu_control_check_c::load_properties( data_scribe_markup_c::node_c const * node )
	{
		menu_control_c::load_properties( node );

		data_scribe_markup_c::attribute_c const * attribute = nullptr;
		
		_mode = mode_e_normal;
		attribute = node->find_attribute( "mode" );
		if ( attribute )
		{
			if ( attribute->get_value() == "normal" )
			{
				_mode = mode_e_normal;
			}
			else if ( attribute->get_value() == "single" )
			{
				_mode = mode_e_single;
			}
		}

		_is_checked = false;
		attribute = node->find_attribute( "checked" );
		if ( attribute )
		{
			ops::convert_string8_to_boolean( attribute->get_value(), _is_checked );
		}
	}

	string_c::reference_c & menu_control_check_c::get_string_reference()
	{
		return _element_text.get_string_reference();
	}

	string16_c menu_control_check_c::get_plain_text() const
	{
		return _element_text.get_plain_text();
	}

	void_c menu_control_check_c::set_plain_text( string16_c const & plain_text )
	{
		_element_text.set_text_format_mode( menu_text_format_mode_e_plain );
		_element_text.set_plain_text( plain_text );
	}

	void_c menu_control_check_c::set_rich_text( string8_c const & plain_text_with_markup )
	{
		_element_text.set_text_format_mode( menu_text_format_mode_e_rich );
		_element_text.set_rich_text( plain_text_with_markup );
	}

	menu_control_check_c::mode_e menu_control_check_c::get_mode() const
	{
		return _mode;
	}

	void_c menu_control_check_c::set_mode( mode_e value )
	{
		_mode = value;
	}

	boolean_c menu_control_check_c::get_is_checked() const
	{
		return _is_checked;
	}

	void_c menu_control_check_c::set_is_checked( boolean_c value )
	{
		if ( value != _is_checked )
		{
			if ( value )
			{
				if ( _mode == mode_e_single && _mother_control != nullptr )
				{
					for ( sint32_c i = 0; i < _mother_control->get_control_count(); i++ )
					{
						menu_control_check_c * other_check = dynamic_cast< menu_control_check_c * >( _mother_control->get_control( i ) );
						if ( other_check != nullptr && other_check != this && other_check->_mode == mode_e_single )
						{
							other_check->set_is_checked( false );
						}
					}
				}
				_is_checked = true;
				_element_mark.set_is_showing( true );
				on_checked_changed.invoke( this );
			}
			else
			{
				_is_checked = false;
				_element_mark.set_is_showing( false );
				on_checked_changed.invoke( this );
			}
		}
	}

}
