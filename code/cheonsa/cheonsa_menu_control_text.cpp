#include "cheonsa_menu_control_text.h"
#include "cheonsa_data_scribe_markup.h"
#include "cheonsa_user_interface.h"
#include "cheonsa_engine.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	void_c menu_control_text_c::_on_is_text_focused_changed( menu_control_c * next_control )
	{
		_text_element.handle_on_is_text_focused_changed( _is_text_focused );
		on_is_text_focused_changed.invoke( menu_event_information_c( this, next_control, nullptr ) );
	}

	void_c menu_control_text_c::_on_clicked( input_event_c * input_event )
	{
		on_clicked.invoke( menu_event_information_c( this, nullptr, input_event ) );
	}

	void_c menu_control_text_c::_on_multi_clicked( input_event_c * input_event )
	{
		_text_element.handle_on_multi_clicked( input_event );
		if ( input_event->get_menu_multi_click_count() == 3 )
		{
			_mother_user_interface->reset_multi_click_detection();
		}
	}

	void_c menu_control_text_c::_on_input( input_event_c * input_event )
	{
		on_input.invoke( menu_event_information_c( this, nullptr, input_event ) );
		if ( !input_event->get_was_handled() )
		{
			// still process even if get_is_actually_enabled() == false, since it's still allowable for the user to select and copy text, but not input.
			_text_element.handle_on_input( input_event );
		}
	}

	void_c menu_control_text_c::_handle_on_plain_text_value_changed_preview( menu_element_text_c * text )
	{
		on_plain_text_value_changed_preview.invoke( this );
	}

	void_c menu_control_text_c::_handle_on_plain_text_value_changed( menu_element_text_c * text )
	{
		on_plain_text_value_changed.invoke( this );
	}

	menu_control_text_c::menu_control_text_c()
		: menu_control_c()
		, _frame_element( string8_c( "frame", core_list_mode_e_static ) )
		, _text_element( string8_c( "text", core_list_mode_e_static ) )
		, _place_holder_text_element( nullptr )
		, _border_frame_element( string8_c( "border_frame", core_list_mode_e_static ) )
		, _horizontal_scroll_bar_visibility_mode( menu_visibility_mode_e_never )
		, _horizontal_scroll_bar( nullptr )
		, _vertical_scroll_bar_visibility_mode( menu_visibility_mode_e_never )
		, _vertical_scroll_bar( nullptr )
	{
		_frame_element.set_shared_color_class( menu_shared_color_class_e_field );
		_frame_element.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_add_daughter_element( &_frame_element );

		_text_element.set_shared_color_class( menu_shared_color_class_e_field );
		_text_element.set_multi_line( false );
		_text_element.set_text_format_mode( menu_text_format_mode_e_plain );
		_text_element.set_text_interact_mode( menu_text_interact_mode_e_editable );
		_text_element.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_text_element.on_text_value_changed_preview.subscribe( this, &menu_control_text_c::_handle_on_plain_text_value_changed_preview );
		_text_element.on_text_value_changed.subscribe( this, &menu_control_text_c::_handle_on_plain_text_value_changed );
		_add_daughter_element( &_text_element );

		_border_frame_element.set_is_overlay( true );
		_add_daughter_element( &_border_frame_element );

		set_style_map_key( string8_c( "e_text", core_list_mode_e_static ) );
	}

	menu_control_text_c::~menu_control_text_c()
	{
		if ( _place_holder_text_element )
		{
			_remove_daughter_element( _place_holder_text_element );
			delete _place_holder_text_element;
			_place_holder_text_element = nullptr;
		}
	}

	void_c menu_control_text_c::update_animations( float32_c time_step )
	{
		menu_control_c::update_animations( time_step );

		vector32x2_c content_offset = vector32x2_c( 0.0f, 0.0f );
		if ( _horizontal_scroll_bar )
		{
			_horizontal_scroll_bar->set_value_range_and_page_size( 0.0, _text_element .get_content_width(), _local_box.get_width() );
			_horizontal_scroll_bar->update_visibility( _horizontal_scroll_bar_visibility_mode );
			content_offset.a = static_cast< float32_c >( _horizontal_scroll_bar->get_value() );
		}
		if ( _vertical_scroll_bar )
		{
			_vertical_scroll_bar->set_value_range_and_page_size( 0.0, _text_element .get_content_height(), _local_box.get_height() );
			_vertical_scroll_bar->update_visibility( _vertical_scroll_bar_visibility_mode );
			content_offset.b = static_cast< float32_c >( _vertical_scroll_bar->get_value() );
		}
		_text_element.set_content_offset( content_offset );
		_text_element.update_animations( time_step );

		boolean_c is_descendant_mouse_focused = is_ascendant_of( _mother_user_interface->get_mouse_focused() );

		_frame_element.set_is_selected( _is_mouse_focused || is_descendant_mouse_focused );
		_frame_element.set_is_pressed( false );

		_text_element.set_is_selected( _is_mouse_focused || is_descendant_mouse_focused );
		_text_element.set_is_pressed( false );
	}

	string16_c menu_control_text_c::get_place_holder_text_value() const
	{
		if ( _place_holder_text_element )
		{
			return _place_holder_text_element->get_plain_text_value();
		}
		return string16_c();
	}

	void_c menu_control_text_c::set_place_holder_plain_text_value( string8_c const & plain_text )
	{
		if ( _place_holder_text_element == nullptr )
		{
			_place_holder_text_element = new menu_element_text_c( string8_c( "place_holder_text", core_list_mode_e_static ) );
			_add_daughter_element( _place_holder_text_element );
		}
		_place_holder_text_element->set_plain_text_value( plain_text );
	}

	void_c menu_control_text_c::set_place_holder_plain_text_value( string16_c const & plain_text )
	{
		if ( _place_holder_text_element == nullptr )
		{
			_place_holder_text_element = new menu_element_text_c( string8_c( "place_holder_text", core_list_mode_e_static ) );
			_add_daughter_element( _place_holder_text_element );
		}
		_place_holder_text_element->set_plain_text_value( plain_text );
	}

	void_c menu_control_text_c::clear_place_holder_text_value()
	{
		if ( _place_holder_text_element )
		{
			_remove_daughter_element( _place_holder_text_element );
			delete _place_holder_text_element;
			_place_holder_text_element = nullptr;
		}
	}

	string16_c menu_control_text_c::get_plain_text_value() const
	{
		return _text_element.get_plain_text_value();
	}

	void_c menu_control_text_c::set_plain_text_value( string8_c const & plain_text )
	{
		_text_element.set_plain_text_value( plain_text );
	}

	void_c menu_control_text_c::set_plain_text_value( string16_c const & plain_text )
	{
		_text_element.set_plain_text_value( plain_text );
	}

	void_c menu_control_text_c::set_rich_text_value( string8_c const & plain_text_with_mark_up )
	{
		_text_element.set_rich_text_value( plain_text_with_mark_up );
	}

	void_c menu_control_text_c::set_rich_text_value( string16_c const & plain_text_with_mark_up )
	{
		_text_element.set_rich_text_value( plain_text_with_mark_up );
	}

	void_c menu_control_text_c::clear_text_value()
	{
		_text_element.clear_text_value();
	}

	menu_text_format_mode_e menu_control_text_c::get_text_format_mode() const
	{
		return _text_element.get_text_format_mode();
	}

	void_c menu_control_text_c::set_text_format_mode( menu_text_format_mode_e value )
	{
		_text_element.set_text_format_mode( value );
	}

	menu_text_interact_mode_e menu_control_text_c::get_text_interact_mode() const
	{
		return _text_element.get_text_interact_mode();
	}

	void_c menu_control_text_c::set_text_interact_mode( menu_text_interact_mode_e value )
	{
		_text_element.set_text_interact_mode( value );
	}

	menu_text_filter_mode_e menu_control_text_c::get_text_filter_mode() const
	{
		return _text_element.get_text_filter_mode();
	}

	void_c menu_control_text_c::set_text_filter_mode( menu_text_filter_mode_e value )
	{
		_text_element.set_text_filter_mode( value );
	}

	void_c menu_control_text_c::set_text_filter_string( string16_c const & value )
	{
		_text_element.set_text_filter_string( value );
	}

	sint32_c menu_control_text_c::get_character_limit() const
	{
		return _text_element.get_character_limit();
	}

	void_c menu_control_text_c::set_character_limit( sint32_c value )
	{
		_text_element.set_character_limit( value );
	}

	boolean_c menu_control_text_c::get_multi_line() const
	{
		return _text_element.get_multi_line();
	}

	void_c menu_control_text_c::set_multi_line( boolean_c value )
	{
		_text_element.set_multi_line( value );
	}

	boolean_c menu_control_text_c::get_word_wrap() const
	{
		return _text_element.get_word_wrap();
	}

	void_c menu_control_text_c::set_word_wrap( boolean_c value )
	{
		_text_element.set_word_wrap( value );
	}

	sint32_c menu_control_text_c::get_cursor_index() const
	{
		return _text_element.get_cursor_index();
	}

	void_c menu_control_text_c::set_cursor_index( sint32_c value )
	{
		_text_element.set_cursor_index( value );
	}

	menu_text_align_horizontal_e menu_control_text_c::get_text_align_horizontal() const
	{
		return _text_element.get_text_align_horizontal();
	}

	void_c menu_control_text_c::set_text_align_horizontal( menu_text_align_horizontal_e value )
	{
		_text_element.set_text_align_horizontal( value );
	}

	menu_text_align_vertical_e menu_control_text_c::get_text_align_vertical() const
	{
		return _text_element.get_text_align_vertical();
	}

	void_c menu_control_text_c::set_text_align_vertical( menu_text_align_vertical_e value )
	{
		_text_element.set_text_align_vertical( value );
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
				_horizontal_scroll_bar = new menu_control_scroll_bar_x_c();
				_horizontal_scroll_bar->set_name( string8_c( "horizontal_scroll_bar", core_list_mode_e_static ) );
				_horizontal_scroll_bar->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, _horizontal_scroll_bar->get_preferred_thickness(), 0.0f, 0.0f ) );
				add_daughter_control( _horizontal_scroll_bar );
			}
		}
		else
		{
			if ( _horizontal_scroll_bar )
			{
				remove_daughter_control( _horizontal_scroll_bar ); // this will remove a reference from _horizontal_scroll_bar, which will (probably) delete it.
				_horizontal_scroll_bar = nullptr;
			}
		}
		if ( _horizontal_scroll_bar )
		{
			_horizontal_scroll_bar->set_value_range_and_page_size( 0.0, _text_element.get_content_width(), _local_box.get_width() );
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
				_vertical_scroll_bar = new menu_control_scroll_bar_y_c();
				_vertical_scroll_bar->set_name( string8_c( "vertical_scroll_bar", core_list_mode_e_static ) );
				_vertical_scroll_bar->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( _vertical_scroll_bar->get_preferred_thickness(), 0.0f, 0.0f, 0.0f ) );
				add_daughter_control( _vertical_scroll_bar );
			}
		}
		else
		{
			if ( _vertical_scroll_bar )
			{
				remove_daughter_control( _vertical_scroll_bar ); // this will remove a reference from _vertical_scroll_bar, which will (probably) delete it.
				_vertical_scroll_bar = nullptr;
			}
		}
		if ( _vertical_scroll_bar )
		{
			_vertical_scroll_bar->set_value_range_and_page_size( 0.0, _text_element.get_content_height(), _local_box.get_height() );
			_vertical_scroll_bar->update_visibility( value );
		}
	}

}
