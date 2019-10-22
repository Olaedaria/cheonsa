#include "cheonsa_menu_window.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	void_c menu_window_c::_on_mouse_focus_lost()
	{
		_grabbed_element = grabbed_element_e_none;
		menu_control_c::_on_mouse_focus_lost();
	}

	void_c menu_window_c::_on_input( input_event_c * input_event )
	{
		if ( input_event->type == input_event_c::type_e_mouse_key_pressed )
		{
			if ( input_event->mouse_key == input_mouse_key_e_left )
			{
				vector32x2_c menu_mouse_position_local = transform_point_from_global_to_local( input_event->mouse_position );

				// check for intersection with horizontal edge.
				if ( menu_mouse_position_local.a >= _local_box.minimum.a && menu_mouse_position_local.a <= _local_box.minimum.a + _local_edge_size )
				{
					_grabbed_element = static_cast< grabbed_element_e >( _grabbed_element | grabbed_element_e_edge_left );
					_grabbed_point_local.a = menu_mouse_position_local.a;
				}
				else if ( menu_mouse_position_local.a <= _local_box.maximum.a && menu_mouse_position_local.a >= _local_box.maximum.a - _local_edge_size )
				{
					_grabbed_element = static_cast< grabbed_element_e >( _grabbed_element | grabbed_element_e_edge_right );
					_grabbed_point_local.a = _local_box.maximum.a - menu_mouse_position_local.a;
				}

				// check for intersection with vertical edge.
				if ( menu_mouse_position_local.b >= _local_box.minimum.b && menu_mouse_position_local.b <= _local_box.minimum.b + _local_edge_size )
				{
					_grabbed_element = static_cast< grabbed_element_e >( _grabbed_element | grabbed_element_e_edge_top );
					_grabbed_point_local.b = menu_mouse_position_local.b;
				}
				else if ( menu_mouse_position_local.b <= _local_box.maximum.b && menu_mouse_position_local.b >= _local_box.maximum.b - _local_edge_size )
				{
					_grabbed_element = static_cast< grabbed_element_e >( _grabbed_element | grabbed_element_e_edge_bottom );
					_grabbed_point_local.b = _local_box.maximum.b - menu_mouse_position_local.b;
				}
				
				if ( _grabbed_element && !_user_can_resize )
				{
					if ( _user_can_move )
					{
						_grabbed_element = grabbed_element_e_title_bar;
						_grabbed_point_local.a = menu_mouse_position_local.a;
						_grabbed_point_local.b = menu_mouse_position_local.b;
					}
					else
					{
						_grabbed_element = grabbed_element_e_none;
					}
				}

				if ( _user_can_move )
				{
					// check for intersection with title bar.
					if ( _grabbed_element == grabbed_element_e_none && menu_mouse_position_local.a > _local_box.minimum.a + _local_edge_size && menu_mouse_position_local.a < _local_box.maximum.a - _local_edge_size && menu_mouse_position_local.b > _local_box.minimum.a + _local_edge_size && menu_mouse_position_local.b <= _local_box.minimum.b + _local_edge_size + _local_title_bar_size )
					{
						_grabbed_element = grabbed_element_e_title_bar;
						_grabbed_point_local.a = menu_mouse_position_local.a;
						_grabbed_point_local.b = menu_mouse_position_local.b;
					}
				}
			}
		}
		else if ( input_event->type == input_event_c::type_e_mouse_key_released )
		{
			if ( input_event->mouse_key == input_mouse_key_e_left )
			{
				_grabbed_element = grabbed_element_e_none;
				constrain_transform();
			}
		}
		else if ( input_event->type == input_event_c::type_e_mouse_move && _grabbed_element != grabbed_element_e_none )
		{
			//vector32x2_c menu_mouse_position_local = transform_point_from_global_to_local( input_event->menu_mouse_position );
			if ( _grabbed_element == grabbed_element_e_title_bar )
			{
				_local_origin = input_event->menu_mouse_position - _grabbed_point_local;
				update_transform_and_layout();
			}
			else
			{
				if ( _grabbed_element & grabbed_element_e_edge_left )
				{
					float32_c left = input_event->menu_mouse_position.a - _grabbed_point_local.a;
					float32_c right = _local_origin.a + _local_box.maximum.a;
					float32_c width = right - left;
					if ( width < _local_size_minimum.a )
					{
						width = _local_size_minimum.a;
					}
					if ( _local_size_maximum.a > 0.0f && width > _local_size_maximum.a )
					{
						width = _local_size_maximum.a;
					}
					_local_box.maximum.a = width;
					_local_origin.a = right - width;
				}
				else if ( _grabbed_element & grabbed_element_e_edge_right )
				{
					float32_c width = input_event->menu_mouse_position.a + _grabbed_point_local.a - _local_origin.a;
					if ( width < _local_size_minimum.a )
					{
						width = _local_size_minimum.a;
					}
					if ( _local_size_maximum.a > 0.0f && width > _local_size_maximum.a )
					{
						width = _local_size_maximum.a;
					}
					_local_box.maximum.a = width;
				}
				if ( _grabbed_element & grabbed_element_e_edge_top )
				{
					float32_c top = input_event->menu_mouse_position.b - _grabbed_point_local.b;
					float32_c bottom = _local_origin.b + _local_box.maximum.b;
					float32_c height = bottom - top;
					if ( height < _local_size_minimum.b )
					{
						height = _local_size_minimum.b;
					}
					if ( _local_size_maximum.b > 0.0f && height > _local_size_maximum.b )
					{
						height = _local_size_maximum.b;
					}
					_local_box.maximum.b = height;
					_local_origin.b = bottom - height;
				}
				else if ( _grabbed_element & grabbed_element_e_edge_bottom )
				{
					float32_c height = input_event->menu_mouse_position.b + _grabbed_point_local.b - _local_origin.b;
					if ( height < _local_size_minimum.b )
					{
						height = _local_size_minimum.b;
					}
					if ( _local_size_maximum.b > 0.0f && height > _local_size_maximum.b )
					{
						height = _local_size_maximum.b;
					}
					_local_box.maximum.b = height;
				}
				update_transform_and_layout();
			}
		}
	}

	menu_window_c::menu_window_c()
		: menu_control_c()
		, _element_frame()
		, _element_text()
		//, _close_button( nullptr )
		, _client_panel( nullptr )
		, _user_can_resize( false )
		, _local_edge_size( 8.0f )
		, _local_size_minimum( 100.0f, 100.0f )
		, _local_size_maximum( 1000.0f, 1000.0f )
		, _user_can_move( false )
		, _local_title_bar_size( 30.0f )
		, _grabbed_element( grabbed_element_e_none )
		, _grabbed_point_local( 0.0f, 0.0f )
	{
		//_select_mode = menu_select_mode_e_mouse;

		_element_frame.set_name( string8_c( mode_e_static, "frame" ) );
		_element_frame.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_add_element( &_element_frame );

		_element_text.set_name( string8_c( mode_e_static, "text" ) );
		_element_text.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( _local_edge_size, _local_edge_size, _local_edge_size, _local_title_bar_size ) );
		_add_element( &_element_text );

		_client_panel = new menu_control_panel_c();
		_client_panel->set_name( string8_c( mode_e_static, "client_panel" ) );
		_client_panel->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( _local_edge_size, _local_edge_size + _local_title_bar_size, _local_edge_size, _local_edge_size ) );
		_add_control( _client_panel );

		//_close_button = new menu_control_button_c();
		//_close_button->set_name( string8_c( mode_e_static, "close_button" ) );
		//_close_button->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right, box32x2_c( 75.0f, 0.0f, 0.0f, 24.0f ) );
		//_add_control( _close_button );

		set_style_map_key( string8_c( mode_e_static, "e_window" ) );
	}

	void_c menu_window_c::update_transform_and_layout()
	{
		menu_control_c::update_transform_and_layout();
	}

	void_c menu_window_c::constrain_transform()
	{
		// constrain window title bar to fit in user interface.
		if ( _user_interface != nullptr )
		{
			float32_c cap_bottom = _user_interface->get_local_box().maximum.b - global_engine_instance.environment.get_window_edge_thickness() - _local_title_bar_size;
			if ( _local_origin.b > cap_bottom )
			{
				_local_origin.b = cap_bottom;
			}

			float32_c cap_top = _user_interface->get_local_box().minimum.b + global_engine_instance.environment.get_window_edge_thickness() + global_engine_instance.environment.get_window_title_bar_thickness();
			if ( _local_origin.b < cap_top )
			{
				_local_origin.b = cap_top;
			}

			float32_c cap_left = _user_interface->get_local_box().minimum.a + global_engine_instance.environment.get_window_edge_thickness() - _local_box.maximum.a + 60.0f;
			if ( _local_origin.a < cap_left )
			{
				_local_origin.a = cap_left;
			}

			float32_c cap_right = _user_interface->get_local_box().maximum.a - global_engine_instance.environment.get_window_edge_thickness() - 60.0f;
			if ( _local_origin.a > cap_right )
			{
				_local_origin.a = cap_right;
			}
			update_transform_and_layout();
		}
	}

	string16_c menu_window_c::get_plain_text() const
	{
		return _element_text.get_plain_text();
	}

	void_c menu_window_c::set_plain_text( string16_c const & value )
	{
		_element_text.set_plain_text( value );
	}

	boolean_c menu_window_c::get_user_can_move() const
	{
		return _user_can_move;
	}

	void_c menu_window_c::set_user_can_move( boolean_c value )
	{
		_user_can_move = true;
	}

	boolean_c menu_window_c::get_user_can_resize() const
	{
		return _user_can_resize;
	}

	void_c menu_window_c::set_user_can_resize( boolean_c value )
	{
		_user_can_resize = value;
	}

	float32_c menu_window_c::get_local_title_bar_size() const
	{
		return _local_title_bar_size;
	}

	void_c menu_window_c::set_local_title_bar_size( float32_c value )
	{
		_local_title_bar_size = value;
		_client_panel->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_right | menu_anchor_e_top | menu_anchor_e_bottom, box32x2_c( _local_edge_size, _local_edge_size + _local_title_bar_size, _local_edge_size, _local_edge_size ) );
	}

	float32_c menu_window_c::get_local_edge_size() const
	{
		return _local_edge_size;
	}

	void_c menu_window_c::set_local_edge_size( float32_c value )
	{
		_local_edge_size = value;
		_client_panel->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_right | menu_anchor_e_top | menu_anchor_e_bottom, box32x2_c( _local_edge_size, _local_edge_size + _local_title_bar_size, _local_edge_size, _local_edge_size ) );
	}

	vector32x2_c const & menu_window_c::get_local_size_minimum() const
	{
		return _local_size_minimum;
	}

	void_c menu_window_c::set_local_size_minimum( vector32x2_c const & value )
	{
		_local_size_minimum = value;
	}

	vector32x2_c const & menu_window_c::get_local_size_maximum() const
	{
		return _local_size_maximum;
	}

	void_c menu_window_c::set_local_size_maximum( vector32x2_c const & value )
	{
		_local_size_maximum = value;
	}

	vector32x2_c const & menu_window_c::get_position() const
	{
		return _local_box.minimum;
	}

	void_c menu_window_c::set_position( vector32x2_c const & value )
	{
		_local_origin = value;
		update_transform_and_layout();
	}

	vector32x2_c menu_window_c::get_size() const
	{
		return _local_box.maximum;
	}

	void_c menu_window_c::set_size( vector32x2_c const & value )
	{
		_local_box.maximum = value;
		update_transform_and_layout();
	}

	void_c menu_window_c::add_control( menu_control_c * control )
	{
		_client_panel->add_control( control );
	}

	void_c menu_window_c::remove_control( menu_control_c * control )
	{
		_client_panel->remove_control( control );
	}

}
