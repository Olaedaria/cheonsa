#include "cheonsa_menu_control_window.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	void_c menu_control_window_c::_apply_client_margins()
	{
		_element_text.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( _edge_size, _edge_size, _edge_size, _top_bar_size ) );
		_set_client_margins( box32x2_c( _edge_size, _edge_size + _top_bar_size, _edge_size, _edge_size + _bottom_bar_size ) );
	}

	void_c menu_control_window_c::_on_is_deep_text_focused_changed()
	{
		if ( _is_deep_text_focused != 0 )
		{
			if ( _user_interface != nullptr )
			{
				_user_interface->bring_control_to_front( this );
			}
		}
		on_is_deep_text_focused_changed.invoke( menu_event_information_c( this, nullptr ) );
	}

	void_c menu_control_window_c::_on_is_mouse_focused_changed()
	{
		if ( _is_mouse_focused == false )
		{
			_grabbed_element = grabbed_element_e_none;
		}
		on_is_mouse_focused_changed.invoke( menu_event_information_c( this, nullptr ) );
	}

	void_c menu_control_window_c::_on_input( input_event_c * input_event )
	{
		vector32x2_c local_mouse_position = transform_global_point_to_local_point( input_event->menu_global_mouse_position );

		if ( input_event->type == input_event_c::type_e_mouse_key_pressed )
		{
			if ( input_event->mouse_key == input_mouse_key_e_left )
			{
				if ( _user_can_resize )
				{
					// check for intersection with horizontal edge.
					if ( local_mouse_position.a >= _local_box.minimum.a && local_mouse_position.a <= _local_box.minimum.a + _edge_size )
					{
						_grabbed_element = static_cast< grabbed_element_e >( _grabbed_element | grabbed_element_e_edge_left );
						_grabbed_point_local.a = local_mouse_position.a;
					}
					else if ( local_mouse_position.a <= _local_box.maximum.a && local_mouse_position.a >= _local_box.maximum.a - _edge_size )
					{
						_grabbed_element = static_cast< grabbed_element_e >( _grabbed_element | grabbed_element_e_edge_right );
						_grabbed_point_local.a = _local_box.maximum.a - local_mouse_position.a;
					}

					// check for intersection with vertical edge.
					if ( local_mouse_position.b >= _local_box.minimum.b && local_mouse_position.b <= _local_box.minimum.b + _edge_size )
					{
						_grabbed_element = static_cast< grabbed_element_e >( _grabbed_element | grabbed_element_e_edge_top );
						_grabbed_point_local.b = local_mouse_position.b;
					}
					else if ( local_mouse_position.b <= _local_box.maximum.b && local_mouse_position.b >= _local_box.maximum.b - _edge_size )
					{
						_grabbed_element = static_cast< grabbed_element_e >( _grabbed_element | grabbed_element_e_edge_bottom );
						_grabbed_point_local.b = _local_box.maximum.b - local_mouse_position.b;
					}
				}
				
				//if ( _grabbed_element && !_user_can_resize )
				//{
				//	if ( _user_can_move )
				//	{
				//		_grabbed_element = grabbed_element_e_title_bar;
				//		_grabbed_point_local.a = local_mouse_position.a;
				//		_grabbed_point_local.b = local_mouse_position.b;
				//	}
				//	else
				//	{
				//		_grabbed_element = grabbed_element_e_none;
				//	}
				//}

				if ( _user_can_move )
				{
					// check for intersection with title bar.
					if ( _grabbed_element == grabbed_element_e_none && local_mouse_position.a > _local_box.minimum.a + _edge_size && local_mouse_position.a < _local_box.maximum.a - _edge_size && local_mouse_position.b > _local_box.minimum.a + _edge_size && local_mouse_position.b <= _local_box.minimum.b + _edge_size + _top_bar_size )
					{
						_grabbed_element = grabbed_element_e_title_bar;
						_grabbed_point_local.a = local_mouse_position.a;
						_grabbed_point_local.b = local_mouse_position.b;
					}
				}
			}
		}
		else if ( input_event->type == input_event_c::type_e_mouse_key_released )
		{
			if ( input_event->mouse_key == input_mouse_key_e_left )
			{
				_grabbed_element = grabbed_element_e_none;
				_update_transform_and_layout();
			}
		}
		else if ( input_event->type == input_event_c::type_e_mouse_move && _grabbed_element != grabbed_element_e_none )
		{
			//vector32x2_c menu_mouse_position_local = transform_point_from_global_to_local( input_event->menu_mouse_position );
			if ( _grabbed_element == grabbed_element_e_title_bar )
			{
				_local_origin = input_event->menu_global_mouse_position - _grabbed_point_local;
				_update_transform_and_layout();
			}
			else
			{
				if ( _grabbed_element & grabbed_element_e_edge_left )
				{
					float32_c left = input_event->menu_global_mouse_position.a - _grabbed_point_local.a;
					float32_c right = _local_origin.a + _local_box.maximum.a;
					float32_c width = right - left;
					if ( width < _minimum_size.a )
					{
						width = _minimum_size.a;
					}
					if ( _maximum_size.a > 0.0f && width > _maximum_size.a )
					{
						width = _maximum_size.a;
					}
					_local_box.maximum.a = width;
					_local_origin.a = right - width;
				}
				else if ( _grabbed_element & grabbed_element_e_edge_right )
				{
					float32_c width = input_event->menu_global_mouse_position.a + _grabbed_point_local.a - _local_origin.a;
					if ( width < _minimum_size.a )
					{
						width = _minimum_size.a;
					}
					if ( _maximum_size.a > 0.0f && width > _maximum_size.a )
					{
						width = _maximum_size.a;
					}
					_local_box.maximum.a = width;
				}
				if ( _grabbed_element & grabbed_element_e_edge_top )
				{
					float32_c top = input_event->menu_global_mouse_position.b - _grabbed_point_local.b;
					float32_c bottom = _local_origin.b + _local_box.maximum.b;
					float32_c height = bottom - top;
					if ( height < _minimum_size.b )
					{
						height = _minimum_size.b;
					}
					if ( _maximum_size.b > 0.0f && height > _maximum_size.b )
					{
						height = _maximum_size.b;
					}
					_local_box.maximum.b = height;
					_local_origin.b = bottom - height;
				}
				else if ( _grabbed_element & grabbed_element_e_edge_bottom )
				{
					float32_c height = input_event->menu_global_mouse_position.b + _grabbed_point_local.b - _local_origin.b;
					if ( height < _minimum_size.b )
					{
						height = _minimum_size.b;
					}
					if ( _maximum_size.b > 0.0f && height > _maximum_size.b )
					{
						height = _maximum_size.b;
					}
					_local_box.maximum.b = height;
				}
				_update_transform_and_layout();
			}
		}
	}

	void_c menu_control_window_c::_update_transform_and_layout()
	{
		if ( _grabbed_element == grabbed_element_e_none )
		{
			constrain_transform();
		}
		menu_control_c::_update_transform_and_layout();
	}

	menu_control_window_c::menu_control_window_c( string8_c const & name )
		: menu_control_panel_i( name )
		, _element_text( string8_c( core_list_mode_e_static, "text" ) )
		, _edge_size( 8.0f )
		, _top_bar_size( 30.0f )
		, _bottom_bar_size( 0.0f )
		, _user_can_move( true )
		, _user_can_resize( true )
		, _minimum_size( 100.0f, 100.0f )
		, _maximum_size( 1000.0f, 1000.0f )
		, _grabbed_element( grabbed_element_e_none )
		, _grabbed_point_local( 0.0f, 0.0f )
		, _modal_screen( nullptr )
	{
		set_size( vector32x2_c( 500, 500 ) );

		_element_text.set_shared_color_class( menu_shared_color_class_e_window );
		_element_text.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( _edge_size, _edge_size, _edge_size, _top_bar_size ) );
		_add_element( &_element_text );

		_apply_client_margins();

		set_style_map_key( string8_c( core_list_mode_e_static, "e_window" ) );
	}

	void_c menu_control_window_c::update_animations( float32_c time_step )
	{
		float32_c transition_step = engine.get_menu_style_manager()->get_shared_transition_speed() * time_step;
		_is_showed_weight = ops::math_saturate( _is_showed_weight + ( _is_showed ? transition_step : -transition_step ) );

		boolean_c is_selected = is_ascendant_of( get_user_interface_root()->get_mouse_overed() ) || get_is_deep_text_focused();
		for ( sint32_c i = 0; i < _element_list.get_length(); i++ )
		{
			menu_element_c * element = _element_list[ i ];
			element->set_is_enabled( _is_enabled );
			element->set_is_selected( is_selected );
			element->set_is_pressed( _is_pressed && _is_mouse_overed );
			element->update_animations( time_step );
		}

		for ( sint32_c i = 0; i < _control_list.get_length(); i++ )
		{
			menu_control_c * control = _control_list[ i ];
			assert( control->get_index() == i );
			control->update_animations( time_step );
			if ( control->get_wants_to_be_deleted() && control->get_is_showed_weight() <= 0.0f )
			{
				_take_control( i );
				delete control;
				i--;
			}
		}
	}

	void_c menu_control_window_c::constrain_transform()
	{
		// constrain window title bar to stay in bounds of user interface.
		if ( _user_interface != nullptr )
		{
			float32_c cap_bottom = _user_interface->get_local_box().maximum.b - engine.get_window_manager()->get_window_edge_thickness() - _top_bar_size;
			if ( _local_origin.b > cap_bottom )
			{
				_local_origin.b = cap_bottom;
			}

			float32_c cap_top = _user_interface->get_local_box().minimum.b + engine.get_window_manager()->get_window_edge_thickness() + engine.get_window_manager()->get_window_edge_thickness();
			if ( _local_origin.b < cap_top )
			{
				_local_origin.b = cap_top;
			}

			float32_c cap_left = _user_interface->get_local_box().minimum.a + engine.get_window_manager()->get_window_edge_thickness() - _local_box.maximum.a + 60.0f;
			if ( _local_origin.a < cap_left )
			{
				_local_origin.a = cap_left;
			}

			float32_c cap_right = _user_interface->get_local_box().maximum.a - engine.get_window_manager()->get_window_edge_thickness() - 60.0f;
			if ( _local_origin.a > cap_right )
			{
				_local_origin.a = cap_right;
			}
		}
	}

	string16_c menu_control_window_c::get_title_text_value() const
	{
		return _element_text.get_plain_text_value();
	}

	void_c menu_control_window_c::set_title_text_value( string8_c const & plain_text )
	{
		_element_text.set_plain_text_value( plain_text );
	}

	void_c menu_control_window_c::set_title_text_value( string16_c const & plain_text )
	{
		_element_text.set_plain_text_value( plain_text );
	}

	void_c menu_control_window_c::clear_title_text_value()
	{
		_element_text.clear_text_value();
	}

	boolean_c menu_control_window_c::get_user_can_move() const
	{
		return _user_can_move;
	}

	void_c menu_control_window_c::set_user_can_move( boolean_c value )
	{
		_user_can_move = value;
	}

	boolean_c menu_control_window_c::get_user_can_resize() const
	{
		return _user_can_resize;
	}

	void_c menu_control_window_c::set_user_can_resize( boolean_c value )
	{
		_user_can_resize = value;
	}

	float32_c menu_control_window_c::get_top_bar_size() const
	{
		return _top_bar_size;
	}

	void_c menu_control_window_c::set_top_bar_size( float32_c value )
	{
		_top_bar_size = value;
		_apply_client_margins();
	}

	float32_c menu_control_window_c::get_bottom_bar_size() const
	{
		return _bottom_bar_size;
	}

	void_c menu_control_window_c::set_bottom_bar_size( float32_c value )
	{
		_bottom_bar_size = value;
		_apply_client_margins();
	}

	float32_c menu_control_window_c::get_edge_size() const
	{
		return _edge_size;
	}

	void_c menu_control_window_c::set_edge_size( float32_c value )
	{
		_edge_size = value;
		_apply_client_margins();
	}

	vector32x2_c const & menu_control_window_c::get_minimum_size() const
	{
		return _minimum_size;
	}

	void_c menu_control_window_c::set_minimum_size( vector32x2_c const & value )
	{
		_minimum_size = value;
	}

	vector32x2_c const & menu_control_window_c::get_maximum_size() const
	{
		return _maximum_size;
	}

	void_c menu_control_window_c::set_maximum_size( vector32x2_c const & value )
	{
		_maximum_size = value;
	}

	vector32x2_c const & menu_control_window_c::get_position() const
	{
		return _local_box.minimum;
	}

	void_c menu_control_window_c::set_position( vector32x2_c const & value )
	{
		_local_origin.a = ops::math_round_down( value.a );
		_local_origin.b = ops::math_round_down( value.b );
		_update_transform_and_layout();
	}

	vector32x2_c menu_control_window_c::get_size() const
	{
		return _local_box.maximum;
	}

	void_c menu_control_window_c::set_size( vector32x2_c const & value )
	{
		_local_box.maximum.a = ops::math_round_down( value.a );
		_local_box.maximum.b = ops::math_round_down( value.b );
		_update_transform_and_layout();
	}

	menu_visibility_mode_e menu_control_window_c::get_horizontal_scroll_bar_visibility_mode() const
	{
		return _get_horizontal_scroll_bar_visibility_mode();
	}

	void_c menu_control_window_c::set_horizontal_scroll_bar_visibility_mode( menu_visibility_mode_e value )
	{
		_set_horizontal_scroll_bar_visibility_mode( value );
	}

	menu_visibility_mode_e menu_control_window_c::get_vertical_scroll_bar_visibility_mode() const
	{
		return _get_vertical_scroll_bar_visibility_mode();
	}

	void_c menu_control_window_c::set_vertical_scroll_bar_visibility_mode( menu_visibility_mode_e value )
	{
		_set_vertical_scroll_bar_visibility_mode( value );
	}

	sint32_c menu_control_window_c::get_controls_in_client_count() const
	{
		return _get_controls_in_client_count();
	}

	menu_control_c const * menu_control_window_c::get_control_in_client( sint32_c control_index ) const
	{
		return _get_control_in_client( control_index );
	}

	menu_control_c * menu_control_window_c::get_control_in_client( sint32_c control_index )
	{
		return _get_control_in_client( control_index );
	}

	sint32_c menu_control_window_c::give_control_to_client( menu_control_c * control, sint32_c index )
	{
		return _give_control_to_client( control, index );
	}

	menu_control_c * menu_control_window_c::take_control_from_client( sint32_c control_index )
	{
		return _take_control_from_client( control_index );
	}

	void_c menu_control_window_c::remove_and_delete_all_controls_from_client()
	{
		_remove_and_delete_all_controls_from_client();
	}

	void_c menu_control_window_c::center()
	{
		user_interface_c * user_interface = get_user_interface_root();
		_local_origin.a = ops::math_round_down( user_interface->get_local_box().get_width() * 0.5f - ( _local_box.get_width() * 0.5f ) );
		_local_origin.b = ops::math_round_down( user_interface->get_local_box().get_height() * 0.5f - ( _local_box.get_height() * 0.5f ) );
		_update_transform_and_layout();
	}

	void_c menu_control_window_c::show_dialog( menu_control_c * modal_screen )
	{
		assert( _modal_screen == nullptr );
		_dialog_result = menu_dialog_result_e_none;
		_modal_screen = modal_screen;
		set_is_showed( true );
		bring_to_front();
		give_text_focus();
	}

	void_c menu_control_window_c::hide_dialog()
	{
		set_is_showed( false );
		if ( _modal_screen != nullptr )
		{
			_modal_screen->set_is_showed( false, true );
			_modal_screen = nullptr;
		}
	}

	menu_dialog_result_e menu_control_window_c::get_dialog_result() const
	{
		return _dialog_result;
	}

}
