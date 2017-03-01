#include "mod_loader_main.h"

const char* const CURSOR_IKA = "CURSOR_IKA";

void InitMod(void)
{
	WriteLong(0x41272B + 1, (int)CURSOR_IKA);
}
