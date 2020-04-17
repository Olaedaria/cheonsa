#pragma once

#include "cheonsa_menu_control_scroll_bar_i.h"

namespace cheonsa
{

	// horizontal orientation scroll bar.
	// while it is possible with this ui system to use the angle property to set the orientation of the scroll bar, it is unintuitive to think about box anchor layouts with angles, so we provide both horizontal and vertical classes separately.
	class menu_control_scroll_bar_x_c
		: public menu_control_scroll_bar_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "scroll_bar_x"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		menu_control_scroll_bar_x_c( string8_c const & name );

	};

}
