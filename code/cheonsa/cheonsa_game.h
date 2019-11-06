#pragma once

#include "cheonsa__types.h"

namespace cheonsa
{

	class resource_file_sprite_set_c;

	class game_c
	{
	public:
		virtual void_c start( char16_c const * const * arguments, sint32_c arguments_count ) = 0; // occurs once right after the engine starts.
		virtual void_c stop() = 0; // occurs once right before the engine stops.

		virtual void_c process_input_event( input_event_c * input_event ) = 0; // occurs any number of times per frame, before update is called.

		virtual void_c update( float32_c time_step ) = 0; // occurs once per frame, after all input events are processed.

	};

}
