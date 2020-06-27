#include "cheonsa_menu_control_panel.h"
#include "cheonsa_user_interface.h"

namespace cheonsa
{

	void_c menu_control_panel_i::menu_control_client_c::_on_clicked( input_event_c * input_event )
	{
		_mother_user_interface->reset_multi_click_detection();
		if ( _mother_control->get_is_actually_enabled() )
		{
			_mother_control->on_clicked.invoke( menu_event_information_c( this, nullptr, input_event ) );
		}
	}

	void_c menu_control_panel_i::menu_control_client_c::_on_multi_clicked( input_event_c * input_event )
	{
		if ( _mother_control->get_is_actually_enabled() )
		{
			_mother_control->on_multi_clicked.invoke( menu_event_information_c( this, nullptr, input_event ) );
		}
	}

	void_c menu_control_panel_i::menu_control_client_c::_on_input( input_event_c * input_event )
	{
		if ( _mother_control->get_is_actually_enabled() )
		{
			_mother_control->on_input.invoke( menu_event_information_c( this, nullptr, input_event ) );
		}
	}

	box32x2_c const & menu_control_panel_i::_get_client_margins() const
	{
		return _client_margins;
	}

	void_c menu_control_panel_i::_set_client_margins( box32x2_c const & value )
	{
		_client_margins = value;
		_client->set_layout_simple( _local_box.minimum + _client_margins.minimum, box32x2_c( 0.0f, 0.0f, _local_box.get_width() - _client_margins.maximum.a - _client_margins.maximum.b, _local_box.get_height() - _client_margins.maximum.b - _client_margins.minimum.b ) );
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
				_horizontal_scroll_bar = new menu_control_scroll_bar_x_c();
				_horizontal_scroll_bar->set_name( string8_c( "horizontal_scroll_bar", core_list_mode_e_static ) );
				_horizontal_scroll_bar->on_value_changed.subscribe( this, &menu_control_panel_i::_handle_scroll_bar_on_value_changed );
				_horizontal_scroll_bar->on_preferred_thickness_changed.subscribe( this, &menu_control_panel_i::_handle_scroll_bar_on_preferred_thickness_changed );
				_lay_out_horizontal_scroll_bar();
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
			_horizontal_scroll_bar->set_value_range_and_page_size( 0.0, _client->get_content_bounds().get_width(), _client->get_local_box().get_width() );
			_horizontal_scroll_bar->update_visibility( value );
		}
	}

	void_c menu_control_panel_i::_lay_out_horizontal_scroll_bar()
	{
		if ( _horizontal_scroll_bar )
		{
			_horizontal_scroll_bar->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( _client_margins.minimum.a, _horizontal_scroll_bar->get_preferred_thickness(), _client_margins.maximum.a, _client_margins.maximum.b ) );
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
				_vertical_scroll_bar = new menu_control_scroll_bar_y_c();
				_vertical_scroll_bar->set_name( string8_c( "vertical_scroll_bar", core_list_mode_e_static ) );
				_vertical_scroll_bar->on_value_changed_preview.subscribe( this, &menu_control_panel_i::_handle_scroll_bar_on_value_changed );
				_vertical_scroll_bar->on_preferred_thickness_changed.subscribe( this, &menu_control_panel_i::_handle_scroll_bar_on_preferred_thickness_changed );
				add_daughter_control( _vertical_scroll_bar );
				_lay_out_vertical_scroll_bar();
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
			_vertical_scroll_bar->set_value_range_and_page_size( 0.0, _client->get_content_bounds().get_height(), _client->get_local_box().get_height() );
			_vertical_scroll_bar->update_visibility( value );
		}
	}

	void_c menu_control_panel_i::_lay_out_vertical_scroll_bar()
	{
		if ( _vertical_scroll_bar )
		{
			box32x2_c const & client_margins = _get_client_margins();
			_vertical_scroll_bar->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( _vertical_scroll_bar->get_preferred_thickness(), client_margins.minimum.b, client_margins.maximum.a, client_margins.maximum.b ) );
		}
	}

	void_c menu_control_panel_i::_update_transform_and_layout()
	{
		menu_control_c::_update_transform_and_layout();
		vector32x2_c client_size;
		client_size.a = _local_box.get_width() - _client_margins.maximum.a - _client_margins.minimum.a;
		if ( _vertical_scroll_bar && _vertical_scroll_bar->get_is_showed() )
		{
			client_size.a -= _vertical_scroll_bar->get_preferred_thickness() + 8.0f;
		}
		client_size.b = _local_box.get_height() - _client_margins.maximum.b - _client_margins.minimum.b;
		if ( _horizontal_scroll_bar && _horizontal_scroll_bar->get_is_showed() )
		{
			client_size.b -= _horizontal_scroll_bar->get_preferred_thickness() + 8.0f;
		}
		_client->set_layout_simple( _local_box.minimum + _client_margins.minimum, box32x2_c( 0.0f, 0.0f, client_size.a, client_size.b ) );
	}

	void_c menu_control_panel_i::_update_scroll_bar_visibility()
	{
		boolean_c changed = false;
		box32x2_c content_bounds = _client->get_content_bounds();
		if ( _horizontal_scroll_bar )
		{
			_horizontal_scroll_bar->set_value_range_and_page_size( content_bounds.minimum.a, content_bounds.maximum.b, _local_box.get_width() );
			_horizontal_scroll_bar->set_line_size( 60.0f );
			changed |= _horizontal_scroll_bar->update_visibility( _horizontal_scroll_bar_visibility_mode );
		}
		if ( _vertical_scroll_bar )
		{
			_vertical_scroll_bar->set_value_range_and_page_size( content_bounds.minimum.b, content_bounds.maximum.b, _local_box.get_height() );
			_vertical_scroll_bar->set_line_size( 60.0f );
			changed |= _vertical_scroll_bar->update_visibility( _vertical_scroll_bar_visibility_mode );
		}
		if ( changed )
		{
			_update_transform_and_layout();
		}
	}

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

	menu_control_panel_i::menu_control_panel_i()
		: menu_control_c()
		, _frame_element( string8_c( "frame", core_list_mode_e_static ) )
		, _border_frame_element( string8_c( "border_frame", core_list_mode_e_static ) )
		, _client_margins( 0.0f, 0.0f, 0.0f, 0.0f )
		, _client( nullptr )
		, _horizontal_scroll_bar_visibility_mode( menu_visibility_mode_e_never )
		, _horizontal_scroll_bar( nullptr )
		, _vertical_scroll_bar_visibility_mode( menu_visibility_mode_e_never )
		, _vertical_scroll_bar( nullptr )
	{
		_frame_element.set_shared_color_class( menu_shared_color_class_e_window );
		_add_daughter_element( &_frame_element );

		_border_frame_element.set_is_overlay( true );
		_add_daughter_element( &_border_frame_element );

		_client = new menu_control_client_c();
		_client->set_name( string8_c( "client", core_list_mode_e_static ) );
		_client->set_layout_simple( _local_box.minimum + _client_margins.minimum, box32x2_c( 0.0f, 0.0f, _local_box.get_width() - _client_margins.maximum.a - _client_margins.maximum.b, _local_box.get_height() - _client_margins.maximum.b - _client_margins.minimum.b ) );
		add_daughter_control( _client );

		set_style_map_key( string8_c( "e_panel", core_list_mode_e_static ) );
	}

	void_c menu_control_panel_i::update_animations( float32_c time_step )
	{
		_frame_element.set_is_selected( _is_mouse_focused || _is_text_focused );
		_update_scroll_bar_visibility();
		menu_control_c::update_animations( time_step );
	}

}
