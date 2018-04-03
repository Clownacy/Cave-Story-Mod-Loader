// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch_boss_explosion.h"

#include "mod_loader.h"

#include "../common.h"

void PatchBossExplosion(void)
{
	ModLoader_WriteLong((void*)0x40EB95 + 3, SCREEN_WIDTH);
	ModLoader_WriteLong((void*)0x40EB9E + 3, SCREEN_WIDTH);
	ModLoader_WriteLong((void*)0x40EBE4 + 6, SCREEN_WIDTH);
	ModLoader_WriteLong((void*)0x40ED03 + 6, SCREEN_WIDTH);
}
