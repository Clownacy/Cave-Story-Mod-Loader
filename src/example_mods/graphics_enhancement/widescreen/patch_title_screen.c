#include "mod_loader.h"

#include "../common.h"
#include "drawsprite1_centred.h"
#include "patch_title_screen.h"

void PatchTitleScreen(void)
{
	// "Version"
	WriteRelativeAddress(0x410026 + 1, DrawSprite1_hijack);
	// "...."
	WriteRelativeAddress(0x410046 + 1, DrawSprite1_hijack);
	WriteLong(0x410059+1, 0x8C + (SCREEN_WIDTH - 320) / 2);
	WriteLong(0x410071+1, 0x9C + (SCREEN_WIDTH - 320) / 2);
	WriteLong(0x410089+1, 0xAC + (SCREEN_WIDTH - 320) / 2);
	WriteLong(0x4100A1+1, 0xBC + (SCREEN_WIDTH - 320) / 2);
	// "Cave Story"
	WriteRelativeAddress(0x4100BD + 1, DrawSprite1_hijack);
	// "New"
	WriteRelativeAddress(0x4100DD + 1, DrawSprite1_hijack);
	// "Load"
	WriteRelativeAddress(0x4100FA + 1, DrawSprite1_hijack);
	// "Studio Pixel"
	WriteRelativeAddress(0x410117 + 1, DrawSprite1_hijack);
	// Quote
	WriteRelativeAddress(0x41031D + 1, DrawSprite1_hijack);
	WriteRelativeAddress(0x410340 + 1, DrawSprite1_hijack);
}
