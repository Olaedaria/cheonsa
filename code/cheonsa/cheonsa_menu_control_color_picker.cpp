#include "cheonsa_menu_control_color_picker.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	void_c menu_control_color_picker_c::_update_rgb_from_hsv()
	{
		ops::convert_hsv_to_rgb( _hsv, _rgb );
	}

	void_c menu_control_color_picker_c::_update_hsv_from_rgb()
	{
		ops::convert_rgb_to_hsv( _rgb, _hsv );
	}

	void_c menu_control_color_picker_c::_sync_control_values()
	{
		if ( _h_scroll_bar != nullptr )
		{
			_h_scroll_bar->set_value( _hsv.a );
		}
		if ( _h_text != nullptr )
		{
			string8_c value_string;
			ops::convert_float64_to_string8( _hsv.a, value_string );
			_h_text->set_plain_text_value( value_string );
		}
		if ( _s_scroll_bar != nullptr )
		{
			_s_scroll_bar->set_value( _hsv.b );
		}
		if ( _s_text != nullptr )
		{
			string8_c value_string;
			ops::convert_float64_to_string8( _hsv.b, value_string );
			_s_text->set_plain_text_value( value_string );
		}
		if ( _v_scroll_bar != nullptr )
		{
			_v_scroll_bar->set_value( _hsv.c );
		}
		if ( _v_text != nullptr )
		{
			string8_c value_string;
			ops::convert_float64_to_string8( _hsv.c, value_string );
			_v_text->set_plain_text_value( value_string );
		}

		if ( _r_scroll_bar != nullptr )
		{
			_r_scroll_bar->set_value( _rgb.a );
		}
		if ( _r_text != nullptr )
		{
			string8_c value_string;
			ops::convert_float64_to_string8( _rgb.a, value_string );
			_r_text->set_plain_text_value( value_string );
		}
		if ( _g_scroll_bar != nullptr )
		{
			_g_scroll_bar->set_value( _rgb.b );
		}
		if ( _g_text != nullptr )
		{
			string8_c value_string;
			ops::convert_float64_to_string8( _rgb.b, value_string );
			_g_text->set_plain_text_value( value_string );
		}
		if ( _b_scroll_bar != nullptr )
		{
			_b_scroll_bar->set_value( _rgb.c );
		}
		if ( _b_text != nullptr )
		{
			string8_c value_string;
			ops::convert_float64_to_string8( _rgb.c, value_string );
			_b_text->set_plain_text_value( value_string );
		}

		if ( _rgba_hex_text != nullptr )
		{
			string8_c value_string;
			if ( _alpha_enable )
			{
				ops::convert_rgba_to_hexadecimal_string8( vector64x4_c( static_cast< float32_c >( _rgb.a ), static_cast< float32_c >( _rgb.b ), static_cast< float32_c >( _rgb.c ), static_cast< float32_c >( _alpha ) ), value_string );
			}
			else
			{
				ops::convert_rgb_to_hexadecimal_string8( vector64x3_c( _rgb ), value_string );
			}
			_rgba_hex_text->set_plain_text_value( value_string );
		}
	}

	void_c menu_control_color_picker_c::_handle_slider_on_value_changed( menu_control_scroll_i * slider )
	{
		if ( slider == _h_scroll_bar )
		{
			_hsv.a = slider->get_value();
			_hsv.a = ops::math_clamp( _hsv.a, 0.0, 360.0 );
			_update_rgb_from_hsv();
			_sync_control_values();
		}
		else if ( slider == _s_scroll_bar )
		{
			_hsv.b = slider->get_value();
			_hsv.b = ops::math_saturate( _hsv.b );
			_update_rgb_from_hsv();
			_sync_control_values();
		}
		else if ( slider == _v_scroll_bar )
		{
			_hsv.c = slider->get_value();
			_hsv.c = ops::math_saturate( _hsv.c );
			_update_rgb_from_hsv();
			_sync_control_values();
		}
		else if ( slider == _r_scroll_bar )
		{
			_rgb.a = slider->get_value();
			_rgb.a = ops::math_saturate( _rgb.a );
			_update_hsv_from_rgb();
			_sync_control_values();
		}
		else if ( slider == _g_scroll_bar )
		{
			_rgb.b = slider->get_value();
			_rgb.b = ops::math_saturate( _rgb.b );
			_update_hsv_from_rgb();
			_sync_control_values();
		}
		else if ( slider == _b_scroll_bar )
		{
			_rgb.c = slider->get_value();
			_rgb.c = ops::math_saturate( _rgb.c );
			_update_hsv_from_rgb();
			_sync_control_values();
		}
		else if ( slider == _a_scroll_bar )
		{
			_alpha = slider->get_value();
			_alpha = ops::math_saturate( _alpha );
			_sync_control_values();
		}
	}

	void_c menu_control_color_picker_c::_handle_text_on_value_changed( menu_control_text_c * text )
	{
		string16_c text_value = text->get_plain_text_value();
		if ( text == _h_text )
		{
			ops::convert_string8_to_float64( string8_c( text_value ), _hsv.a );
			_hsv.a = ops::math_clamp( _hsv.a, 0.0, 360.0 );
			_update_rgb_from_hsv();
			_sync_control_values();
		}
		else if ( text == _s_text )
		{
			ops::convert_string8_to_float64( string8_c( text_value ), _hsv.b );
			_hsv.c = ops::math_saturate( _hsv.c );
			_update_rgb_from_hsv();
			_sync_control_values();
		}
		else if ( text == _v_text )
		{
			ops::convert_string8_to_float64( string8_c( text_value ), _hsv.c );
			_hsv.c = ops::math_saturate( _hsv.c );
			_update_rgb_from_hsv();
			_sync_control_values();
		}
		else if ( text == _r_text )
		{
			ops::convert_string8_to_float64( string8_c( text_value ), _rgb.a );
			_rgb.a = ops::math_saturate( _rgb.a );
			_update_hsv_from_rgb();
			_sync_control_values();
		}
		else if ( text == _g_text )
		{
			ops::convert_string8_to_float64( string8_c( text_value ), _rgb.b );
			_rgb.b = ops::math_saturate( _rgb.b );
			_update_hsv_from_rgb();
			_sync_control_values();
		}
		else if ( text == _b_text )
		{
			ops::convert_string8_to_float64( string8_c( text_value ), _rgb.c );
			_rgb.c = ops::math_saturate( _rgb.c );
			_update_hsv_from_rgb();
			_sync_control_values();
		}
		else if ( text == _a_text )
		{
			ops::convert_string8_to_float64( string8_c( text_value ), _alpha );
			_alpha = ops::math_saturate( _alpha );
			_sync_control_values();
		}
		else if ( text == _rgba_hex_text )
		{
			vector64x4_c rgba;
			ops::convert_string8_to_rgba( string8_c( text_value ), rgba );
			_rgb.a = rgba.a;
			_rgb.b = rgba.b;
			_rgb.c = rgba.c;
			if ( _alpha_enable )
			{
				_alpha = rgba.d;
			}
			_update_hsv_from_rgb();
			_sync_control_values();
		}
	}

	menu_control_color_picker_c::menu_control_color_picker_c()
		: menu_control_c()
		, _element_frame()
		, _rgb( 0.0, 0.0, 0.0 )
		, _hsv( 0.0, 0.0, 0.0 )
		, _alpha( 1.0 )
		, _alpha_enable( true )
		, _h_scroll_bar( nullptr )
		, _h_text( nullptr )
		, _s_scroll_bar( nullptr )
		, _s_text( nullptr )
		, _v_scroll_bar( nullptr )
		, _v_text( nullptr )
		, _r_scroll_bar( nullptr )
		, _r_text( nullptr )
		, _g_scroll_bar( nullptr )
		, _g_text( nullptr )
		, _b_scroll_bar( nullptr )
		, _b_text( nullptr )
		, _a_scroll_bar( nullptr )
		, _a_text( nullptr )
		, _rgba_hex_text( nullptr )
	{
		_element_frame.set_name( string8_c( mode_e_static, "frame" ) );
		_add_element( &_element_frame );

		_h_scroll_bar = new menu_control_scroll_bar_horizontal_c();
		_h_scroll_bar->set_name( string8_c( mode_e_static, "h_scroll" ) );
		_h_scroll_bar->on_value_changed_preview.subscribe( this, &menu_control_color_picker_c::_handle_slider_on_value_changed );
		_h_scroll_bar->on_value_changed.subscribe( this, &menu_control_color_picker_c::_handle_slider_on_value_changed );
		_add_control( _h_scroll_bar );

		_h_text = new menu_control_text_c();
		_h_text->set_name( string8_c( mode_e_static, "h_text" ) );
		_h_text->set_multi_line( false );
		_h_text->set_word_wrap( false );
		_h_text->set_text_value_length_limit( 50 );
		_h_text->on_value_changed_preview.subscribe( this, &menu_control_color_picker_c::_handle_text_on_value_changed );
		_h_text->on_value_changed_commit.subscribe( this, &menu_control_color_picker_c::_handle_text_on_value_changed );
		_add_control( _h_text );

		_s_scroll_bar = new menu_control_scroll_bar_horizontal_c();
		_s_scroll_bar->set_name( string8_c( mode_e_static, "s_scroll_bar" ) );
		_s_scroll_bar->on_value_changed_preview.subscribe( this, &menu_control_color_picker_c::_handle_slider_on_value_changed );
		_s_scroll_bar->on_value_changed.subscribe( this, &menu_control_color_picker_c::_handle_slider_on_value_changed );
		_add_control( _s_scroll_bar );

		_s_text = new menu_control_text_c();
		_s_text->set_name( string8_c( mode_e_static, "s_text" ) );
		_s_text->set_multi_line( false );
		_s_text->set_word_wrap( false );
		_s_text->set_text_value_length_limit( 50 );
		_s_text->set_text_filter_mode( menu_text_filter_mode_e_number_real );
		_s_text->set_text_interact_mode( menu_text_interact_mode_e_editable );
		_s_text->on_value_changed_preview.subscribe( this, &menu_control_color_picker_c::_handle_text_on_value_changed );
		_s_text->on_value_changed_commit.subscribe( this, &menu_control_color_picker_c::_handle_text_on_value_changed );
		_add_control( _s_text );

		_v_scroll_bar = new menu_control_scroll_bar_horizontal_c();
		_v_scroll_bar->set_name( string8_c( mode_e_static, "v_scroll_bar" ) );
		_v_scroll_bar->on_value_changed_preview.subscribe( this, &menu_control_color_picker_c::_handle_slider_on_value_changed );
		_v_scroll_bar->on_value_changed.subscribe( this, &menu_control_color_picker_c::_handle_slider_on_value_changed );
		_add_control( _v_scroll_bar );

		_v_text = new menu_control_text_c();
		_v_text->set_name( string8_c( mode_e_static, "v_text" ) );
		_v_text->set_multi_line( false );
		_v_text->set_word_wrap( false );
		_v_text->set_text_value_length_limit( 50 );
		_v_text->set_text_filter_mode( menu_text_filter_mode_e_number_real );
		_v_text->set_text_interact_mode( menu_text_interact_mode_e_editable );
		_v_text->on_value_changed_preview.subscribe( this, &menu_control_color_picker_c::_handle_text_on_value_changed );
		_v_text->on_value_changed_commit.subscribe( this, &menu_control_color_picker_c::_handle_text_on_value_changed );
		_add_control( _v_text );

		_r_scroll_bar = new menu_control_scroll_bar_horizontal_c();
		_r_scroll_bar->set_name( string8_c( mode_e_static, "r_scroll" ) );
		_r_scroll_bar->on_value_changed_preview.subscribe( this, &menu_control_color_picker_c::_handle_slider_on_value_changed );
		_r_scroll_bar->on_value_changed.subscribe( this, &menu_control_color_picker_c::_handle_slider_on_value_changed );
		_add_control( _r_scroll_bar );

		_r_text = new menu_control_text_c();
		_r_text->set_name( string8_c( mode_e_static, "r_text" ) );
		_r_text->set_multi_line( false );
		_r_text->set_word_wrap( false );
		_r_text->set_text_value_length_limit( 50 );
		_r_text->set_text_filter_mode( menu_text_filter_mode_e_number_real );
		_r_text->set_text_interact_mode( menu_text_interact_mode_e_editable );
		_r_text->on_value_changed_preview.subscribe( this, &menu_control_color_picker_c::_handle_text_on_value_changed );
		_r_text->on_value_changed_commit.subscribe( this, &menu_control_color_picker_c::_handle_text_on_value_changed );
		_add_control( _r_text );

		_g_scroll_bar = new menu_control_scroll_bar_horizontal_c();
		_g_scroll_bar->set_name( string8_c( mode_e_static, "g_scroll" ) );
		_g_scroll_bar->on_value_changed_preview.subscribe( this, &menu_control_color_picker_c::_handle_slider_on_value_changed );
		_g_scroll_bar->on_value_changed.subscribe( this, &menu_control_color_picker_c::_handle_slider_on_value_changed );
		_add_control( _g_scroll_bar );

		_g_text = new menu_control_text_c();
		_g_text->set_name( string8_c( mode_e_static, "g_text" ) );
		_g_text->set_multi_line( false );
		_g_text->set_word_wrap( false );
		_g_text->set_text_value_length_limit( 50 );
		_g_text->set_text_filter_mode( menu_text_filter_mode_e_number_real );
		_g_text->set_text_interact_mode( menu_text_interact_mode_e_editable );
		_g_text->on_value_changed_preview.subscribe( this, &menu_control_color_picker_c::_handle_text_on_value_changed );
		_g_text->on_value_changed_commit.subscribe( this, &menu_control_color_picker_c::_handle_text_on_value_changed );
		_add_control( _g_text );

		_b_scroll_bar = new menu_control_scroll_bar_horizontal_c();
		_b_scroll_bar->set_name( string8_c( mode_e_static, "b_scroll" ) );
		_b_scroll_bar->on_value_changed_preview.subscribe( this, &menu_control_color_picker_c::_handle_slider_on_value_changed );
		_b_scroll_bar->on_value_changed.subscribe( this, &menu_control_color_picker_c::_handle_slider_on_value_changed );
		_add_control( _b_scroll_bar );

		_b_text = new menu_control_text_c();
		_b_text->set_name( string8_c( mode_e_static, "b_text" ) );
		_b_text->set_multi_line( false );
		_b_text->set_word_wrap( false );
		_b_text->set_text_value_length_limit( 50 );
		_b_text->set_text_filter_mode( menu_text_filter_mode_e_number_real );
		_b_text->set_text_interact_mode( menu_text_interact_mode_e_editable );
		_b_text->on_value_changed_preview.subscribe( this, &menu_control_color_picker_c::_handle_text_on_value_changed );
		_b_text->on_value_changed_commit.subscribe( this, &menu_control_color_picker_c::_handle_text_on_value_changed );
		_add_control( _b_text );

		_a_scroll_bar = new menu_control_scroll_bar_horizontal_c();
		_a_scroll_bar->set_name( string8_c( mode_e_static, "a_scroll" ) );
		_a_scroll_bar->on_value_changed_preview.subscribe( this, &menu_control_color_picker_c::_handle_slider_on_value_changed );
		_a_scroll_bar->on_value_changed.subscribe( this, &menu_control_color_picker_c::_handle_slider_on_value_changed );
		_add_control( _a_scroll_bar );

		_a_text = new menu_control_text_c();
		_a_text->set_name( string8_c( mode_e_static, "a_text" ) );
		_a_text->set_multi_line( false );
		_a_text->set_word_wrap( false );
		_a_text->set_text_value_length_limit( 50 );
		_a_text->set_text_filter_mode( menu_text_filter_mode_e_number_real );
		_a_text->set_text_interact_mode( menu_text_interact_mode_e_editable );
		_a_text->on_value_changed_preview.subscribe( this, &menu_control_color_picker_c::_handle_text_on_value_changed );
		_a_text->on_value_changed_commit.subscribe( this, &menu_control_color_picker_c::_handle_text_on_value_changed );
		_add_control( _a_text );

		_rgba_hex_text = new menu_control_text_c();
		_rgba_hex_text->set_name( string8_c( mode_e_static, "rgba_hex_text" ) );
		_rgba_hex_text->set_multi_line( false );
		_rgba_hex_text->set_word_wrap( false );
		_rgba_hex_text->set_text_value_length_limit( 16 );
		_rgba_hex_text->set_text_interact_mode( menu_text_interact_mode_e_editable );
		_rgba_hex_text->on_value_changed_preview.subscribe( this, &menu_control_color_picker_c::_handle_text_on_value_changed );
		_rgba_hex_text->on_value_changed_commit.subscribe( this, &menu_control_color_picker_c::_handle_text_on_value_changed );
		_add_control( _rgba_hex_text );

		_rgba_float_text = new menu_control_text_c();
		_rgba_float_text->set_name( string8_c( mode_e_static, "rgba_float_text" ) );
		_rgba_float_text->set_multi_line( false );
		_rgba_float_text->set_word_wrap( false );
		_rgba_float_text->set_text_value_length_limit( 64 );
		_rgba_float_text->on_value_changed_preview.subscribe( this, &menu_control_color_picker_c::_handle_text_on_value_changed );
		_rgba_float_text->on_value_changed_commit.subscribe( this, &menu_control_color_picker_c::_handle_text_on_value_changed );
		_add_control( _rgba_float_text );
	}

	vector64x3_c menu_control_color_picker_c::get_rgb() const
	{
		return _rgb;
	}

	void_c menu_control_color_picker_c::set_rgb( vector64x3_c value )
	{
		_rgb.a = ops::math_saturate( value.a );
		_rgb.b = ops::math_saturate( value.b );
		_rgb.c = ops::math_saturate( value.c );
		_update_hsv_from_rgb();
		_sync_control_values();
	}

	vector64x4_c menu_control_color_picker_c::get_rgba() const
	{
		return vector64x4_c( _rgb.a, _rgb.b, _rgb.c, _alpha );
	}

	void_c menu_control_color_picker_c::set_rgba( vector64x4_c value )
	{
		_rgb.a = ops::math_saturate( value.a );
		_rgb.b = ops::math_saturate( value.b );
		_rgb.c = ops::math_saturate( value.c );
		_alpha = ops::math_saturate( value.d );
		_update_hsv_from_rgb();
		_sync_control_values();
	}

	vector64x3_c menu_control_color_picker_c::get_hsv() const
	{
		return _hsv;
	}

	void_c menu_control_color_picker_c::set_hsv( vector64x3_c value )
	{
		_hsv.a = ops::math_clamp( value.a, 0.0, 360.0 );
		_hsv.b = ops::math_saturate( value.b );
		_hsv.c = ops::math_saturate( value.c );
		_update_rgb_from_hsv();
		_sync_control_values();
	}

	vector64x4_c menu_control_color_picker_c::get_hsva() const
	{
		return vector64x4_c( _hsv.a, _hsv.b, _hsv.c, _alpha );
	}

	void_c menu_control_color_picker_c::set_hsva( vector64x4_c value )
	{
		_hsv.a = ops::math_clamp( value.a, 0.0, 360.0 );
		_hsv.b = ops::math_saturate( value.b );
		_hsv.c = ops::math_saturate( value.c );
		_alpha = ops::math_saturate( value.d );
		_update_rgb_from_hsv();
		_sync_control_values();
	}

	float64_c menu_control_color_picker_c::get_alpha() const
	{
		return _alpha;
	}

	void_c menu_control_color_picker_c::set_alpha( float64_c value )
	{
		_alpha = ops::math_saturate( value );
		_sync_control_values();
	}

	boolean_c menu_control_color_picker_c::get_alpha_enable() const
	{
		return _alpha_enable;
	}

	void_c menu_control_color_picker_c::set_alpha_enable( boolean_c value )
	{
		_alpha_enable = value;
		if ( _alpha_enable == false )
		{
			_alpha = 1.0;
		}
	}

}
