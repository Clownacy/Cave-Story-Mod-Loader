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

	borderless_fullscreen = GetSettingBool("borderless_fullscreen");
	borderless_fullscreen_auto_aspect_ratio = GetSettingBool("borderless_fullscreen_auto_aspect_ratio");
	borderless_fullscreen_auto_window_upscale = GetSettingBool("borderless_fullscreen_auto_window_upscale");

	int _aspect_ratio_x = GetSettingInt("aspect_ratio_x");
	if (_aspect_ratio_x == 0)
		_aspect_ratio_x = 16;

	aspect_ratio_x = _aspect_ratio_x;

	int _aspect_ratio_y = GetSettingInt("aspect_ratio_y");
	if (_aspect_ratio_y == 0)
		_aspect_ratio_y = 9;

	aspect_ratio_y = _aspect_ratio_y;

	int sprite_resolution_factor = GetSettingInt("sprite_resolution");
	if (sprite_resolution_factor == 0)
		sprite_resolution_factor = 1;

	int _window_upscale_factor = GetSettingInt("window_upscale");
	if (_window_upscale_factor == 0)
		_window_upscale_factor = 1;

	window_upscale_factor = _window_upscale_factor;

	if (borderless_fullscreen)
		ApplyBorderlessFullscreenPatch();

	if (aspect_ratio_x != 4 || aspect_ratio_y != 3)
		SetWidescreen();

	if (sprite_resolution_factor != 1)
		SetSpriteResolution(sprite_resolution_factor);

	if (window_upscale_factor != 2)
		UpscaleWindow();
}
