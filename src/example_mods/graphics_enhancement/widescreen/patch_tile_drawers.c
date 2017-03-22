// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch_tile_drawers.h"

#include "mod_loader.h"

#include "../common.h"

void PatchTileDrawers(void)
{
	// DrawBackground
	WriteLong(0x402445 + 3, SCREEN_WIDTH);
	WriteLong(0x4024D7 + 3, SCREEN_WIDTH);
	WriteLong(0x40255F + 3, SCREEN_WIDTH);
	WriteLong(0x4025D9 + 3, SCREEN_WIDTH);
	// DrawForegroundBG
	WriteLong(0x413AF6 + 3, ScreenWidthToTiles(16) + 1);
	// DrawForegroundFG
	WriteLong(0x413C82 + 3, ScreenWidthToTiles(16) + 1);
	// DrawRushingWaterParticles
	WriteLong(0x413E55 + 3, ScreenWidthToTiles(16) + 1);
	// Patch Core water to cover screen
	WriteByte(0x402896 + 2, ScreenWidthToTiles(32) + 1);
}
