// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch_screen_flash.h"

#include "mod_loader.h"

#include "../common.h"

void PatchScreenFlash(void)
{
	WriteLong((void*)0x40ED90 + 6, SCREEN_WIDTH);
}
