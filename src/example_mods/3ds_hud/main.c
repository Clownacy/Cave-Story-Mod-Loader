// 3DS HUD mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "cave_story.h"
#include "mod_loader.h"

#define HUD_X 8
#define HUD_Y 4

void DrawHUD_WeaponIcons_hijack(void *clip_rect, int x_pos, int y_pos, void *src_rect, int surface_id)
{
	DrawSprite_WithTransparency(clip_rect, x_pos + HUD_X + 56, y_pos, src_rect, surface_id);
}

void InitMod(void)
{
	// DrawHUD_EXPAndText
	// hud_scroll_x_pos
	WriteLong((void*)0x401003 + 6, HUD_X + 16);
	WriteLong((void*)0x401F04 + 6, HUD_X + 16);
	WriteLong((void*)0x402166 + 6, HUD_X + 16);
	WriteLong((void*)0x40220E + 6, HUD_X - 16);
	WriteLong((void*)0x40224D + 6, HUD_X + 16);
	WriteByte((void*)0x419D95 + 6, HUD_X);
	WriteByte((void*)0x419DAB + 6, HUD_X);

	WriteByte((void*)0x419EB1 + 1, HUD_Y + 1);	// Lv Y
	WriteByte((void*)0x419EDA + 1, HUD_Y + 1);	// Weapon level Y
	WriteByte((void*)0x419FAF + 1, HUD_Y + 1);	// EXP bar back Y
	WriteByte((void*)0x419FF6 + 1, HUD_Y + 1);	// EXP bar max Y
	WriteByte((void*)0x41A038 + 1, HUD_Y + 1);	// EXP bar Y
	WriteByte((void*)0x41A08D + 1, HUD_Y + 1);	// EXP bar lag Y

	// Current ammo
	WriteByte((void*)0x419DE6 + 1, HUD_Y + 1);	// Y
	WriteByte((void*)0x419DEE + 2, HUD_X + 88);	// X
	// Max ammo
	WriteByte((void*)0x419E0B + 1, HUD_Y + 9);	// Y
	WriteByte((void*)0x419E13 + 2, HUD_X + 88);	// X
	// Current ammo (blank)
	WriteByte((void*)0x419E27 + 1, HUD_Y + 1);	// Y
	WriteByte((void*)0x419E2F + 2, HUD_X + 104);	// X
	// Max ammo (blank)
	WriteByte((void*)0x419E48 + 1, HUD_Y + 9);	// Y
	WriteByte((void*)0x419E50 + 2, HUD_X + 104);	// X
	// Slash
	WriteByte((void*)0x419E90 + 1, HUD_Y + 9);	// Y
	WriteByte((void*)0x419E98 + 2, HUD_X + 88);	// X

	// DrawHUD_Health
	// Health bar back
	WriteByte((void*)0x41A2EC + 1, HUD_Y + 9);	// Y
	WriteByte((void*)0x41A2EE + 1, HUD_X);		// X
	// Health bar lag
	WriteByte((void*)0x41A303 + 1, HUD_Y + 9);	// Y
	WriteByte((void*)0x41A305 + 1, HUD_X + 24);	// X
	// Health bar
	WriteByte((void*)0x41A31A + 1, HUD_Y + 9);	// Y
	WriteByte((void*)0x41A31C + 1, HUD_X + 24);	// X
	// Health number
	WriteByte((void*)0x41A334 + 1, HUD_Y + 9);	// Y
	WriteByte((void*)0x41A336 + 1, HUD_X - 8);	// X

	// WeaponIcons
	WriteByte((void*)0x41A1B0 + 1, HUD_Y);		// Y
	WriteRelativeAddress((void*)0x41A1BB + 1, DrawHUD_WeaponIcons_hijack);	// X

	// DrawHUD_HellTimer
	// Patch the call in the main gameplay loop
	WriteByte((void*)0x41081F + 1, HUD_Y + 20);	// Y
	WriteByte((void*)0x410821 + 1, HUD_X);		// X
	// Patch the call in the 'island falling' loop
	WriteByte((void*)0x40DD35 + 1, HUD_Y + 20);	// Y
	WriteByte((void*)0x40DD37 + 1, HUD_X);		// X
	// I would patch the call in the title screen loop, but there's really no point
}
