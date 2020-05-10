#pragma once

#include "cheonsa_menu_control_scroll_bar_i.h"

namespace cheonsa
{

	// vertical orientation scrub bar.
	// while it is possible with this ui system to use the angle property to set the orientation of the scroll bar, it is unintuitive to think about box anchor layouts with angles, so we provide both horizontal and vertical classes separately.
	class menu_control_scrub_bar_y_c
		: public menu_control_scroll_bar_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "scrub_bar_y"; }
		virtual inline char8_c const * get_type_name() const { return get_type_name_static(); }

	protected:
		menu_control_scrub_bar_y_c( string8_c const & name );

	public:
		virtual ~menu_control_scrub_bar_y_c() override;

		static menu_control_scrub_bar_y_c * make_new_instance( string8_c const & name ); // creates a new instance on the heap with reference count of 0.

	};

}
