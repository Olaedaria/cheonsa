#pragma once

#include "cheonsa_menu_control_scroll_bar_i.h"

namespace cheonsa
{

	class menu_control_scroll_bar_x_c
		: public menu_control_scroll_bar_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "scroll_bar_x"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		menu_control_scroll_bar_x_c();

	};

}
