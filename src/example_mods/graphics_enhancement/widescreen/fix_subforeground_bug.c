// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "fix_subforeground_bug.h"

#include "cave_story.h"
#include "mod_loader.h"

// This here fixes a bug in the game's sub-FG renderer, where out-of-bound tiles
// are drawn. The function returns 0 in the event of an error, but 0 is a valid
// number for sub-FG, so we use a flag instead.

unsigned char bad_tile_flag;

static unsigned char GetTileID_new(const int tile_x, const int tile_y)
{
	unsigned char tile_id;

	if (tile_x >= 0 && tile_y >= 0 && tile_x < CS_level_width && tile_y < CS_level_height)
	{
		bad_tile_flag = 0;
		tile_id = CS_pxa_buffer[CS_pxm_buffer[tile_x + (tile_y * CS_level_width)]];
	}
	else
	{
		bad_tile_flag = 0xFF;
		tile_id = 0;
	}

	return tile_id;
}

__asm(
"_DrawForegroundBG_patch:\n"
"	movb	_bad_tile_flag, %al\n"
"	test	%al, %al\n"
"	jnz	2f\n"
"	cmp	$0x20, -0x28(%ebp)\n"
"	jl	3f\n"
"2:\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x413B68\n"
"3:\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x413BAF\n"
);
extern char DrawForegroundBG_patch;

void FixSubForegroundBug(void)
{
	ModLoader_WriteJump((void*)0x4139A0, GetTileID_new);
	ModLoader_WriteJump((void*)0x413BA7, &DrawForegroundBG_patch);
}
