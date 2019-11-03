# cheonsa

cheonsa is a 3D game engine.

it is made with Visual Studio Community 2017, written in C++11, for Direct3D 11 and Windows 10 (desktop).

it is currently not feature complete, so i am not yet going to write a guide for how to build and use it. i don't think it's worth trying to build yet, but if you want to here are some hints: it is configured for x64 and for static linking with the multi-threaded C runtime. it depends on freetype 2.10.1 ("freetype.lib") and bullet3 2.88 ("LinearMath_vs2010_x64_debug.lib", "BulletCollision_vs2010_x64_debug.lib", and "BulletDynamics_vs2010_x64_debug.lib"). i have included the bare minimum headers for freetype and bullet already, so all you need to do is provide those static link libraries. the engine once running will look for a copy of "d3dcompiler_47.dll" in the "[cheonsa]/data/engine/" folder.

# Additional
Portions of this software are copyright © 2019 The FreeType Project (www.freetype.org). All rights reserved.

Additional third-party dependencies have their own separate licenses, listed in their respective source code files.
