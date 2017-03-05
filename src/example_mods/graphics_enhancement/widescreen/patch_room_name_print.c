// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "mod_loader.h"

#include "drawsprite1_centred.h"
#include "patch_room_name_print.h"

void PatchRoomNamePrint(void)
{
	WriteRelativeAddress(0x4142B2 + 1, DrawSprite1_hijack);
	WriteRelativeAddress(0x4142D5 + 1, DrawSprite1_hijack);
}
