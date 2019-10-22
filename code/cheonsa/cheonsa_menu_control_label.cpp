#include "cheonsa_menu_control_label.h"

namespace cheonsa
{

	menu_control_label_c::menu_control_label_c()
		: menu_control_c()
		, _element_frame()
		, _element_text()
	{
		_element_frame.set_name( string8_c( mode_e_static, "frame" ) );
		_element_frame.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_add_element( &_element_frame );

		_element_text.set_name( string8_c( mode_e_static, "text" ) );
		_element_text.set_multi_line( true );
		_element_text.set_text_format_mode( menu_text_format_mode_e_rich );
		_element_text.set_text_edit_mode( menu_text_edit_mode_e_static );
		_element_text.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_add_element( &_element_text );

		set_style_map_key( string8_c( mode_e_static, "e_label" ) );
	}

	void_c menu_control_label_c::load_properties( data_scribe_markup_c::node_c const * node )
	{
		menu_control_c::load_properties( node );

		data_scribe_markup_c::attribute_c const * attribute = nullptr;

		attribute = node->find_attribute( "text_format_mode" );
		if ( attribute )
		{
			if ( attribute->get_value() == "rich" )
			{
				set_text_format_mode( menu_text_format_mode_e_rich );
			}
			else
			{
				set_text_format_mode( menu_text_format_mode_e_plain );
			}
		}

		attribute = node->find_attribute( "text_plain" );
		if ( attribute )
		{
			set_plain_text( string16_c( attribute->get_value() ) );
		}

		attribute = node->find_attribute( "text_rich" );
		if ( attribute )
		{
			set_rich_text( attribute->get_value() );
		}

		attribute = node->find_attribute( "string_key" );
		if ( attribute )
		{
			get_string_reference().set_key( attribute->get_value() );
		}
	}

	string_c::reference_c & menu_control_label_c::get_string_reference()
	{
		return _element_text.get_string_reference();
	}

	string16_c menu_control_label_c::get_plain_text() const
	{
		return _element_text.get_plain_text();
	}

	void_c menu_control_label_c::set_plain_text( string16_c const & plain_text )
	{
		_element_text.set_plain_text( plain_text );
	}

	void_c menu_control_label_c::set_rich_text( string8_c const & plain_text_with_markup )
	{
		_element_text.set_rich_text( plain_text_with_markup );
	}

	menu_text_format_mode_e menu_control_label_c::get_text_format_mode() const
	{
		return _element_text.get_text_format_mode();
	}

	void_c menu_control_label_c::set_text_format_mode( menu_text_format_mode_e value )
	{
		_element_text.set_text_format_mode( value );
	}

	boolean_c menu_control_label_c::get_word_wrap() const
	{
		return _element_text.get_word_wrap();
	}

	void_c menu_control_label_c::set_word_wrap( boolean_c value )
	{
		_element_text.set_word_wrap( value );
	}

	menu_text_align_horizontal_e menu_control_label_c::get_text_align_horizontal() const
	{
		return _element_text.get_text_align_horizontal();
	}

	void_c menu_control_label_c::set_text_align_horizontal( menu_text_align_horizontal_e value )
	{
		_element_text.set_text_align_horizontal( value );
	}

	menu_text_align_vertical_e menu_control_label_c::get_text_align_vertical() const
	{
		return _element_text.get_text_align_vertical();
	}

	void_c menu_control_label_c::set_text_align_vertical( menu_text_align_vertical_e value )
	{
		_element_text.set_text_align_vertical( value );
	}

}
