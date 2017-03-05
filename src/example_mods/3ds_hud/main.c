// 3DS HUD mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "mod_loader.h"

__asm (
"_DrawHUD_WeaponIcons_hijack:\n"
"	movl	8(%esp), %eax\n"
"	addl	$64, %eax\n"
"	movl	%eax, 8(%esp)\n"
"	jmp	0x40C3C0\n"
);
void DrawHUD_WeaponIcons_hijack(void);

void InitMod(void)
{
	// EXPAndText
	WriteLong(0x401003 + 6, 8 + 16);
	WriteLong(0x401F04 + 6, 8 + 16);
	WriteLong(0x402166 + 6, 8 + 16);
	WriteLong(0x40224D + 6, 8 + 16);
	WriteByte(0x419D95 + 6, 8);
	WriteByte(0x419DAB + 6, 8);

	WriteByte(0x419EB1 + 1, 5);
	WriteByte(0x419EDA + 1, 5);
	WriteByte(0x419FAF + 1, 5);
	WriteByte(0x41A038 + 1, 5);
	WriteByte(0x419FF6 + 1, 5);
	WriteByte(0x41A08D + 1, 5);

	WriteByte(0x419DEE + 2, 96);
	WriteByte(0x419DE6 + 1, 5);
	WriteByte(0x419E13 + 2, 96);
	WriteByte(0x419E0B + 1, 5 + 8);
	WriteByte(0x419E2F + 2, 96 + 16);
	WriteByte(0x419E27 + 1, 5);
	WriteByte(0x419E50 + 2, 96 + 16);
	WriteByte(0x419E48 + 1, 5 + 8);

	WriteByte(0x419E90 + 1, 5 + 8);
	WriteByte(0x419E98 + 2, 96);

	// Health
	WriteByte(0x41A2EE + 1, 8);
	WriteByte(0x41A305 + 1, 32);
	WriteByte(0x41A31C + 1, 32);
	WriteByte(0x41A2EC + 1, 13);
	WriteByte(0x41A303 + 1, 13);
	WriteByte(0x41A31A + 1, 13);
	WriteByte(0x41A336 + 1, 0);
	WriteByte(0x41A334 + 1, 13);

	// WeaponIcons
	WriteByte(0x41A1B0 + 1, 4);
	WriteRelativeAddress(0x41A1BB + 1, DrawHUD_WeaponIcons_hijack);
}
