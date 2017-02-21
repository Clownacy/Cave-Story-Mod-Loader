#include "mod_loader.h"

#include "drawsprite1_centred.h"
#include "patch_boss_health.h"

void PatchBossHealth(void)
{
	WriteRelativeAddress(0x47B67A + 1, DrawSprite1_hijack);
	WriteRelativeAddress(0x47B694 + 1, DrawSprite1_hijack);
	WriteRelativeAddress(0x47B6AE + 1, DrawSprite1_hijack);
	WriteRelativeAddress(0x47B6C8 + 1, DrawSprite1_hijack);
	WriteRelativeAddress(0x47B6E2 + 1, DrawSprite1_hijack);
}
