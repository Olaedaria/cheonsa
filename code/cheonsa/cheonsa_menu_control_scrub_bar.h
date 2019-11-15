#pragma once

#include "cheonsa_menu_control_scroll_i.h"

namespace cheonsa
{

	class menu_control_scrub_bar_horizontal_c
		: public menu_control_scroll_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "scrub_bar_horizontal"; }
		virtual inline char8_c const * get_type_name() const { return get_type_name_static(); }

	public:
		menu_control_scrub_bar_horizontal_c();

	};

	class menu_control_scrub_bar_vertical_c
		: public menu_control_scroll_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "scrub_bar_vertical"; }
		virtual inline char8_c const * get_type_name() const { return get_type_name_static(); }

	public:
		menu_control_scrub_bar_vertical_c();

	};

}
