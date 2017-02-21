#include "mod_loader.h"

#include "common.h"
#include "drawsprite1_centred.h"
#include "patch_teleport_screen.h"

void PatchTeleportScreen(void)
{
	// Clip rectangle
	WriteLong(0x41D854 + 3, SCREEN_WIDTH);
	// "WARP"
	WriteRelativeAddress(0x41D8E2 + 1, DrawSprite1_hijack);
	// Cursor
	WriteRelativeAddress(0x41D961 + 1, DrawSprite1_hijack);
	// Location icons
	WriteRelativeAddress(0x41D9EE + 1, DrawSprite1_hijack);
}
