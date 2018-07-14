// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

// This here centres the camera properly on Quote, but also makes rooms that are
// smaller than the screen centred.

// Unfortuately, there's a problem that's very hard to work around:
// When you're in a small room, the camera moves left to fit it in the middle.
// This has the effect of making the camera X coordinate negative.
// Some divisions are performed on the camera X to calculate the left boundary
// of some sprites. Doing these calculations on the now-negative camera X will
// have bad results. This causes a visible bug in the background of some small
// rooms, where the leftmost column of tiles does not appear.
// To work around this, we only move the foreground's camera to the left,
// leaving the background (and Core water) camera in the positive range.

#include "patch_camera.h"

#include <stdbool.h>

#include "cave_story.h"
#include "mod_loader.h"

#include "../common.h"
#include "black_bars.h"

static bool small_room;

__stdcall void UpdateCamera_extra(const int level_width, const int level_height)
{
	current_level_width = level_width;

	int level_width_pixels = (level_width - 1) * 16;

	if (level_width_pixels > SCREEN_WIDTH)
	{
		small_room = false;

		if (CS_gamemode_flags & 8)	// If in credits, emulate the original camera boundaries (fixes flying Balrog scene)
		{
			if (CS_camera_x_pos < 0)
				CS_camera_x_pos = 0;
			if (CS_camera_x_pos > (((level_width - 1) * 16) - 320) * 0x200)
				CS_camera_x_pos = (((level_width - 1) * 16) - 320) * 0x200;

			CS_camera_x_pos -= ((SCREEN_WIDTH - 320) / 2) * 0x200;
		}
		else
		{
			if (CS_camera_x_pos < 0)
				CS_camera_x_pos = 0;
			if (CS_camera_x_pos > (((level_width - 1) * 16) - SCREEN_WIDTH) * 0x200)
				CS_camera_x_pos = (((level_width - 1) * 16) - SCREEN_WIDTH) * 0x200;
		}
	}
	else
	{
		small_room = true;

		CS_camera_x_pos = -(((SCREEN_WIDTH - level_width_pixels) / 2) * 0x200);
	}

	if (CS_camera_y_pos < 0)
		CS_camera_y_pos = 0;
	if (CS_camera_y_pos > (((level_height - 1) * 16) - 240) * 0x200)
		CS_camera_y_pos = (((level_height - 1) * 16) - 240) * 0x200;
}


__asm(
"_UpdateCamera_extra_asm:\n"
"	movswl	-4(%ebp), %eax\n"
"	push	%eax\n"
"	movswl	-8(%ebp), %eax\n"
"	push	%eax\n"
"	call	_UpdateCamera_extra@8\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x40EF76\n"
);
extern char UpdateCamera_extra_asm;

#define hijackCheckSmallRoom(name, address) void name##_hijack(int x, int y) \
{ \
	void (*name)(int x, int y) = (void(*)(int x, int y))address; \
 \
	name(small_room ? 0 : x, y); \
} \

hijackCheckSmallRoom(DrawBackground, 0x4023D0);

__asm(
"_DrawWater_new:\n"
"	movl	_small_room, %eax\n"
"	test	%eax, %eax\n"
"	jz	notSmallRoom\n"
"	movl	$0, 4(%esp)\n"
"notSmallRoom:\n"
"	pushl	%ebp\n"
"	movl	%esp, %ebp\n"
"	subl	$0x44, %esp\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x402836\n"

);
void DrawWater_new(void);

void PatchCamera(void)
{
	// Patch camera to centre properly
	ModLoader_WriteLong((void*)0x40EE90 + 1, (SCREEN_WIDTH / 2) * 0x200);	// Patch camera so Quote is still centred
	ModLoader_WriteLong((void*)0x40EF19 + 2, SCREEN_WIDTH);
	ModLoader_WriteLong((void*)0x40EF34 + 2, SCREEN_WIDTH);
	ModLoader_WriteJump((void*)0x40EED1, &UpdateCamera_extra_asm);
	// SetCameraUponEnterRoom - This function is what decides where the camera starts off when you enter a room
	ModLoader_WriteLong((void*)0x40F15B + 2, (SCREEN_WIDTH / 2) * 0x200);
	ModLoader_WriteLong((void*)0x40F1BE + 1, SCREEN_WIDTH);
	ModLoader_WriteLong((void*)0x40F1D8 + 2, SCREEN_WIDTH);

	ModLoader_WriteRelativeAddress((void*)0x40F871 + 1, DrawBackground_hijack);
	ModLoader_WriteRelativeAddress((void*)0x410633 + 1, DrawBackground_hijack);
	ModLoader_WriteJump((void*)0x402830, DrawWater_new);
}
