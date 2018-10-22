// WASD keyboard input mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <windows.h>

#include "cave_story.h"
#include "controls.h"
#include "mod_loader.h"

int key_is_being_released;

static void DoKey(unsigned int key_mask)
{
	if (key_is_being_released)
		CS_input_bitfield_held &= ~key_mask;
	else
		CS_input_bitfield_held |= key_mask;
}

__stdcall void HandleKeyEvent(const int key_code)
{
	switch(key_code)
	{
		case VK_ESCAPE:
		{
			DoKey(INPUT_QUIT);
			break;
		}
		case VK_OEM_6:
		{
			DoKey(INPUT_MAP);
			break;
		}
		case 'A':
		{
			DoKey(INPUT_LEFT | INPUT_ALT_LEFT);
			break;
		}
		case 'D':
		{
			DoKey(INPUT_RIGHT | INPUT_ALT_RIGHT);
			break;
		}
		case 'W':
		{
			DoKey(INPUT_UP | INPUT_ALT_UP | INPUT_ALT_UP2);
			break;
		}
		case 'S':
		{
			DoKey(INPUT_DOWN | INPUT_ALT_DOWN);
			break;
		}
		case 'O':
		{
			DoKey(INPUT_SHOOT);
			break;
		}
		case 'P':
		{
			DoKey(INPUT_JUMP);
			break;
		}
		case '0':
		{
			DoKey(INPUT_NEXTWEAPON);
			break;
		}
		case '9':
		{
			DoKey(INPUT_PREVIOUSWEAPON);
			break;
		}
		case VK_F1:
		{
			DoKey(INPUT_F1);
			break;
		}
		case VK_F2:
		{
			DoKey(INPUT_F2);
			break;
		}
		case VK_OEM_4:
		{
			DoKey(INPUT_INVENTORY);
			break;
		}
		case VK_F5:
		{
			if (!key_is_being_released)
				CS_gamepad_enabled = 0;
			break;
		}
		default:
		{
			break;
		}
	}
}

__asm(
"_HandleKeyPress_caller:\n"
"	movl	$0, _key_is_being_released\n"
"	pushl	0x10(%ebp)\n"
"	call	_HandleKeyEvent@4\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x41300E\n"
);
extern char HandleKeyPress_caller;

__asm(
"_HandleKeyRelease_caller:\n"
"	movl	$-1, _key_is_being_released\n"
"	pushl	0x10(%ebp)\n"
"	call	_HandleKeyEvent@4\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x4131B6\n"
);
extern char HandleKeyRelease_caller;

void InitMod(void)
{
	// Fix door-opening bug, so I can map both down keys at once
	ModLoader_FixDoorEnterBug();
	// WASD controls
	ModLoader_WriteRelativeAddress((void*)0x412CEE + 2, &HandleKeyPress_caller);
	ModLoader_WriteRelativeAddress((void*)0x412CBC + 2, &HandleKeyRelease_caller);
}
