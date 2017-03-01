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

#include <stdbool.h>

#include "mod_loader.h"

#include "../common.h"
#include "patch_camera.h"

bool small_room;

bool __stdcall UpdateCamera_extra(const unsigned int level_width, const unsigned int level_height)
{
	signed int *camera_x_pos = (signed int*)0x49E1C8;
	signed int *camera_y_pos = (signed int*)0x49E1CC;

	if (((level_width - 1) * 16) > SCREEN_WIDTH)
	{
		small_room = false;
		return false;
	}

	*camera_x_pos = -(((SCREEN_WIDTH - (level_width - 1) * 16) / 2) * 0x200);
	small_room = true;

	if ((*camera_y_pos / 0x200) < 0)
		*camera_y_pos = 0;
	else if ((*camera_y_pos / 0x200) > (((level_height - 1) * 16) - 240))
		*camera_y_pos = (((level_height - 1) * 16) - 240) * 0x200;

	return true;
}

__asm(
"_UpdateCamera_extra_asm:\n"
"	push	%edx\n"
"	push	-4(%ebp)\n"
"	push	-8(%ebp)\n"
"	call	_UpdateCamera_extra@8\n"
"	pop	%edx\n"
"	test	%eax, %eax\n"
"	jnz	0x40EF76\n"
"	movl	0x49E1C8, %eax\n"
"	jmp	0x40EED6\n"
);
void UpdateCamera_extra_asm(void);

#define hijackCheckSmallRoom(name, address) void name##_hijack(int x, int y) \
{ \
	void (*name)(int x, int y) = (void(*)(int x, int y))address; \
 \
	name(small_room ? 0 : x, y); \
} \

hijackCheckSmallRoom(DrawBackground, 0x4023D0);
hijackCheckSmallRoom(DrawWater, 0x402830);

void PatchCamera(void)
{
	// Patch camera to centre properly
	WriteLong(0x40EE90 + 1, (SCREEN_WIDTH / 2) * 0x200);	// Patch camera so Quote is still centred
	WriteLong(0x40EF19 + 2, SCREEN_WIDTH);
	WriteLong(0x40EF34 + 2, SCREEN_WIDTH);
	WriteJump(0x40EED1, UpdateCamera_extra_asm);
	// SetCameraUponEnterRoom - This function is what decides where the camera starts off when you enter a room
	WriteLong(0x40F15B + 2, (SCREEN_WIDTH / 2) * 0x200);
	WriteLong(0x40F1BE + 1, SCREEN_WIDTH);
	WriteLong(0x40F1D8 + 2, SCREEN_WIDTH);

	WriteRelativeAddress(0x410633+1, DrawBackground_hijack);
	WriteRelativeAddress(0x4106C3+1, DrawWater_hijack);
}
