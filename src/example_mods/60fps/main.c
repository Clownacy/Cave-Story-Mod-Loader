// 60FPS mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "mod_loader.h"

#define FPS 60
#define FPS_TICKS (1000 + (FPS / 2)) / FPS	// Round to nearest whole number, instead of just flooring

void InitMod(void)
{
	WriteByte((void*)0x40B36F, FPS_TICKS);
	WriteByte((void*)0x40B3A9, FPS_TICKS);
}
