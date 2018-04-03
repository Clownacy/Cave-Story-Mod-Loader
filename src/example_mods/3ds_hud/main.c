// 3DS HUD mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "cave_story.h"
#include "mod_loader.h"

#define HUD_X 8
#define HUD_Y 4

static void DrawHUD_WeaponIcons_hijack(void *clip_rect, int x_pos, int y_pos, void *src_rect, int surface_id)
{
	CS_DrawSprite_WithTransparency(clip_rect, x_pos + HUD_X + 56, y_pos, src_rect, surface_id);
}

void InitMod(void)
{
	// DrawHUD_EXPAndText
	// hud_scroll_x_pos
	ModLoader_WriteLong((void*)0x401003 + 6, HUD_X + 16);
	ModLoader_WriteLong((void*)0x401F04 + 6, HUD_X + 16);
	ModLoader_WriteLong((void*)0x402166 + 6, HUD_X + 16);
	ModLoader_WriteLong((void*)0x40220E + 6, HUD_X - 16);
	ModLoader_WriteLong((void*)0x40224D + 6, HUD_X + 16);
	ModLoader_WriteByte((void*)0x419D95 + 6, HUD_X);
	ModLoader_WriteByte((void*)0x419DAB + 6, HUD_X);

	ModLoader_WriteByte((void*)0x419EB1 + 1, HUD_Y + 1);	// Lv Y
	ModLoader_WriteByte((void*)0x419EDA + 1, HUD_Y + 1);	// Weapon level Y
	ModLoader_WriteByte((void*)0x419FAF + 1, HUD_Y + 1);	// EXP bar back Y
	ModLoader_WriteByte((void*)0x419FF6 + 1, HUD_Y + 1);	// EXP bar max Y
	ModLoader_WriteByte((void*)0x41A038 + 1, HUD_Y + 1);	// EXP bar Y
	ModLoader_WriteByte((void*)0x41A08D + 1, HUD_Y + 1);	// EXP bar lag Y

	// Current ammo
	ModLoader_WriteByte((void*)0x419DE6 + 1, HUD_Y + 1);	// Y
	ModLoader_WriteByte((void*)0x419DEE + 2, HUD_X + 88);	// X
	// Max ammo
	ModLoader_WriteByte((void*)0x419E0B + 1, HUD_Y + 9);	// Y
	ModLoader_WriteByte((void*)0x419E13 + 2, HUD_X + 88);	// X
	// Current ammo (blank)
	ModLoader_WriteByte((void*)0x419E27 + 1, HUD_Y + 1);	// Y
	ModLoader_WriteByte((void*)0x419E2F + 2, HUD_X + 104);	// X
	// Max ammo (blank)
	ModLoader_WriteByte((void*)0x419E48 + 1, HUD_Y + 9);	// Y
	ModLoader_WriteByte((void*)0x419E50 + 2, HUD_X + 104);	// X
	// Slash
	ModLoader_WriteByte((void*)0x419E90 + 1, HUD_Y + 9);	// Y
	ModLoader_WriteByte((void*)0x419E98 + 2, HUD_X + 88);	// X

	// DrawHUD_Health
	// Health bar back
	ModLoader_WriteByte((void*)0x41A2EC + 1, HUD_Y + 9);	// Y
	ModLoader_WriteByte((void*)0x41A2EE + 1, HUD_X);		// X
	// Health bar lag
	ModLoader_WriteByte((void*)0x41A303 + 1, HUD_Y + 9);	// Y
	ModLoader_WriteByte((void*)0x41A305 + 1, HUD_X + 24);	// X
	// Health bar
	ModLoader_WriteByte((void*)0x41A31A + 1, HUD_Y + 9);	// Y
	ModLoader_WriteByte((void*)0x41A31C + 1, HUD_X + 24);	// X
	// Health number
	ModLoader_WriteByte((void*)0x41A334 + 1, HUD_Y + 9);	// Y
	ModLoader_WriteByte((void*)0x41A336 + 1, HUD_X - 8);	// X

	// WeaponIcons
	ModLoader_WriteByte((void*)0x41A1B0 + 1, HUD_Y);		// Y
	ModLoader_WriteRelativeAddress((void*)0x41A1BB + 1, DrawHUD_WeaponIcons_hijack);	// X

	// DrawHUD_HellTimer
	// Patch the call in the main gameplay loop
	ModLoader_WriteByte((void*)0x41081F + 1, HUD_Y + 20);	// Y
	ModLoader_WriteByte((void*)0x410821 + 1, HUD_X);		// X
	// Patch the call in the 'island falling' loop
	ModLoader_WriteByte((void*)0x40DD35 + 1, HUD_Y + 20);	// Y
	ModLoader_WriteByte((void*)0x40DD37 + 1, HUD_X);		// X
	// I would patch the call in the title screen loop, but there's really no point
}
