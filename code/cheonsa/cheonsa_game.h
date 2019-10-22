#pragma once

#include "cheonsa_base_types.h"

namespace cheonsa
{

	class resource_object_sprite_set_c;

	class game_c
	{
	public:
		virtual void_c start( char16_c const * const * arguments, sint32_c arguments_count ) = 0; // occurs once right after the engine starts.
		virtual void_c stop() = 0; // occurs once right before the engine stops.

		virtual void_c process_input_event( input_event_c * input_event ) = 0; // occurs any number of times per frame, before update is called.

		virtual void_c update( float32_c time_step ) = 0; // occurs once per frame, after all input events are processed.

		//virtual void_c render( float32_c time_step ) = 0; // occurs as frequently as possible but not more than once per frame.

		//virtual boolean_c resolve_string( string8_c const & key, string8_c & result ) = 0; // called by text elements that contain game string references when they lay out their glyphs.
		//virtual boolean_c resolve_sprite( string8_c const & key, resource_object_sprite_set_c::reference_c & result ) = 0; // called by text elements that contain sprite references when they lay out their glyphs.

	};

}
