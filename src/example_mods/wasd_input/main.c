#include "controls.h"
#include "mod_loader_main.h"

int* const input_bitfield = (int* const)0x49E210;
int* const gamepad_enabled = (int* const)0x49E45C;

void __stdcall HandleKeyPress(const int key_code)
{
	switch(key_code)
	{
		case VK_ESCAPE:
		{
			*input_bitfield |= INPUT_QUIT;
			break;
		}
		case VK_OEM_6:
		{
			*input_bitfield |= INPUT_MAP;
			break;
		}
		case 'A':
		{
			*input_bitfield |= (INPUT_LEFT | INPUT_ALT_LEFT);
			break;
		}
		case 'D':
		{
			*input_bitfield |= (INPUT_RIGHT | INPUT_ALT_RIGHT);
			break;
		}
		case 'W':
		{
			*input_bitfield |= (INPUT_UP | INPUT_ALT_UP | INPUT_ALT_UP2);
			break;
		}
		case 'S':
		{
			*input_bitfield |= (INPUT_DOWN | INPUT_ALT_DOWN);
			break;
		}
		case 'O':
		{
			*input_bitfield |= INPUT_SHOOT;
			break;
		}
		case 'P':
		{
			*input_bitfield |= INPUT_JUMP;
			break;
		}
		case '0':
		{
			*input_bitfield |= INPUT_NEXTWEAPON;
			break;
		}
		case '9':
		{
			*input_bitfield |= INPUT_PREVIOUSWEAPON;
			break;
		}
		case VK_F1:
		{
			*input_bitfield |= INPUT_F1;
			break;
		}
		case VK_F2:
		{
			*input_bitfield |= INPUT_F2;
			break;
		}
		case VK_OEM_4:
		{
			*input_bitfield |= INPUT_INVENTORY;
			break;
		}
		case VK_F5:
		{
			*gamepad_enabled = 0;
			break;
		}
		default:
		{
			break;
		}
	}
}

void __stdcall HandleKeyRelease(const int key_code)
{
	switch(key_code)
	{
		case VK_ESCAPE:
		{
			*input_bitfield &= ~INPUT_QUIT;
			break;
		}
		case VK_OEM_6:
		{
			*input_bitfield &= ~INPUT_MAP;
			break;
		}
		case 'A':
		{
			*input_bitfield &= ~(INPUT_LEFT | INPUT_ALT_LEFT);
			break;
		}
		case 'D':
		{
			*input_bitfield &= ~(INPUT_RIGHT | INPUT_ALT_RIGHT);
			break;
		}
		case 'W':
		{
			*input_bitfield &= ~(INPUT_UP | INPUT_ALT_UP | INPUT_ALT_UP2);
			break;
		}
		case 'S':
		{
			*input_bitfield &= ~(INPUT_DOWN | INPUT_ALT_DOWN);
			break;
		}
		case 'O':
		{
			*input_bitfield &= ~INPUT_SHOOT;
			break;
		}
		case 'P':
		{
			*input_bitfield &= ~INPUT_JUMP;
			break;
		}
		case '0':
		{
			*input_bitfield &= ~INPUT_NEXTWEAPON;
			break;
		}
		case '9':
		{
			*input_bitfield &= ~INPUT_PREVIOUSWEAPON;
			break;
		}
		case VK_F1:
		{
			*input_bitfield &= ~INPUT_F1;
			break;
		}
		case VK_F2:
		{
			*input_bitfield &= ~INPUT_F2;
			break;
		}
		case VK_OEM_4:
		{
			*input_bitfield &= ~INPUT_INVENTORY;
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
extern void HandleKeyPress_caller(void);

__asm(
"_HandleKeyRelease_caller:\n"
"	pushl	0x10(%ebp)\n"
"	call	_HandleKeyRelease@4\n"
"	jmp	0x4131B6\n"
);
extern void HandleKeyRelease_caller(void);

void InitMod(void)
{
	// Fix door-opening bug, so I can map both down keys at once
	FixDoorEnterBug();
	// WASD controls
	WriteRelativeAddress(0x412CEE + 2, HandleKeyPress_caller);
	WriteRelativeAddress(0x412CBC + 2, HandleKeyRelease_caller);
}
