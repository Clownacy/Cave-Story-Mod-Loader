// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "../common.h"
#include "drawsprite1_centred.h"

void DrawSprite1_hijack(void* clip_rect, int x, int y, void* src_rect, int surface_ID)
{
	void (*DrawSprite1)(void*, int x, int y, void*, int) = (void(*)(void*, int x, int y, void*, int))0x40C3C0;

	DrawSprite1(clip_rect, ((SCREEN_WIDTH - 320) / 2) + x, y, src_rect, surface_ID);
}
