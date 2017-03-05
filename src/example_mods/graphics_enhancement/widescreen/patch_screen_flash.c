// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "mod_loader.h"

#include "../common.h"
#include "patch_screen_flash.h"

void PatchScreenFlash(void)
{
	WriteLong(0x40ED90 + 6, SCREEN_WIDTH);
}
