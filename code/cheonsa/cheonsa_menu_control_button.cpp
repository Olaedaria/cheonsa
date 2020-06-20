#include "cheonsa_menu_control_button.h"
#include "cheonsa_user_interface.h"

namespace cheonsa
{

	menu_control_button_c::menu_control_button_c()
		: menu_control_c()
		, _frame_element( string8_c( "frame", core_list_mode_e_static ) )
		, _text_element( string8_c( "text", core_list_mode_e_static ) )
	{
		_select_mode = menu_select_mode_e_mouse_and_directional;

		_frame_element.set_shared_color_class( menu_shared_color_class_e_button );
		_add_daughter_element( &_frame_element );

		_text_element.set_shared_color_class( menu_shared_color_class_e_button );
		_text_element.set_text_format_mode( menu_text_format_mode_e_rich );
		_text_element.set_text_interact_mode( menu_text_interact_mode_e_static );
		_add_daughter_element( &_text_element );

		set_style_map_key( string8_c( "e_button", core_list_mode_e_static ) );
	}

	string16_c menu_control_button_c::get_plain_text_value() const
	{
		return _text_element.get_plain_text_value();
	}

	void_c menu_control_button_c::set_plain_text_value( string8_c const & plain_text )
	{
		_text_element.set_plain_text_value( plain_text );
	}

	void_c menu_control_button_c::set_plain_text_value( string16_c const & plain_text )
	{
		_text_element.set_plain_text_value( plain_text );
	}

	void_c menu_control_button_c::set_rich_text_value( string8_c const & plain_text_with_mark_up )
	{
		_text_element.set_rich_text_value( plain_text_with_mark_up );
	}

	void_c menu_control_button_c::set_rich_text_value( string16_c const & plain_text_with_mark_up )
	{
		_text_element.set_rich_text_value( plain_text_with_mark_up );
	}

	void_c menu_control_button_c::clear_text_value()
	{
		_text_element.clear_text_value();
	}

	void_c menu_control_button_c::load_static_data_properties( data_scribe_markup_c::node_c const * node )
	{
		menu_control_c::load_static_data_properties( node );

		data_scribe_markup_c::attribute_c const * attribute = node->find_attribute( "plain_text_value" );
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

}
