#pragma once

#include "cheonsa_audio_types.h"

namespace cheonsa
{

	// abstract audio buffer interface.
	// uses reference counting because this instance may be shared between multiple threads.
	// only create instances on the heap with new, don't create instances on the stack.
	// once you create a new instance, the constuctor will initialize its reference count to 1.
	// when you are done with it, just call remove_reference() and forget about it, don't delete it yourself.
	class audio_buffer_c
	{
	protected:
		sint32_c _reference_count;
		boolean_c _is_loaded;

	public:
		audio_buffer_c();
		virtual ~audio_buffer_c() = 0;

		// audio buffer has to be loaded at constructor time.
		// just to avoid issues that could arise from trying to modify the audio buffer's data during run time because multiple threads will be accessing it concurrently.
		// this lets the game determine if the load succeeded or not, and how to proceed.
		boolean_c get_is_loaded() const;

		void_c add_reference() const;
		void_c remove_reference() const;

		// makes a new audio player of the appropriate type that sources this audio buffer.
		// when you're done with it just call remove_reference() and forget about it.
		virtual audio_player_c * spawn_new_audio_player() const = 0;

	};

}
