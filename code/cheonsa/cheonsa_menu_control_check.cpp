#include "cheonsa_menu_control_check.h"
#include "cheonsa_user_interface.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	void_c menu_control_check_c::_on_clicked( input_event_c * input_event )
	{
		set_is_checked( !_is_checked );
	}

	void_c menu_control_check_c::_update_daughter_element_animations( float32_c time_step )
	{
		boolean_c is_descendant_mouse_focused = is_ascendant_of( _mother_user_interface->get_mouse_focused() );
		_box_element.set_is_selected( _is_mouse_focused || is_descendant_mouse_focused );
		_box_element.set_is_pressed( _is_pressed );
		_mark_element.set_is_selected( _is_mouse_focused || is_descendant_mouse_focused );
		_mark_element.set_is_pressed( _is_pressed );
		_text_element.set_is_selected( _is_mouse_focused || is_descendant_mouse_focused );
		_text_element.set_is_pressed( _is_pressed );
	}

	menu_control_check_c::menu_control_check_c()
		: menu_control_c()
		, _mode( mode_e_normal )
		, _is_checked( false )
		, _box_element( string8_c( core_list_mode_e_static, "box_frame" ) )
		, _mark_element( string8_c( core_list_mode_e_static, "mark_frame" ) )
		, _text_element( string8_c( core_list_mode_e_static, "text" ) )
	{
		_select_mode = menu_select_mode_e_mouse_and_directional;

		_box_element.set_shared_color_class( menu_shared_color_class_e_field );
		_box_element.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 16.0f ) );
		_add_daughter_element( &_box_element );

		_mark_element.set_shared_color_class( menu_shared_color_class_e_field );
		_mark_element.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 16.0f ) );
		_mark_element.set_is_showed( _is_checked );
		_add_daughter_element( &_mark_element );

		_text_element.set_shared_color_class( menu_shared_color_class_e_field );
		_text_element.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 18.0f, 0.0f, 0.0f, 0.0f ) );
		_add_daughter_element( &_text_element );

		set_style_map_key( string8_c( core_list_mode_e_static, "e_check" ) );
	}

	menu_control_check_c::~menu_control_check_c()
	{
	}

	string16_c menu_control_check_c::get_plain_text_value() const
	{
		return _text_element.get_plain_text_value();
	}

	void_c menu_control_check_c::set_plain_text_value( string8_c const & plain_text )
	{
		_text_element.set_plain_text_value( plain_text );
	}

	void_c menu_control_check_c::set_plain_text_value( string16_c const & plain_text )
	{
		_text_element.set_plain_text_value( plain_text );
	}

	void_c menu_control_check_c::set_rich_text_value( string8_c const & plain_text_with_mark_up )
	{
		_text_element.set_rich_text_value( plain_text_with_mark_up );
	}

	void_c menu_control_check_c::set_rich_text_value( string16_c const & plain_text_with_mark_up )
	{
		_text_element.set_rich_text_value( plain_text_with_mark_up );
	}

	void_c menu_control_check_c::clear_text_value()
	{
		_text_element.clear_text_value();
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
				if ( _mode == mode_e_single && _mother_control )
				{
					// un-check all other related single mode check boxes.
					core_linked_list_c< menu_control_c * >::node_c const * check_list_node = _mother_control->get_daughter_control_list().get_first();
					while ( check_list_node )
					{
						menu_control_check_c * other_check = dynamic_cast< menu_control_check_c * >( check_list_node->get_value() );
						if ( other_check && other_check != this && other_check->_mode == mode_e_single )
						{
							other_check->set_is_checked( false );
						}
						check_list_node = check_list_node->get_next();
					}
				}
				_is_checked = true;
				_mark_element.set_is_showed( true );
				on_checked_changed.invoke( this );
			}
			else
			{
				_is_checked = false;
				_mark_element.set_is_showed( false );
				on_checked_changed.invoke( this );
			}
		}
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
