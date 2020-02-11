= NOTICE OF DEPRECATION =
This project is no longer being worked on, in favour of CSE2:
https://github.com/Clownacy/Cave-Story-Engine-2

While I will try to fix any remaining bugs in this project, I will not be
expanding it with new features.


= readme.txt =
This is a DLL mod loader for the 2004 Windows version of Cave Story.
Also included in this repo are some mods.

Everything is designed to be compiled with MinGW-w64 in an MSYS2 environment.
https://www.msys2.org/


Dependencies:

sdl_controller_input requires SDL2 (available on MSYS2's Pacman).

ogg_music requires libFLAC (available on Pacman), as well as
libxmp-lite (not on Pacman - you'll have to grab it yourself).

https://www.libsdl.org/
https://xiph.org/flac/
https://github.com/cmatsuoka/libxmp/


The Makefile is designed to link all libraries statically, so their respective
dependencies apply as well.


More info here:
https://www.cavestory.org/forums/threads/dll-mod-loader-ogg-music-mod.13879/
