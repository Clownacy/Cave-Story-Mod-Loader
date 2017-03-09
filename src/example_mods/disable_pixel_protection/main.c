#include "mod_loader.h"

int PixelProtection(void)
{
	return 1;
}

void InitMod(void)
{
	WriteJump(0x4112E0, PixelProtection);
}
