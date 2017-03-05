// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "mod_loader.h"

#include "../common.h"
#include "upscale_window.h"

void UpscaleWindow(const int upscale_factor)
{
	// Patch window creation
	WriteLong(0x40B4D1 + 6, upscale_factor);
	WriteLong(0x4127D2 + 6, SCREEN_WIDTH * upscale_factor);
	WriteLong(0x4127DC + 6, 240 * upscale_factor);
	// Patch font creation
	WriteByte(0x40CD64 + 3, upscale_factor);
	WriteLong(0x40CD7C + 3, 4 + (8 * upscale_factor));
	WriteLong(0x40CD83 + 3, 2 + (4 * upscale_factor));
}
