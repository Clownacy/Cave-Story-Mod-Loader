// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch_fade.h"

#include <stdlib.h>

#include "cave_story.h"
#include "mod_loader.h"

#include "../common.h"

static int real_fade_counter;

static void ResetFadeCounter(void)
{
	real_fade_counter = 0;
	CS_fade_counter = 0;
}

static void TickFadeCounter(void)
{
	const int screen_width_tiles = ScreenWidthToTiles(16);
	const int screen_height_tiles = (240 + (16 - 1)) / 16;

	CS_fade_counter = (++real_fade_counter * (screen_width_tiles + screen_height_tiles + 16)) / 36;
}

void PatchFade(void)
{
	const int screen_width_tiles = ScreenWidthToTiles(16);
	const int screen_height_tiles = (240 + (16 - 1)) / 16;

	char *fade_buffer_bool = malloc(screen_width_tiles * screen_height_tiles);
	char *fade_buffer_int = malloc(screen_width_tiles * screen_height_tiles);

	// BeginFadeOut
	ModLoader_WriteCall((void*)0x40DED0, ResetFadeCounter);
	ModLoader_WriteNOPs((void*)0x40DED0 + 5, 5);
	ModLoader_WriteLong((void*)0x40DF25 + 3, (int)fade_buffer_int);
	ModLoader_WriteLong((void*)0x40DF36 + 3, (int)fade_buffer_bool);
	ModLoader_WriteByte((void*)0x40DF16 + 3, screen_width_tiles);
	ModLoader_WriteByte((void*)0x40DF1F + 2, screen_width_tiles);
	ModLoader_WriteByte((void*)0x40DF30 + 2, screen_width_tiles);

	// BeginFadeIn
	ModLoader_WriteCall((void*)0x40DF60, ResetFadeCounter);
	ModLoader_WriteNOPs((void*)0x40DF60 + 5, 5);
	ModLoader_WriteLong((void*)0x40DFB5 + 3, (int)fade_buffer_int);
	ModLoader_WriteLong((void*)0x40DFC6 + 3, (int)fade_buffer_bool);
	ModLoader_WriteByte((void*)0x40DFA6 + 3, screen_width_tiles);
	ModLoader_WriteByte((void*)0x40DFAF + 2, screen_width_tiles);
	ModLoader_WriteByte((void*)0x40DFC0 + 2, screen_width_tiles);

	// DrawFade
	ModLoader_WriteLong((void*)0x40E7E9 + 4, (int)fade_buffer_int);
	ModLoader_WriteByte((void*)0x40E7DA + 3, screen_width_tiles);
	ModLoader_WriteByte((void*)0x40E7E3 + 2, screen_width_tiles);

	// ProcessFade
	ModLoader_WriteByte((void*)0x40E3E3 + 3, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E3E9 + 1, screen_width_tiles - 1);
	ModLoader_WriteByte((void*)0x40E3FC + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E402 + 3, (int)fade_buffer_bool);

	ModLoader_WriteByte((void*)0x40E43D + 3, screen_width_tiles);
	ModLoader_WriteByte((void*)0x40E451 + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E457 + 3, (int)fade_buffer_bool);

	ModLoader_WriteByte((void*)0x40E492 + 3, screen_width_tiles);
	ModLoader_WriteByte((void*)0x40E4AB + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E4B1 + 3, (int)fade_buffer_bool);

	ModLoader_WriteByte((void*)0x40E4EC + 3, screen_width_tiles);
	ModLoader_WriteByte((void*)0x40E500 + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E506 + 3, (int)fade_buffer_bool);

	ModLoader_WriteByte((void*)0x40E541 + 3, screen_width_tiles / 2);
	ModLoader_WriteLong((void*)0x40E547 + 1, screen_width_tiles - 1);
	ModLoader_WriteByte((void*)0x40E55F + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E565 + 3, (int)fade_buffer_bool);

	ModLoader_WriteLong((void*)0x40E589 + 3, screen_width_tiles / 2);
	ModLoader_WriteByte((void*)0x40E59B + 3, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E5A1 + 1, screen_width_tiles - 1);
	ModLoader_WriteLong((void*)0x40E5AC + 1, screen_width_tiles - 1);
	ModLoader_WriteByte((void*)0x40E5BE + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E5C4 + 3, (int)fade_buffer_bool);

	ModLoader_WriteByte((void*)0x40E5FA + 3, screen_width_tiles / 2);
	ModLoader_WriteLong((void*)0x40E600 + 1, screen_width_tiles - 1);
	ModLoader_WriteByte((void*)0x40E61D + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E623 + 3, (int)fade_buffer_bool);

	ModLoader_WriteLong((void*)0x40E647 + 3, screen_width_tiles / 2);
	ModLoader_WriteByte((void*)0x40E659 + 3, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E65F + 1, screen_width_tiles - 1);
	ModLoader_WriteLong((void*)0x40E66A + 1, screen_width_tiles - 1);
	ModLoader_WriteByte((void*)0x40E683 + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E689 + 3, (int)fade_buffer_bool);

	ModLoader_WriteByte((void*)0x40E6BF + 3, screen_width_tiles);
	ModLoader_WriteByte((void*)0x40E6C8 + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E6CE + 4, (int)fade_buffer_int);
	ModLoader_WriteByte((void*)0x40E6DD + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E6E3 + 4, (int)fade_buffer_bool);
	ModLoader_WriteByte((void*)0x40E6F2 + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E6F8 + 3, (int)fade_buffer_int);
	ModLoader_WriteByte((void*)0x40E705 + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E70B + 3, (int)fade_buffer_int);
	ModLoader_WriteByte((void*)0x40E725 + 6, screen_width_tiles + screen_height_tiles + 16);

	ModLoader_WriteByte((void*)0x40E04B + 3, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E051 + 1, screen_width_tiles - 1);
	ModLoader_WriteByte((void*)0x40E064 + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E06A + 3, (int)fade_buffer_bool);

	ModLoader_WriteByte((void*)0x40E0A5 + 3, screen_width_tiles);
	ModLoader_WriteByte((void*)0x40E0B9 + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E0BF + 3, (int)fade_buffer_bool);

	ModLoader_WriteByte((void*)0x40E0FA + 3, screen_width_tiles);
	ModLoader_WriteByte((void*)0x40E113 + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E119 + 3, (int)fade_buffer_bool);

	ModLoader_WriteByte((void*)0x40E154 + 3, screen_width_tiles);
	ModLoader_WriteByte((void*)0x40E168 + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E16E + 3, (int)fade_buffer_bool);

	ModLoader_WriteByte((void*)0x40E1A9 + 3, screen_width_tiles / 2);
	ModLoader_WriteByte((void*)0x40E1C0 + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E1C6 + 3, (int)fade_buffer_bool);

	ModLoader_WriteLong((void*)0x40E1EA + 3, screen_width_tiles / 2);
	ModLoader_WriteByte((void*)0x40E1FC + 3, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E202 + 1, screen_width_tiles - 1);
	ModLoader_WriteByte((void*)0x40E218 + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E21E + 3, (int)fade_buffer_bool);

	ModLoader_WriteByte((void*)0x40E254 + 3, screen_width_tiles / 2);
	ModLoader_WriteByte((void*)0x40E270 + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E276 + 3, (int)fade_buffer_bool);

	ModLoader_WriteLong((void*)0x40E29A + 3, screen_width_tiles / 2);
	ModLoader_WriteByte((void*)0x40E2AC + 3, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E2B2 + 1, screen_width_tiles - 1);
	ModLoader_WriteByte((void*)0x40E2CF + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E2D5 + 3, (int)fade_buffer_bool);

	ModLoader_WriteByte((void*)0x40E30B + 3, screen_width_tiles);
	ModLoader_WriteByte((void*)0x40E314 + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E31A + 4, (int)fade_buffer_int);
	ModLoader_WriteByte((void*)0x40E32A + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E330 + 4, (int)fade_buffer_bool);
	ModLoader_WriteByte((void*)0x40E33F + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E345 + 3, (int)fade_buffer_int);
	ModLoader_WriteByte((void*)0x40E351 + 2, screen_width_tiles);
	ModLoader_WriteLong((void*)0x40E357 + 3, (int)fade_buffer_int);
	ModLoader_WriteByte((void*)0x40E36F + 6, screen_width_tiles + screen_height_tiles + 16);	// timer (15) + 1 ?

	ModLoader_WriteByte((void*)0x40E05F, 0x7F);	// jg
	ModLoader_WriteByte((void*)0x40E0B4, 0x7C);	// jl
	ModLoader_WriteByte((void*)0x40E10E, 0x7F);	// jg
	ModLoader_WriteByte((void*)0x40E163, 0x7C);	// jl
	ModLoader_WriteByte((void*)0x40E1BB, 0x7C);	// jl
	ModLoader_WriteByte((void*)0x40E213, 0x7C);	// jl
	ModLoader_WriteByte((void*)0x40E26B, 0x7C);	// jl
	ModLoader_WriteByte((void*)0x40E2CA, 0x7C);	// jl

	ModLoader_WriteCall((void*)0x40E36A, TickFadeCounter);

	ModLoader_WriteByte((void*)0x40E3F7, 0x7F);	// jg
	ModLoader_WriteByte((void*)0x40E44C, 0x7C);	// jl
	ModLoader_WriteByte((void*)0x40E4A6, 0x7F);	// jg
	ModLoader_WriteByte((void*)0x40E4FB, 0x7C);	// jl
	ModLoader_WriteByte((void*)0x40E55A, 0x7F);	// jg
	ModLoader_WriteByte((void*)0x40E5B9, 0x7F);	// jg
	ModLoader_WriteByte((void*)0x40E618, 0x7F);	// jg
	ModLoader_WriteByte((void*)0x40E67E, 0x7F);	// jg

	ModLoader_WriteCall((void*)0x40E71F, TickFadeCounter);
	ModLoader_WriteNOPs((void*)0x40E71F + 5, 1);
}
