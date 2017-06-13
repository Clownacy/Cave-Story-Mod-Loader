// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#pragma once

#include <stdbool.h>

extern bool borderless_fullscreen;
extern bool borderless_fullscreen_auto_aspect_ratio;
extern bool borderless_fullscreen_auto_window_upscale;
extern unsigned int aspect_ratio_x;
extern unsigned int aspect_ratio_y;
extern unsigned int window_upscale_factor;

#define SCREEN_WIDTH ((240 * aspect_ratio_x) / aspect_ratio_y)

extern unsigned int ScreenWidthToTiles(const unsigned int tile_size);
