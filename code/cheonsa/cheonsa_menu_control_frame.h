#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_element_frame.h"

namespace cheonsa
{

	class menu_control_frame_c
		: public menu_control_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "frame"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		menu_element_frame_c _element_frame; // name is "frame".

	public:
		menu_control_frame_c( string8_c const & name );

	};

}
