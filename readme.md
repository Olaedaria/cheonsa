# cheonsa

support me on [SubscribeStar](https://subscribestar.adult/olaedaria)!

[documentation](https://olaedaria.github.io/cheonsa/) (place holder at the moment)

## how to build

these are just hints.

use microsoft visual studio community 2017.

cheonsa's solution is configured out of the box to build:
* debug
* to target x64
* to static link with Multi-threaded Debug (/MTd).

at build time, cheonsa depends on four static link library files from two 3rd party projects. you will need to build (with the same configuration as cheonsa) these static link libraries yourself. they are from open source projects so you can find their source code easily with some searching, but figuring out how to build them might take longer.

* from "free type 2.10.1"
    * "freetype.lib"
* from "bullet3 2.88"
    * "LinearMath_vs2010_x64_debug.lib"
    * "BulletCollision_vs2010_x64_debug.lib"
    * "BulletDynamics_vs2010_x64_debug.lib"

once you have the four static link library files, copy them to cheonsa's build output folder at "[project folder]\code\x64\Debug Static" so that the linker can find them. at this point, you can try building to see if it works.

at run time, cheonsa depends on one dynamic link library from the windows sdk (or maybe even the directx sdk):

* "d3dcompiler_47.dll"

this is included with the windows sdk, which you should have if you installed visual studio community. on my system this is located at "C:\Program Files (x86)\Windows Kits\10\Redist\D3D\x64\d3dcompiler_47.dll". once you have the dynamic link library file, copy it to "[project folder]\data\engine\d3dcompiler_47.dll". the engine will look for this dll at start up.
