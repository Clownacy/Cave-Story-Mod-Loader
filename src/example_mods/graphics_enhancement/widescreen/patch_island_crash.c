// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch_island_crash.h"

#include "mod_loader.h"

#include "../common.h"
#include "drawsprite1_centred.h"

void PatchIslandCrash(void)
{
	WriteLong((void*)0x40DB76 + 3, 0x50 + ((SCREEN_WIDTH - 320) / 2));
	WriteLong((void*)0x40DB84 + 3, 0xF0 + ((SCREEN_WIDTH - 320) / 2));
	WriteLong((void*)0x40DBE6 + 3, 0x15000 + (((SCREEN_WIDTH - 320) / 2) * 0x200));
	WriteRelativeAddress((void*)0x40DCDC + 1, DrawSprite1_hijack);
	WriteRelativeAddress((void*)0x40DD2D + 1, DrawSprite1_hijack);
}
