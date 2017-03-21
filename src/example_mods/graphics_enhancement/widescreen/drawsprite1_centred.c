// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "cave_story.h"

#include "../common.h"
#include "drawsprite1_centred.h"

void DrawSprite1_hijack(void* clip_rect, int x, int y, void* src_rect, int surface_ID)
{
	DrawSprite_WithTransparency(clip_rect, ((SCREEN_WIDTH - 320) / 2) + x, y, src_rect, surface_ID);
}
