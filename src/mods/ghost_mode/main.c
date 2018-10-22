// Ghost Mode mod for Freeware Cave Story
// Copyright © 2018 Clownacy

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "cave_story.h"
#include "mod_loader.h"

#include "in.h"
#include "out.h"

static GhostIn *ghosts[8];

void DoGhostStuff(int camera_x, int camera_y)
{
	GhostOut_Write();

	for (unsigned int i = 0; i < sizeof(ghosts) / sizeof(ghosts[0]); ++i)
		if (ghosts[i])
			GhostIn_Update(ghosts[i], camera_x, camera_y);
}

__asm(
"_DrawPlayerAndWeapon_hijack:\n"
"	call	0x415250\n"	// CS_DrawPlayerAndWeapon
"	call	_DoGhostStuff\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x410688\n"
);
extern char DrawPlayerAndWeapon_hijack;

void InitGhostData(void)
{
	GhostOut_Reset();

	for (unsigned int i = 0; i < sizeof(ghosts) / sizeof(ghosts[0]); ++i)
		if (ghosts[i])
			GhostIn_Reset(ghosts[i]);
}

__asm(
"_StartNewGame_hijack:\n"
"	call	_InitGhostData\n"
"	mov	$1,%eax\n"
"	ret\n"
);
extern char StartNewGame_hijack;

void LoadGhostData(void)
{
	GhostOut_Load();

	for (unsigned int i = 0; i < sizeof(ghosts) / sizeof(ghosts[0]); ++i)
		if (ghosts[i])
			GhostIn_Resume(ghosts[i]);
}

__asm(
"_LoadProfile_hijack:\n"
"	call	_LoadGhostData\n"
"	mov	$1,%eax\n"
"	ret\n"
);
extern char LoadProfile_hijack;

void SaveGhostData(void)
{
	GhostOut_Save();

	for (unsigned int i = 0; i < sizeof(ghosts) / sizeof(ghosts[0]); ++i)
		if (ghosts[i])
			GhostIn_Save(ghosts[i]);
}

__asm(
"_SaveProfile_hijack:\n"
"	call	_SaveGhostData\n"
"	mov	$1,%eax\n"
"	ret\n"
);
extern char SaveProfile_hijack;

static BOOL InitCredits_hijack(void)
{
	SaveGhostData();
	return CS_InitCredits();
}

void InitMod(void)
{
	ModLoader_WriteCall((void*)0x41D5FC, &StartNewGame_hijack);
	ModLoader_WriteCall((void*)0x41D53A, &LoadProfile_hijack);
	ModLoader_WriteCall((void*)0x41D241, &SaveProfile_hijack);
	ModLoader_WriteRelativeAddress((void*)0x425069 + 1, InitCredits_hijack);

	ModLoader_WriteJump((void*)0x410683, &DrawPlayerAndWeapon_hijack);

	ghosts[0] = GhostIn_Open("ghostgud.dat");
	ghosts[1] = GhostIn_Open("ghostgud2.dat");
	ghosts[2] = GhostIn_Open("ghostgud3.dat");
	ghosts[3] = GhostIn_Open("ghostgud4.dat");
	ghosts[4] = GhostIn_Open("ghostgud5.dat");
	ghosts[5] = GhostIn_Open("ghostgud6.dat");
	ghosts[6] = GhostIn_Open("ghostgud7.dat");
	ghosts[7] = GhostIn_Open("ghostgud8.dat");
}
