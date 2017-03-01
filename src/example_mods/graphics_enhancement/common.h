#pragma once

#define ASPECT_RATIO_X 21
#define ASPECT_RATIO_Y 9

#define SCREEN_WIDTH (240 * ASPECT_RATIO_X) / ASPECT_RATIO_Y

extern unsigned int ScreenWidthToTiles(const unsigned int tile_size);
