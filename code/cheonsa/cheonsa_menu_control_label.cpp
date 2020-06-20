#include "cheonsa_menu_control_label.h"

namespace cheonsa
{

	menu_control_label_c::menu_control_label_c()
		: menu_control_c()
		, _text_element( string8_c( "text", core_list_mode_e_static ) )
	{
		_text_element.set_shared_color_class( menu_shared_color_class_e_window );
		_text_element.set_multi_line( true );
		_text_element.set_text_format_mode( menu_text_format_mode_e_rich );
		_text_element.set_text_interact_mode( menu_text_interact_mode_e_static );
		_add_daughter_element( &_text_element );

		set_style_map_key( string8_c( "e_label", core_list_mode_e_static ) );
	}

	menu_control_label_c::~menu_control_label_c()
	{
	}

	void_c menu_control_label_c::load_static_data_properties( data_scribe_markup_c::node_c const * node )
	{
		menu_control_c::load_static_data_properties( node );

		data_scribe_markup_c::attribute_c const * attribute = nullptr;

		attribute = node->find_attribute( "plain_text_value" );
		if ( attribute )
		{
			set_plain_text_value( attribute->get_value() );
		}

		attribute = node->find_attribute( "rich_text_value" );
		if ( attribute )
		{
			set_rich_text_value( attribute->get_value() );
		}
	}

	string16_c menu_control_label_c::get_plain_text_value() const
	{
		return _text_element.get_plain_text_value();
	}

	void_c menu_control_label_c::set_plain_text_value( string8_c const & plain_text )
	{
		_text_element.set_plain_text_value( plain_text );
	}

	void_c menu_control_label_c::set_plain_text_value( string16_c const & plain_text )
	{
		_text_element.set_plain_text_value( plain_text );
	}

	void_c menu_control_label_c::set_rich_text_value( string8_c const & plain_text_with_mark_up )
	{
		_text_element.set_rich_text_value( plain_text_with_mark_up );
	}

	void_c menu_control_label_c::set_rich_text_value( string16_c const & plain_text_with_mark_up )
	{
		_text_element.set_rich_text_value( plain_text_with_mark_up );
	}

	void_c menu_control_label_c::clear_text_value()
	{
		_text_element.clear_text_value();
	}

	menu_text_format_mode_e menu_control_label_c::get_text_format_mode() const
	{
		return _text_element.get_text_format_mode();
	}

	void_c menu_control_label_c::set_text_format_mode( menu_text_format_mode_e value )
	{
		_text_element.set_text_format_mode( value );
	}

	boolean_c menu_control_label_c::get_word_wrap() const
	{
		return _text_element.get_word_wrap();
	}

	void_c menu_control_label_c::set_word_wrap( boolean_c value )
	{
		_text_element.set_word_wrap( value );
	}

	menu_text_align_horizontal_e menu_control_label_c::get_text_align_horizontal() const
	{
		return _text_element.get_text_align_horizontal();
	}

	void_c menu_control_label_c::set_text_align_horizontal( menu_text_align_horizontal_e value )
	{
		_text_element.set_text_align_horizontal( value );
	}

	menu_text_align_vertical_e menu_control_label_c::get_text_align_vertical() const
	{
		return _text_element.get_text_align_vertical();
	}

	void_c menu_control_label_c::set_text_align_vertical( menu_text_align_vertical_e value )
	{
		_text_element.set_text_align_vertical( value );
	}

}
