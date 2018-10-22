// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch_room_name_print.h"

#include "mod_loader.h"

#include "drawsprite1_centred.h"

void PatchRoomNamePrint(void)
{
	ModLoader_WriteRelativeAddress((void*)0x4142B2 + 1, DrawSprite1_hijack);
	ModLoader_WriteRelativeAddress((void*)0x4142D5 + 1, DrawSprite1_hijack);
}
