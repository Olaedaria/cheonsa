#include "cheonsa_menu_control_panel.h"

namespace cheonsa
{

	void_c menu_control_panel_i::_handle_scroll_bar_on_value_changed( menu_control_scroll_bar_i * scroll_bar )
	{
		if ( scroll_bar == _horizontal_scroll_bar )
		{
			_client->set_content_offset_horizontal( static_cast< float32_c >( -_horizontal_scroll_bar->get_value() ) );
		}
		else if ( scroll_bar == _vertical_scroll_bar )
		{
			_client->set_content_offset_vertical( static_cast< float32_c >( -_vertical_scroll_bar->get_value() ) );
		}
	}

	void_c menu_control_panel_i::_handle_scroll_bar_on_preferred_thickness_changed( menu_control_scroll_bar_i * scroll_bar )
	{
		if ( scroll_bar == _horizontal_scroll_bar )
		{
			_lay_out_horizontal_scroll_bar();
		}
		else if ( scroll_bar == _vertical_scroll_bar )
		{
			_lay_out_vertical_scroll_bar();
		}
	}

	sint32_c menu_control_panel_i::_get_controls_in_client_count() const
	{
		return _client->get_control_count();
	}

	menu_control_c const * menu_control_panel_i::_get_control_in_client( sint32_c control_index ) const
	{
		return _client->get_control( control_index );
	}

	menu_control_c * menu_control_panel_i::_get_control_in_client( sint32_c control_index )
	{
		return _client->get_control( control_index );
	}

	sint32_c menu_control_panel_i::_give_control_to_client( menu_control_c * control, sint32_c index )
	{
		return _client->_give_control( control, index );
	}

	menu_control_c * menu_control_panel_i::_take_control_from_client( sint32_c control_index )
	{
		return _client->_take_control( control_index );
	}

	void_c menu_control_panel_i::_remove_and_delete_all_controls_from_client()
	{
		_client->_remove_and_delete_all_controls();
	}

	menu_control_panel_i::menu_control_panel_i( string8_c const & name )
		: menu_control_c( name )
		, _element_frame( string8_c( core_list_mode_e_static, "frame" ) )
		, _client()
		, _horizontal_scroll_bar_visibility_mode( menu_visibility_mode_e_never )
		, _horizontal_scroll_bar( nullptr )
		, _vertical_scroll_bar_visibility_mode( menu_visibility_mode_e_never )
		, _vertical_scroll_bar( nullptr )
	{
		_element_frame.set_shared_color_class( menu_shared_color_class_e_window );
		_element_frame.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_add_element( &_element_frame );

		_client = new menu_control_c( string8_c( core_list_mode_e_static, "client" ) );
		_client->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_give_control( _client );

		set_style_map_key( string8_c( core_list_mode_e_static, "e_panel" ) );
	}

	void_c menu_control_panel_i::update_animations( float32_c time_step )
	{
		_element_frame.set_is_selected( _is_mouse_focused || _is_text_focused );
		menu_control_c::update_animations( time_step );
	}

	box32x2_c const & menu_control_panel_i::_get_client_margins() const
	{
		return _client->get_local_anchor_measures();
	}

	void_c menu_control_panel_i::_set_client_margins( box32x2_c const & value )
	{
		_client->set_local_anchor_measures( value );
		_lay_out_vertical_scroll_bar();
		_lay_out_horizontal_scroll_bar();
	}

	menu_visibility_mode_e menu_control_panel_i::_get_horizontal_scroll_bar_visibility_mode() const
	{
		return _horizontal_scroll_bar_visibility_mode;
	}

	void_c menu_control_panel_i::_set_horizontal_scroll_bar_visibility_mode( menu_visibility_mode_e value )
	{
		_horizontal_scroll_bar_visibility_mode = value;
		if ( value == menu_visibility_mode_e_always || value == menu_visibility_mode_e_automatic )
		{
			if ( _horizontal_scroll_bar == nullptr )
			{
				_horizontal_scroll_bar = new menu_control_scroll_bar_x_c( string8_c( core_list_mode_e_static, "horizontal_scroll_bar" ) );
				_horizontal_scroll_bar->on_value_changed.subscribe( this, &menu_control_panel_i::_handle_scroll_bar_on_value_changed );
				_horizontal_scroll_bar->on_preferred_thickness_changed.subscribe( this, &menu_control_panel_i::_handle_scroll_bar_on_preferred_thickness_changed );
				_lay_out_horizontal_scroll_bar();
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
			_horizontal_scroll_bar->set_value_range_and_page_size( 0.0, _client->get_content_bounds().get_width(), _client->get_local_box().get_width() );
			_horizontal_scroll_bar->update_visibility( value );
		}
	}

	void_c menu_control_panel_i::_lay_out_horizontal_scroll_bar()
	{
		if ( _horizontal_scroll_bar != nullptr )
		{
			box32x2_c const & client_margins = _get_client_margins();
			_horizontal_scroll_bar->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( client_margins.minimum.a, _horizontal_scroll_bar->get_preferred_thickness(), client_margins.maximum.a, client_margins.maximum.b ) );
		}
	}

	menu_visibility_mode_e menu_control_panel_i::_get_vertical_scroll_bar_visibility_mode() const
	{
		return _vertical_scroll_bar_visibility_mode;
	}

	void_c menu_control_panel_i::_set_vertical_scroll_bar_visibility_mode( menu_visibility_mode_e value )
	{
		_vertical_scroll_bar_visibility_mode = value;
		if ( value == menu_visibility_mode_e_always || value == menu_visibility_mode_e_automatic )
		{
			if ( _vertical_scroll_bar == nullptr )
			{
				_vertical_scroll_bar = new menu_control_scroll_bar_y_c( string8_c( core_list_mode_e_static, "vertical_scroll_bar" ) );
				_vertical_scroll_bar->on_value_changed.subscribe( this, &menu_control_panel_i::_handle_scroll_bar_on_value_changed );
				_vertical_scroll_bar->on_preferred_thickness_changed.subscribe( this, &menu_control_panel_i::_handle_scroll_bar_on_preferred_thickness_changed );
				_lay_out_vertical_scroll_bar();
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
			_vertical_scroll_bar->set_value_range_and_page_size( 0.0, _client->get_content_bounds().get_height(), _client->get_local_box().get_height() );
			_vertical_scroll_bar->update_visibility( value );
		}
	}

	void_c menu_control_panel_i::_lay_out_vertical_scroll_bar()
	{
		if ( _vertical_scroll_bar != nullptr )
		{
			box32x2_c const & client_margins = _get_client_margins();
			_vertical_scroll_bar->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( _vertical_scroll_bar->get_preferred_thickness(), client_margins.minimum.b, client_margins.maximum.a, client_margins.maximum.b ) );
		}
	}

}
