#include "cheonsa_menu_control_window_color_picker.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	vector32x2_c menu_control_window_color_picker_c::default_size = vector32x2_c( 500, 650 );

	void_c menu_control_window_color_picker_c::_update_rgb_from_hsv()
	{
		vector64x3_c rgb;
		ops::convert_hsv_to_rgb( _hsv, rgb );
		_rgba.a = rgb.a;
		_rgba.b = rgb.b;
		_rgba.c = rgb.c;
	}

	void_c menu_control_window_color_picker_c::_update_hsv_from_rgb()
	{
		vector64x3_c rgb;
		rgb.a = _rgba.a;
		rgb.b = _rgba.b;
		rgb.c = _rgba.c;
		ops::convert_rgb_to_hsv( rgb, _hsv );
	}

	void_c menu_control_window_color_picker_c::_sync_control_values()
	{
		assert( _is_muted == false );

		_is_muted = true;

		_swatch->set_color( vector32x3_c( static_cast< float32_c >( _rgba.a ), static_cast< float32_c >( _rgba.b ), static_cast< float32_c >( _rgba.c ) ) );

		string8_c value_string;
		vector64x3_c rgb;

		_h_color_slider->set_value( _hsv.a );
		ops::convert_float64_to_string8( _hsv.a, value_string );
		_h_text->set_plain_text_value( value_string );

		_s_color_slider->set_value( _hsv.b );
		ops::convert_hsv_to_rgb( vector64x3_c( _hsv.a, 1.0f, 1.0f ), rgb );
		_s_color_slider->set_color( vector32x3_c( rgb ) );
		ops::convert_float64_to_string8( _hsv.b, value_string );
		_s_text->set_plain_text_value( value_string );

		_v_color_slider->set_value( _hsv.c );
		ops::convert_hsv_to_rgb( vector64x3_c( _hsv.a, _hsv.b, 1.0f ), rgb );
		_v_color_slider->set_color( vector32x3_c( rgb ) );
		ops::convert_float64_to_string8( _hsv.c, value_string );
		_v_text->set_plain_text_value( value_string );

		_r_color_slider->set_value( _rgba.a );
		ops::convert_float64_to_string8( _rgba.a, value_string );
		_r_text->set_plain_text_value( value_string );

		_g_color_slider->set_value( _rgba.b );
		ops::convert_float64_to_string8( _rgba.b, value_string );
		_g_text->set_plain_text_value( value_string );

		_b_color_slider->set_value( _rgba.c );
		ops::convert_float64_to_string8( _rgba.c, value_string );
		_b_text->set_plain_text_value( value_string );

		_a_color_slider->set_value( _rgba.d );
		ops::convert_float64_to_string8( _rgba.d, value_string );
		_a_text->set_plain_text_value( value_string );

		if ( _alpha_is_enabled )
		{
			ops::convert_rgba_to_hexadecimal_string8( _rgba, value_string );
		}
		else
		{
			vector64x3_c rgb;
			rgb.a = _rgba.a;
			rgb.b = _rgba.b;
			rgb.c = _rgba.c;
			ops::convert_rgb_to_hexadecimal_string8( rgb, value_string );
		}
		_rgba_hex_text->set_plain_text_value( value_string );

		ops::convert_float64xn_to_string8( core_list_c< float64_c >( core_list_mode_e_static, _rgba.as_array(), _alpha_is_enabled ? 4 : 3 ), value_string );
		_rgba_float_text->set_plain_text_value( value_string );

		_is_muted = false;
	}

	void_c menu_control_window_color_picker_c::_handle_color_slider_on_value_changed( menu_control_color_slider_c * color_slider )
	{
		if ( color_slider == _r_color_slider )
		{
			_rgba.a = ops::math_saturate( color_slider->get_value() );
			_update_hsv_from_rgb();
			_sync_control_values();
		}
		else if ( color_slider == _g_color_slider )
		{
			_rgba.b = ops::math_saturate( color_slider->get_value() );
			_update_hsv_from_rgb();
			_sync_control_values();
		}
		else if ( color_slider == _b_color_slider )
		{
			_rgba.c = ops::math_saturate( color_slider->get_value() );
			_update_hsv_from_rgb();
			_sync_control_values();
		}
		else if ( color_slider == _a_color_slider )
		{
			_rgba.d = ops::math_saturate( color_slider->get_value() );
			_sync_control_values();
		}
		else if ( color_slider == _h_color_slider )
		{
			_hsv.a = ops::math_clamp( color_slider->get_value(), 0.0, 360.0 );
			_update_rgb_from_hsv();
			_sync_control_values();
		}
		else if ( color_slider == _s_color_slider )
		{
			_hsv.b = ops::math_saturate( color_slider->get_value() );
			_update_rgb_from_hsv();
			_sync_control_values();
		}
		else if ( color_slider == _v_color_slider )
		{
			_hsv.c = ops::math_saturate( color_slider->get_value() );
			_update_rgb_from_hsv();
			_sync_control_values();
		}
	}

	void_c menu_control_window_color_picker_c::_handle_text_on_value_changed( menu_control_text_c * text )
	{
		if ( _is_muted )
		{
			return;
		}

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
			ops::convert_string8_to_float64( string8_c( text_value ), _rgba.a );
			_rgba.a = ops::math_saturate( _rgba.a );
			_update_hsv_from_rgb();
			_sync_control_values();
		}
		else if ( text == _g_text )
		{
			ops::convert_string8_to_float64( string8_c( text_value ), _rgba.b );
			_rgba.b = ops::math_saturate( _rgba.b );
			_update_hsv_from_rgb();
			_sync_control_values();
		}
		else if ( text == _b_text )
		{
			ops::convert_string8_to_float64( string8_c( text_value ), _rgba.c );
			_rgba.c = ops::math_saturate( _rgba.c );
			_update_hsv_from_rgb();
			_sync_control_values();
		}
		else if ( text == _a_text )
		{
			ops::convert_string8_to_float64( string8_c( text_value ), _rgba.d );
			_rgba.d = ops::math_saturate( _rgba.d );
			_sync_control_values();
		}
		else if ( text == _rgba_hex_text )
		{
			vector64x4_c rgba;
			if ( ops::convert_string8_to_rgba( string8_c( text_value ), rgba ) )
			{
				_rgba.a = rgba.a;
				_rgba.b = rgba.b;
				_rgba.c = rgba.c;
				if ( _alpha_is_enabled )
				{
					_rgba.d = rgba.d;
				}
				_update_hsv_from_rgb();
				_sync_control_values();
			}
		}
		else if ( text == _rgba_float_text )
		{
			vector64x4_c rgba;
			if ( ops::convert_string8_to_rgba( string8_c( text_value ), rgba ) )
			{
				_rgba.a = rgba.a;
				_rgba.b = rgba.b;
				_rgba.c = rgba.c;
				if ( _alpha_is_enabled )
				{
					_rgba.d = rgba.d;
				}
				_update_hsv_from_rgb();
				_sync_control_values();
			}
		}
	}

	void_c menu_control_window_color_picker_c::_handle_button_on_clicked( menu_event_information_c event_information )
	{
		if ( event_information.control == _cancel_button )
		{
			_dialog_result = menu_dialog_result_e_cancel;
		}
		else if ( event_information.control == _okay_button )
		{
			_dialog_result = menu_dialog_result_e_okay;
		}
		on_dialog_result.invoke( this );
	}

	void_c menu_control_window_color_picker_c::_on_input( input_event_c * input_event )
	{
		menu_control_window_c::_on_input( input_event );
		if ( !input_event->get_was_handled() )
		{
			if ( input_event->get_type() == input_event_c::type_e_keyboard_key_pressed )
			{
				if ( input_event->get_keyboard_key() == input_keyboard_key_e_escape )
				{
					_dialog_result = menu_dialog_result_e_cancel;
					on_dialog_result.invoke( this );
				}
			}
		}
	}

	menu_control_window_color_picker_c::menu_control_window_color_picker_c()
		: menu_control_window_c()
		, _is_muted( false )
		, _rgba( 0.0, 0.0, 0.0, 0.0 )
		, _hsv( 0.0, 0.0, 0.0 )
		, _alpha_is_enabled( true )
		, _swatch( nullptr )
		, _h_label( nullptr )
		, _h_color_slider( nullptr )
		, _h_text( nullptr )
		, _s_label( nullptr )
		, _s_color_slider( nullptr )
		, _s_text( nullptr )
		, _v_label( nullptr )
		, _v_color_slider( nullptr )
		, _v_text( nullptr )
		, _r_label( nullptr )
		, _r_color_slider( nullptr )
		, _r_text( nullptr )
		, _g_label( nullptr )
		, _g_color_slider( nullptr )
		, _g_text( nullptr )
		, _b_label( nullptr )
		, _b_color_slider( nullptr )
		, _b_text( nullptr )
		, _a_label( nullptr )
		, _a_color_slider( nullptr )
		, _a_text( nullptr )
		, _rgba_hex_text( nullptr )
		, _cancel_button( nullptr )
		, _okay_button( nullptr )
	{
		set_size( vector32x2_c( default_size.a, default_size.b ) );
		set_user_can_resize( false );
		set_title_bar_text_value( string16_c( L"color picker", core_list_mode_e_static ) );

		// labels.
		float32_c label_left = 8;
		float32_c label_width = 80;

		// sliders.
		float32_c slider_left = 96;
		float32_c slider_right = 116;

		// texts.
		float32_c text_right = 8;
		float32_c text_width = 100;

		// rows.
		float32_c row_top = 8;
		float32_c row_height = 30;

		_swatch = new menu_control_color_slider_c();
		_swatch->set_name( string8_c( "swatch", core_list_mode_e_static ) );
		_swatch->set_mode( menu_control_color_slider_c::mode_e_swatch );
		_swatch->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( label_left, row_top, text_right, 200 ) );
		add_daughter_control_to_client( _swatch );
		row_top += 200 + 8;

		_h_label = new menu_control_label_c();
		_h_label->set_name( string8_c( "h_label", core_list_mode_e_static ) );
		_h_label->set_plain_text_value( string16_c( L"h", core_list_mode_e_static ) );
		_h_label->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top, box32x2_c( label_left, row_top, label_width, row_height ) );
		add_daughter_control_to_client( _h_label );
		_h_color_slider = new menu_control_color_slider_c();
		_h_color_slider->set_name( string8_c( "h_color_slider", core_list_mode_e_static ) );
		_h_color_slider->set_mode( menu_control_color_slider_c::mode_e_hue );
		_h_color_slider->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( slider_left, row_top, slider_right, row_height ) );
		_h_color_slider->on_value_changed.subscribe( this, &menu_control_window_color_picker_c::_handle_color_slider_on_value_changed );
		add_daughter_control_to_client( _h_color_slider );
		_h_text = new menu_control_text_c();
		_h_text->set_name( string8_c( "h_text", core_list_mode_e_static ) );
		_h_text->set_multi_line( false );
		_h_text->set_word_wrap( false );
		_h_text->set_character_limit( 50 );
		_h_text->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right, box32x2_c( text_width, row_top, text_right, row_height ) );
		_h_text->on_plain_text_value_changed.subscribe( this, &menu_control_window_color_picker_c::_handle_text_on_value_changed );
		add_daughter_control_to_client( _h_text );
		row_top += row_height + 8;

		_s_label = new menu_control_label_c();
		_s_label->set_name( string8_c( "s_label", core_list_mode_e_static ) );
		_s_label->set_plain_text_value( string16_c( L"s", core_list_mode_e_static ) );
		_s_label->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top, box32x2_c( label_left, row_top, label_width, row_height ) ) ;
		add_daughter_control_to_client( _s_label );
		_s_color_slider = new menu_control_color_slider_c();
		_s_color_slider->set_name( string8_c( "s_color_slider", core_list_mode_e_static ) );
		_s_color_slider->set_mode( menu_control_color_slider_c::mode_e_saturation );
		_s_color_slider->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( slider_left, row_top, slider_right, row_height ) );
		_s_color_slider->on_value_changed.subscribe( this, &menu_control_window_color_picker_c::_handle_color_slider_on_value_changed );
		add_daughter_control_to_client( _s_color_slider );
		_s_text = new menu_control_text_c();
		_s_text->set_name( string8_c( "s_text", core_list_mode_e_static ) );
		_s_text->set_multi_line( false );
		_s_text->set_word_wrap( false );
		_s_text->set_character_limit( 50 );
		_s_text->set_text_filter_mode( menu_text_filter_mode_e_number_real );
		_s_text->set_text_interact_mode( menu_text_interact_mode_e_editable );
		_s_text->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right, box32x2_c( text_width, row_top, text_right, row_height ) );
		_s_text->on_plain_text_value_changed.subscribe( this, &menu_control_window_color_picker_c::_handle_text_on_value_changed );
		add_daughter_control_to_client( _s_text );
		row_top += row_height + 8;

		_v_label = new menu_control_label_c();
		_v_label->set_name( string8_c( "v_label", core_list_mode_e_static ) );
		_v_label->set_plain_text_value( string16_c( L"v", core_list_mode_e_static ) );
		_v_label->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top, box32x2_c( label_left, row_top, label_width, row_height ) );
		add_daughter_control_to_client( _v_label );
		_v_color_slider = new menu_control_color_slider_c();
		_v_color_slider->set_name( string8_c( "v_color_slider", core_list_mode_e_static ) );
		_v_color_slider->set_mode( menu_control_color_slider_c::mode_e_value );
		_v_color_slider->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( slider_left, row_top, slider_right, row_height ) );
		_v_color_slider->on_value_changed.subscribe( this, &menu_control_window_color_picker_c::_handle_color_slider_on_value_changed );
		add_daughter_control_to_client( _v_color_slider );
		_v_text = new menu_control_text_c();
		_v_text->set_name( string8_c( "v_text", core_list_mode_e_static ) );
		_v_text->set_multi_line( false );
		_v_text->set_word_wrap( false );
		_v_text->set_character_limit( 50 );
		_v_text->set_text_filter_mode( menu_text_filter_mode_e_number_real );
		_v_text->set_text_interact_mode( menu_text_interact_mode_e_editable );
		_v_text->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right, box32x2_c( text_width, row_top, text_right, row_height ) );
		_v_text->on_plain_text_value_changed.subscribe( this, &menu_control_window_color_picker_c::_handle_text_on_value_changed );
		add_daughter_control_to_client( _v_text );
		row_top += row_height + 8;

		_r_label = new menu_control_label_c();
		_r_label->set_name( string8_c( "r_label", core_list_mode_e_static ) );
		_r_label->set_plain_text_value( string16_c( L"r", core_list_mode_e_static ) );
		_r_label->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top, box32x2_c( label_left, row_top, label_width, row_height ) );
		add_daughter_control_to_client( _r_label );
		_r_color_slider = new menu_control_color_slider_c();
		_r_color_slider->set_name( string8_c( "r_color_slider", core_list_mode_e_static ) );
		_r_color_slider->set_mode( menu_control_color_slider_c::mode_e_red );
		_r_color_slider->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( slider_left, row_top, slider_right, row_height ) );
		_r_color_slider->on_value_changed.subscribe( this, &menu_control_window_color_picker_c::_handle_color_slider_on_value_changed );
		add_daughter_control_to_client( _r_color_slider );
		_r_text = new menu_control_text_c();
		_r_text->set_name( string8_c( "r_text", core_list_mode_e_static ) );
		_r_text->set_multi_line( false );
		_r_text->set_word_wrap( false );
		_r_text->set_character_limit( 50 );
		_r_text->set_text_filter_mode( menu_text_filter_mode_e_number_real );
		_r_text->set_text_interact_mode( menu_text_interact_mode_e_editable );
		_r_text->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right, box32x2_c( text_width, row_top, text_right, row_height ) );
		_r_text->on_plain_text_value_changed.subscribe( this, &menu_control_window_color_picker_c::_handle_text_on_value_changed );
		add_daughter_control_to_client( _r_text );
		row_top += row_height + 8;

		_g_label = new menu_control_label_c();
		_g_label->set_name( string8_c( "g_label", core_list_mode_e_static ) );
		_g_label->set_plain_text_value( string16_c( L"g", core_list_mode_e_static ) );
		_g_label->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top, box32x2_c( label_left, row_top, label_width, row_height ) );
		add_daughter_control_to_client( _g_label );
		_g_color_slider = new menu_control_color_slider_c();
		_g_color_slider->set_name( string8_c( "g_color_slider", core_list_mode_e_static ) );
		_g_color_slider->set_mode( menu_control_color_slider_c::mode_e_green );
		_g_color_slider->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( slider_left, row_top, slider_right, row_height ) );
		_g_color_slider->on_value_changed.subscribe( this, &menu_control_window_color_picker_c::_handle_color_slider_on_value_changed );
		add_daughter_control_to_client( _g_color_slider );
		_g_text = new menu_control_text_c();
		_g_text->set_name( string8_c( "g_text", core_list_mode_e_static ) );
		_g_text->set_multi_line( false );
		_g_text->set_word_wrap( false );
		_g_text->set_character_limit( 50 );
		_g_text->set_text_filter_mode( menu_text_filter_mode_e_number_real );
		_g_text->set_text_interact_mode( menu_text_interact_mode_e_editable );
		_g_text->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right, box32x2_c( text_width, row_top, text_right, row_height ) );
		_g_text->on_plain_text_value_changed.subscribe( this, &menu_control_window_color_picker_c::_handle_text_on_value_changed );
		add_daughter_control_to_client( _g_text );
		row_top += row_height + 8;

		_b_label = new menu_control_label_c();
		_b_label->set_name( string8_c( "b_label", core_list_mode_e_static ) );
		_b_label->set_plain_text_value( string16_c( L"b", core_list_mode_e_static ) );
		_b_label->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top, box32x2_c( label_left, row_top, label_width, row_height ) );
		add_daughter_control_to_client( _b_label );
		_b_color_slider = new menu_control_color_slider_c();
		_b_color_slider->set_name( string8_c( "b_color_slider", core_list_mode_e_static ) );
		_b_color_slider->set_mode( menu_control_color_slider_c::mode_e_blue );
		_b_color_slider->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( slider_left, row_top, slider_right, row_height ) );
		_b_color_slider->on_value_changed.subscribe( this, &menu_control_window_color_picker_c::_handle_color_slider_on_value_changed );
		add_daughter_control_to_client( _b_color_slider );
		_b_text = new menu_control_text_c();
		_b_text->set_name( string8_c( "b_text", core_list_mode_e_static ) );
		_b_text->set_multi_line( false );
		_b_text->set_word_wrap( false );
		_b_text->set_character_limit( 50 );
		_b_text->set_text_filter_mode( menu_text_filter_mode_e_number_real );
		_b_text->set_text_interact_mode( menu_text_interact_mode_e_editable );
		_b_text->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right, box32x2_c( text_width, row_top, text_right, row_height ) );
		_b_text->on_plain_text_value_changed.subscribe( this, &menu_control_window_color_picker_c::_handle_text_on_value_changed );
		add_daughter_control_to_client( _b_text );
		row_top += row_height + 8;

		_a_label = new menu_control_label_c();
		_a_label->set_name( string8_c( "a_label", core_list_mode_e_static ) );
		_a_label->set_plain_text_value( string16_c( L"a", core_list_mode_e_static ) );
		_a_label->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top, box32x2_c( label_left, row_top, label_width, row_height ) );
		add_daughter_control_to_client( _a_label );
		_a_color_slider = new menu_control_color_slider_c();
		_a_color_slider->set_name( string8_c( "a_color_slider", core_list_mode_e_static ) );
		_a_color_slider->set_mode( menu_control_color_slider_c::mode_e_alpha );
		_a_color_slider->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( slider_left, row_top, slider_right, row_height ) );
		_a_color_slider->on_value_changed.subscribe( this, &menu_control_window_color_picker_c::_handle_color_slider_on_value_changed );
		add_daughter_control_to_client( _a_color_slider );
		_a_text = new menu_control_text_c();
		_a_text->set_name( string8_c( "a_text", core_list_mode_e_static ) );
		_a_text->set_multi_line( false );
		_a_text->set_word_wrap( false );
		_a_text->set_character_limit( 50 );
		_a_text->set_text_filter_mode( menu_text_filter_mode_e_number_real );
		_a_text->set_text_interact_mode( menu_text_interact_mode_e_editable );
		_a_text->set_layout_box_anchor( menu_anchor_e_top | menu_anchor_e_right, box32x2_c( text_width, row_top, text_right, row_height ) );
		_a_text->on_plain_text_value_changed.subscribe( this, &menu_control_window_color_picker_c::_handle_text_on_value_changed );
		add_daughter_control_to_client( _a_text );
		row_top += row_height + 8;

		_rgba_hex_label = new menu_control_label_c();
		_rgba_hex_label->set_name( string8_c( "rgba_hex_label", core_list_mode_e_static ) );
		_rgba_hex_label->set_plain_text_value( string16_c( L"rgba hex", core_list_mode_e_static ) );
		_rgba_hex_label->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top, box32x2_c( label_left, row_top, label_width, row_height ) );
		add_daughter_control_to_client( _rgba_hex_label );
		_rgba_hex_text = new menu_control_text_c();
		_rgba_hex_text->set_name( string8_c( "rgba_hex_text", core_list_mode_e_static ) );
		_rgba_hex_text->set_multi_line( false );
		_rgba_hex_text->set_word_wrap( false );
		_rgba_hex_text->set_character_limit( 16 );
		_rgba_hex_text->set_text_interact_mode( menu_text_interact_mode_e_editable );
		_rgba_hex_text->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( slider_left, row_top, text_right, row_height) );
		_rgba_hex_text->on_plain_text_value_changed.subscribe( this, &menu_control_window_color_picker_c::_handle_text_on_value_changed );
		add_daughter_control_to_client( _rgba_hex_text );
		row_top += row_height + 8;

		_rgba_float_label = new menu_control_label_c();
		_rgba_float_label->set_name( string8_c( "rgba_float_label", core_list_mode_e_static ) );
		_rgba_float_label->set_plain_text_value( string16_c( L"rgba float", core_list_mode_e_static ) );
		_rgba_float_label->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top, box32x2_c( label_left, row_top, label_width, row_height ) );
		add_daughter_control_to_client( _rgba_float_label );
		_rgba_float_text = new menu_control_text_c();
		_rgba_float_text->set_name( string8_c( "rgba_float_text", core_list_mode_e_static ) );
		_rgba_float_text->set_multi_line( false );
		_rgba_float_text->set_word_wrap( false );
		_rgba_float_text->set_character_limit( 64 );
		_rgba_float_text->set_text_interact_mode( menu_text_interact_mode_e_editable );
		_rgba_float_text->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( slider_left, row_top, text_right, row_height ) );
		_rgba_float_text->on_plain_text_value_changed.subscribe( this, &menu_control_window_color_picker_c::_handle_text_on_value_changed );
		add_daughter_control_to_client( _rgba_float_text );
		row_top += row_height + 8;

		_cancel_button = new menu_control_button_c();
		_cancel_button->set_name( string8_c( "cancel_button", core_list_mode_e_static ) );
		_cancel_button->set_plain_text_value( string16_c( L"cancel", core_list_mode_e_static ) );
		_cancel_button->on_clicked.subscribe( this, &menu_control_window_color_picker_c::_handle_button_on_clicked );
		_cancel_button->set_layout_box_anchor( menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 100, 30, 116, 8 ) );
		add_daughter_control_to_client( _cancel_button );

		_okay_button = new menu_control_button_c();
		_okay_button->set_name( string8_c( "okay_button", core_list_mode_e_static ) );
		_okay_button->set_plain_text_value( string16_c( L"okay", core_list_mode_e_static ) );
		_okay_button->on_clicked.subscribe( this, &menu_control_window_color_picker_c::_handle_button_on_clicked );
		_okay_button->set_layout_box_anchor( menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 100, 30, 8, 8 ) );
		add_daughter_control_to_client( _okay_button );

		_sync_control_values();
	}

	menu_control_window_color_picker_c::~menu_control_window_color_picker_c()
	{
	}

	vector64x3_c menu_control_window_color_picker_c::get_rgb() const
	{
		return vector64x3_c( _rgba.a, _rgba.b, _rgba.c );
	}

	void_c menu_control_window_color_picker_c::set_rgb( vector64x3_c value )
	{
		_rgba.a = ops::math_saturate( value.a );
		_rgba.b = ops::math_saturate( value.b );
		_rgba.c = ops::math_saturate( value.c );
		_update_hsv_from_rgb();
		_sync_control_values();
	}

	vector64x4_c menu_control_window_color_picker_c::get_rgba() const
	{
		return _rgba;
	}

	void_c menu_control_window_color_picker_c::set_rgba( vector64x4_c value )
	{
		_rgba.a = ops::math_saturate( value.a );
		_rgba.b = ops::math_saturate( value.b );
		_rgba.c = ops::math_saturate( value.c );
		_rgba.d = ops::math_saturate( value.d );
		_update_hsv_from_rgb();
		_sync_control_values();
	}

	vector64x3_c menu_control_window_color_picker_c::get_hsv() const
	{
		return _hsv;
	}

	void_c menu_control_window_color_picker_c::set_hsv( vector64x3_c value )
	{
		_hsv.a = ops::math_clamp( value.a, 0.0, 360.0 );
		_hsv.b = ops::math_saturate( value.b );
		_hsv.c = ops::math_saturate( value.c );
		_update_rgb_from_hsv();
		_sync_control_values();
	}

	vector64x4_c menu_control_window_color_picker_c::get_hsva() const
	{
		return vector64x4_c( _hsv.a, _hsv.b, _hsv.c, _rgba.d );
	}

	void_c menu_control_window_color_picker_c::set_hsva( vector64x4_c value )
	{
		_hsv.a = ops::math_clamp( value.a, 0.0, 360.0 );
		_hsv.b = ops::math_saturate( value.b );
		_hsv.c = ops::math_saturate( value.c );
		_rgba.d = ops::math_saturate( value.d );
		_update_rgb_from_hsv();
		_sync_control_values();
	}

	float64_c menu_control_window_color_picker_c::get_alpha() const
	{
		return _rgba.d;
	}

	void_c menu_control_window_color_picker_c::set_alpha( float64_c value )
	{
		_rgba.d = ops::math_saturate( value );
		_sync_control_values();
	}

	boolean_c menu_control_window_color_picker_c::get_alpha_is_enabled() const
	{
		return _alpha_is_enabled;
	}

	void_c menu_control_window_color_picker_c::set_alpha_is_enabled( boolean_c value )
	{
		_alpha_is_enabled = value;
		if ( _alpha_is_enabled == false )
		{
			_rgba.d = 1.0;
		}
	}

}
