#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_element_text.h"

namespace cheonsa
{

	// shows per-frame run-time statistics.
	// it grabs these statistics from the engine's user interface's canvas.
	// static vertex count, cpu skinned vertex count, gpu skinned vertex count, bone count, triangle count, shadow view count, mirror view count, etc.
	class menu_control_debug_statistics_c
		: public menu_control_c
	{
	private:
		menu_element_text_c _text_element;

	public:
		menu_control_debug_statistics_c();

		virtual void_c update_animations( float32_c time_step ) override;

	};

}
