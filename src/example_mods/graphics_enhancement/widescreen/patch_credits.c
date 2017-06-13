// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch_credits.h"

#include <windows.h>

#include "mod_loader.h"

#include "../common.h"

int screen_nudge;

__asm(
"_DrawSprite_WithTransparency_nudged:\n"
"	movl	_screen_nudge, %eax\n"
"	addl	%eax, 8(%esp)\n"
"	jmp	0x40C3C0\n"
);
extern char DrawSprite_WithTransparency_nudged;

void DrawCreditsImage_new(void)
{
	void (*DrawSprite_WithTransparency)(RECT *clip_rect, int x_pos, int y_pos, RECT *src_rect, int surface_ID) = (void (*)(RECT*, int, int, RECT*, int))0x40C3C0;
	int credits_picture_x = *(int*)0x49D60C;

	RECT clip_rect = {(SCREEN_WIDTH - 320) / 2, 0, ((SCREEN_WIDTH - 320) / 2) + 320, 240};
	RECT src_rect = {0, 0, 160, 240};
	DrawSprite_WithTransparency(&clip_rect, (credits_picture_x / 0x200) + ((SCREEN_WIDTH - 320) / 2), 0, &src_rect, 36);
}

void PatchCredits(void)
{
	screen_nudge = (SCREEN_WIDTH - 320) / 2;
	WriteRelativeAddress((void*)0x410865 + 1, DrawCreditsImage_new);
	WriteLong((void*)0x40D56F + 6, SCREEN_WIDTH / 2);
	WriteRelativeAddress((void*)0x40D0A7 + 1, &DrawSprite_WithTransparency_nudged);
	WriteRelativeAddress((void*)0x40D138 + 1, &DrawSprite_WithTransparency_nudged);
}
