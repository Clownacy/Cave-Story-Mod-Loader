// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2018 Clownacy

#include "common.h"

#include <stdbool.h>

bool borderless_fullscreen;
bool fullscreen_auto_aspect_ratio;
bool fullscreen_auto_window_upscale;
bool fullscreen_vsync;
bool sixty_fps;
unsigned int aspect_ratio_x;
unsigned int aspect_ratio_y;
unsigned int window_upscale_factor;
unsigned int sprite_resolution_factor;

unsigned int ScreenWidthToTiles(const unsigned int tile_size)
{
	return (SCREEN_WIDTH + (tile_size - 1)) / tile_size;
}
