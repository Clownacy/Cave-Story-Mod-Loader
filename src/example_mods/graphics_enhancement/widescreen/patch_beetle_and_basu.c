// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch_beetle_and_basu.h"

#include "mod_loader.h"

#include "../common.h"

void PatchBeetleAndBasu(void)
{
	// Beetle (object 8)
	ModLoader_WriteLong((void*)0x427F9A + 1, (((SCREEN_WIDTH - 320) / 2) + 16) * 0x200);
	ModLoader_WriteLong((void*)0x427FB1 + 2, (((SCREEN_WIDTH - 320) / 2) + 16) * 0x200);
	ModLoader_WriteLong((void*)0x428010 + 2, ((SCREEN_WIDTH / 2) + 96) * 0x200);
	ModLoader_WriteLong((void*)0x42802E + 2, ((SCREEN_WIDTH / 2) + 96) * 0x200);
	// Basu (object 58)
	ModLoader_WriteLong((void*)0x431CF2 + 1, (((SCREEN_WIDTH - 320) / 2) + 16) * 0x200);
	ModLoader_WriteLong((void*)0x431D09 + 2, (((SCREEN_WIDTH - 320) / 2) + 16) * 0x200);
	ModLoader_WriteLong((void*)0x431D93 + 1, ((SCREEN_WIDTH / 2) + 96) * 0x200);
	ModLoader_WriteLong((void*)0x431DAF + 1, ((SCREEN_WIDTH / 2) + 96) * 0x200);
	// Beetle2 (object 210)
	ModLoader_WriteLong((void*)0x45049A + 1, (((SCREEN_WIDTH - 320) / 2) + 16) * 0x200);
	ModLoader_WriteLong((void*)0x4504B1 + 2, (((SCREEN_WIDTH - 320) / 2) + 16) * 0x200);
	ModLoader_WriteLong((void*)0x450510 + 2, ((SCREEN_WIDTH / 2) + 96) * 0x200);
	ModLoader_WriteLong((void*)0x45052E + 2, ((SCREEN_WIDTH / 2) + 96) * 0x200);
	// Basu2 (object 208)
	ModLoader_WriteLong((void*)0x44FD82 + 1, (((SCREEN_WIDTH - 320) / 2) + 16) * 0x200);
	ModLoader_WriteLong((void*)0x44FD99 + 2, (((SCREEN_WIDTH - 320) / 2) + 16) * 0x200);
	ModLoader_WriteLong((void*)0x44FE23 + 1, ((SCREEN_WIDTH / 2) + 96) * 0x200);
	ModLoader_WriteLong((void*)0x44FE3F + 1, ((SCREEN_WIDTH / 2) + 96) * 0x200);
}
