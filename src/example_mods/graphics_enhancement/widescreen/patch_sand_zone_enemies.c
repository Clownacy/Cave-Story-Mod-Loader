// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2018 Clownacy

#include "patch_sand_zone_enemies.h"

#include "mod_loader.h"

#include "../common.h"

void PatchSandZoneEnemies(void)
{
	// Obj51_Crow&SkullHead
	ModLoader_WriteLong((void*)0x4301F3 + 1, ((SCREEN_WIDTH / 2) + 160) * 0x200);
	ModLoader_WriteLong((void*)0x43020A + 2, ((SCREEN_WIDTH / 2) + 160) * 0x200);
	// Obj134_Armadillo
	ModLoader_WriteLong((void*)0x441C2E + 2, ((SCREEN_WIDTH / 2) + 160) * 0x200);
	ModLoader_WriteLong((void*)0x441C42 + 1, ((SCREEN_WIDTH / 2) + 160) * 0x200);
}
