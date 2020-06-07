#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_control_scroll_bar_x.h"
#include "cheonsa_menu_control_scroll_bar_y.h"
#include "cheonsa_menu_element_frame.h"

namespace cheonsa
{

	// provides a basic private implementation for menu_control_list_c, menu_control_menu_c, and menu_control_combo_list_c.
	// holds daughter controls within a _client daughter control, which is sized to fit within specified client margins.
	// vertical and horizontal scroll bars may optionally be enabled or disabled.
	// the scroll bars overlap the right and bottom edges of the _client daughter control, so they may inadvertently obstruct some of the contents of the _client.
	class menu_control_panel_i
		: public menu_control_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "panel_i"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }
	
	protected:
		menu_element_frame_c _frame_element; // name is "frame", draws the background of this panel.
		menu_element_frame_c _border_frame_element; // name is "border_frame".
		
		menu_control_c * _client; // name is "client". panel control that makes up the client area and publicly accessable controls, and for the scroll bars to plug their values into.

		box32x2_c const & _get_client_margins() const;
		void_c _set_client_margins( box32x2_c const & value );

		menu_visibility_mode_e _horizontal_scroll_bar_visibility_mode;
		menu_control_scroll_bar_x_c * _horizontal_scroll_bar;
		menu_visibility_mode_e _get_horizontal_scroll_bar_visibility_mode() const;
		void_c _set_horizontal_scroll_bar_visibility_mode( menu_visibility_mode_e value );
		void_c _lay_out_horizontal_scroll_bar();

		menu_visibility_mode_e _vertical_scroll_bar_visibility_mode;
		menu_control_scroll_bar_y_c * _vertical_scroll_bar;
		menu_visibility_mode_e _get_vertical_scroll_bar_visibility_mode() const;
		void_c _set_vertical_scroll_bar_visibility_mode( menu_visibility_mode_e value );
		void_c _lay_out_vertical_scroll_bar();

		void_c _update_scroll_bar_visibility(); // evaluates content size and updates visibility of scroll bars accordingly.

		void_c _handle_scroll_bar_on_value_changed( menu_control_scroll_bar_i * scroll_bar );
		void_c _handle_scroll_bar_on_preferred_thickness_changed( menu_control_scroll_bar_i * scroll_bar );

		menu_control_panel_i();

	public:
		virtual void_c update_animations( float32_c time_step ) override;

	};

}
