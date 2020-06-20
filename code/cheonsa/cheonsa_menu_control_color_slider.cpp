#include "cheonsa_menu_control_color_slider.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	void_c menu_control_color_slider_c::_build_mesh()
	{
		_mesh_element.vertex_list.remove_all();
		_mesh_element.index_list.remove_all();
		if ( _mode != mode_e_hue )
		{
			// 0-2
			// |/|
			// 1-3

			// build vertices.
			_mesh_element.vertex_list.set_length_absolute( 4 );
			video_renderer_vertex_menu_c * vertex = &_mesh_element.vertex_list[ 0 ];
			vertex->position.a = _local_box.minimum.a;
			vertex->position.b = _local_box.minimum.b;
			vertex = &_mesh_element.vertex_list[ 1 ];
			vertex->position.a = _local_box.minimum.a;
			vertex->position.b = _local_box.maximum.b;
			vertex = &_mesh_element.vertex_list[ 2 ];
			vertex->position.a = _local_box.maximum.a;
			vertex->position.b = _local_box.minimum.b;
			vertex = &_mesh_element.vertex_list[ 3 ];
			vertex->position.a = _local_box.maximum.a;
			vertex->position.b = _local_box.maximum.b;

			// build indices.
			_mesh_element.index_list.set_length_absolute( 6 );
			_mesh_element.index_list[  0 ] = 0; // triangle 1.
			_mesh_element.index_list[  1 ] = 1;
			_mesh_element.index_list[  2 ] = 2;
			_mesh_element.index_list[  3 ] = 2; // triangle 2.
			_mesh_element.index_list[  4 ] = 1;
			_mesh_element.index_list[  5 ] = 3;

			//_mesh_element.pixel_shader = engine.get_video_renderer_shader_manager()->get_menu_ps_solid_color();
		}
		else
		{
			// 0-2-4-6-8-0-2
			// |/|/|/|/|/|/|
			// 1-3-5-7-9-1-3
			// r y g c b m r
			//  e e r y l a e
			//   d l e a u g d
			//      l e n e e
			//       o n     n
			//        w       t
			//                 a
			// build vertices.
			_mesh_element.vertex_list.set_length_absolute( 14 );
			for ( sint32_c i = 0; i < 7; i++ )
			{
				float32_c x = ops::interpolate_linear( _local_box.minimum.a, _local_box.maximum.a, i / 6.0f );
				_mesh_element.vertex_list[ i * 2     ].position = vector32x3_c( x, _local_box.minimum.b, 0.0f );
				_mesh_element.vertex_list[ i * 2 + 1 ].position = vector32x3_c( x, _local_box.maximum.b, 0.0f );
			}

			// build indices.
			_mesh_element.index_list.set_length_absolute( 36 );
			for ( uint16_c i = 0, j = 0; i < 6; i++, j += 2 )
			{
				_mesh_element.index_list[ i * 6     ] = j;
				_mesh_element.index_list[ i * 6 + 1 ] = j + 1;
				_mesh_element.index_list[ i * 6 + 2 ] = j + 2;
				_mesh_element.index_list[ i * 6 + 3 ] = j + 2;
				_mesh_element.index_list[ i * 6 + 4 ] = j + 1;
				_mesh_element.index_list[ i * 6 + 5 ] = j + 3;
			}

			//_mesh_element.pixel_shader = engine.get_video_renderer_shader_manager()->get_menu_ps_solid_color_hue_slider();
		}
		_mesh_box = _local_box;
	}

	void_c menu_control_color_slider_c::_update_mesh_colors()
	{
		if ( _mode == mode_e_hue )
		{
			assert( _mesh_element.vertex_list.get_length() == 14 );
			_mesh_element.vertex_list[  0 ].color = vector32x4_c( 1.0f, 0.0f, 0.0f, 1.0f );
			_mesh_element.vertex_list[  1 ].color = vector32x4_c( 1.0f, 0.0f, 0.0f, 1.0f );
			_mesh_element.vertex_list[  2 ].color = vector32x4_c( 1.0f, 1.0f, 0.0f, 1.0f );
			_mesh_element.vertex_list[  3 ].color = vector32x4_c( 1.0f, 1.0f, 0.0f, 1.0f );
			_mesh_element.vertex_list[  4 ].color = vector32x4_c( 0.0f, 1.0f, 0.0f, 1.0f );
			_mesh_element.vertex_list[  5 ].color = vector32x4_c( 0.0f, 1.0f, 0.0f, 1.0f );
			_mesh_element.vertex_list[  6 ].color = vector32x4_c( 0.0f, 1.0f, 1.0f, 1.0f );
			_mesh_element.vertex_list[  7 ].color = vector32x4_c( 0.0f, 1.0f, 1.0f, 1.0f );
			_mesh_element.vertex_list[  8 ].color = vector32x4_c( 0.0f, 0.0f, 1.0f, 1.0f );
			_mesh_element.vertex_list[  9 ].color = vector32x4_c( 0.0f, 0.0f, 1.0f, 1.0f );
			_mesh_element.vertex_list[ 10 ].color = vector32x4_c( 1.0f, 0.0f, 1.0f, 1.0f );
			_mesh_element.vertex_list[ 11 ].color = vector32x4_c( 1.0f, 0.0f, 1.0f, 1.0f );
			_mesh_element.vertex_list[ 12 ].color = vector32x4_c( 1.0f, 0.0f, 0.0f, 1.0f );
			_mesh_element.vertex_list[ 13 ].color = vector32x4_c( 1.0f, 0.0f, 0.0f, 1.0f );
		}
		else if ( _mode == mode_e_red )
		{
			assert( _mesh_element.vertex_list.get_length() == 4 );
			_mesh_element.vertex_list[ 0 ].color = vector32x4_c( 0.0f, 0.0f, 0.0f, 1.0f );
			_mesh_element.vertex_list[ 1 ].color = vector32x4_c( 0.0f, 0.0f, 0.0f, 1.0f );
			_mesh_element.vertex_list[ 2 ].color = vector32x4_c( 1.0f, 0.0f, 0.0f, 1.0f );
			_mesh_element.vertex_list[ 3 ].color = vector32x4_c( 1.0f, 0.0f, 0.0f, 1.0f );
		}
		else if ( _mode == mode_e_green )
		{
			assert( _mesh_element.vertex_list.get_length() == 4 );
			_mesh_element.vertex_list[ 0 ].color = vector32x4_c( 0.0f, 0.0f, 0.0f, 1.0f );
			_mesh_element.vertex_list[ 1 ].color = vector32x4_c( 0.0f, 0.0f, 0.0f, 1.0f );
			_mesh_element.vertex_list[ 2 ].color = vector32x4_c( 0.0f, 1.0f, 0.0f, 1.0f );
			_mesh_element.vertex_list[ 3 ].color = vector32x4_c( 0.0f, 1.0f, 0.0f, 1.0f );
		}
		else if ( _mode == mode_e_blue )
		{
			assert( _mesh_element.vertex_list.get_length() == 4 );
			_mesh_element.vertex_list[ 0 ].color = vector32x4_c( 0.0f, 0.0f, 0.0f, 1.0f );
			_mesh_element.vertex_list[ 1 ].color = vector32x4_c( 0.0f, 0.0f, 0.0f, 1.0f );
			_mesh_element.vertex_list[ 2 ].color = vector32x4_c( 0.0f, 0.0f, 1.0f, 1.0f );
			_mesh_element.vertex_list[ 3 ].color = vector32x4_c( 0.0f, 0.0f, 1.0f, 1.0f );
		}
		else if ( _mode == mode_e_alpha )
		{
			assert( _mesh_element.vertex_list.get_length() == 4 );
			_mesh_element.vertex_list[ 0 ].color = vector32x4_c( 0.0f, 0.0f, 0.0f, 1.0f );
			_mesh_element.vertex_list[ 1 ].color = vector32x4_c( 0.0f, 0.0f, 0.0f, 1.0f );
			_mesh_element.vertex_list[ 2 ].color = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
			_mesh_element.vertex_list[ 3 ].color = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		}
		else if ( _mode == mode_e_saturation )
		{
			assert( _mesh_element.vertex_list.get_length() == 4 );
			_mesh_element.vertex_list[ 0 ].color = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
			_mesh_element.vertex_list[ 1 ].color = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
			_mesh_element.vertex_list[ 2 ].color = vector32x4_c( _color.a, _color.b, _color.c, 1.0f );
			_mesh_element.vertex_list[ 3 ].color = vector32x4_c( _color.a, _color.b, _color.c, 1.0f );
		}
		else if ( _mode == mode_e_value )
		{
			assert( _mesh_element.vertex_list.get_length() == 4 );
			_mesh_element.vertex_list[ 0 ].color = vector32x4_c( 0.0f, 0.0f, 0.0f, 1.0f );
			_mesh_element.vertex_list[ 1 ].color = vector32x4_c( 0.0f, 0.0f, 0.0f, 1.0f );
			_mesh_element.vertex_list[ 2 ].color = vector32x4_c( _color.a, _color.b, _color.c, 1.0f );
			_mesh_element.vertex_list[ 3 ].color = vector32x4_c( _color.a, _color.b, _color.c, 1.0f );
		}
		else if ( _mode == mode_e_swatch )
		{
			assert( _mesh_element.vertex_list.get_length() == 4 );
			_mesh_element.vertex_list[ 0 ].color = vector32x4_c( _color.a, _color.b, _color.c, 1.0f );
			_mesh_element.vertex_list[ 1 ].color = vector32x4_c( _color.a, _color.b, _color.c, 1.0f );
			_mesh_element.vertex_list[ 2 ].color = vector32x4_c( _color.a, _color.b, _color.c, 1.0f );
			_mesh_element.vertex_list[ 3 ].color = vector32x4_c( _color.a, _color.b, _color.c, 1.0f );
		}
	}

	void_c menu_control_color_slider_c::_on_is_mouse_focused_changed()
	{
		if ( _is_mouse_focused )
		{
		}
		else
		{
			if ( _mouse_is_grabbed )
			{
				_mouse_is_grabbed = false;
			}
		}
		on_is_mouse_focused_changed.invoke( menu_event_information_c( this, nullptr, nullptr ) );
	}

	void_c menu_control_color_slider_c::_on_input( input_event_c * input_event )
	{
		if ( input_event->get_type() == input_event_c::type_e_mouse_key_pressed )
		{
			if ( input_event->get_mouse_key() == input_mouse_key_e_left )
			{
				_mouse_is_grabbed = true;
			}
		}
		else if ( input_event->get_type() == input_event_c::type_e_mouse_key_released )
		{
			if ( input_event->get_mouse_key() == input_mouse_key_e_left )
			{
				_mouse_is_grabbed = false;
			}
		}
		if ( _mouse_is_grabbed )
		{
			vector32x2_c local_mouse_position = transform_global_point_to_local_point( input_event->get_menu_mouse_position() );
			float64_c old_value = _value;
			_value = ( local_mouse_position.a - _local_box.minimum.a ) / ( _local_box.maximum.a - _local_box.minimum.a );
			if ( _mode == mode_e_hue )
			{
				_value *= 360.0;
			}
			if ( _value != old_value )
			{
				on_value_changed.invoke( this );
			}
		}
	}
		
	void_c menu_control_color_slider_c::_update_transform_and_layout()
	{
		menu_control_c::_update_transform_and_layout();

		if ( _mesh_box != _local_box )
		{
			_build_mesh();
			_update_mesh_colors();
		}

		float64_c value = _value;
		if ( _mode == mode_e_hue )
		{
			value /= 360.0;
		}
		float32_c slider_position = ops::math_round_down( static_cast< float32_c >( value * _local_box.get_width() ) );
		_slider_frame_element.set_layout_simple( box32x2_c( _local_box.minimum.a + slider_position - 1, _local_box.minimum.b, _local_box.minimum.a + slider_position + 2, _local_box.maximum.b ) );
	}

	menu_control_color_slider_c::menu_control_color_slider_c()
		: menu_control_c()
		, _mesh_element( string8_c( "mesh", core_list_mode_e_static ) )
		, _slider_frame_element( string8_c( "slider_frame", core_list_mode_e_static ) )
		, _border_frame_element( string8_c( "border_frame", core_list_mode_e_static ) )
		, _mode( mode_e_red )
		, _value( 0.0f )
		, _color()
		, _mesh_box()
		, _mouse_is_grabbed( false )
	{
		_mesh_element.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_add_daughter_element( &_mesh_element );

		_slider_frame_element.set_style_key( string8_c( "e_precision_slider_frame", core_list_mode_e_static ) );
		_add_daughter_element( &_slider_frame_element );

		_border_frame_element.set_is_overlay( true );
		_add_daughter_element( &_border_frame_element );

		set_shared_color_class( menu_shared_color_class_e_button );
		set_style_map_key( string8_c( "e_color_slider", core_list_mode_e_static ) );
	}

	menu_control_color_slider_c::~menu_control_color_slider_c()
	{
	}

	void_c menu_control_color_slider_c::update_animations( float32_c time_step )
	{
		// we don't necessarily need to call this on each update.
		// but it will keep the layout of the elements in sync with the current value.
		_update_transform_and_layout();

		menu_control_c::update_animations( time_step );
	}

	menu_control_color_slider_c::mode_e menu_control_color_slider_c::get_mode() const
	{
		return _mode;
	}

	void_c menu_control_color_slider_c::set_mode( mode_e value )
	{
		_mode = value;
		_slider_frame_element.set_is_showed( _mode != mode_e_swatch );
		set_value( _value );
		_build_mesh();
		_update_mesh_colors();
	}

	float64_c menu_control_color_slider_c::get_value() const
	{
		return _value;
	}

	void_c menu_control_color_slider_c::set_value( float64_c value )
	{
		if ( _mode == mode_e_hue )
		{
			_value = ops::math_clamp( value, 0.0, 360.0 );
		}
		else
		{
			_value = ops::math_clamp( value, 0.0, 1.0 );
		}
	}

	vector32x3_c const & menu_control_color_slider_c::get_color() const
	{
		return _color;
	}

	void_c menu_control_color_slider_c::set_color( vector32x3_c const & value )
	{
		_color = value;
		if ( _mode == mode_e_saturation || _mode == mode_e_value || _mode == mode_e_swatch )
		{
			_update_mesh_colors();
		}
	}

	void_c menu_control_color_slider_c::set_color( vector32x4_c const & value )
	{
		_color = vector32x3_c( value.a, value.b, value.c );
		if ( _mode == mode_e_saturation || _mode == mode_e_value || _mode == mode_e_swatch )
		{
			_update_mesh_colors();
		}
	}

}
