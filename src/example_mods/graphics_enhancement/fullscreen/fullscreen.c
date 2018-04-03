// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2018 Clownacy

#include "fullscreen.h"

#include <stddef.h>
#include <stdio.h>
#include <windows.h>

#include "cave_story.h"
#include "mod_loader.h"

#include "../common.h"

static IDirectDrawSurface7 *vsync_new_screen_surface;

__stdcall void CalculateFullscreenDimensionsAndPadding(const int window_size)
{
	if (window_size == 2)	// Fullscreen mode enabled in settings
	{
		const unsigned int monitor_width = GetSystemMetrics(SM_CXSCREEN);
		const unsigned int monitor_height = GetSystemMetrics(SM_CYSCREEN);

		const unsigned int game_width = SCREEN_WIDTH;
		const unsigned int game_height = 240;

		if ((double)game_width / game_height >= (double)monitor_width / monitor_height)
		{
			CS_window_surface_width = monitor_width;
			CS_window_surface_height = (game_height * monitor_width) / game_width;
		}
		else
		{
			CS_window_surface_width = (game_width * monitor_height) / game_height;
			CS_window_surface_height = monitor_height;
		}

		CS_window_padding_w = (monitor_width - CS_window_surface_width) / 2;
		CS_window_padding_h = (monitor_height - CS_window_surface_height) / 2;
	}
}

__asm(
"_CalculateFullscreenDimensionsAndPadding_asm:\n"
"	movl	%ecx, 0x49D378\n"	// This is just the instruction we overwrote
"	pushl	0xC(%ebp)\n"
"	call	_CalculateFullscreenDimensionsAndPadding@4\n"
"	ret\n"
);
extern char CalculateFullscreenDimensionsAndPadding_asm;

void VSyncInitDirectDraw(void)
{
	DDSCAPS2 ddscaps;
	ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
	vsync_new_screen_surface->lpVtbl->GetAttachedSurface(vsync_new_screen_surface, &ddscaps, &CS_screen_primary_surface);
}

__asm(
"_VSyncInitDirectDraw_asm:\n"
"	call	_VSyncInitDirectDraw\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x40B6B4\n"
);
extern char VSyncInitDirectDraw_asm;

void VSyncDrawScreen(void)
{
	vsync_new_screen_surface->lpVtbl->Flip(vsync_new_screen_surface, NULL, DDFLIP_WAIT);
}

__asm(
"_VSyncDrawScreen_asm:\n"
"	call	*0x14(%eax)\n"
"	call	_VSyncDrawScreen\n"
"	call	_RegenerateVSyncSurface\n"
"	test	%al,%al\n"
"	jz	2f\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x40B436\n"
"2:\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x40B445\n"
);
extern char VSyncDrawScreen_asm;

__asm(
"_SkipFramerateControl_asm:\n"
"	call	*3f\n"
"	test	%eax,%eax\n"
"	jz	2f\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x40B3B0\n"
"2:\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x40B355\n"
"3:\n"
"	.long	0x413570\n"
);
extern char SkipFramerateControl_asm;

bool RegenerateVSyncSurface(void)
{
	bool surface_needs_regenerating = vsync_new_screen_surface && vsync_new_screen_surface->lpVtbl->IsLost(vsync_new_screen_surface) == DDERR_SURFACELOST;

	if (surface_needs_regenerating)
		vsync_new_screen_surface->lpVtbl->Restore(vsync_new_screen_surface);

	return surface_needs_regenerating || CS_RegenerateSurfaces();
}

void SetupVSync(void)
{
	// Change surface creation to flip-capable exclusive fullscreen
	ModLoader_WriteLong((void*)0x40B5C8 + 3, DDSD_CAPS | DDSD_BACKBUFFERCOUNT);
	ModLoader_WriteLong((void*)0x40B5CF + 3, DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX);
	ModLoader_WriteLong((void*)0x40B5D6 + 3, 1);
	ModLoader_WriteLong((void*)0x40B5DF + 1, (unsigned int)&vsync_new_screen_surface);
	ModLoader_WriteJump((void*)0x40B663, &VSyncInitDirectDraw_asm);
	// Handle flipping during drawing
	ModLoader_WriteJump((void*)0x40B42A, &VSyncDrawScreen_asm);
	// Dummy out the framerate manager
	ModLoader_WriteJump((void*)0x40B343, &SkipFramerateControl_asm);
}

void ApplyFullscreenPatches(void)
{
	unsigned char video_mode = 0;	// If there's no file, Cave Story defaults to fullscreen

	FILE *file = fopen("Config.dat", "rb");
	if (file != NULL)
	{
		fseek(file, 0x6C, SEEK_SET);
		video_mode = fgetc(file);
		fclose(file);
	}

	if (video_mode != 0 && video_mode != 3 && video_mode != 4)
	{
		// Not in fullscreen
		fullscreen_auto_aspect_ratio = false;
		fullscreen_auto_window_upscale = false;
	}
	else
	{
		ModLoader_PrintDebug("Applying fullscreen patches\n");

		// Disable window.rect
		ModLoader_WriteByte((void*)0x40F705, 0xEB);
		ModLoader_WriteByte((void*)0x412DC3, 0xEB);

		// Hijack the window_surface_width/window_surface_height setter
		ModLoader_WriteByte((void*)0x40B5AB, 0x90);
		ModLoader_WriteCall((void*)0x40B5AB + 1, &CalculateFullscreenDimensionsAndPadding_asm);

		// Force the DirectDraw initialiser into windowed mode
		// (the window creator is still in fullscreen mode)
		if (borderless_fullscreen)
			ModLoader_WriteJump((void*)0x40B4FE, (void*)0x40B4D1);

		if (!borderless_fullscreen && fullscreen_vsync)
			SetupVSync();

		// Change the window background colour to black instead of grey
		ModLoader_WriteByte((void*)0x412717 + 1, BLACK_BRUSH);

		if (fullscreen_auto_aspect_ratio)
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

			ModLoader_PrintDebug("Auto-detected aspect ratio is %d:%d\n", aspect_ratio_x, aspect_ratio_y);
		}

		if (fullscreen_auto_window_upscale)
		{
			// Compute best window upscale factor (for font rendering)
			const unsigned int monitor_width = GetSystemMetrics(SM_CXSCREEN);
			const unsigned int monitor_height = GetSystemMetrics(SM_CYSCREEN);

			const unsigned int game_width = SCREEN_WIDTH * sprite_resolution_factor;
			const unsigned int game_height = 240 * sprite_resolution_factor;

			unsigned int output_window_height;
			if ((double)game_width / game_height >= (double)monitor_width / monitor_height)
			{
				output_window_height = (game_height * monitor_width) / game_width;
			}
			else
			{
				output_window_height = monitor_height;
			}

			window_upscale_factor = ((output_window_height + (game_height / 2)) / game_height) * sprite_resolution_factor;

			ModLoader_PrintDebug("Auto-detected window upscale factor is %d\n", window_upscale_factor);
		}
	}
}
