// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "mod_loader.h"

#include "../common.h"
#include "patch_ironhead.h"

void PatchIronhead(void)
{
	// Patch current/dust generator (object 219)
	WriteLong(0x451E3B + 1, SCREEN_WIDTH / 2);
	WriteLong(0x451E40 + 1, -(SCREEN_WIDTH / 2));
	// Patch Ironhead (boss object 5)
	WriteLong(0x47A971 + 3, (488 - ((SCREEN_WIDTH / 2) + 168)) * 0x200);
	WriteLong(0x47AB3B + 3, (488 - ((SCREEN_WIDTH / 2) + 88)) * 0x200);
	WriteLong(0x47AD94 + 3, (488 - ((SCREEN_WIDTH / 2) + 56)) * 0x200);
	WriteLong(0x47AB58 + 3, (488 + ((SCREEN_WIDTH / 2) + 72)) * 0x200);
	WriteLong(0x47AD69 + 3, (488 + ((SCREEN_WIDTH / 2) + 72)) * 0x200);
	WriteByte(0x47AAFA + 1, (488 - ((SCREEN_WIDTH / 2) + 40)) / 16);
	WriteByte(0x47AAFC + 1, (488 - ((SCREEN_WIDTH / 2) + 88)) / 16);
	// Patch moving blocks (object 196)
	WriteLong(0x44DFB3 + 3, (488 - ((SCREEN_WIDTH / 2) + 24)) * 0x200);
	WriteLong(0x44DFC2 + 2, (SCREEN_WIDTH + 32) * 0x200);
	// Patch pufferfish (object 197)
	WriteByte(0x44E1FB + 2, 2048 / ((SCREEN_WIDTH * 2 * 512) / 2048));	// Logarithmic math is fun
}
