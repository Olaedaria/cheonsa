#include "cheonsa_menu_control_panel.h"
#include "cheonsa_user_interface.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	void_c menu_control_panel_i::menu_control_client_c::_on_clicked( input_event_c * input_event )
	{
		_mother_user_interface->reset_multi_click_detection();
		_mother_control->_on_clicked( input_event );
	}

	void_c menu_control_panel_i::menu_control_client_c::_on_multi_clicked( input_event_c * input_event )
	{
		_mother_control->_on_multi_clicked( input_event );
	}

	void_c menu_control_panel_i::menu_control_client_c::_on_input( input_event_c * input_event )
	{
		_mother_control->_on_input( input_event );
	}

	box32x2_c const & menu_control_panel_i::_get_client_margins() const
	{
		return _client_margins;
	}

	void_c menu_control_panel_i::_set_client_margins( box32x2_c const & value )
	{
		_client_margins = value;
		_client->set_layout_simple( _local_box.minimum + _client_margins.minimum, box32x2_c( 0.0f, 0.0f, _local_box.get_width() - _client_margins.maximum.a - _client_margins.maximum.b, _local_box.get_height() - _client_margins.maximum.b - _client_margins.minimum.b ) );
		_lay_out_scroll_bar_y();
		_lay_out_scroll_bar_x();
	}

	menu_visibility_mode_e menu_control_panel_i::_get_scroll_bar_x_visibility_mode() const
	{
		return _scroll_bar_x_visibility_mode;
	}

	void_c menu_control_panel_i::_set_scroll_bar_x_visibility_mode( menu_visibility_mode_e value )
	{
		_scroll_bar_x_visibility_mode = value;
		if ( value == menu_visibility_mode_e_always || value == menu_visibility_mode_e_automatic )
		{
			if ( _scroll_bar_x == nullptr )
			{
				_scroll_bar_x = new menu_control_scroll_bar_x_c();
				_scroll_bar_x->set_name( string8_c( "scroll_bar_x", core_list_mode_e_static ) );
				_scroll_bar_x->on_value_changed.subscribe( this, &menu_control_panel_i::_handle_scroll_bar_on_value_changed );
				_scroll_bar_x->on_preferred_thickness_changed.subscribe( this, &menu_control_panel_i::_handle_scroll_bar_on_preferred_thickness_changed );
				_lay_out_scroll_bar_x();
				add_daughter_control( _scroll_bar_x );
			}
		}
		else
		{
			if ( _scroll_bar_x )
			{
				remove_daughter_control( _scroll_bar_x ); // this will remove a reference from _scroll_bar_x, which will (probably) delete it.
				_scroll_bar_x = nullptr;
			}
		}
		if ( _scroll_bar_x )
		{
			_scroll_bar_x->set_value_range_and_page_size( 0.0, _client->get_content_bounds().get_width(), _client->get_local_box().get_width() );
			_scroll_bar_x->update_visibility( value );
		}
	}

	void_c menu_control_panel_i::_lay_out_scroll_bar_x()
	{
		if ( _scroll_bar_x )
		{
			_scroll_bar_x->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( _client_margins.minimum.a, _scroll_bar_x->get_preferred_thickness(), _client_margins.maximum.a, _client_margins.maximum.b ) );
		}
	}

	menu_visibility_mode_e menu_control_panel_i::_get_scroll_bar_y_visibility_mode() const
	{
		return _scroll_bar_y_visibility_mode;
	}

	void_c menu_control_panel_i::_set_scroll_bar_y_visibility_mode( menu_visibility_mode_e value )
	{
		_scroll_bar_y_visibility_mode = value;
		if ( value == menu_visibility_mode_e_always || value == menu_visibility_mode_e_automatic )
		{
			if ( _scroll_bar_y == nullptr )
			{
				_scroll_bar_y = new menu_control_scroll_bar_y_c();
				_scroll_bar_y->set_name( string8_c( "scroll_bar_y", core_list_mode_e_static ) );
				_scroll_bar_y->on_value_changed_preview.subscribe( this, &menu_control_panel_i::_handle_scroll_bar_on_value_changed );
				_scroll_bar_y->on_preferred_thickness_changed.subscribe( this, &menu_control_panel_i::_handle_scroll_bar_on_preferred_thickness_changed );
				add_daughter_control( _scroll_bar_y );
				_lay_out_scroll_bar_y();
			}
		}
		else
		{
			if ( _scroll_bar_y )
			{
				remove_daughter_control( _scroll_bar_y ); // this will remove a reference from _scroll_bar_y, which will (probably) delete it.
				_scroll_bar_y = nullptr;
			}
		}
		if ( _scroll_bar_y )
		{
			_scroll_bar_y->set_value_range_and_page_size( 0.0, _client->get_content_bounds().get_height(), _client->get_local_box().get_height() );
			_scroll_bar_y->update_visibility( value );
		}
	}

	void_c menu_control_panel_i::_lay_out_scroll_bar_y()
	{
		if ( _scroll_bar_y )
		{
			box32x2_c const & client_margins = _get_client_margins();
			_scroll_bar_y->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( _scroll_bar_y->get_preferred_thickness(), client_margins.minimum.b, client_margins.maximum.a, client_margins.maximum.b ) );
		}
	}

	void_c menu_control_panel_i::_update_transform_and_layout()
	{
		menu_control_c::_update_transform_and_layout();
		vector32x2_c client_size;
		client_size.a = _local_box.get_width() - _client_margins.maximum.a - _client_margins.minimum.a;
		if ( _scroll_bar_y && _scroll_bar_y->get_is_showed() )
		{
			client_size.a -= _scroll_bar_y->get_preferred_thickness() + 8.0f;
		}
		client_size.b = _local_box.get_height() - _client_margins.maximum.b - _client_margins.minimum.b;
		if ( _scroll_bar_x && _scroll_bar_x->get_is_showed() )
		{
			client_size.b -= _scroll_bar_x->get_preferred_thickness() + 8.0f;
		}
		_client->set_layout_simple( _local_box.minimum + _client_margins.minimum, box32x2_c( 0.0f, 0.0f, client_size.a, client_size.b ) );
	}

	void_c menu_control_panel_i::_update_scroll_bar_visibility()
	{
		boolean_c changed = false;
		box32x2_c content_bounds = _client->get_content_bounds();
		if ( _scroll_bar_x )
		{
			_scroll_bar_x->set_value_range_and_page_size( content_bounds.minimum.a, content_bounds.maximum.b, _local_box.get_width() );
			_scroll_bar_x->set_line_size( 60.0f );
			changed |= _scroll_bar_x->update_visibility( _scroll_bar_x_visibility_mode );
		}
		if ( _scroll_bar_y )
		{
			_scroll_bar_y->set_value_range_and_page_size( content_bounds.minimum.b, content_bounds.maximum.b, _local_box.get_height() );
			_scroll_bar_y->set_line_size( 60.0f );
			changed |= _scroll_bar_y->update_visibility( _scroll_bar_y_visibility_mode );
		}
		if ( changed )
		{
			_update_transform_and_layout();
		}
	}

	void_c menu_control_panel_i::_handle_scroll_bar_on_value_changed( menu_control_scroll_bar_i * scroll_bar )
	{
		if ( scroll_bar == _scroll_bar_x )
		{
			_client->set_content_offset_horizontal( static_cast< float32_c >( -_scroll_bar_x->get_value() ) );
		}
		else if ( scroll_bar == _scroll_bar_y )
		{
			_client->set_content_offset_vertical( static_cast< float32_c >( -_scroll_bar_y->get_value() ) );
		}
	}

	void_c menu_control_panel_i::_handle_scroll_bar_on_preferred_thickness_changed( menu_control_scroll_bar_i * scroll_bar )
	{
		if ( scroll_bar == _scroll_bar_x )
		{
			_lay_out_scroll_bar_x();
		}
		else if ( scroll_bar == _scroll_bar_y )
		{
			_lay_out_scroll_bar_y();
		}
	}

	menu_control_panel_i::menu_control_panel_i()
		: menu_control_c()
		, _frame_element()
		, _border_frame_element()
		, _client_margins( 0.0f, 0.0f, 0.0f, 0.0f )
		, _client( nullptr )
		, _scroll_bar_x_visibility_mode( menu_visibility_mode_e_never )
		, _scroll_bar_x( nullptr )
		, _scroll_bar_y_visibility_mode( menu_visibility_mode_e_never )
		, _scroll_bar_y( nullptr )
	{
		_frame_element.set_name( string8_c( "frame", core_list_mode_e_static ) );
		_add_daughter_element( &_frame_element );

		_border_frame_element.set_name( string8_c( "border_frame", core_list_mode_e_static ) );
		_border_frame_element.set_is_overlay( true );
		_add_daughter_element( &_border_frame_element );

		_client = new menu_control_client_c();
		_client->set_name( string8_c( "client", core_list_mode_e_static ) );
		_client->set_layout_simple( _local_box.minimum + _client_margins.minimum, box32x2_c( 0.0f, 0.0f, _local_box.get_width() - _client_margins.maximum.a - _client_margins.maximum.b, _local_box.get_height() - _client_margins.maximum.b - _client_margins.minimum.b ) );
		add_daughter_control( _client );

		set_style_map_key( string8_c( "e_panel", core_list_mode_e_static ) );
		set_color_theme( engine.get_menu_style_manager()->get_internal_window_color_theme() );
	}

	void_c menu_control_panel_i::update_animations( float32_c time_step )
	{
		_frame_element.set_is_selected( _is_mouse_focused || _is_text_focused );
		_update_scroll_bar_visibility();
		menu_control_c::update_animations( time_step );
	}

}
