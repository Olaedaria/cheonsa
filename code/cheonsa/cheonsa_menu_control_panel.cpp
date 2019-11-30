#include "cheonsa_menu_control_panel.h"

namespace cheonsa
{

	void_c menu_control_panel_c::_handle_scroll_on_value_changed( menu_control_scroll_i * scroll )
	{
		if ( scroll == _horizontal_scroll_bar )
		{
			_client->set_content_offset_horizontal( static_cast< float32_c >( -_horizontal_scroll_bar->get_value() ) );
		}
		else if ( scroll == _vertical_scroll_bar )
		{
			_client->set_content_offset_vertical( static_cast< float32_c >( -_vertical_scroll_bar->get_value() ) );
		}
	}

	menu_control_panel_c::menu_control_panel_c()
		: menu_control_c()
		, _element_frame()
		, _client()
		, _horizontal_scroll_bar_visibility_mode( menu_visibility_mode_e_never )
		, _horizontal_scroll_bar( nullptr )
		, _vertical_scroll_bar_visibility_mode( menu_visibility_mode_e_never )
		, _vertical_scroll_bar( nullptr )
	{
		_element_frame.set_name( string8_c( mode_e_static, "frame" ) );
		_element_frame.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_add_element( &_element_frame );

		_client = new menu_control_c();
		_client->set_name( string8_c( mode_e_static, "client" ) );
		_client->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_add_control( _client );

		set_style_map_key( string8_c( mode_e_static, "e_panel" ) );
	}
	
	void_c menu_control_panel_c::update_animations( float32_c time_step )
	{
		_element_frame.set_is_selected( _is_mouse_focused || _is_text_focused );
		menu_control_c::update_animations( time_step );
	}

	void_c menu_control_panel_c::add_control( menu_control_c * control )
	{
		_client->_add_control( control );
	}

	void_c menu_control_panel_c::remove_control( menu_control_c * control )
	{
		_client->_remove_control( control->get_index() );
	}

	menu_visibility_mode_e menu_control_panel_c::get_horizontal_scroll_visibitly_mode() const
	{
		return _horizontal_scroll_bar_visibility_mode;
	}

	void_c menu_control_panel_c::set_horizontal_scroll_visibility_mode( menu_visibility_mode_e value )
	{
		_horizontal_scroll_bar_visibility_mode = value;
		if ( value == menu_visibility_mode_e_always || value == menu_visibility_mode_e_automatic )
		{
			if ( _horizontal_scroll_bar == nullptr )
			{
				_horizontal_scroll_bar = new menu_control_scroll_bar_horizontal_c();
				_horizontal_scroll_bar->set_name( string8_c( mode_e_static, "horizontal_scroll_bar" ) );
				_horizontal_scroll_bar->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 8.0f, 0.0f, 0.0f ) );
				_horizontal_scroll_bar->on_value_changed.subscribe( this, &menu_control_panel_c::_handle_scroll_on_value_changed );
				_add_control( _horizontal_scroll_bar );
			}
		}
		else
		{
			if ( _horizontal_scroll_bar != nullptr )
			{
				_remove_control( _horizontal_scroll_bar->get_index() );
				delete _horizontal_scroll_bar;
				_horizontal_scroll_bar = nullptr;
			}
		}
		if ( _horizontal_scroll_bar != nullptr )
		{
			_horizontal_scroll_bar->set_value_range_and_page_size( 0.0, _client->get_content_box().get_width(), _client->get_local_box().get_width() );
			_horizontal_scroll_bar->update_visibility( value );
		}
	}

	menu_visibility_mode_e menu_control_panel_c::get_vertical_scroll_visibility_mode() const
	{
		return _vertical_scroll_bar_visibility_mode;
	}

	void_c menu_control_panel_c::set_vertical_scroll_visibility_mode( menu_visibility_mode_e value )
	{
		_vertical_scroll_bar_visibility_mode = value;
		if ( value == menu_visibility_mode_e_always || value == menu_visibility_mode_e_automatic )
		{
			if ( _vertical_scroll_bar == nullptr )
			{
				_vertical_scroll_bar = new menu_control_scroll_bar_vertical_c();
				_vertical_scroll_bar->set_name( string8_c( mode_e_static, "vertical_scroll_bar" ) );
				_vertical_scroll_bar->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 8.0f, 0.0f, 0.0f, 0.0f ) );
				_vertical_scroll_bar->on_value_changed.subscribe( this, &menu_control_panel_c::_handle_scroll_on_value_changed );
				_add_control( _vertical_scroll_bar );
			}
		}
		else
		{
			if ( _vertical_scroll_bar != nullptr )
			{
				_remove_control( _vertical_scroll_bar->get_index() );
				delete _vertical_scroll_bar;
				_vertical_scroll_bar = nullptr;
			}
		}
		if ( _vertical_scroll_bar != nullptr )
		{
			_vertical_scroll_bar->set_value_range_and_page_size( 0.0, _client->get_content_box().get_height(), _client->get_local_box().get_height() );
			_vertical_scroll_bar->update_visibility( value );
		}
	}

}
