#include <windows.h>

const char* const CURSOR_IKA = "CURSOR_IKA";

__declspec(dllexport) void InitMod(HMODULE mod_loader_hmodule)
{
	WriteProcessMemory(GetCurrentProcess(), (void*)0x41272B + 1, &CURSOR_IKA, 4, NULL);
}
