// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "black_bars.h"

#include <windows.h>

#include "cave_story.h"
#include "mod_loader.h"

#include "../common.h"

int current_level_width;

void DrawBlackBars(int x_pos, int y_pos)
{
	if (!(*(int*)0x49E1E8 & 8))	// Detect if credits are running
	{
		DrawWater(x_pos, y_pos);

		const int bar_width = (SCREEN_WIDTH - (current_level_width * 16)) / 2;

		if (bar_width > 0)
		{
			int rumble_delta = -((x_pos / 0x200) + (16 / 2)) - bar_width;

			// Draw left black bar
			RECT bar_rect;
			bar_rect.left = 0;
			bar_rect.top = 0;
			bar_rect.right = bar_width + rumble_delta;
			bar_rect.bottom = 240;

			DrawColourFill(&bar_rect, 0x00000000);

			// Draw right black bar
			bar_rect.left = SCREEN_WIDTH - (bar_width - rumble_delta);
			bar_rect.right = SCREEN_WIDTH;

			DrawColourFill(&bar_rect, 0x00000000);
		}
	}
}

void PatchBlackBars(void)
{
	if (GetSettingBool("black_bars"))
		WriteRelativeAddress((void*)0x4106C3 + 1, DrawBlackBars);
}
