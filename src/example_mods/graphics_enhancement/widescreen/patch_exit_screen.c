// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch_exit_screen.h"

#include "cave_story.h"
#include "mod_loader.h"

#include "../common.h"

static void DrawSprite1_hijack2(void* clip_rect, int x, int y, void* src_rect, int surface_ID)
{
	DrawSprite_WithTransparency(clip_rect, ((SCREEN_WIDTH - 208) / 2), y, src_rect, surface_ID);
}

void PatchExitScreen(void)
{
	WriteRelativeAddress((void*)0x40DE1D + 1, DrawSprite1_hijack2);
}
