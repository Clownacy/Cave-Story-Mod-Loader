// WASD keyboard input mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <windows.h>

#include "cave_story.h"
#include "controls.h"
#include "mod_loader.h"

__stdcall void HandleKeyPress(const int key_code)
{
	switch(key_code)
	{
		case VK_ESCAPE:
		{
			CS_input_bitfield_held |= INPUT_QUIT;
			break;
		}
		case VK_OEM_6:
		{
			CS_input_bitfield_held |= INPUT_MAP;
			break;
		}
		case 'A':
		{
			CS_input_bitfield_held |= (INPUT_LEFT | INPUT_ALT_LEFT);
			break;
		}
		case 'D':
		{
			CS_input_bitfield_held |= (INPUT_RIGHT | INPUT_ALT_RIGHT);
			break;
		}
		case 'W':
		{
			CS_input_bitfield_held |= (INPUT_UP | INPUT_ALT_UP | INPUT_ALT_UP2);
			break;
		}
		case 'S':
		{
			CS_input_bitfield_held |= (INPUT_DOWN | INPUT_ALT_DOWN);
			break;
		}
		case 'O':
		{
			CS_input_bitfield_held |= INPUT_SHOOT;
			break;
		}
		case 'P':
		{
			CS_input_bitfield_held |= INPUT_JUMP;
			break;
		}
		case '0':
		{
			CS_input_bitfield_held |= INPUT_NEXTWEAPON;
			break;
		}
		case '9':
		{
			CS_input_bitfield_held |= INPUT_PREVIOUSWEAPON;
			break;
		}
		case VK_F1:
		{
			CS_input_bitfield_held |= INPUT_F1;
			break;
		}
		case VK_F2:
		{
			CS_input_bitfield_held |= INPUT_F2;
			break;
		}
		case VK_OEM_4:
		{
			CS_input_bitfield_held |= INPUT_INVENTORY;
			break;
		}
		case VK_F5:
		{
			CS_gamepad_enabled = 0;
			break;
		}
		default:
		{
			break;
		}
	}
}

__stdcall void HandleKeyRelease(const int key_code)
{
	switch(key_code)
	{
		case VK_ESCAPE:
		{
			CS_input_bitfield_held &= ~INPUT_QUIT;
			break;
		}
		case VK_OEM_6:
		{
			CS_input_bitfield_held &= ~INPUT_MAP;
			break;
		}
		case 'A':
		{
			CS_input_bitfield_held &= ~(INPUT_LEFT | INPUT_ALT_LEFT);
			break;
		}
		case 'D':
		{
			CS_input_bitfield_held &= ~(INPUT_RIGHT | INPUT_ALT_RIGHT);
			break;
		}
		case 'W':
		{
			CS_input_bitfield_held &= ~(INPUT_UP | INPUT_ALT_UP | INPUT_ALT_UP2);
			break;
		}
		case 'S':
		{
			CS_input_bitfield_held &= ~(INPUT_DOWN | INPUT_ALT_DOWN);
			break;
		}
		case 'O':
		{
			CS_input_bitfield_held &= ~INPUT_SHOOT;
			break;
		}
		case 'P':
		{
			CS_input_bitfield_held &= ~INPUT_JUMP;
			break;
		}
		case '0':
		{
			CS_input_bitfield_held &= ~INPUT_NEXTWEAPON;
			break;
		}
		case '9':
		{
			CS_input_bitfield_held &= ~INPUT_PREVIOUSWEAPON;
			break;
		}
		case VK_F1:
		{
			CS_input_bitfield_held &= ~INPUT_F1;
			break;
		}
		case VK_F2:
		{
			CS_input_bitfield_held &= ~INPUT_F2;
			break;
		}
		case VK_OEM_4:
		{
			CS_input_bitfield_held &= ~INPUT_INVENTORY;
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
"	pushl	0x10(%ebp)\n"
"	call	_HandleKeyPress@4\n"
"	jmp	0x41300E\n"
);
extern char HandleKeyPress_caller;

__asm(
"_HandleKeyRelease_caller:\n"
"	pushl	0x10(%ebp)\n"
"	call	_HandleKeyRelease@4\n"
"	jmp	0x4131B6\n"
);
extern char HandleKeyRelease_caller;

void InitMod(void)
{
	// Fix door-opening bug, so I can map both down keys at once
	FixDoorEnterBug();
	// WASD controls
	WriteRelativeAddress((void*)0x412CEE + 2, &HandleKeyPress_caller);
	WriteRelativeAddress((void*)0x412CBC + 2, &HandleKeyRelease_caller);
}
