#pragma once

#include <windows.h>
#include "cave_story.h"

#define ConvertPixelMacro(value) (((value) * 512 * (int)window_upscale_factor) / 512)
#define ConvertCoordMacro(value) (((value) * (int)window_upscale_factor) / 512)

void RemoveSpriteAlignment(void);

void DrawColourFill_RawXY(RECT *dst_rect, int colour);
void DrawSpriteWithTransparency_RawXY(RECT *clip_rect, int x, int y, RECT *src_rect, CS_SurfaceID surface_id);
void DrawSpriteNoTransparency_RawXY(RECT *clip_rect, int x, int y, RECT *src_rect, CS_SurfaceID surface_id);
