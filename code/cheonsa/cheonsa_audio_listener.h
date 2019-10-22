#pragma once

#include "cheonsa_math_types.h"

namespace cheonsa
{

	// defines how to mix 3d audio sources into the final output.
	class audio_listener_c
	{
	private:
		friend class wave_out_process_c;

		space_transform_c const * _world_space_transform; // usually will be set to point to the world space transform of the scene's camera.
		float32_c _damping; // factor that affects the loss of energy of sound waves as they propagate through a medium.
		float32_c _volume; // linear scale of sound waves that are mixed for this listener.

	public:
		audio_listener_c();

		float32_c get_damping() const;
		void_c set_damping( float32_c value );

		float32_c get_volume() const;
		void_c set_volume( float32_c value );

		void_c set_world_space_transform( space_transform_c const * value );

	};

}
