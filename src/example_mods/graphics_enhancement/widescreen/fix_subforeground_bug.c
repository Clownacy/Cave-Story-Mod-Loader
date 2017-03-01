#include "mod_loader.h"

#include "fix_subforeground_bug.h"

// This here fixes a bug in the game's sub-FG renderer, where out-of-bound tiles
// are draw. The function returns 0 in the event of an error, but 0 is a valid
// number for sub-FG, so we use a flag instead.

char bad_tile_flag;

char GetTileID_new(const int tile_x, const int tile_y)
{
	unsigned short *level_width = (unsigned short*)0x49E586;
	unsigned short *level_height = (unsigned short*)0x49E588;

	char *byte_49E484 = (char*)0x49E484;
	int *dword_49E480 = (int*)0x49E480;

	if (tile_x >= 0 && tile_y >= 0 && tile_x < (*level_width) && tile_y < (*level_height))
	{
		bad_tile_flag = 0;
		return byte_49E484[*((unsigned char*)(*dword_49E480) + tile_x + (tile_y * (*level_width)))];
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
void DrawForegroundBG_patch(void);

void FixSubForegroundBug(void)
{
	WriteJump(0x4139A0, GetTileID_new);
	WriteJump(0x413BA7, DrawForegroundBG_patch);
}
