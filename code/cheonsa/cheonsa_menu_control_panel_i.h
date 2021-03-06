#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_control_scroll_bar_x.h"
#include "cheonsa_menu_control_scroll_bar_y.h"
#include "cheonsa_menu_element_frame.h"

namespace cheonsa
{

	// provides a basic private implementation for menu_control_list_c, menu_control_menu_c, and menu_control_combo_c.
	// holds daughter controls within a _client daughter control, which is sized to fit within specified client margins.
	// vertical and horizontal scroll bars may optionally be enabled or disabled.
	// the scroll bars overlap the right and bottom edges of the _client daughter control, so they may inadvertently obstruct some of the contents of the _client.
	class menu_control_panel_i
		: public menu_control_c
	{

		// overrides some first responders so we can route them through the mother panel instead.
		// users will subscribe to those event handlers instead of these.
		// we only override three here, the other ones are okay as they are.
		class menu_control_client_c
			: public menu_control_c
		{
			virtual void_c _on_clicked( input_event_c * input_event ) override;
			virtual void_c _on_multi_clicked( input_event_c * input_event ) override;
			virtual void_c _on_input( input_event_c * input_event ) override;

		};

	public:
		static inline char8_c const * get_type_name_static() { return "panel_i"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }
	
	protected:
		menu_element_frame_c _frame_element; // name is "frame", draws the background of this panel.
		menu_element_frame_c _border_frame_element; // name is "border_frame".

		box32x2_c _client_margins;
		menu_control_c * _client; // name is "client". panel control that makes up the client area and publicly accessable controls, and for the scroll bars to plug their values into.

		box32x2_c const & _get_client_margins() const;
		void_c _set_client_margins( box32x2_c const & value );

		menu_visibility_mode_e _scroll_bar_x_visibility_mode;
		menu_control_scroll_bar_x_c * _scroll_bar_x;
		menu_visibility_mode_e _get_scroll_bar_x_visibility_mode() const;
		void_c _set_scroll_bar_x_visibility_mode( menu_visibility_mode_e value );
		void_c _lay_out_scroll_bar_x();

		menu_visibility_mode_e _scroll_bar_y_visibility_mode;
		menu_control_scroll_bar_y_c * _scroll_bar_y;
		menu_visibility_mode_e _get_scroll_bar_y_visibility_mode() const;
		void_c _set_scroll_bar_y_visibility_mode( menu_visibility_mode_e value );
		void_c _lay_out_scroll_bar_y();

		virtual void_c _update_transform_and_layout() override;

		void_c _update_scroll_bar_visibility(); // evaluates content size and updates visibility of scroll bars accordingly.

		void_c _handle_scroll_bar_on_value_changed( menu_control_scroll_bar_i * scroll_bar );
		void_c _handle_scroll_bar_on_preferred_thickness_changed( menu_control_scroll_bar_i * scroll_bar );

		menu_control_panel_i();

	public:
		virtual void_c update_animations( float32_c time_step ) override;

	};

}
