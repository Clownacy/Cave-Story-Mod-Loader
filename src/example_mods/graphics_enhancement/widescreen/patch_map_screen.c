// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch_map_screen.h"

#include "mod_loader.h"

#include "../common.h"

void PatchMapMenu(void)
{
	// Centre actual map graphic
	ModLoader_WriteLong((void*)0x414733 + 1, SCREEN_WIDTH / 2);
	ModLoader_WriteLong((void*)0x414756 + 1, SCREEN_WIDTH / 2);
	ModLoader_WriteLong((void*)0x414A4B + 1, SCREEN_WIDTH / 2);
	ModLoader_WriteLong((void*)0x414A6E + 1, SCREEN_WIDTH / 2);
	// Centre bar that level name gets placed over
	ModLoader_WriteLong((void*)0x414264 + 3, SCREEN_WIDTH / 2);
	ModLoader_WriteLong((void*)0x41427F + 3, SCREEN_WIDTH);
}
