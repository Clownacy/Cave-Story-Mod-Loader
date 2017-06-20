// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "borderless_fullscreen.h"

#include <stdio.h>
#include <windows.h>

#include "cave_story.h"
#include "mod_loader.h"

#include "../common.h"

__stdcall void SetupFullscreenBlitting(const int window_size)
{
	if (window_size == 2)	// Fullscreen mode enabled in settings
	{
		const unsigned int monitor_width = GetSystemMetrics(SM_CXSCREEN);
		const unsigned int monitor_height = GetSystemMetrics(SM_CYSCREEN);

		const unsigned int game_width = SCREEN_WIDTH;
		const unsigned int game_height = 240;

		if ((double)game_width / game_height >= (double)monitor_width / monitor_height)
		{
			*window_surface_width = monitor_width;
			*window_surface_height = (game_height * monitor_width) / game_width;
		}
		else
		{
			*window_surface_width = (game_width * monitor_height) / game_height;
			*window_surface_height = monitor_height;
		}

		*window_padding_w = (monitor_width - *window_surface_width) / 2;
		*window_padding_h = (monitor_height - *window_surface_height) / 2;
	}
}

__asm(
"_SetupFullscreenBlitting_asm:\n"
"	movl	%ecx, 0x49D378\n"	// This is just the instruction we overwrote
"	pushl	0xC(%ebp)\n"
"	call	_SetupFullscreenBlitting@4\n"
"	ret\n"
);
extern char SetupFullscreenBlitting_asm;

void ApplyBorderlessFullscreenPatch(void)
{
	PrintDebug("Applying 'borderless fullscreen' patch\n");

	// Hijack the window_surface_width/window_surface_height setter
	WriteByte((void*)0x40B5AB, 0x90);
	WriteCall((void*)0x40B5AB + 1, &SetupFullscreenBlitting_asm);

	// Force the DirectDraw initialiser into windowed mode
	// (the window creator is still in fullscreen mode)
	WriteJump((void*)0x40B4FE, (void*)0x40B4D1);

	// Change the window background colour to black instead of grey
	WriteByte((void*)0x412717 + 1, BLACK_BRUSH);

	FILE *file = fopen("Config.dat", "rb");
	if (file != NULL)
	{
		fseek(file, 0x6C, SEEK_SET);
		const unsigned char video_mode = fgetc(file);
		fclose(file);

		if (video_mode != 0 && video_mode != 3 && video_mode != 4)
		{
			// Not in fullscreen
			borderless_fullscreen_auto_aspect_ratio = false;
			borderless_fullscreen_auto_window_upscale = false;
		}
	}

	if (borderless_fullscreen_auto_aspect_ratio)
	{
		// Compute best aspect ratio to fill the monitor
		const unsigned int monitor_width = GetSystemMetrics(SM_CXSCREEN);
		const unsigned int monitor_height = GetSystemMetrics(SM_CYSCREEN);

		unsigned int gcd_value1 = monitor_width;
		unsigned int gcd_value2 = monitor_height;
		while (gcd_value2 != 0)
		{
			const unsigned int remainder = gcd_value1 % gcd_value2;
			gcd_value1 = gcd_value2;
			gcd_value2 = remainder;
		}

		const unsigned int greatest_common_divisor = gcd_value1;

		aspect_ratio_x = monitor_width / greatest_common_divisor;
		aspect_ratio_y = monitor_height / greatest_common_divisor;
	}

	if (borderless_fullscreen_auto_window_upscale)
	{
		// Compute best window upscale factor (for font rendering)
		const unsigned int monitor_width = GetSystemMetrics(SM_CXSCREEN);
		const unsigned int monitor_height = GetSystemMetrics(SM_CYSCREEN);

		const unsigned int game_width = SCREEN_WIDTH;
		const unsigned int game_height = 240;

		unsigned int output_window_height;
		if ((double)game_width / game_height >= (double)monitor_width / monitor_height)
		{
			output_window_height = (game_height * monitor_width) / game_width;
		}
		else
		{
			output_window_height = monitor_height;
		}

		window_upscale_factor = (output_window_height - (output_window_height % 240)) / 240;
	}
}
