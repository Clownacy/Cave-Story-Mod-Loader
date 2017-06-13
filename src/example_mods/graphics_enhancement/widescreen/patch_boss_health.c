// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch_boss_health.h"

#include "mod_loader.h"

#include "drawsprite1_centred.h"

void PatchBossHealth(void)
{
	WriteRelativeAddress((void*)0x47B67A + 1, DrawSprite1_hijack);
	WriteRelativeAddress((void*)0x47B694 + 1, DrawSprite1_hijack);
	WriteRelativeAddress((void*)0x47B6AE + 1, DrawSprite1_hijack);
	WriteRelativeAddress((void*)0x47B6C8 + 1, DrawSprite1_hijack);
	WriteRelativeAddress((void*)0x47B6E2 + 1, DrawSprite1_hijack);
}
