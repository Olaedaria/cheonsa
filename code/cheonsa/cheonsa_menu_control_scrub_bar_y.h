#pragma once

#include "cheonsa_menu_control_scroll_bar_i.h"

namespace cheonsa
{

	class menu_control_scrub_bar_y_c
		: public menu_control_scroll_bar_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "scrub_bar_y"; }
		virtual inline char8_c const * get_type_name() const { return get_type_name_static(); }

	public:
		menu_control_scrub_bar_y_c();

	};

}
