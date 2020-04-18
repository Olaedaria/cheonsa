#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_element_mesh.h"
#include "cheonsa_menu_element_frame.h"
#include "cheonsa_menu_element_text.h"

namespace cheonsa
{

	// a slider that can be configured for: red, green, blue, alpha, hue, saturation, value, lightness.
	class menu_control_color_slider_c
		: public menu_control_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "color_slider"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		enum mode_e
		{
			mode_e_hue, // slider lets the user pick a value between 0 and 1.
			mode_e_red, // slider lets the user pick a value between 0 and 1.
			mode_e_green, // slider lets the user pick a value between 0 and 1.
			mode_e_blue, // slider lets the user pick a value between 0 and 1.
			mode_e_alpha, // slider lets the user pick a value between 0 and 1.
			mode_e_saturation, // slider lets the user pick a value between 0 and 1, is gradient colored from white to fully saturated.
			mode_e_value, // slider lets the user pick a value between 0 and 1, is gradient colored from black to fully saturated.
			mode_e_swatch, // displays a single solid color, no slider.
		};

	protected:
		menu_element_mesh_c _element_mesh;
		menu_element_frame_c _element_slider;
		menu_element_frame_c _element_border_frame;

		mode_e _mode;

		float64_c _value; // this is the value of the slider, between 0 or 1, or 0 and 360 (depending on the mode).
		vector32x3_c _color; // if the slider is operating in saturation, value, or lightness mode, then this is the color to blend with. it should be fully saturated.

		box32x2_c _mesh_box;
		void_c _build_mesh(); // builds the mesh. _update_mesh_colors() needs to be called afterward.
		void_c _update_mesh_colors(); // sets the colors on the vertices of the already built mesh.

		boolean_c _mouse_is_grabbed;

		virtual void_c _on_is_mouse_focused_changed() override;
		virtual void_c _on_input( input_event_c * input_event ) override;
		
		virtual void_c _update_transform_and_layout() override; // updates layout of elements based on current mode and value.

	public:
		menu_control_color_slider_c( string8_c const & name );

		virtual void_c update_animations( float32_c time_step ) override;

		mode_e get_mode() const;
		void_c set_mode( mode_e value );

		float64_c get_value() const;
		void_c set_value( float64_c value );

		vector32x3_c const & get_color() const;
		void_c set_color( vector32x3_c const & value );
		void_c set_color( vector32x4_c const & value );

		core_event_c< void_c, menu_control_color_slider_c * > on_value_changed;

	};

}
