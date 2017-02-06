#include <windows.h>

#include "mod_loader.h"

const char* const CURSOR_IKA = "CURSOR_IKA";

void InitMod(void)
{
	WriteProcessMemory(GetCurrentProcess(), (void*)0x41272B + 1, &CURSOR_IKA, 4, NULL);
}
