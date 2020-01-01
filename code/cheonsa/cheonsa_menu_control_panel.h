#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_control_scroll_bar.h"
#include "cheonsa_menu_element_frame.h"

namespace cheonsa
{

	// holds publically accessable daughter controls.
	// also manages scroll bars to scroll contents.
	class menu_control_panel_c : public menu_control_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "panel"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		menu_element_frame_c _element_frame; // name is "frame".
		menu_control_c * _client; // panel control that makes up the client area and publicly accessable controls, and for the scroll bars to plug their values into.
		menu_visibility_mode_e _horizontal_scroll_bar_visibility_mode;
		menu_control_scroll_bar_horizontal_c * _horizontal_scroll_bar;
		menu_visibility_mode_e _vertical_scroll_bar_visibility_mode;
		menu_control_scroll_bar_vertical_c * _vertical_scroll_bar;
		void_c _handle_scroll_on_value_changed( menu_control_scroll_i * scroll );

	public:
		menu_control_panel_c();

		virtual void_c update_animations( float32_c time_step ) override;

		void_c give_control( menu_control_c * control );
		menu_control_c * take_control( menu_control_c * control );

		menu_visibility_mode_e get_horizontal_scroll_visibitly_mode() const;
		void_c set_horizontal_scroll_visibility_mode( menu_visibility_mode_e value );

		menu_visibility_mode_e get_vertical_scroll_visibility_mode() const;
		void_c set_vertical_scroll_visibility_mode( menu_visibility_mode_e value );

	};

}
