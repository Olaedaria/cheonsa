#include "cheonsa_menu_control_scroll_i.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{


	void_c menu_control_scroll_i::_on_mouse_focus_gained()
	{
		_rail_element.set_is_selected( true );
		_grip_element.set_is_selected( true );
		menu_control_c::_on_mouse_focus_gained();
	}

	void_c menu_control_scroll_i::_on_mouse_focus_lost()
	{
		_rail_element.set_is_selected( false );
		_grip_element.set_is_selected( false );
		menu_control_c::_on_mouse_focus_lost();
		// cancel drag operation if one was under way.
		if ( _mouse_grab )
		{
			_mouse_grab = false;
			_mouse_grab_offset = 0.0f;
			_value = _value_original;
			_value_original = 0.0;
			_scrub_input = 0.0;
			_grip_element.set_is_pressed( false );
			on_value_changed_preview.invoke( this );
		}
	}

	void_c menu_control_scroll_i::_on_input( input_event_c * input_event )
	{
		// we input response from layout here.
		// current layout state informs how we interpret the input event to drive changes to _value.
		// but layout state of the grip is driven by the _value (for scroll bar) or _scrub_input (for scrub wheel).

		if ( input_event->type == input_event_c::type_e_mouse_key_pressed )
		{
			if ( input_event->mouse_key == input_mouse_key_e_left )
			{
				vector32x2_c local_mouse_position = transform_global_point_to_local_point( input_event->menu_global_mouse_position );
				if ( ops::intersect_rectangle_vs_point( _grip_element.get_local_box(), local_mouse_position ) )
				{
					_mouse_grab = true;
					_value_original = _value;
					if ( _orientation == orientation_e_horizontal )
					{
						_mouse_grab_offset = local_mouse_position.a - _grip_element.get_local_box().minimum.a;
					}
					else
					{
						_mouse_grab_offset = local_mouse_position.b - _grip_element.get_local_box().minimum.b;
					}
					_grip_element.set_is_pressed( true );
				}
			}
		}
		else if ( input_event->type == input_event_c::type_e_mouse_key_released )
		{
			if ( input_event->mouse_key == input_mouse_key_e_left )
			{
				_mouse_grab = false;
				_mouse_grab_offset = 0.0f;
				_scrub_input = 0.0;
				_grip_element.set_is_pressed( false );
				if ( _value_original != _value )
				{
					on_value_changed.invoke( this );
					_value_original = 0.0;
				}
			}
		}
		else if ( input_event->type == input_event_c::type_e_mouse_move )
		{
			if ( _mouse_grab )
			{
				vector32x2_c local_mouse_position = transform_global_point_to_local_point( input_event->menu_global_mouse_position );
				float32_c grip_effective_range = ( _orientation == orientation_e_vertical ? _local_box.get_height() : _local_box.get_width() ) - _dynamic_grip_length;
				float64_c grip_position = ops::math_clamp( ( _orientation == orientation_e_vertical ? local_mouse_position.b : local_mouse_position.a ) - _mouse_grab_offset - ( _orientation == orientation_e_vertical ? _local_box.minimum.b : _local_box.minimum.a ), 0.0f, grip_effective_range );
				float64_c grip_position_percent = grip_position / grip_effective_range;
				float64_c value_effective_range = _value_maximum - _value_minimum - _page_size;
				if ( value_effective_range < 0.0 )
				{
					value_effective_range = 0.0;
				}
				if ( _mode == mode_e_scroll_bar )
				{
					float64_c old_value = _value;
					_value = value_effective_range * grip_position_percent + _value_minimum;
					if ( _value_increment > 0.0 )
					{
						_value = ops::math_nearest_multiple( _value - _value_minimum, _value_increment ) + _value_minimum;
					}
					if ( _value != old_value )
					{
						on_value_changed_preview.invoke( this );
					}
				}
				else if ( _mode == mode_e_scrub_bar )
				{
					_scrub_input = grip_position_percent * 2.0 - 1.0;
				}
			}
		}
	}

	void_c menu_control_scroll_i::update_transform_and_layout()
	{
		/*
		// apply thickness to anchor measures in case it changed.
		if ( _layout_mode == menu_layout_mode_e_box_anchor )
		{
			if ( _orientation == orientation_e_horizontal )
			{
				if ( ( _local_anchor & menu_anchor_e_top ) != 0 && ( _local_anchor & menu_anchor_e_bottom ) == 0 )
				{
					_local_anchor_measures.maximum.b = _thickness;
				}
				else if ( ( _local_anchor & menu_anchor_e_top ) == 0 && ( _local_anchor & menu_anchor_e_bottom ) != 0 )
				{
					_local_anchor_measures.minimum.b = _thickness;
				}
			}
			else if ( _orientation == orientation_e_vertical )
			{
				if ( ( _local_anchor & menu_anchor_e_left ) != 0 && ( _local_anchor & menu_anchor_e_right ) == 0 )
				{
					_local_anchor_measures.maximum.a = _thickness;
				}
				else if ( ( _local_anchor & menu_anchor_e_left ) == 0 && ( _local_anchor & menu_anchor_e_right ) != 0 )
				{
					_local_anchor_measures.minimum.a = _thickness;
				}
			}
		}
		else if ( _layout_mode == menu_layout_mode_e_point_anchor || _layout_mode == menu_layout_mode_e_simple )
		{
			if ( _orientation == orientation_e_horizontal )
			{
				float32_c y = ( _local_box.minimum.b + _local_box.maximum.b ) * 0.5f;
				_local_box.minimum.b = y - ( _thickness * 0.5f );
				_local_box.maximum.b = y + ( _thickness * 0.5f );
			}
			else if ( _orientation == orientation_e_vertical )
			{
				float32_c x = ( _local_box.minimum.a + _local_box.maximum.a ) * 0.5f;
				_local_box.minimum.a = x - ( _thickness * 0.5f );
				_local_box.maximum.a = x + ( _thickness * 0.5f );
			}
		}
		*/

		// this will update layout, and detect changes to layout.
		menu_control_c::update_transform_and_layout();

		_rail_element.set_layout_simple( _local_box );

		float64_c value_effective_range = _value_maximum - _value_minimum - _page_size;

		if ( _mode == mode_e_scroll_bar )
		{
			if ( _fixed_grip_length >= 1.0 )
			{
				// fixed length grip.
				_dynamic_grip_length = _fixed_grip_length;
			}
			else
			{
				// variable length grip.
				float64_c grip_length_percent = ops::math_saturate( _page_size / ( _value_maximum - _value_minimum ) );
				_dynamic_grip_length = static_cast< float32_c >( ( _orientation == orientation_e_vertical ? _rail_element.get_local_box().get_height() : _rail_element.get_local_box().get_width() ) * grip_length_percent );
			}

			if ( value_effective_range > 0.0 )
			{
				float32_c grip_effective_range = ( _orientation == orientation_e_vertical ? _local_box.get_height() : _local_box.get_width() ) - _dynamic_grip_length;
				if ( grip_effective_range == 0.0f )
				{
					_dynamic_grip_position = 0.0f;
					_value = 0.0;
				}
				else
				{
					float64_c scroll_percent = ops::math_saturate( _value / value_effective_range );
					_dynamic_grip_position = grip_effective_range * static_cast< float32_c >( scroll_percent );
				}
			}
			else
			{
				_dynamic_grip_position = 0.0f;
				_value = 0.0;
			}
		}
		else if ( _mode == mode_e_scrub_bar )
		{
			_dynamic_grip_length = _fixed_grip_length >= 1.0 ? _fixed_grip_length : ( _orientation == orientation_e_vertical ? _local_box.get_height() : _local_box.get_width() ) / 11.0f;
			float32_c grip_effective_range = ( _orientation == orientation_e_vertical ? _local_box.get_height() : _local_box.get_width() ) - _dynamic_grip_length;
			_dynamic_grip_position = static_cast< float32_c >( ( ( grip_effective_range * 0.5 ) - ( _dynamic_grip_length * 0.5 ) ) * ( _scrub_input * 0.5 + 0.5 ) );
		}

		if ( _orientation == orientation_e_vertical )
		{
			_grip_element.set_layout_simple( box32x2_c( _local_box.minimum.a, _local_box.minimum.b + _dynamic_grip_position, _local_box.maximum.a, _local_box.minimum.b + _dynamic_grip_position + _dynamic_grip_length ) );
		}
		else
		{
			_grip_element.set_layout_simple( box32x2_c( _local_box.minimum.a + _dynamic_grip_position, _local_box.minimum.b, _local_box.minimum.a + _dynamic_grip_position + _dynamic_grip_length, _local_box.maximum.b ) );
		}
	}

	menu_control_scroll_i::menu_control_scroll_i( mode_e mode, orientation_e orientation )
		: menu_control_c()
		, _rail_element()
		, _grip_element()
		, _mode( mode )
		, _orientation( orientation )
		, _page_size( 0.0 )
		, _line_size( 0.0 )
		, _value_minimum( 0.0 )
		, _value_maximum( 0.0 )
		, _value_increment( 0.0 )
		, _value( 0.0 )
		, _value_smoothed( 0.0 )
		, _value_original( 0.0 )
		, _scrub_speed( 1.0 )
		, _scrub_speed_ease_in( 1.0 )
		, _scrub_input( 0.0 )
		, _scrub_value_accumulated( 0.0 )
		//, _thickness( 10.0f )
		, _fixed_grip_length( 0.0f )
		, _dynamic_grip_length( 0.0f )
		, _dynamic_grip_position( 0.0f )
		, _mouse_grab( false )
		, _mouse_grab_offset( 0.0f )
	{
		if ( _mode == mode_e_scrub_bar )
		{
			_fixed_grip_length = 10.0f;
		}

		_rail_element.set_name( string8_c( mode_e_static, "rail_frame" ) );
		_add_element( &_rail_element );

		_grip_element.set_name( string8_c( mode_e_static, "grip_frame" ) );
		_add_element( &_grip_element );

		set_style_map_key( string8_c( mode_e_static, "engine_scroll" ) );

		update_transform_and_layout();
	}

	/*
	void_c menu_control_scroll_i::load_properties( data_scribe_markup_c const * mark_up, data_scribe_markup_c::node_c const * node )
	{
		menu_control_c::load_properties( mark_up, node );

		_smooth_scroll_enable = true;

		data_scribe_markup_c::attribute_c const * attribute = nullptr;

		attribute = mark_up->find_daughter_attribute( node, "mode" );
		if ( attribute )
		{
			string16_c attribute_value = mark_up->get_string( attribute->value_start );
			_mode = mode_e_scroller;
			if ( attribute_value == "slider" )
			{
				_mode = mode_e_slider;
			}
		}

		attribute = mark_up->find_daughter_attribute( node, "slider_grip_length" );
		if ( attribute )
		{
			string16_c attribute_value = mark_up->get_string( attribute->value_start );
			convert_string_to_float32( attribute_value, _slider_grip_length );
		}

		attribute = mark_up->find_daughter_attribute( node, "value_minimum" );
		if ( attribute )
		{
			string16_c attribute_value = mark_up->get_string( attribute->value_start );
			convert_string_to_float32( attribute_value, _value_minimum );
		}

		attribute = mark_up->find_daughter_attribute( node, "value_maximum" );
		if ( attribute )
		{
			string16_c attribute_value = mark_up->get_string( attribute->value_start );
			convert_string_to_float32( attribute_value, _value_maximum );
		}

		attribute = mark_up->find_daughter_attribute( node, "page_range" );
		if ( attribute )
		{
			string16_c attribute_value = mark_up->get_string( attribute->value_start );
			convert_string_to_float32( attribute_value, _page_range );
		}

		attribute = mark_up->find_daughter_attribute( node, "scroll_increment" );
		if ( attribute )
		{
			string16_c attribute_value = mark_up->get_string( attribute->value_start );
			convert_string_to_float32( attribute_value, _scroll_increment );
		}

		attribute = mark_up->find_daughter_attribute( node, "smooth_scroll_enable" );
		if ( attribute )
		{
			string16_c attribute_value = mark_up->get_string( attribute->value_start );
			_smooth_scroll_enable = attribute_value == "true";
		}
	}
	*/

	void_c menu_control_scroll_i::update_animations( float32_c time_step )
	{
		if ( _mode == mode_e_scroll_bar )
		{
			// clamp value here in update, because certain factors might have changed.
			float64_c total_range = _value_maximum - _value_minimum;
			if ( total_range < _page_size )
			{
				_value = _value_minimum; // value range is smaller than page size, scroll to top.
			}
			else if ( _value > _value_maximum - _page_size )
			{
				_value = _value_maximum - _page_size;
			}
			else if ( _value < _value_minimum )
			{
				_value = _value_minimum;
			}
		}
		else if ( _mode == mode_e_scrub_bar )
		{
			_scrub_value_accumulated += ops::math_power( ops::math_absolute_value( _scrub_input ), _scrub_speed_ease_in ) * ( _scrub_speed * time_step ) * ops::math_sign( _scrub_input );
			if ( _value_increment > 0.0 )
			{
				float64_c scrub = ops::math_nearest_multiple( _scrub_value_accumulated, _value_increment );
				_value += scrub;
				_scrub_value_accumulated -= scrub;
			}
			else
			{
				_value += _scrub_value_accumulated;
				_scrub_value_accumulated = 0.0;
			}
			if ( _value < _value_maximum )
			{
				_value = _value_minimum;
			}
			else if ( _value > _value_maximum )
			{
				_value = _value_maximum;
			}
		}

		// we don't necessarily need to call this on each update.
		// but it will keep the layout of the elements in sync with the current value.
		update_transform_and_layout();

		// update smooth scroll
		float64_c value_difference = _value_smoothed - _value;
		if ( value_difference != 0.0 )
		{
			if ( value_difference < 0.0 )
			{
				value_difference = -value_difference;
			}
			if ( value_difference > 1.0 )
			{
				_value_smoothed = ( _value_smoothed * 0.5 ) + ( _value * 0.5 );
			}
			else
			{
				_value_smoothed = _value;
			}
			on_smooth_value_changed.invoke( this );
		}

		menu_control_c::update_animations( time_step );
	}

	void_c menu_control_scroll_i::load_properties( data_scribe_markup_c::node_c const * node )
	{
		menu_control_c::load_properties( node );

		data_scribe_markup_c::attribute_c const * attribute = nullptr;

		_page_size = 0.0;
		attribute = node->find_attribute( "page_size" );
		if ( attribute )
		{
			ops::convert_string8_to_float64( attribute->get_value(), _page_size );
		}

		_line_size = 0.0;
		attribute = node->find_attribute( "line_size" );
		if ( attribute )
		{
			ops::convert_string8_to_float64( attribute->get_value(), _line_size );
		}

		_value_minimum = 0.0;
		attribute = node->find_attribute( "value_minimum" );
		if ( attribute )
		{
			ops::convert_string8_to_float64( attribute->get_value(), _value_minimum );
		}

		_value_maximum = 0.0;
		attribute = node->find_attribute( "value_maximum" );
		if ( attribute )
		{
			ops::convert_string8_to_float64( attribute->get_value(), _value_maximum );
		}

		_value_increment = 0.0;
		attribute = node->find_attribute( "value_increment" );
		if ( attribute )
		{
			ops::convert_string8_to_float64( attribute->get_value(), _value_increment );
		}

		_value = 0.0;
		attribute = node->find_attribute( "value" );
		if ( attribute )
		{
			ops::convert_string8_to_float64( attribute->get_value(), _value );
		}

		_value_smoothed = _value;
		
		_scrub_speed = 1.0;
		attribute = node->find_attribute( "scrub_speed" );
		if ( attribute )
		{
			ops::convert_string8_to_float64( attribute->get_value(), _scrub_speed );
		}

		_scrub_speed_ease_in = 1.0;
		attribute = node->find_attribute( "scrub_speed_ease_in" );
		if ( attribute )
		{
			ops::convert_string8_to_float64( attribute->get_value(), _scrub_speed_ease_in );
		}

		_scrub_value_accumulated = 0.0;

		//_thickness = 10.0f;
		//attribute = node->find_attribute( "_thickness" );
		//if ( attribute )
		//{
		//	ops::convert_string8_to_float32( attribute->get_value(), _thickness );
		//}

		_fixed_grip_length = 0.0f;
		attribute = node->find_attribute( "fixed_grip_length" );
		if ( attribute )
		{
			ops::convert_string8_to_float32( attribute->get_value(), _fixed_grip_length );
		}
	}

	menu_control_scroll_i::orientation_e menu_control_scroll_i::get_orientation() const
	{
		return _orientation;
	}

	menu_control_scroll_i::mode_e menu_control_scroll_i::get_mode() const
	{
		return _mode;
	}

	void_c menu_control_scroll_i::update_visibility( menu_visibility_mode_e value )
	{
		if ( value == menu_visibility_mode_e_automatic )
		{
			if ( _value_maximum - _value_minimum > _page_size )
			{
				show();
			}
			else
			{
				hide();
			}
		}
		else if ( value == menu_visibility_mode_e_always )
		{
			show();
		}
		else
		{
			hide();
		}
	}

	void_c menu_control_scroll_i::set_value_range_and_page_size( float64_c value_minimum, float64_c value_maximum, float64_c page_size )
	{
		_value_minimum = value_minimum;
		_value_maximum = value_maximum;
		_page_size = page_size;
		update_transform_and_layout();
	}

	float64_c menu_control_scroll_i::get_page_size() const
	{
		return _page_size;
	}

	void_c menu_control_scroll_i::set_page_size( float64_c value )
	{
		_page_size = value;
	}

	float64_c menu_control_scroll_i::get_line_size() const
	{
		return _line_size;
	}

	void_c menu_control_scroll_i::set_line_size( float64_c value )
	{
		_line_size = value;
	}

	float64_c menu_control_scroll_i::get_value_minimum() const
	{
		return _value_minimum;
	}

	void_c menu_control_scroll_i::set_value_minimum( float64_c value )
	{
		_value_minimum = value;
		update_transform_and_layout();
	}

	float64_c menu_control_scroll_i::get_value_maximum() const
	{
		return _value_maximum;
	}

	void_c menu_control_scroll_i::set_value_maximum( float64_c value )
	{
		_value_maximum = value;
		update_transform_and_layout();
	}

	float64_c menu_control_scroll_i::get_value_increment() const
	{
		return _value_increment;
	}

	void_c menu_control_scroll_i::set_value_increment( float64_c value )
	{
		_value_increment = value;
		if ( _value_increment > 0.0 )
		{
			_value = ops::math_nearest_multiple( _value - _value_minimum, _value_increment ) + _value_minimum;
		}
	}

	float64_c menu_control_scroll_i::get_value() const
	{
		return _value;
	}

	void_c menu_control_scroll_i::set_value( float64_c value )
	{
		value = ops::math_clamp( value, _value_minimum, _value_maximum );
		if ( _value_increment > 0.0 )
		{
			value = ops::math_nearest_multiple( value - _value_minimum, _value_increment ) + _value_minimum;
		}
		if ( _value != value )
		{
			_value = value;
			on_value_changed.invoke( this );
		}
	}

	float64_c menu_control_scroll_i::get_value_smoothed() const
	{
		return _value_smoothed;
	}

	float64_c menu_control_scroll_i::get_scrub_speed_ease_in() const
	{
		return _scrub_speed_ease_in;
	}

	void_c menu_control_scroll_i::set_scrub_speed_ease_in( float64_c value )
	{
		_scrub_speed_ease_in = ops::math_clamp( value, 0.5, 10.0 );
	}

	float64_c menu_control_scroll_i::get_scrub_speed() const
	{
		return _scrub_speed;
	}

	void_c menu_control_scroll_i::set_scrub_speed( float64_c value )
	{
		if ( value < 0.0 )
		{
			value = 0.0;
		}
		_scrub_speed = value;
	}

	//float32_c menu_control_scroll_i::get_thickness() const
	//{
	//	return _thickness;
	//}

	//void_c menu_control_scroll_i::set_thickness( float32_c value )
	//{
	//	_thickness = value;
	//	update_transform_and_layout();
	//}

	float32_c menu_control_scroll_i::get_fixed_grip_length() const
	{
		return _fixed_grip_length;
	}

	void_c menu_control_scroll_i::set_fixed_grip_length( float32_c value )
	{
		if ( value < 1.0 )
		{
			value = 1.0;
		}
		_fixed_grip_length = value;
		update_transform_and_layout();
	}

	void_c menu_control_scroll_i::inject_mouse_wheel_input( float32_c value )
	{
		set_value( _value + ( value * _line_size ) );
	}

}