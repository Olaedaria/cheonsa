#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_control_text.h"
#include "cheonsa_menu_control_scroll_bar.h"

namespace cheonsa
{

	// h = hue.
	// s = saturation.
	// v = value.
	// r = red.
	// g = green.
	// b = blue.
	// a = alpha (used for transparency, 0 is fully transparent, 1 is fully opaque).
	class menu_control_color_picker_c : public menu_control_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "color_picker"; }
		virtual inline char8_c const * get_type_name() const { return get_type_name_static(); }

	protected:
		menu_element_frame_c _element_frame; // name is "frame", forms background of this control.

		vector64x3_c _rgb; // current color value in rgb format.
		vector64x3_c _hsv; // current color value in hsv format.
		float64_c _alpha; // current alpha value. separate from _rgb so that we can use _rgb|_hsv conversion functions.
		boolean_c _alpha_enable; // if true then alpha controls will be enabled.

		menu_control_scroll_bar_horizontal_c * _h_scroll_bar;
		menu_control_text_c * _h_text;

		menu_control_scroll_bar_horizontal_c * _s_scroll_bar;
		menu_control_text_c * _s_text;

		menu_control_scroll_bar_horizontal_c * _v_scroll_bar;
		menu_control_text_c * _v_text;

		menu_control_scroll_bar_horizontal_c * _r_scroll_bar;
		menu_control_text_c * _r_text;

		menu_control_scroll_bar_horizontal_c * _g_scroll_bar;
		menu_control_text_c * _g_text;

		menu_control_scroll_bar_horizontal_c * _b_scroll_bar;
		menu_control_text_c * _b_text;

		menu_control_scroll_bar_horizontal_c * _a_scroll_bar;
		menu_control_text_c * _a_text;

		menu_control_text_c * _rgba_hex_text; // rgba hex integer value.
		menu_control_text_c * _rgba_float_text; // rgba float value.

		void_c _update_rgb_from_hsv(); // calculates _rgb from _hsv.
		void_c _update_hsv_from_rgb(); // calculates _hsv from _rgb.
		void_c _sync_control_values(); // updates controls to reflect internal state.

		void_c _handle_slider_on_value_changed( menu_control_scroll_i * slider );
		void_c _handle_text_on_value_changed( menu_control_text_c * text );

	public:
		menu_control_color_picker_c();

		vector64x3_c get_rgb() const;
		void_c set_rgb( vector64x3_c value );

		vector64x4_c get_rgba() const;
		void_c set_rgba( vector64x4_c value );

		vector64x3_c get_hsv() const;
		void_c set_hsv( vector64x3_c value );

		vector64x4_c get_hsva() const;
		void_c set_hsva( vector64x4_c value );

		float64_c get_alpha() const;
		void_c set_alpha( float64_c value );

		boolean_c get_alpha_enable() const;
		void_c set_alpha_enable( boolean_c value );

		core_event_c< void_c, menu_control_color_picker_c * > on_value_changed;

	};

}
