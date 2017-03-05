// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <stdlib.h>

#include "mod_loader.h"

#include "common.h"
#include "sprite_resolution/sprite_resolution.h"
#include "upscale_window/upscale_window.h"
#include "widescreen/widescreen.h"

void InitMod(void)
{
	int _aspect_ratio_x = strtol(GetSetting("aspect_ratio_x"), NULL, 10);
	if (_aspect_ratio_x == 0)
		_aspect_ratio_x = 16;

	aspect_ratio_x = _aspect_ratio_x;

	int _aspect_ratio_y = strtol(GetSetting("aspect_ratio_y"), NULL, 10);
	if (_aspect_ratio_y == 0)
		_aspect_ratio_y = 9;

	aspect_ratio_y = _aspect_ratio_y;

	SetWidescreen();

	int sprite_resolution_factor = strtol(GetSetting("sprite_resolution"), NULL, 10);
	if (sprite_resolution_factor == 0)
		sprite_resolution_factor = 1;

	SetSpriteResolution(sprite_resolution_factor);

	int window_upscale_factor = strtol(GetSetting("window_upscale"), NULL, 10);
	if (window_upscale_factor == 0)
		window_upscale_factor = 1;

	UpscaleWindow(window_upscale_factor);
}
