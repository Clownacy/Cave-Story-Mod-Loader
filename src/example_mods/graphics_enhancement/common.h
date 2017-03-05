// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#pragma once

extern unsigned int aspect_ratio_x;
extern unsigned int aspect_ratio_y;

#define SCREEN_WIDTH (240 * aspect_ratio_x) / aspect_ratio_y

extern unsigned int ScreenWidthToTiles(const unsigned int tile_size);
