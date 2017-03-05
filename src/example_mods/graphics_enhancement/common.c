// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "common.h"

unsigned int aspect_ratio_x;
unsigned int aspect_ratio_y;

unsigned int ScreenWidthToTiles(const unsigned int tile_size)
{
	return (SCREEN_WIDTH + (tile_size - 1)) / tile_size;
}
