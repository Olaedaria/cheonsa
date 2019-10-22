#include "cheonsa_menu_control_button.h"

namespace cheonsa
{

	menu_control_button_c::menu_control_button_c()
		: menu_control_c()
		, _element_frame()
		, _element_text()
	{
		_select_mode = menu_select_mode_e_mouse_and_directional;

		_element_frame.set_name( string8_c( mode_e_static, "frame" ) );
		_element_frame.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_add_element( &_element_frame );

		_element_text.set_name( string8_c( mode_e_static, "text" ) );
		_element_text.set_text_format_mode( menu_text_format_mode_e_rich );
		_element_text.set_text_edit_mode( menu_text_edit_mode_e_static );
		_element_text.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_add_element( &_element_text );

		set_style_map_key( string8_c( mode_e_static, "e_button" ) );
	}

	void_c menu_control_button_c::update_animations( float32_c time_step )
	{
		boolean_c is_descendant_mouse_focused = _get_is_descendant_mouse_focused();

		_element_frame.set_is_selected( _is_mouse_focused || is_descendant_mouse_focused );
		_element_frame.set_is_pressed( _is_pressed );

		_element_text.set_is_selected( _is_mouse_focused || is_descendant_mouse_focused );
		_element_text.set_is_pressed( _is_pressed );

		menu_control_c::update_animations( time_step );
	}

	void_c menu_control_button_c::load_properties( data_scribe_markup_c::node_c const * node )
	{
		menu_control_c::load_properties( node );

		data_scribe_markup_c::attribute_c const * attribute = nullptr;

		attribute = node->find_attribute( "plain_text" );
		if ( attribute )
		{
			set_plain_text( string16_c( attribute->get_value() ) );
		}

		attribute = node->find_attribute( "rich_text" );
		if ( attribute )
		{
			set_rich_text( attribute->get_value() );
		}
	}

	string16_c menu_control_button_c::get_plain_text() const
	{
		return _element_text.get_plain_text();
	}

	void_c menu_control_button_c::set_plain_text( string16_c const & value )
	{
		_element_text.set_plain_text( value );
	}

	void_c menu_control_button_c::set_rich_text( string8_c const & plain_text_with_markup )
	{
		_element_text.set_rich_text( plain_text_with_markup );
	}

}
