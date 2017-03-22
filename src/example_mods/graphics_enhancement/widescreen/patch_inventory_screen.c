// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch_inventory_screen.h"

#include "cave_story.h"
#include "mod_loader.h"

#include "../common.h"
#include "drawsprite1_centred.h"

void SomethingHUDNumbers_hijack(int x, int y, int a3, int a4)
{
	DrawNumber(((SCREEN_WIDTH - 320) / 2) + x, y, a3, a4);
}

void PatchInventoryScreen(void)
{
	WriteLong(0x401779 + 6, SCREEN_WIDTH);

	WriteLong(0x401D2F + 3, SCREEN_WIDTH);
	WriteLong(0x41152B + 1, SCREEN_WIDTH);	// TODO: this fixes more than just the inventory - it's the main screengrab surface

	WriteRelativeAddress(0x4018BE + 1, DrawSprite1_hijack);
	WriteRelativeAddress(0x4018F8 + 1, DrawSprite1_hijack);
	WriteRelativeAddress(0x40191F + 1, DrawSprite1_hijack);
	WriteRelativeAddress(0x401955 + 1, DrawSprite1_hijack);
	WriteRelativeAddress(0x401976 + 1, DrawSprite1_hijack);
	WriteRelativeAddress(0x4019C3 + 1, DrawSprite1_hijack);
	WriteRelativeAddress(0x4019E8 + 1, DrawSprite1_hijack);
	WriteRelativeAddress(0x401A8E + 1, DrawSprite1_hijack);
	WriteRelativeAddress(0x401AB2 + 1, DrawSprite1_hijack);
	WriteRelativeAddress(0x401AD3 + 1, DrawSprite1_hijack);
	WriteRelativeAddress(0x401AF6 + 1, SomethingHUDNumbers_hijack);
	WriteRelativeAddress(0x401B28 + 1, SomethingHUDNumbers_hijack);
	WriteRelativeAddress(0x401B4B + 1, SomethingHUDNumbers_hijack);
	WriteRelativeAddress(0x401B71 + 1, DrawSprite1_hijack);
	WriteRelativeAddress(0x401B95 + 1, DrawSprite1_hijack);
	WriteRelativeAddress(0x401BF7 + 1, DrawSprite1_hijack);
	WriteRelativeAddress(0x401C39 + 1, DrawSprite1_hijack);
	WriteRelativeAddress(0x401CFC + 1, DrawSprite1_hijack);
}
