#include "cheonsa_menu_control_check.h"
#include "cheonsa_user_interface.h"
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

		_element_box.set_name( string8_c( core_list_mode_e_static, "box_frame" ) );
		_element_box.set_shared_color_class( menu_shared_color_class_e_field );
		_element_box.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 16.0f ) );
		_add_element( &_element_box );

		_element_mark.set_name( string8_c( core_list_mode_e_static, "mark_frame" ) );
		_element_mark.set_shared_color_class( menu_shared_color_class_e_field );
		_element_mark.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 16.0f ) );
		_element_mark.set_is_showed( _is_checked );
		_add_element( &_element_mark );

		_element_text.set_name( string8_c( core_list_mode_e_static, "text" ) );
		_element_text.set_shared_color_class( menu_shared_color_class_e_field );
		_element_text.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 18.0f, 0.0f, 0.0f, 0.0f ) );
		_add_element( &_element_text );

		set_style_map_key( string8_c( core_list_mode_e_static, "e_check" ) );
	}

	string16_c menu_control_check_c::get_plain_text_value() const
	{
		return _element_text.get_plain_text_value();
	}

	void_c menu_control_check_c::set_plain_text_value( string8_c const & plain_text )
	{
		_element_text.set_plain_text_value( plain_text );
	}

	void_c menu_control_check_c::set_plain_text_value( string16_c const & plain_text )
	{
		_element_text.set_plain_text_value( plain_text );
	}

	void_c menu_control_check_c::set_rich_text_value( string8_c const & plain_text_with_mark_up )
	{
		_element_text.set_rich_text_value( plain_text_with_mark_up );
	}

	void_c menu_control_check_c::set_rich_text_value( string16_c const & plain_text_with_mark_up )
	{
		_element_text.set_rich_text_value( plain_text_with_mark_up );
	}

	void_c menu_control_check_c::clear_text_value()
	{
		_element_text.clear_text_value();
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
				_element_mark.set_is_showed( true );
				on_checked_changed.invoke( this );
			}
			else
			{
				_is_checked = false;
				_element_mark.set_is_showed( false );
				on_checked_changed.invoke( this );
			}
		}
	}

	void_c menu_control_check_c::update_animations( float32_c time_step )
	{
		boolean_c is_descendant_mouse_focused = is_ascendant_of( get_user_interface_root()->get_mouse_focused() );
		_element_box.set_is_selected( _is_mouse_focused || is_descendant_mouse_focused );
		_element_box.set_is_pressed( _is_pressed );
		_element_mark.set_is_selected( _is_mouse_focused || is_descendant_mouse_focused );
		_element_mark.set_is_pressed( _is_pressed );
		_element_text.set_is_selected( _is_mouse_focused || is_descendant_mouse_focused );
		_element_text.set_is_pressed( _is_pressed );
		menu_control_c::update_animations( time_step );
	}

	void_c menu_control_check_c::load_static_data_properties( data_scribe_markup_c::node_c const * node )
	{
		menu_control_c::load_static_data_properties( node );

		_mode = mode_e_normal;
		data_scribe_markup_c::attribute_c const * attribute = node->find_attribute( "mode" );
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

}
