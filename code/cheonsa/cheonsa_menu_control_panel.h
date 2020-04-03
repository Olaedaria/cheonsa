#pragma once

#include "cheonsa_menu_control_panel_i.h"

namespace cheonsa
{

	// holds daughter controls within a _client daughter control, which is sized to fit within _client_margins.
	// optionally, vertical and horizontal scroll bars may be enabled or disabled, automatically or manually.
	// the scroll bars overlap the right and bottom edges of the _client control, they are not daughters of the _client control so they may inadvertently obstruct some of the contents of the _client control.
	// generic implementation.
	class menu_control_panel_c
		: public menu_control_panel_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "panel"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		menu_control_panel_c();

		sint32_c give_control( menu_control_c * control, sint32_c index = -1 ); // gives a control to client area control.
		menu_control_c * take_control( sint32_c control_index ); // takes a control from client area control.

		menu_visibility_mode_e get_horizontal_scroll_bar_visibility_mode() const;
		void_c set_horizontal_scroll_bar_visibility_mode( menu_visibility_mode_e value );

		menu_visibility_mode_e get_vertical_scroll_bar_visibility_mode() const;
		void_c set_vertical_scroll_bar_visibility_mode( menu_visibility_mode_e value );

	};

}
