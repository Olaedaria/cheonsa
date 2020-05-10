#include "cheonsa_menu_control_button.h"
#include "cheonsa_user_interface.h"

namespace cheonsa
{

	menu_control_button_c::menu_control_button_c( string8_c const & name )
		: menu_control_c( name )
		, _element_frame( string8_c( core_list_mode_e_static, "frame" ) )
		, _element_text( string8_c( core_list_mode_e_static, "text" ) )
	{
		_select_mode = menu_select_mode_e_mouse_and_directional;

		_element_frame.set_shared_color_class( menu_shared_color_class_e_button );
		_add_daughter_element( &_element_frame );

		_element_text.set_shared_color_class( menu_shared_color_class_e_button );
		_element_text.set_text_format_mode( menu_text_format_mode_e_rich );
		_element_text.set_text_interact_mode( menu_text_interact_mode_e_static );
		_add_daughter_element( &_element_text );

		set_style_map_key( string8_c( core_list_mode_e_static, "e_button" ) );
	}

	menu_control_button_c * menu_control_button_c::make_new_instance( string8_c const & name )
	{
		return new menu_control_button_c( name );
	}

	string16_c menu_control_button_c::get_plain_text_value() const
	{
		return _element_text.get_plain_text_value();
	}

	void_c menu_control_button_c::set_plain_text_value( string8_c const & plain_text )
	{
		_element_text.set_plain_text_value( plain_text );
	}

	void_c menu_control_button_c::set_plain_text_value( string16_c const & plain_text )
	{
		_element_text.set_plain_text_value( plain_text );
	}

	void_c menu_control_button_c::set_rich_text_value( string8_c const & plain_text_with_mark_up )
	{
		_element_text.set_rich_text_value( plain_text_with_mark_up );
	}

	void_c menu_control_button_c::set_rich_text_value( string16_c const & plain_text_with_mark_up )
	{
		_element_text.set_rich_text_value( plain_text_with_mark_up );
	}

	void_c menu_control_button_c::clear_text_value()
	{
		_element_text.clear_text_value();
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
