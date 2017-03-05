// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "mod_loader.h"

#include "../common.h"
#include "patch_gaudi.h"

void PatchGaudi(void)
{
	// Regular Gaudi
	WriteLong(0x446029 + 1, SCREEN_WIDTH * 0x200);
	WriteLong(0x44603C + 2, SCREEN_WIDTH * 0x200);
	// Armoured Gaudi
	WriteLong(0x449E6A + 1, SCREEN_WIDTH * 0x200);
	WriteLong(0x449E7D + 2, SCREEN_WIDTH * 0x200);
}