// Image integrity check disabler mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "mod_loader.h"

static int PixelProtection(void)
{
	return 1;
}

void InitMod(void)
{
	ModLoader_WriteJump((void*)0x4112E0, PixelProtection);
}
