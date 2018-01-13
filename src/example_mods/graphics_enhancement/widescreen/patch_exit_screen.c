// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch_exit_screen.h"

#include "cave_story.h"
#include "mod_loader.h"

#include "../common.h"

static void DrawSprite1_hijack2(RECT* clip_rect, int x, int y, RECT* src_rect, int surface_id)
{
	CS_DrawSprite_WithTransparency(clip_rect, ((SCREEN_WIDTH - 208) / 2), y, src_rect, surface_id);
}

void PatchExitScreen(void)
{
	WriteRelativeAddress((void*)0x40DE1D + 1, DrawSprite1_hijack2);
}
