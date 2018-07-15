// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

extern bool borderless_fullscreen;
extern bool fullscreen_auto_aspect_ratio;
extern bool fullscreen_auto_window_upscale;
extern bool fullscreen_vsync;
extern bool fullscreen_integer_scaling;
extern bool sixty_fps;
extern unsigned int aspect_ratio_x;
extern unsigned int aspect_ratio_y;
extern unsigned int window_upscale_factor;
extern unsigned int sprite_resolution_factor;

#define SCREEN_WIDTH ((signed int)(((240 * aspect_ratio_x) + (aspect_ratio_y - 1)) / aspect_ratio_y))

extern unsigned int ScreenWidthToTiles(const unsigned int tile_size);
