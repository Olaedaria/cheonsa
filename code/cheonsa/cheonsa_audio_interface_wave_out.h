#pragma once

#include "cheonsa_audio_interface.h"
#include "cheonsa___build.h"

#if defined( cheonsa_platform_windows )

namespace cheonsa
{

	// audio interface for windows.
	class audio_interface_wave_out_c
		: public audio_interface_c
	{
	protected:
		friend class wave_out_process_c;

	public:
		audio_interface_wave_out_c();
		virtual ~audio_interface_wave_out_c() override;

		virtual boolean_c start() override;

	};

}

#endif
