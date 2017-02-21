#include "mod_loader.h"

#include "common.h"
#include "patch_map_screen.h"

void PatchMapMenu(void)
{
	// Centre actual map graphic
	WriteLong(0x414733 + 1, SCREEN_WIDTH / 2);
	WriteLong(0x414756 + 1, SCREEN_WIDTH / 2);
	WriteLong(0x414A4B + 1, SCREEN_WIDTH / 2);
	WriteLong(0x414A6E + 1, SCREEN_WIDTH / 2);
	// Centre bar that level name gets placed over
	WriteLong(0x414264 + 3, SCREEN_WIDTH / 2);
	WriteLong(0x41427F + 3, SCREEN_WIDTH);
}
