This is a DLL mod loader for the 2004 Windows version of Cave Story. Also included in this repo are some example mods.

Everything is designed to be compiled with MinGW-w64, particularly in an MSYS2 environment
https://www.msys2.org/

Dependencies:

sdl_controller_input requires SDL2
https://libsdl.org/

ogg_music requires SDL2, libogg, and libvorbis
https://libsdl.org/
https://github.com/xiph/ogg
https://github.com/xiph/vorbis

The Cubeb version requires Cubeb instead of SDL2
https://github.com/kinetiknz/cubeb

The mod loader itself requires the inih library (its source is already included in the repo)
https://github.com/benhoyt/inih

More info here:
https://www.cavestory.org/forums/threads/dll-mod-loader-ogg-music-mod.13879/
