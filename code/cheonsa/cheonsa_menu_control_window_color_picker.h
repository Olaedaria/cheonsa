#pragma once

#include "cheonsa_menu_control_window.h"
#include "cheonsa_menu_control_button.h"
#include "cheonsa_menu_control_color_slider.h"
#include "cheonsa_menu_control_frame.h"
#include "cheonsa_menu_control_label.h"
#include "cheonsa_menu_control_text.h"

namespace cheonsa
{

	// a dialog window with a color picker.
	class menu_control_window_color_picker_c
		: public menu_control_window_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "window_color_picker"; }
		virtual inline char8_c const * get_type_name() const { return get_type_name_static(); }

		static vector32x2_c default_size; // new color picker windows will be set to this size by default.

	protected:
		boolean_c _is_muted;

		//vector64x3_c _rgb; // current color value in rgb format.
		vector64x4_c _rgba;
		vector64x3_c _hsv; // current color value in hsv format.
		//float64_c _alpha; // current alpha value. separate from _rgb so that we can use _rgb|_hsv conversion functions.
		boolean_c _alpha_is_enabled; // if true then alpha controls will be enabled.

		menu_control_color_slider_c * _swatch;

		menu_control_label_c * _h_label;
		menu_control_color_slider_c * _h_color_slider;
		menu_control_text_c * _h_text;

		menu_control_label_c * _s_label;
		menu_control_color_slider_c * _s_color_slider;
		menu_control_text_c * _s_text;

		menu_control_label_c * _v_label;
		menu_control_color_slider_c * _v_color_slider;
		menu_control_text_c * _v_text;

		menu_control_label_c * _r_label;
		menu_control_color_slider_c * _r_color_slider;
		menu_control_text_c * _r_text;

		menu_control_label_c * _g_label;
		menu_control_color_slider_c * _g_color_slider;
		menu_control_text_c * _g_text;

		menu_control_label_c * _b_label;
		menu_control_color_slider_c * _b_color_slider;
		menu_control_text_c * _b_text;

		menu_control_label_c * _a_label;
		menu_control_color_slider_c * _a_color_slider;
		menu_control_text_c * _a_text;

		menu_control_label_c * _rgba_hex_label;
		menu_control_text_c * _rgba_hex_text; // rgba hex integer value.

		menu_control_label_c * _rgba_float_label;
		menu_control_text_c * _rgba_float_text; // rgba float value.

		menu_control_button_c * _cancel_button;
		menu_control_button_c * _okay_button;

		void_c _update_rgb_from_hsv(); // calculates _rgb from _hsv.
		void_c _update_hsv_from_rgb(); // calculates _hsv from _rgb.
		void_c _sync_control_values(); // updates controls to reflect internal state.

		void_c _handle_color_slider_on_value_changed( menu_control_color_slider_c * color_slider );
		void_c _handle_text_on_value_changed( menu_control_text_c * text );
		void_c _handle_button_on_clicked( menu_event_information_c event_information );

		menu_control_window_color_picker_c( string8_c const & name );

	public:
		virtual ~menu_control_window_color_picker_c() override;

		static menu_control_window_color_picker_c * make_new_instance( string8_c const & name ); // creates a new instance on the heap with reference count of 0.

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

		boolean_c get_alpha_is_enabled() const;
		void_c set_alpha_is_enabled( boolean_c value );

		core_event_c< void_c, menu_control_window_color_picker_c * > on_value_changed;

	};

}
