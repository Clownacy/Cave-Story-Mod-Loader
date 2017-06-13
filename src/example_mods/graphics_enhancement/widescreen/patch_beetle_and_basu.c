// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch_beetle_and_basu.h"

#include "mod_loader.h"

#include "../common.h"

void PatchBeetleAndBasu(void)
{
	// Beetle (object 8)
	WriteLong((void*)0x428010 + 2, ((SCREEN_WIDTH / 2) + 96) * 0x200);
	WriteLong((void*)0x42802E + 2, ((SCREEN_WIDTH / 2) + 96) * 0x200);
	// Basu (object 58)
	WriteLong((void*)0x431D93 + 1, ((SCREEN_WIDTH / 2) + 96) * 0x200);
	WriteLong((void*)0x431DAF + 1, ((SCREEN_WIDTH / 2) + 96) * 0x200);
	// Beetle2 (object 210)
	WriteLong((void*)0x450510 + 2, ((SCREEN_WIDTH / 2) + 96) * 0x200);
	WriteLong((void*)0x45052E + 2, ((SCREEN_WIDTH / 2) + 96) * 0x200);
	// Basu2 (object 208)
	WriteLong((void*)0x44FE23 + 1, ((SCREEN_WIDTH / 2) + 96) * 0x200);
	WriteLong((void*)0x44FE3F + 1, ((SCREEN_WIDTH / 2) + 96) * 0x200);
}
