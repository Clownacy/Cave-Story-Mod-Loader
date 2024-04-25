// 60FPS mod for Freeware Cave Story
// Copyright © 2018 Clownacy

#include "60fps.h"

#include <stddef.h>
#include <windows.h>

#include "cave_story.h"
#include "mod_loader.h"

void UpdateTicks(void)
{
	const unsigned int delay_ticks[] = {16, 17, 17};

	static unsigned char counter;

	ModLoader_WriteByte((void*)0x40B36F, delay_ticks[counter % 3]);
	ModLoader_WriteByte((void*)0x40B3A9, delay_ticks[counter % 3]);

	++counter;
}

__asm(
"_UpdateTicks_ASM:\n"
"	call	_UpdateTicks\n"
"	pushl	0x48C1B8\n"
"	ret\n"
);
extern char UpdateTicks_ASM;

void Apply60FPSPatch(void)
{
	ModLoader_WriteByte((void*)0x40B3B9, 0x90);
	ModLoader_WriteCall((void*)0x40B3B9 + 1, &UpdateTicks_ASM);

	ModLoader_WriteLong((void*)0x41A4A5 + 6, 100 * 60 * 60);
	ModLoader_WriteLong((void*)0x41A52F + 1, 60 * 60);
	ModLoader_WriteByte((void*)0x41A54F + 1, 60);
	ModLoader_WriteByte((void*)0x41A57A + 1, 6);

	ModLoader_WriteLong((void*)0x40FE03 + 6, 6 * 60 * 60);
	ModLoader_WriteLong((void*)0x40FE22 + 6, 5 * 60 * 60);
	ModLoader_WriteLong((void*)0x40FE41 + 6, 4 * 60 * 60);
	ModLoader_WriteLong((void*)0x40FE60 + 6, 3 * 60 * 60);
}
