// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2018 Clownacy

#include "upscale_window.h"

#include "mod_loader.h"

#include "../common.h"

void UpscaleWindow(void)
{
	PrintDebug("Applying 'upscale window' patch\n");

	// Patch window creation
	WriteLong((void*)0x40B4D1 + 6, window_upscale_factor);
	WriteLong((void*)0x40B4FE + 6, window_upscale_factor);
	WriteLong((void*)0x4127D2 + 6, SCREEN_WIDTH * window_upscale_factor);
	WriteLong((void*)0x4127DC + 6, 240 * window_upscale_factor);
	// Patch font creation
	WriteByte((void*)0x40CD64 + 3, window_upscale_factor);

	const int font_width = window_upscale_factor == 1 ? 6 : window_upscale_factor * 5;
	const int font_height = window_upscale_factor == 1 ? 12 : window_upscale_factor * 10;
	WriteLong((void*)0x40CD7C + 3, font_height);
	WriteLong((void*)0x40CD83 + 3, font_width);
}
