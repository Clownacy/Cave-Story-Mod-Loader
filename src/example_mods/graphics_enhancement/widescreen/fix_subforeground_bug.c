// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "fix_subforeground_bug.h"

#include "cave_story.h"
#include "mod_loader.h"

// This here fixes a bug in the game's sub-FG renderer, where out-of-bound tiles
// are drawn. The function returns 0 in the event of an error, but 0 is a valid
// number for sub-FG, so we use a flag instead.

char bad_tile_flag;

static char GetTileID_new(const int tile_x, const int tile_y)
{
	if (tile_x >= 0 && tile_y >= 0 && tile_x < CS_level_width && tile_y < CS_level_height)
	{
		bad_tile_flag = 0;
		return CS_byte_49E484[*((unsigned char*)CS_level_layout_buffer + tile_x + (tile_y * CS_level_width))];
	}
	else
	{
		bad_tile_flag = 0xFF;
		return 0;
	}
}

__asm(
"_DrawForegroundBG_patch:\n"
"	movb	_bad_tile_flag, %al\n"
"	test	%al, %al\n"
"	jnz	0x413B68\n"
"	cmp	$0x20, -0x28(%ebp)\n"
"	jl	0x413BAF\n"
"	jmp	0x413B68\n"
);
extern char DrawForegroundBG_patch;

void FixSubForegroundBug(void)
{
	WriteJump((void*)0x4139A0, GetTileID_new);
	WriteJump((void*)0x413BA7, &DrawForegroundBG_patch);
}
