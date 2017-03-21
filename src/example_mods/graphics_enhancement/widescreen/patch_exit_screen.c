// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "cave_story.h"
#include "mod_loader.h"

#include "../common.h"
#include "patch_exit_screen.h"

void DrawSprite1_hijack2(void* clip_rect, int x, int y, void* src_rect, int surface_ID)
{
	DrawSprite1(clip_rect, ((SCREEN_WIDTH - 208) / 2), y, src_rect, surface_ID);
}

void PatchExitScreen(void)
{
	WriteRelativeAddress(0x40DE1D + 1, DrawSprite1_hijack2);
}
