// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2018 Clownacy

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "mod_loader.h"

#include "common.h"
#include "60fps/60fps.h"
#include "fullscreen/fullscreen.h"
#include "remove_sprite_alignment/remove_sprite_alignment.h"
#include "sprite_resolution/sprite_resolution.h"
#include "upscale_window/upscale_window.h"
#include "widescreen/widescreen.h"

__asm(
"_LoadWindowRect_ASM:\n"
"	push	%ecx\n"
"	call	*1f\n"
"	add	$4,%esp\n"
"	lea	-0x24(%ebp),%eax\n"
"	push	%eax\n"
"	call	_LoadWindowRect_NewCode\n"
"	add	$4,%esp\n"
"	ret\n"
"1:\n"
"	.long	0x480E1B\n"
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
	borderless_fullscreen = ModLoader_GetSettingBool("borderless_fullscreen", false);
	fullscreen_auto_aspect_ratio = ModLoader_GetSettingBool("fullscreen_auto_aspect_ratio", true);
	fullscreen_auto_window_upscale = ModLoader_GetSettingBool("fullscreen_auto_window_upscale", true);
	fullscreen_vsync = ModLoader_GetSettingBool("fullscreen_vsync", false);
	sixty_fps = ModLoader_GetSettingBool("60fps", true);

	aspect_ratio_x = ModLoader_GetSettingInt("aspect_ratio_x", 16);
	if (aspect_ratio_x == 0)
		ModLoader_PrintErrorMessageBox("You're joking, right?\n\n'aspect_ratio_x = 0'?\n\nWell, it's your funeral.");

	aspect_ratio_y = ModLoader_GetSettingInt("aspect_ratio_y", 9);
	if (aspect_ratio_y == 0)
		ModLoader_PrintErrorMessageBox("You're joking, right?\n\n'aspect_ratio_y = 0'?\n\nWell, it's your funeral.");

	sprite_resolution_factor = ModLoader_GetSettingInt("sprite_resolution", 1);
	if (sprite_resolution_factor == 0)
		sprite_resolution_factor = 1;

	window_upscale_factor = ModLoader_GetSettingInt("window_upscale", 2);
	if (window_upscale_factor == 0)
		window_upscale_factor = 1;

	if ((aspect_ratio_x != 4 || aspect_ratio_y != 3) || (window_upscale_factor != 2))
		ModLoader_WriteRelativeAddress((void*)0x411062 + 1, (void*)&LoadWindowRect_ASM);

	if (sixty_fps)
		Apply60FPSPatch();

	ApplyFullscreenPatches();

	if (aspect_ratio_x != 4 || aspect_ratio_y != 3)
		SetWidescreen();

	if (sprite_resolution_factor != 1)
		SetSpriteResolution();

	if (window_upscale_factor != 2)
		UpscaleWindow();

	if (ModLoader_GetSettingBool("remove_sprite_alignment", true))
		RemoveSpriteAlignment();
}
