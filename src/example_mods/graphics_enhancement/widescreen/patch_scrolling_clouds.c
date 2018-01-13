// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch_scrolling_clouds.h"

#include <windows.h>

#include "cave_story.h"
#include "mod_loader.h"

#include "../common.h"

static void DrawSkyRow(const int scroll_type)
{
	RECT src_rect_sky = {0, 0, 320, 88};
	const int sky_width = src_rect_sky.right - src_rect_sky.left;
	RECT src_rect_no_moon = {0, 0, 240, 88};
	const int moon_width = src_rect_no_moon.right - src_rect_no_moon.left;
	RECT src_rect_no_sun = {108, 0, 320, 88};
	const int sun_width = src_rect_no_sun.right - src_rect_no_sun.left;

	CS_DrawSprite_NoTransparency(&CS_clip_rect_common, (SCREEN_WIDTH - sky_width) / 2, 0, &src_rect_sky, 28);

	RECT src_rect_current;
	int width_current;
	if (scroll_type == 6)
	{
		// bkMoon
		src_rect_current = src_rect_no_moon;
		width_current = moon_width;
	}
	else// if (scroll_type == 7)
	{
		// bkFog
		src_rect_current = src_rect_no_sun;
		width_current = sun_width;
	}

	for (int i = (((SCREEN_WIDTH - sky_width) / 2) - width_current); (i + width_current) > 0; i -= width_current)
	{
		CS_DrawSprite_NoTransparency(&CS_clip_rect_common, i, src_rect_current.top, &src_rect_current, 28);
	}

	for (int i = (((SCREEN_WIDTH - sky_width) / 2) + sky_width); i < SCREEN_WIDTH; i += width_current)
	{
		CS_DrawSprite_NoTransparency(&CS_clip_rect_common, i, src_rect_current.top, &src_rect_current, 28);
	}
}

__stdcall void ScrollClouds(const int scroll_type)
{
	DrawSkyRow(scroll_type);

	// Cloud layers

	RECT src_rect = {0, 88, 320, 123};
	const int sprite_width = (src_rect.right - src_rect.left);

	for (int i=0; i <= (SCREEN_WIDTH / sprite_width) + 1; ++i)
	{
		CS_DrawSprite_NoTransparency(&CS_clip_rect_common, (sprite_width * i) - ((CS_dword_499C8C / 2) % sprite_width), src_rect.top, &src_rect, 28);
	}

	src_rect.top = 123;
	src_rect.bottom = 146;
	for (int i=0; i <= (SCREEN_WIDTH / sprite_width) + 1; ++i)
	{
		CS_DrawSprite_NoTransparency(&CS_clip_rect_common, (sprite_width * i) - (CS_dword_499C8C % sprite_width), src_rect.top, &src_rect, 28);
	}

	src_rect.top = 146;
	src_rect.bottom = 176;
	for (int i=0; i <= (SCREEN_WIDTH / sprite_width) + 1; ++i)
	{
		CS_DrawSprite_NoTransparency(&CS_clip_rect_common, (sprite_width * i) - ((CS_dword_499C8C * 2) % sprite_width), src_rect.top, &src_rect, 28);
	}

	src_rect.top = 176;
	src_rect.bottom = 240;
	for (int i=0; i <= (SCREEN_WIDTH / sprite_width) + 1; ++i)
	{
		CS_DrawSprite_NoTransparency(&CS_clip_rect_common, (sprite_width * i) - ((CS_dword_499C8C * 4) % sprite_width), src_rect.top, &src_rect, 28);
	}
}

__asm(
"_ScrollClouds_asm:\n"
"	push	%eax\n"
"	call	_ScrollClouds@4\n"
"	jmp	0x402805\n"
);
extern char ScrollClouds_asm;

void PatchScrollingClouds(void)
{
	WriteLong((void*)0x402809 + (4 * 6), (int)&ScrollClouds_asm);
	WriteLong((void*)0x402809 + (4 * 7), (int)&ScrollClouds_asm);
}
