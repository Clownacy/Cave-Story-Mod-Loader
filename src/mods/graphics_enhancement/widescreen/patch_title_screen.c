// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch_title_screen.h"

#include "mod_loader.h"

#include "../common.h"
#include "drawsprite1_centred.h"

void PatchTitleScreen(void)
{
	// "Version"
	ModLoader_WriteRelativeAddress((void*)0x410026 + 1, DrawSprite1_hijack);
	// "...."
	ModLoader_WriteRelativeAddress((void*)0x410046 + 1, DrawSprite1_hijack);
	ModLoader_WriteLong((void*)0x410059+1, 0x8C + (SCREEN_WIDTH - 320) / 2);
	ModLoader_WriteLong((void*)0x410071+1, 0x9C + (SCREEN_WIDTH - 320) / 2);
	ModLoader_WriteLong((void*)0x410089+1, 0xAC + (SCREEN_WIDTH - 320) / 2);
	ModLoader_WriteLong((void*)0x4100A1+1, 0xBC + (SCREEN_WIDTH - 320) / 2);
	// "Cave Story"
	ModLoader_WriteRelativeAddress((void*)0x4100BD + 1, DrawSprite1_hijack);
	// "New"
	ModLoader_WriteRelativeAddress((void*)0x4100DD + 1, DrawSprite1_hijack);
	// "Load"
	ModLoader_WriteRelativeAddress((void*)0x4100FA + 1, DrawSprite1_hijack);
	// "Studio Pixel"
	ModLoader_WriteRelativeAddress((void*)0x410117 + 1, DrawSprite1_hijack);
	// Quote
	ModLoader_WriteRelativeAddress((void*)0x41031D + 1, DrawSprite1_hijack);
	ModLoader_WriteRelativeAddress((void*)0x410340 + 1, DrawSprite1_hijack);
}
