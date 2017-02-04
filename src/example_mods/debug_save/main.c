#include <windows.h>

#include "mod_loader.h"

__declspec(dllexport) void InitMod(HMODULE mod_loader_hmodule)
{
	GetModLoaderFunctions(mod_loader_hmodule);
	// Stop Debug Save menu from being deleted
	WriteByte(0x412D4D, 0xEB);
	WriteByte(0x412D4D + 1, 0x0F);
}
