#pragma once

#include "cheonsa_audio_types.h"
#include "cheonsa_audio_listener.h"
#include "cheonsa_platform_critical_section.h"
#include "cheonsa_core_list.h"

namespace cheonsa
{

	// the audio scene works with the shared audio interface to mix together sounds in 3D space.
	// uses reference counting because this instance may be shared between multiple threads.
	// only create instances on the heap with new, don't create instances on the stack.
	// once you create a new instance, the constuctor will initialize its reference count to 1.
	// when you are done with it, just call remove_reference() and forget about it, don't delete it yourself.
	class audio_scene_c
	{
	protected:
		friend class audio_interface_c;
		friend class wave_out_process_c;

		audio_interface_c * _audio_interface; // the audio interface that this scene is added to.
		audio_listener_c _audio_listener; // this must be set by the game in order for the audio sources in this audio scene to mix into the audio output stream.
		boolean_c _is_playing; // if false
		core_list_c< audio_source_c * > _audio_sources; // all of the audio sources in this scene, managed by the audio thread.
		platform_critical_section_c _critical_section; // so that the main thread or the audio mixing thread can have exclusive access to _audio_sources.
		sint32_c _reference_count; // when this reaches 0, it deletes itself.

	public:
		audio_scene_c();
		~audio_scene_c();

		audio_listener_c const & get_audio_listener() const;
		audio_listener_c & get_audio_listener();

		void_c add_source( audio_source_c * audio_source ); // adds an audio source to this scene and to the audio interface, which starts it to mix into the audio output.
		void_c remove_source( audio_source_c * audio_source ); // removes an audio source from this scene and from the audio interface, which stops it from mixing into the audio output.

		boolean_c get_is_playing() const;
		void_c set_is_playing( boolean_c value ); // can be used to pause and resume playback and mixing of sources in this scene. 

		void_c add_reference() const;
		void_c remove_reference() const;

	};

}
