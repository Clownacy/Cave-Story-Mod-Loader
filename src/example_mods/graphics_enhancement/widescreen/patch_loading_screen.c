// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "mod_loader.h"

#include "../common.h"
#include "patch_loading_screen.h"

void PatchLoadingScreen(void)
{
	// Clip rectangle
	WriteLong(0x412A81 + 6, SCREEN_WIDTH);
	// x_pos
	WriteLong(0x412AC6 + 1, (SCREEN_WIDTH - 64) / 2);
}
