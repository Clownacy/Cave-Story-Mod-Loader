// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "mod_loader.h"

#include "common.h"
#include "borderless_fullscreen/borderless_fullscreen.h"
#include "sprite_resolution/sprite_resolution.h"
#include "upscale_window/upscale_window.h"
#include "widescreen/widescreen.h"

void InitMod(void)
{
	// Disable window.rect (this entire mod breaks it anyway)
	WriteByte((void*)0x40F705, 0xEB);
	WriteByte((void*)0x412DC3, 0xEB);

	borderless_fullscreen = (strcmp(GetSetting("borderless_fullscreen"), "true") == 0);
	borderless_fullscreen_auto_aspect_ratio = (strcmp(GetSetting("borderless_fullscreen_auto_aspect_ratio"), "true") == 0);
	borderless_fullscreen_auto_window_upscale = (strcmp(GetSetting("borderless_fullscreen_auto_window_upscale"), "true") == 0);

	int _aspect_ratio_x = strtol(GetSetting("aspect_ratio_x"), NULL, 10);
	if (_aspect_ratio_x == 0)
		_aspect_ratio_x = 16;

	aspect_ratio_x = _aspect_ratio_x;

	int _aspect_ratio_y = strtol(GetSetting("aspect_ratio_y"), NULL, 10);
	if (_aspect_ratio_y == 0)
		_aspect_ratio_y = 9;

	aspect_ratio_y = _aspect_ratio_y;

	int sprite_resolution_factor = strtol(GetSetting("sprite_resolution"), NULL, 10);
	if (sprite_resolution_factor == 0)
		sprite_resolution_factor = 1;

	int _window_upscale_factor = strtol(GetSetting("window_upscale"), NULL, 10);
	if (_window_upscale_factor == 0)
		_window_upscale_factor = 1;

	window_upscale_factor = _window_upscale_factor;

	if (borderless_fullscreen)
		ApplyBorderlessFullscreenPatch();
	
	SetWidescreen();

	SetSpriteResolution(sprite_resolution_factor);

	UpscaleWindow();
}
