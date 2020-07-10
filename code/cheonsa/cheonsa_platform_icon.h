#pragma once

#include "cheonsa__types.h"
#include "cheonsa_string16.h"

namespace cheonsa
{

	// operating system specific window icon implementation.
	// lets you load an icon from a file in the file system.
	// lets you load an icon that is packed in the exe.
	// on windows 10:
	//     uses LoadImage under the hood, so it can load whatever formats that can load, which as far as i know isn't documented on docs.microsoft.com...
	//     i know from experience that it can at least load ".png" and ".ico" files, but maybe it can also load some other formats too.
	// optimal icon sizes in windows 10:
	//     i think that alt+tab task switcher icons and task bar icons (large icon setting) are 32x32.
	//     i think that window title bar icons and task bar icons (small icon setting) are 16x16.
	// optimal icon sizes in windows vista/7/8:
	//     i don't know, but maybe it doesn't matter any more.
	// i think the most optimal thing to do is:
	//     create and load a single ".ico" file that contains multiple images at different sizes:
	//         16, 24, 32, 48, 64, 96, 128, 256.
	//     this way windows can pick the size that will work best for what it needs.
	class platform_icon_c
	{
	public:
		void_c * icon_handle;

	public:
		platform_icon_c();
		~platform_icon_c();

		// loads an icon that is compiled (packed) as a "resource" into the exe.
		boolean_c load_from_exe( string8_c const & resource_name );

		// loads an icon that is compiled (packed) as a "resource" into the exe.
		boolean_c load_from_exe( string16_c const & resource_name );

		// loads an icon from the file system, using a file path that is absolute.
		boolean_c load_from_absolute_file_path( string16_c const & file_path_absolute );

		// loads an icon from a the file system, using a file path that is relative to the current set of data folders defined by the engine's content manager.
		boolean_c load_from_relative_file_path( string16_c const & file_path_relative );

		// unloads the current icon.
		void_c unload();

	};

}
