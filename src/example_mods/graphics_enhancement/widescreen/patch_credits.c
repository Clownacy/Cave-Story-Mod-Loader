// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch_credits.h"

#include <windows.h>

#include "cave_story.h"
#include "mod_loader.h"

#include "../common.h"

int screen_nudge;

__asm(
"_DrawSprite_WithTransparency_nudged:\n"
"	movl	_screen_nudge, %eax\n"
"	addl	%eax, 8(%esp)\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x40C3C0\n"
);
extern char DrawSprite_WithTransparency_nudged;

static void ResetClipRect(void)
{
	CS_clip_rect_common.left = 0;
	CS_clip_rect_common.right = SCREEN_WIDTH;
}

static void SetCreditsClipRect(void)
{
	CS_clip_rect_common.left = SCREEN_WIDTH / 2;
	CS_clip_rect_common.right = ((SCREEN_WIDTH - 320) / 2) + 320;
}

static void DrawCreditsImage_new(void)
{
	int credits_picture_x = *(int*)0x49D60C;

	RECT clip_rect = {(SCREEN_WIDTH - 320) / 2, 0, ((SCREEN_WIDTH - 320) / 2) + 320, 240};
	RECT src_rect = {0, 0, 160, 240};
	CS_DrawSprite_WithTransparency(&clip_rect, (credits_picture_x / 0x200) + ((SCREEN_WIDTH - 320) / 2), 0, &src_rect, 36);
}

void PatchCredits(void)
{
	screen_nudge = (SCREEN_WIDTH - 320) / 2;
	WriteRelativeAddress((void*)0x410865 + 1, DrawCreditsImage_new);
	WriteRelativeAddress((void*)0x40D0A7 + 1, &DrawSprite_WithTransparency_nudged);
	WriteRelativeAddress((void*)0x40D138 + 1, &DrawSprite_WithTransparency_nudged);

	WriteCall((void*)0x40D56F, SetCreditsClipRect);
	WriteNOPs((void*)0x40D56F + 5, 5);

	WriteCall((void*)0x40F77D, ResetClipRect);
	WriteNOPs((void*)0x40F77D + 5, 5);

	WriteCall((void*)0x40FED4, ResetClipRect);
	WriteNOPs((void*)0x40FED4 + 5, 5);

	WriteCall((void*)0x410427, ResetClipRect);
	WriteNOPs((void*)0x410427 + 5, 5);
}
