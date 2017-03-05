// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "mod_loader.h"

#include "../common.h"
#include "patch_boss_explosion.h"

void PatchBossExplosion(void)
{
	WriteLong(0x40EB95 + 3, SCREEN_WIDTH);
	WriteLong(0x40EB9E + 3, SCREEN_WIDTH);
	WriteLong(0x40EBE4 + 6, SCREEN_WIDTH);
	WriteLong(0x40ED03 + 6, SCREEN_WIDTH);
}
