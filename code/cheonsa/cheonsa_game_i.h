#pragma once

#include "cheonsa__types.h"
#include "cheonsa_input_types.h"
#include "cheonsa_string16.h"

namespace cheonsa
{

	// base game interface class.
	// keep it simple.
	class game_i
	{
	public:
		virtual void_c start( char16_c const * const * arguments, sint32_c arguments_count ) = 0; // occurs once right after the engine starts.
		virtual void_c stop() = 0; // occurs once right before the engine stops.

		virtual void_c process_input_event( input_event_c * input_event ) = 0; // occurs any number of times per frame, before update is called.

		virtual void_c update( float32_c time_step ) = 0; // occurs once per frame, after all input events are processed.

		virtual void_c refresh() = 0; // may occur between updates. if applicable to the game's design, the game should scan its files for changes and reload as needed.

		virtual string16_c get_user_data_folder_name() const = 0; // user generated setting files, save game files, and screen shot files should be saved in a folder with this name.

	};

}
