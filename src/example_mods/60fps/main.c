#include <windows.h>

#include "mod_loader.h"

__declspec(dllexport) void InitMod(HMODULE mod_loader_hmodule)
{
	GetModLoaderFunctions(mod_loader_hmodule);
	// 60 FPS
	WriteByte(0x40B36F, 17);
	WriteByte(0x40B3A9, 17);
}
