// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "mod_loader.h"

#include "common.h"
#include "borderless_fullscreen/borderless_fullscreen.h"
#include "remove_sprite_alignment/remove_sprite_alignment.h"
#include "sprite_resolution/sprite_resolution.h"
#include "upscale_window/upscale_window.h"
#include "widescreen/widescreen.h"

__asm(
"_LoadWindowRect_ASM:\n"
"	push	%ecx\n"
"	call	0x480E1B\n"
"	add	$4,%esp\n"
"	lea	-0x24(%ebp),%eax\n"
"	push	%eax\n"
"	call	_LoadWindowRect_NewCode\n"
"	add	$4,%esp\n"
"	ret\n"
);
extern char LoadWindowRect_ASM;

void LoadWindowRect_NewCode(RECT *rect)
{
	const int window_width = ((240 * aspect_ratio_x) / aspect_ratio_y) * window_upscale_factor;
	const int window_height = 240 * window_upscale_factor;

	if (rect->right - rect->left != window_width)
		rect->right = rect->left + window_width;
	if (rect->bottom - rect->top != window_height)
		rect->bottom = rect->top + window_height;
}

void InitMod(void)
{
	borderless_fullscreen = GetSettingBool("borderless_fullscreen", true);
	borderless_fullscreen_auto_aspect_ratio = GetSettingBool("borderless_fullscreen_auto_aspect_ratio", true);
	borderless_fullscreen_auto_window_upscale = GetSettingBool("borderless_fullscreen_auto_window_upscale", true);

	int _aspect_ratio_x = GetSettingInt("aspect_ratio_x", 16);
	if (_aspect_ratio_x == 0)
		_aspect_ratio_x = 16;

	aspect_ratio_x = _aspect_ratio_x;

	int _aspect_ratio_y = GetSettingInt("aspect_ratio_y", 9);
	if (_aspect_ratio_y == 0)
		_aspect_ratio_y = 9;

	aspect_ratio_y = _aspect_ratio_y;

	int sprite_resolution_factor = GetSettingInt("sprite_resolution", 1);
	if (sprite_resolution_factor == 0)
		sprite_resolution_factor = 1;

	int _window_upscale_factor = GetSettingInt("window_upscale", 2);
	if (_window_upscale_factor == 0)
		_window_upscale_factor = 1;

	window_upscale_factor = _window_upscale_factor;

	if ((aspect_ratio_x != 4 || aspect_ratio_y != 3) || (window_upscale_factor != 2))
		WriteRelativeAddress((void*)0x411062 + 1, (void*)&LoadWindowRect_ASM);

	if (borderless_fullscreen)
		ApplyBorderlessFullscreenPatch();

	if (aspect_ratio_x != 4 || aspect_ratio_y != 3)
		SetWidescreen();

	if (sprite_resolution_factor != 1)
		SetSpriteResolution(sprite_resolution_factor);

	if (window_upscale_factor != 2)
		UpscaleWindow();

	if (GetSettingBool("remove_sprite_alignment", true))
		RemoveSpriteAlignment();
}
