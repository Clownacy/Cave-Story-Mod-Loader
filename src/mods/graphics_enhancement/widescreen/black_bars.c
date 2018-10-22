// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "black_bars.h"

#include <stdbool.h>
#include <windows.h>

#include "cave_story.h"
#include "mod_loader.h"

#include "../common.h"

int current_level_width;

static bool disable_ironhead_black_bars;

static void DrawBlackBars(int x_pos, int y_pos)
{
	CS_DrawWater(x_pos, y_pos);	// We replaced this call while inserting the hook

	if (!(CS_gamemode_flags & 8))	// Detect if credits are running
	{
		const bool is_ironhead_arena = !disable_ironhead_black_bars && CS_current_room == 0x1F;
		const int room_width = is_ironhead_arena ? 320 : (current_level_width * 16);
		const int bar_width = (SCREEN_WIDTH - room_width) / 2;

		if (bar_width > 0)
		{
			const int rumble_delta = is_ironhead_arena ? 0 : -((x_pos / 0x200) + (16 / 2)) - bar_width;

			// Draw left black bar
			RECT bar_rect;
			bar_rect.left = 0;
			bar_rect.top = 0;
			bar_rect.right = bar_width + rumble_delta;
			bar_rect.bottom = 240;

			CS_DrawColourFill(&bar_rect, 0x00000000);

			// Draw right black bar
			bar_rect.left = SCREEN_WIDTH - (bar_width - rumble_delta) - ((SCREEN_WIDTH - room_width) & 1);
			bar_rect.right = SCREEN_WIDTH;

			CS_DrawColourFill(&bar_rect, 0x00000000);
		}
	}
}

void PatchBlackBars(void)
{
	if (ModLoader_GetSettingBool("black_bars", true))
	{
		ModLoader_WriteRelativeAddress((void*)0x40F8D1 + 1, DrawBlackBars);
		ModLoader_WriteRelativeAddress((void*)0x4106C3 + 1, DrawBlackBars);

		disable_ironhead_black_bars = ModLoader_GetSettingBool("disable_ironhead_black_bars", false);
	}
}
