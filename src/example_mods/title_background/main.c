// Title Screen background mod for Freeware Cave Story
// Copyright © 2018 Clownacy

#include "cave_story.h"
#include "mod_loader.h"

static const char *setting_filename;
static int setting_type;

void LoadTitleBackground(void)
{
	const int types[] = {0, 5, 6, 7};
	CS_LoadBackgroundSprite(setting_filename, types[setting_type]);
}

__asm (
"_LoadTitleBackground_asm:\n"
"	movl	$0,-0x118(%ebp)\n"	// Just the instruction we overwrote
"	call	_LoadTitleBackground\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x40FF04\n"
);
extern char LoadTitleBackground_asm;

static void DrawColourFill_Hijack(RECT *dst_rect, int colour)
{
	CS_DrawColourFill(dst_rect, colour);

	CS_IncrementBGScroll();
	CS_DrawBackground(0, 0);
}

void InitMod(void)
{
	setting_filename = ModLoader_GetSettingString("filename", "bkMoon");
	setting_type = ModLoader_GetSettingInt("type", 2);

	ModLoader_WriteJump((void*)0x40FEFA, &LoadTitleBackground_asm);	
	ModLoader_WriteRelativeAddress((void*)0x41000C+ 1, DrawColourFill_Hijack);
}
