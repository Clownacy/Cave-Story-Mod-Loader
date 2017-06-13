// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch_gaudi.h"

#include "mod_loader.h"

#include "../common.h"

void PatchGaudi(void)
{
	// Regular Gaudi
	WriteLong((void*)0x446029 + 1, SCREEN_WIDTH * 0x200);
	WriteLong((void*)0x44603C + 2, SCREEN_WIDTH * 0x200);
	// Flying Gaudi
	WriteLong((void*)0x44671B + 1, SCREEN_WIDTH * 0x200);
	WriteLong((void*)0x44672E + 2, SCREEN_WIDTH * 0x200);
	// Armoured Gaudi
	WriteLong((void*)0x449E6A + 1, SCREEN_WIDTH * 0x200);
	WriteLong((void*)0x449E7D + 2, SCREEN_WIDTH * 0x200);
}
