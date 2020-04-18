#include "cheonsa_menu_control_text.h"
#include "cheonsa_data_scribe_markup.h"
#include "cheonsa_user_interface.h"
#include "cheonsa_engine.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	void_c menu_control_text_c::_on_is_text_focused_changed()
	{
		_element_text.handle_on_is_text_focused_changed( _is_text_focused );
		on_is_text_focused_changed.invoke( menu_event_information_c( this, nullptr ) );
	}

	void_c menu_control_text_c::_on_clicked( input_event_c * input_event )
	{
		on_clicked.invoke( menu_event_information_c( this, input_event ) );
	}

	void_c menu_control_text_c::_on_multi_clicked( input_event_c * input_event )
	{
		_element_text.handle_on_multi_clicked( input_event );
		if ( input_event->multi_click_count == 3 )
		{
			get_user_interface_root()->reset_multi_click_detection();
		}
	}

	void_c menu_control_text_c::_on_input( input_event_c * input_event )
	{
		_element_text.handle_on_input( input_event );
	}

	void_c menu_control_text_c::_handle_on_value_changed_preview( menu_element_text_c * text )
	{
		on_value_changed_preview.invoke( this );
	}

	void_c menu_control_text_c::_handle_on_value_changed( menu_element_text_c * text )
	{
		on_value_changed.invoke( this );
	}

	menu_control_text_c::menu_control_text_c( string8_c const & name )
		: menu_control_c( name )
		, _element_frame( string8_c( core_list_mode_e_static, "frame" ) )
		, _element_text( string8_c( core_list_mode_e_static, "text" ) )
		, _element_place_holder_text( nullptr )
		, _place_holder_text_style_key()
		, _element_border_frame( string8_c( core_list_mode_e_static, "border_frame" ) )
		, _horizontal_scroll_bar_visibility_mode( menu_visibility_mode_e_never )
		, _horizontal_scroll_bar( nullptr )
		, _vertical_scroll_bar_visibility_mode( menu_visibility_mode_e_never )
		, _vertical_scroll_bar( nullptr )
	{
		_element_frame.set_shared_color_class( menu_shared_color_class_e_field );
		_element_frame.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_add_element( &_element_frame );

		_element_text.set_shared_color_class( menu_shared_color_class_e_field );
		_element_text.set_multi_line( false );
		_element_text.set_text_format_mode( menu_text_format_mode_e_plain );
		_element_text.set_text_interact_mode( menu_text_interact_mode_e_editable );
		_element_text.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_element_text.on_text_value_changed_preview.subscribe( this, &menu_control_text_c::_handle_on_value_changed_preview );
		_element_text.on_text_value_changed.subscribe( this, &menu_control_text_c::_handle_on_value_changed );
		_add_element( &_element_text );

		_element_border_frame.set_is_overlay( true );
		_add_element( &_element_border_frame );

		set_style_map_key( string8_c( core_list_mode_e_static, "e_text" ) );
	}

	menu_control_text_c::~menu_control_text_c()
	{
		if ( _element_place_holder_text != nullptr )
		{
			_remove_element( _element_place_holder_text );
			delete _element_place_holder_text;
			_element_place_holder_text = nullptr;
		}
	}

	void_c menu_control_text_c::update_animations( float32_c time_step )
	{
		menu_control_c::update_animations( time_step );

		vector32x2_c content_offset = vector32x2_c( 0.0f, 0.0f );
		if ( _horizontal_scroll_bar )
		{
			_horizontal_scroll_bar->set_value_range_and_page_size( 0.0, _element_text .get_content_width(), _local_box.get_width() );
			_horizontal_scroll_bar->update_visibility( _horizontal_scroll_bar_visibility_mode );
			content_offset.a = static_cast< float32_c >( _horizontal_scroll_bar->get_value() );
		}
		if ( _vertical_scroll_bar )
		{
			_vertical_scroll_bar->set_value_range_and_page_size( 0.0, _element_text .get_content_height(), _local_box.get_height() );
			_vertical_scroll_bar->update_visibility( _vertical_scroll_bar_visibility_mode );
			content_offset.b = static_cast< float32_c >( _vertical_scroll_bar->get_value() );
		}
		_element_text.set_content_offset( content_offset );
		_element_text.update_animations( time_step );

		boolean_c is_descendant_mouse_focused = is_ascendant_of( get_user_interface_root()->get_mouse_focused() );

		_element_frame.set_is_selected( _is_mouse_focused || is_descendant_mouse_focused );
		_element_frame.set_is_pressed( false );

		_element_text.set_is_selected( _is_mouse_focused || is_descendant_mouse_focused );
		_element_text.set_is_pressed( false );
	}

	string16_c menu_control_text_c::get_place_holder_text_value() const
	{
		if ( _element_place_holder_text != nullptr )
		{
			return _element_place_holder_text->get_plain_text_value();
		}
		return string16_c();
	}

	void_c menu_control_text_c::set_place_holder_plain_text_value( string8_c const & plain_text )
	{
		if ( _element_place_holder_text == nullptr )
		{
			_element_place_holder_text = new menu_element_text_c( string8_c( core_list_mode_e_static, "place_holder_text" ) );
			_element_place_holder_text->set_style_key( _place_holder_text_style_key );
			_add_element( _element_place_holder_text );
		}
		_element_place_holder_text->set_plain_text_value( plain_text );
	}

	void_c menu_control_text_c::set_place_holder_plain_text_value( string16_c const & plain_text )
	{
		if ( _element_place_holder_text == nullptr )
		{
			_element_place_holder_text = new menu_element_text_c( string8_c( core_list_mode_e_static, "place_holder_text" ) );
			_element_place_holder_text->set_style_key( _place_holder_text_style_key );
			_add_element( _element_place_holder_text );
		}
		_element_place_holder_text->set_plain_text_value( plain_text );
	}

	void_c menu_control_text_c::clear_place_holder_text_value()
	{
		if ( _element_place_holder_text != nullptr )
		{
			_remove_element( _element_place_holder_text );
			delete _element_place_holder_text;
			_element_place_holder_text = nullptr;
		}
	}

	string16_c menu_control_text_c::get_plain_text_value() const
	{
		return _element_text.get_plain_text_value();
	}

	void_c menu_control_text_c::set_plain_text_value( string8_c const & plain_text )
	{
		_element_text.set_plain_text_value( plain_text );
	}

	void_c menu_control_text_c::set_plain_text_value( string16_c const & plain_text )
	{
		_element_text.set_plain_text_value( plain_text );
	}

	void_c menu_control_text_c::set_rich_text_value( string8_c const & plain_text_with_mark_up )
	{
		_element_text.set_rich_text_value( plain_text_with_mark_up );
	}

	void_c menu_control_text_c::set_rich_text_value( string16_c const & plain_text_with_mark_up )
	{
		_element_text.set_rich_text_value( plain_text_with_mark_up );
	}

	void_c menu_control_text_c::clear_text_value()
	{
		_element_text.clear_text_value();
	}

	menu_text_format_mode_e menu_control_text_c::get_text_format_mode() const
	{
		return _element_text.get_text_format_mode();
	}

	void_c menu_control_text_c::set_text_format_mode( menu_text_format_mode_e value )
	{
		_element_text.set_text_format_mode( value );
	}

	menu_text_interact_mode_e menu_control_text_c::get_text_interact_mode() const
	{
		return _element_text.get_text_interact_mode();
	}

	void_c menu_control_text_c::set_text_interact_mode( menu_text_interact_mode_e value )
	{
		_element_text.set_text_interact_mode( value );
	}

	menu_text_filter_mode_e menu_control_text_c::get_text_filter_mode() const
	{
		return _element_text.get_text_filter_mode();
	}

	void_c menu_control_text_c::set_text_filter_mode( menu_text_filter_mode_e value )
	{
		_element_text.set_text_filter_mode( value );
	}

	void_c menu_control_text_c::set_text_filter_string( string16_c const & value )
	{
		_element_text.set_text_filter_string( value );
	}

	sint32_c menu_control_text_c::get_text_value_length_limit() const
	{
		return _element_text.get_text_value_length_limit();
	}

	void_c menu_control_text_c::set_text_value_length_limit( sint32_c value )
	{
		_element_text.set_text_value_length_limit( value );
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
		return _horizontal_scroll_bar_visibility_mode;
	}

	void_c menu_control_text_c::set_horizontal_scroll_visibility_mode( menu_visibility_mode_e value )
	{
		_horizontal_scroll_bar_visibility_mode = value;
		if ( value == menu_visibility_mode_e_always || value == menu_visibility_mode_e_automatic )
		{
			if ( _horizontal_scroll_bar == nullptr )
			{
				_horizontal_scroll_bar = new menu_control_scroll_bar_x_c( string8_c( core_list_mode_e_static, "horizontal_scroll_bar" ) );
				_horizontal_scroll_bar->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, _horizontal_scroll_bar->get_preferred_thickness(), 0.0f, 0.0f ) );
				_give_control( _horizontal_scroll_bar );
			}
		}
		else
		{
			if ( _horizontal_scroll_bar != nullptr )
			{
				_take_control( _horizontal_scroll_bar->get_index() );
				delete _horizontal_scroll_bar;
				_horizontal_scroll_bar = nullptr;
			}
		}
		if ( _horizontal_scroll_bar != nullptr )
		{
			_horizontal_scroll_bar->set_value_range_and_page_size( 0.0, _element_text.get_content_width(), _local_box.get_width() );
			_horizontal_scroll_bar->update_visibility( value );
		}
	}

	menu_visibility_mode_e menu_control_text_c::get_vertical_scroll_visibility_mode() const
	{
		return _vertical_scroll_bar_visibility_mode;
	}

	void_c menu_control_text_c::set_vertical_scroll_visibility_mode( menu_visibility_mode_e value )
	{
		_vertical_scroll_bar_visibility_mode = value;
		if ( value == menu_visibility_mode_e_always || value == menu_visibility_mode_e_automatic )
		{
			if ( _vertical_scroll_bar == nullptr )
			{
				_vertical_scroll_bar = new menu_control_scroll_bar_y_c( string8_c( core_list_mode_e_static, "vertical_scroll_bar" ) );
				_vertical_scroll_bar->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( _vertical_scroll_bar->get_preferred_thickness(), 0.0f, 0.0f, 0.0f ) );
				_give_control( _vertical_scroll_bar );
			}
		}
		else
		{
			if ( _vertical_scroll_bar != nullptr )
			{
				_take_control( _vertical_scroll_bar->get_index() );
				delete _vertical_scroll_bar;
				_vertical_scroll_bar = nullptr;
			}
		}
		if ( _vertical_scroll_bar != nullptr )
		{
			_vertical_scroll_bar->set_value_range_and_page_size( 0.0, _element_text.get_content_height(), _local_box.get_height() );
			_vertical_scroll_bar->update_visibility( value );
		}
	}

}
