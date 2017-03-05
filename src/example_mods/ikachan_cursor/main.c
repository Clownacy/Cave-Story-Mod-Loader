#include "mod_loader_main.h"

const char* const cursor_filename = "CURSOR_IKA";

void InitMod(void)
{
	WriteLong(0x41272B + 1, (int)cursor_filename);
}
