#include <windows.h>

#include "mod_loader.h"

#include "../common.h"
#include "black_bars.h"

int current_level_width;

void DrawBlackBars(int x_pos, int y_pos)
{
	void (*DrawColourFillOntoScreen)(RECT *dst_rect, int colour) = (void(*)(RECT*, int))0x40C9E0;

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

		DrawColourFillOntoScreen(&bar_rect, 0x00000000);

		// Draw right black bar
		bar_rect.left = SCREEN_WIDTH - (bar_width - rumble_delta);
		bar_rect.right = SCREEN_WIDTH;

		DrawColourFillOntoScreen(&bar_rect, 0x00000000);
	}
}

void PatchBlackBars(void)
{
	const char* const black_bars_setting = GetSetting("black_bars");

	if (black_bars_setting != NULL && strcmp(black_bars_setting, "true") == 0)
		WriteRelativeAddress(0x4106E8 + 1, DrawBlackBars);
}
