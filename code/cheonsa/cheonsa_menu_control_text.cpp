#include "cheonsa_menu_control_text.h"
#include "cheonsa_data_scribe_markup.h"
#include "cheonsa_ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	void_c menu_control_text_c::_on_text_focus_gained()
	{
		_element_text.handle_on_character_focus_gain();
		on_text_focus_gained.invoke( menu_event_info_c( this, nullptr ) );
	}

	void_c menu_control_text_c::_on_text_focus_lost()
	{
		_element_text.handle_on_character_focus_lose();
		on_text_focus_lost.invoke( menu_event_info_c( this, nullptr ) );
	}

	void_c menu_control_text_c::_on_input( input_event_c * input_event )
	{
		_element_text.handle_input( input_event );
	}

	void_c menu_control_text_c::_handle_on_value_changed_preview( menu_element_text_c * text )
	{
		on_value_changed_preview.invoke( this );
	}

	void_c menu_control_text_c::_handle_on_value_changed_commit( menu_element_text_c * text )
	{
		on_value_changed_commit.invoke( this );
	}

	menu_control_text_c::menu_control_text_c()
		: _element_frame()
		, _element_text()
		, _horizontal_scroll_visibility_mode( menu_visibility_mode_e_never )
		, _horizontal_scroll( nullptr )
		, _vertical_scroll_visibility_mode( menu_visibility_mode_e_never )
		, _vertical_scroll( nullptr )
	{
		_element_frame.set_name( string8_c( mode_e_static, "frame" ) );
		_element_frame.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_add_element( &_element_frame );

		_element_text.set_name( string8_c( mode_e_static, "text" ) );
		_element_text.set_multi_line( false );
		_element_text.set_text_format_mode( menu_text_format_mode_e_plain );
		_element_text.set_text_edit_mode( menu_text_edit_mode_e_editable );
		_element_text.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_element_text.on_value_changed_preview.subscribe( this, &menu_control_text_c::_handle_on_value_changed_preview );
		_element_text.on_value_changed_commit.subscribe( this, &menu_control_text_c::_handle_on_value_changed_commit );
		_add_element( &_element_text );

		set_style_map_key( string8_c( mode_e_static, "e_text" ) );
	}

	void_c menu_control_text_c::update_animations( float32_c time_step )
	{
		menu_control_c::update_animations( time_step );
		vector32x2_c content_offset = vector32x2_c( 0.0f, 0.0f );
		if ( _horizontal_scroll )
		{
			_horizontal_scroll->set_value_range_and_page_size( 0.0, _element_text .get_content_width(), _local_box.get_width() );
			_horizontal_scroll->update_visibility( _horizontal_scroll_visibility_mode );
			content_offset.a = static_cast< float32_c >( _horizontal_scroll->get_value() );
		}
		if ( _vertical_scroll )
		{
			_vertical_scroll->set_value_range_and_page_size( 0.0, _element_text .get_content_height(), _local_box.get_height() );
			_vertical_scroll->update_visibility( _vertical_scroll_visibility_mode );
			content_offset.b = static_cast< float32_c >( _vertical_scroll->get_value() );
		}
		_element_text.set_content_offset( content_offset );
		_element_text.update_animations( time_step );
	}

	void_c menu_control_text_c::load_properties( data_scribe_markup_c::node_c const * node )
	{
		menu_control_c::load_properties( node );

		data_scribe_markup_c::attribute_c const * attribute = nullptr;

		attribute = node->find_attribute( "horizontal_scroll_visibility_mode" );
		if ( attribute )
		{
			menu_visibility_mode_e horizontal_scroll_visibility_mode = menu_visibility_mode_e_never;
			if ( attribute->get_value() == "automatic" )
			{
				horizontal_scroll_visibility_mode = menu_visibility_mode_e_automatic;
			}
			else if ( attribute->get_value() == "always" )
			{
				horizontal_scroll_visibility_mode = menu_visibility_mode_e_always;
			}
			set_horizontal_scroll_visibility_mode( horizontal_scroll_visibility_mode );
		}

		attribute = node->find_attribute( "vertical_scroll_bar_mode" );
		if ( attribute )
		{
			menu_visibility_mode_e vertical_scroll_visibility_mode = menu_visibility_mode_e_never;
			if ( attribute->get_value() == "automatic" )
			{
				vertical_scroll_visibility_mode = menu_visibility_mode_e_automatic;
			}
			else if ( attribute->get_value() == "always" )
			{
				vertical_scroll_visibility_mode = menu_visibility_mode_e_always;
			}
			set_vertical_scroll_visibility_mode( vertical_scroll_visibility_mode );
		}

		//menu_text_format_mode_e text_format_mode = menu_text_format_mode_e_plain;
		//attribute = node->find_attribute( "text_format_mode" );
		//if ( attribute )
		//{
		//	if ( attribute->value == "rich" )
		//	{
		//		text_format_mode = menu_text_format_mode_e_rich;
		//	}
		//}
		//set_text_format_mode( text_format_mode);

		attribute = node->find_attribute( "text_edit_mode" );
		if ( attribute )
		{
			menu_text_edit_mode_e text_edit_mode = menu_text_edit_mode_e_editable;
			if ( attribute->get_value() == "static_selectable" )
			{
				text_edit_mode = menu_text_edit_mode_e_static_selectable;
			}
			else if ( attribute->get_value() == "editable" )
			{
				text_edit_mode = menu_text_edit_mode_e_editable;
			}
			set_text_edit_mode( text_edit_mode );
		}

		attribute = node->find_attribute( "multi_line" );
		if ( attribute )
		{
			boolean_c multi_line = true;
			if ( ops::convert_string8_to_boolean( attribute->get_value(), multi_line ) )
			{
				set_multi_line( multi_line );
			}
		}

		attribute = node->find_attribute( "word_wrap" );
		if ( attribute )
		{
			boolean_c word_wrap = true;
			if ( ops::convert_string8_to_boolean( attribute->get_value(), word_wrap ) )
			{
				set_word_wrap( word_wrap );
			}
		}

		attribute = node->find_attribute( "character_limit" );
		if ( attribute )
		{
			sint32_c character_limit = 1000;
			if ( ops::convert_string8_to_sint32( attribute->get_value(), character_limit ) )
			{
				character_limit = ops::math_clamp( character_limit, 0, 1000000 );
				set_character_limit( character_limit );
			}
		}

		attribute = node->find_attribute( "placeholder_text_style_key" );
		if ( attribute )
		{
			set_placeholder_text_style_key( attribute->get_value() );
		}

		attribute = node->find_attribute( "placeholder_text_plain_text" );
		if ( attribute )
		{
			set_placeholder_text_plain_text( string16_c( attribute->get_value() ) );
		}

		attribute = node->find_attribute( "placeholder_text_string_key" );
		if ( attribute )
		{
			set_placeholder_text_string_key( attribute->get_value() );
		}

		attribute = node->find_attribute( "text_style_key" );
		if ( attribute )
		{
			set_text_style_key( attribute->get_value() );
		}

		attribute = node->find_attribute( "plain_text" );
		if ( attribute )
		{
			set_plain_text( string16_c( attribute->get_value() ) );
		}

		attribute = node->find_attribute( "text_string_key" );
		if ( attribute )
		{
			set_text_string_key( attribute->get_value() );
		}
	}

	string16_c menu_control_text_c::get_placeholder_text_plain_text() const
	{
		if ( _element_text_placeholder )
		{
			return _element_text_placeholder->get_plain_text();
		}
		return string16_c();
	}

	void_c menu_control_text_c::set_placeholder_text_style_key( string8_c const & style_key )
	{
		_placeholder_style_key = style_key;
		if ( _element_text_placeholder )
		{
			_element_text_placeholder->set_style_key( _placeholder_style_key );
		}
	}

	void_c menu_control_text_c::set_placeholder_text_plain_text( string16_c const & plain_text )
	{
		_placeholder_string_key = string8_c();
		if ( _element_text_placeholder == nullptr )
		{
			_element_text_placeholder = new menu_element_text_c();
			_element_text_placeholder->set_name( string8_c( mode_e_static, "text_placeholder" ) );
			_element_text_placeholder->set_style_key( _placeholder_style_key );
			_add_element( _element_text_placeholder );
		}
		_element_text_placeholder->get_string_reference().clear();
		_element_text_placeholder->set_plain_text( plain_text );
	}

	void_c menu_control_text_c::set_placeholder_text_string_key( string8_c const & string_key )
	{
		_placeholder_string_key = string_key;
		if ( _element_text_placeholder == nullptr )
		{
			_element_text_placeholder = new menu_element_text_c();
			_element_text_placeholder->set_name( string8_c( mode_e_static, "text_placeholder" ) );
			_element_text_placeholder->set_style_key( _placeholder_style_key );
			_add_element( _element_text_placeholder );
		}
		_element_text_placeholder->get_string_reference().set_key( _placeholder_string_key );
	}

	void_c menu_control_text_c::clear_placeholder_text()
	{
		if ( _element_text_placeholder != nullptr )
		{
			_remove_element( _element_text_placeholder );
			delete _element_text_placeholder;
			_element_text_placeholder = nullptr;
		}
	}

	void_c menu_control_text_c::set_text_style_key( string8_c const & style_key )
	{
		_element_text.set_style_key( style_key );
	}

	string16_c menu_control_text_c::get_plain_text() const
	{
		return _element_text.get_plain_text();
	}

	void_c menu_control_text_c::set_plain_text( string16_c const & plain_text )
	{
		_element_text.set_plain_text( plain_text );
	}

	void_c menu_control_text_c::set_text_string_key( string8_c const & string_key )
	{
		_element_text.get_string_reference().set_key( string_key );
	}

	void_c menu_control_text_c::clear_text()
	{
		_element_text.clear_text();
	}

	menu_text_filter_mode_e menu_control_text_c::get_text_filter_mode() const
	{
		return _element_text.get_text_filter_mode();
	}

	void_c menu_control_text_c::set_text_filter_mode( menu_text_filter_mode_e filter_mode, string16_c const & filter_string )
	{
		_element_text.set_text_filter_mode( filter_mode, filter_string );
	}

	menu_text_edit_mode_e menu_control_text_c::get_text_edit_mode() const
	{
		return _element_text.get_text_edit_mode();
	}

	void_c menu_control_text_c::set_text_edit_mode( menu_text_edit_mode_e value )
	{
		_element_text.set_text_edit_mode( value );
	}

	boolean_c menu_control_text_c::get_multi_line() const
	{
		return _element_text.get_multi_line();
	}

	void_c menu_control_text_c::set_multi_line( boolean_c value )
	{
		_element_text.set_multi_line( value );
	}

	boolean_c menu_control_text_c::get_word_wrap() const
	{
		return _element_text.get_word_wrap();
	}

	void_c menu_control_text_c::set_word_wrap( boolean_c value )
	{
		_element_text.set_word_wrap( value );
	}

	sint32_c menu_control_text_c::get_character_limit() const
	{
		return _element_text.get_character_limit();
	}

	void_c menu_control_text_c::set_character_limit( sint32_c value )
	{
		_element_text.set_character_limit( value );
	}

	sint32_c menu_control_text_c::get_cursor_index() const
	{
		return _element_text.get_cursor_index();
	}

	void_c menu_control_text_c::set_cursor_index( sint32_c value )
	{
		_element_text.set_cursor_index( value );
	}

	menu_text_align_horizontal_e menu_control_text_c::get_text_align_horizontal() const
	{
		return _element_text.get_text_align_horizontal();
	}

	void_c menu_control_text_c::set_text_align_horizontal( menu_text_align_horizontal_e value )
	{
		_element_text.set_text_align_horizontal( value );
	}

	menu_text_align_vertical_e menu_control_text_c::get_text_align_vertical() const
	{
		return _element_text.get_text_align_vertical();
	}

	void_c menu_control_text_c::set_text_align_vertical( menu_text_align_vertical_e value )
	{
		_element_text.set_text_align_vertical( value );
	}

	menu_visibility_mode_e menu_control_text_c::get_horizontal_scroll_visibility_mode() const
	{
		return _horizontal_scroll_visibility_mode;
	}

	void_c menu_control_text_c::set_horizontal_scroll_visibility_mode( menu_visibility_mode_e value )
	{
		_horizontal_scroll_visibility_mode = value;
		if ( value == menu_visibility_mode_e_always || value == menu_visibility_mode_e_automatic )
		{
			if ( _horizontal_scroll == nullptr )
			{
				_horizontal_scroll = new menu_control_scroll_c();
				_horizontal_scroll->set_name( string8_c( mode_e_static, "horizontal_scroll" ) );
				_horizontal_scroll->set_orientation( menu_control_scroll_c::orientation_e_horizontal );
				_horizontal_scroll->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 8.0f, 0.0f, 0.0f ) );
				_add_control( _horizontal_scroll );
			}
		}
		else
		{
			if ( _horizontal_scroll != nullptr )
			{
				_remove_control( _horizontal_scroll->get_index() );
				delete _horizontal_scroll;
				_horizontal_scroll = nullptr;
			}
		}
		if ( _horizontal_scroll != nullptr )
		{
			_horizontal_scroll->set_value_range_and_page_size( 0.0, _element_text.get_content_width(), _local_box.get_width() );
			_horizontal_scroll->update_visibility( value );
		}
	}

	menu_visibility_mode_e menu_control_text_c::get_vertical_scroll_visibility_mode() const
	{
		return _vertical_scroll_visibility_mode;
	}

	void_c menu_control_text_c::set_vertical_scroll_visibility_mode( menu_visibility_mode_e value )
	{
		_vertical_scroll_visibility_mode = value;
		if ( value == menu_visibility_mode_e_always || value == menu_visibility_mode_e_automatic )
		{
			if ( _vertical_scroll == nullptr )
			{
				_vertical_scroll = new menu_control_scroll_c();
				_vertical_scroll->set_name( string8_c( mode_e_static, "vertical_scroll" ) );
				_vertical_scroll->set_orientation( menu_control_scroll_c::orientation_e_vertical );
				_vertical_scroll->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 8.0f, 0.0f, 0.0f, 0.0f ) );
				_add_control( _vertical_scroll );
			}
		}
		else
		{
			if ( _vertical_scroll != nullptr )
			{
				_remove_control( _vertical_scroll->get_index() );
				delete _vertical_scroll;
				_vertical_scroll = nullptr;
			}
		}
		if ( _vertical_scroll != nullptr )
		{
			_vertical_scroll->set_value_range_and_page_size( 0.0, _element_text.get_content_height(), _local_box.get_height() );
			_vertical_scroll->update_visibility( value );
		}
	}

}
