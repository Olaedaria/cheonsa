#pragma once

#include "cheonsa_audio_types.h"

namespace cheonsa
{

	// abstract audio player interface instance.
	// an audio player instance decodes an audio stream from the audio buffer so that it can be mixed into the output stream.
	// uses reference counting because this instance may be shared between multiple threads.
	// only create instances on the heap with new, don't create instances on the stack.
	// once you create a new instance, the constuctor will initialize its reference count to 1.
	// when you are done with it, just call remove_reference() and forget about it, don't delete it yourself.
	class audio_player_c
	{
	protected:
		friend class audio_music_c;
		friend class audio_source_c;
		friend class wave_out_process_c;

		// number of users of this resource.
		// this resource will delete itself when this reference count reaches zero via a call to remove_reference().
		sint32_c _reference_count;

		// the buffer that contains the audio that this player is sampling audio from.
		audio_buffer_c const * _audio_buffer;

		// will be set to true as long as the player is playing.
		// will be set to false once the player reaches the end of the buffer.
		boolean_c _is_playing;

		// called by the audio thread.
		// decodes and samples from the underlying audio buffer stream, mixes into the provided output buffer, advances the stream by the number of samples consumed.
		// _is_playing will be set to false if the end of the stream is reached.
		// the format of the audio buffer stream and the format of the mix may be different, in which case conversions are performed automatically.
		// output_channel_volumes is an array of volumes to use for each channel, the 3d mixer will use this to make it sound 3d.
		// returns true if any audio was sampled.
		// returns false if end of audio buffer stream was reached or if some kind of error was encountered.
		virtual void_c _sample_and_mix( float64_c play_back_speed, sint32_c output_channel_count, sint32_c output_sample_rate, sint32_c output_sample_buffer_length, float32_c * output_sample_buffer, float32_c * output_channel_volumes ) = 0;

	public:
		audio_player_c();
		virtual ~audio_player_c() = 0;

		// gets the current playing state of this player.
		// so that the game can restart the play back if it wants to.
		boolean_c get_is_playing() const;

		// starts or restarts play back of this audio player.
		// if randomize_seek is true then play back will start at a random position within the stream, otherwise play back will start at the start of the stream.
		// the game should randomize seek for ambient sound texture emitters in the scene so that they aren't all synchronized upon loading in to the scene.
		virtual void_c play( boolean_c randomize_seek ) = 0;

		virtual void_c add_reference() const;
		virtual void_c remove_reference() const;

	};

}
