// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <windows.h>

#include "mod_loader.h"

#include "../common.h"
#include "patch_text_box.h"

void DrawHUD_Air_hijack(int x, int y)
{
	void (*DrawHUD_Air)(int x, int y) = (void(*)(int x, int y))0x41A350;

	DrawHUD_Air(((SCREEN_WIDTH - 320) / 2) + x, y);
}

void TextBoxDrawSpriteHijack(RECT* clip, int x, int y, RECT* src, int slot)
{
	void (*DrawSprite)(RECT* clip, int x, int y, RECT* src, int slot) = (void(*)(RECT* clip, int x, int y, RECT* src, int slot))0x40C3C0;

	DrawSprite(clip, ((SCREEN_WIDTH - 244) / 2) + (x - 38), y, src, slot);
}

void ClearScreenHijack(RECT* rect, int flags)
{
	rect->left += (SCREEN_WIDTH - 320) / 2;
	rect->right += (SCREEN_WIDTH - 320) / 2;
	void (*ClearScreen)(RECT* rect, int flags) = (void(*)(RECT* rect, int flags))0x40C9E0;

	ClearScreen(rect, flags);
}

void PatchTextBox(void)
{
//	WriteByte(0x41A350, 0xC3);
	WriteRelativeAddress(0x42201A + 1, TextBoxDrawSpriteHijack);
	WriteRelativeAddress(0x422058 + 1, TextBoxDrawSpriteHijack);
	WriteRelativeAddress(0x42207F + 1, TextBoxDrawSpriteHijack);
	WriteRelativeAddress(0x422118 + 1, TextBoxDrawSpriteHijack);
	WriteRelativeAddress(0x422180 + 1, TextBoxDrawSpriteHijack);
	WriteRelativeAddress(0x4222C6 + 1, TextBoxDrawSpriteHijack);
	WriteRelativeAddress(0x4222E0 + 1, TextBoxDrawSpriteHijack);
	WriteRelativeAddress(0x422300 + 1, TextBoxDrawSpriteHijack);
	WriteRelativeAddress(0x42231D + 1, TextBoxDrawSpriteHijack);
	WriteRelativeAddress(0x42233A + 1, TextBoxDrawSpriteHijack);
	WriteRelativeAddress(0x422357 + 1, TextBoxDrawSpriteHijack);
	WriteRelativeAddress(0x4223DB + 1, TextBoxDrawSpriteHijack);
	WriteRelativeAddress(0x422446 + 1, TextBoxDrawSpriteHijack);
	WriteRelativeAddress(0x4224CD + 1, TextBoxDrawSpriteHijack);
	WriteRelativeAddress(0x4224FE + 1, TextBoxDrawSpriteHijack);
	WriteRelativeAddress(0x42220D + 1, ClearScreenHijack);
	WriteRelativeAddress(0x41084E + 1, DrawHUD_Air_hijack);

	WriteLong(0x4219F9 + 6, ((SCREEN_WIDTH - 244) / 2) + 14);
	WriteLong(0x421A0D + 6, SCREEN_WIDTH - (((SCREEN_WIDTH - 244) / 2) + 14));
}
