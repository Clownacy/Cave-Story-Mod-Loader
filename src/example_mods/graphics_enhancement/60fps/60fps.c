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

	WriteByte((void*)0x40B36F, delay_ticks[counter % 3]);
	WriteByte((void*)0x40B3A9, delay_ticks[counter % 3]);

	++counter;
}

__asm(
"_UpdateTicks_ASM:\n"
"	call	_UpdateTicks\n"
"	jmp	*0x48C1B8\n"
);
extern char UpdateTicks_ASM;

void Apply60FPSPatch(void)
{
	WriteByte((void*)0x40B3B9, 0x90);
	WriteCall((void*)0x40B3B9 + 1, &UpdateTicks_ASM);
}
