#include "mod_loader.h"

__declspec(dllexport) void InitMod(void)
{
	// 60 FPS
	WriteByte(0x40B36F, 17);
	WriteByte(0x40B3A9, 17);
}
