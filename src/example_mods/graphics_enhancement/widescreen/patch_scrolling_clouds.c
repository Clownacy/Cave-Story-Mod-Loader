#include <windows.h>

#include "mod_loader.h"

#include "../common.h"
#include "patch_scrolling_clouds.h"

void __stdcall ScrollClouds(const int scroll_type)
{
	void (*DrawSprite2)(RECT*, int, int, RECT*, int) = (void(*)(RECT*, int, int, RECT*, int))0x40C5B0;
	int *dword_499C8C = (int*)0x499C8C;

	RECT *clip_rect = (RECT*)0x48F91C;
	RECT src_rect = {0, 0, 320, 88};
	int sprite_width = (src_rect.right - src_rect.left);
	DrawSprite2(clip_rect, 0, src_rect.top, &src_rect, 28);

	if (scroll_type == 6)
	{
		// bkMoon
		src_rect.left = 0;
		src_rect.right = 240;
	}
	else// if (scroll_type == 6)
	{
		// bkFog
		src_rect.left = 108;
		src_rect.right = 320;
	}

	sprite_width = (src_rect.right - src_rect.left);
	for (int i=0; i <= ((SCREEN_WIDTH - 320) / sprite_width); ++i)
	{
		DrawSprite2(clip_rect, 320 + (sprite_width * i), src_rect.top, &src_rect, 28);
	}

	src_rect.left = 0;
	src_rect.right = 320;
	src_rect.top = 88;
	src_rect.bottom = 123;
	sprite_width = (src_rect.right - src_rect.left);
	for (int i=0; i <= (SCREEN_WIDTH / sprite_width) + 1; ++i)
	{
		DrawSprite2(clip_rect, (sprite_width * i) - ((*dword_499C8C / 2) % sprite_width), src_rect.top, &src_rect, 28);
	}

	src_rect.top = 123;
	src_rect.bottom = 146;
	for (int i=0; i <= (SCREEN_WIDTH / sprite_width) + 1; ++i)
	{
		DrawSprite2(clip_rect, (sprite_width * i) - (*dword_499C8C % sprite_width), src_rect.top, &src_rect, 28);
	}

	src_rect.top = 146;
	src_rect.bottom = 176;
	for (int i=0; i <= (SCREEN_WIDTH / sprite_width) + 1; ++i)
	{
		DrawSprite2(clip_rect, (sprite_width * i) - ((*dword_499C8C * 2) % sprite_width), src_rect.top, &src_rect, 28);
	}

	src_rect.top = 176;
	src_rect.bottom = 240;
	for (int i=0; i <= (SCREEN_WIDTH / sprite_width) + 1; ++i)
	{
		DrawSprite2(clip_rect, (sprite_width * i) - ((*dword_499C8C * 4) % sprite_width), src_rect.top, &src_rect, 28);
	}
}

__asm(
"_ScrollClouds_asm:\n"
"	push	%eax\n"
"	call	_ScrollClouds@4\n"
"	jmp	0x402805\n"
);
void ScrollClouds_asm(void);

void PatchScrollingClouds(void)
{
	WriteLong(0x402809 + (4 * 6), (int)ScrollClouds_asm);
	WriteLong(0x402809 + (4 * 7), (int)ScrollClouds_asm);
}
