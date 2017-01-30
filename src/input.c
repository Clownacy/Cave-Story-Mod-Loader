#include <stdint.h>
#include <windows.h>

#include "input.h"
#include "main.h"

#define INPUT_QUIT		0x00008000
#define INPUT_MAP		0x00000010
#define INPUT_LEFT		0x00000001
#define INPUT_RIGHT		0x00000002
#define INPUT_UP		0x00000004
#define INPUT_DOWN		0x00000008
#define INPUT_SHOOT		0x00000020
#define INPUT_JUMP		0x00000040
#define INPUT_NEXTWEAPON	0x00000080
#define INPUT_PREVIOUSWEAPON	0x00000100
#define INPUT_UNKNOWN		0x00000200
#define INPUT_F1		0x00000400
#define INPUT_F2		0x00000800
#define INPUT_INVENTORY		0x00001000
#define INPUT_ALT_LEFT		0x00010000
#define INPUT_ALT_DOWN		0x00020000
#define INPUT_ALT_RIGHT		0x00040000
#define INPUT_ALT_UP		0x00080000
#define INPUT_ALT_UP2		0x00100000

uint32_t* const InputBitfield = (uint32_t* const)0x49E210;
uint32_t* const GamePadEnabled = (uint32_t* const)0x49E45C;

void __stdcall HandleKeyPress(const uint32_t key_code)
{
	switch(key_code)
	{
		case VK_ESCAPE:
		{
			*InputBitfield |= INPUT_QUIT;
			break;
		}
		case VK_OEM_6:
		{
			*InputBitfield |= INPUT_MAP;
			break;
		}
		case 'A':
		{
			*InputBitfield |= INPUT_LEFT | INPUT_ALT_LEFT;
			break;
		}
		case 'D':
		{
			*InputBitfield |= INPUT_RIGHT | INPUT_ALT_RIGHT;
			break;
		}
		case 'W':
		{
			*InputBitfield |= INPUT_UP | INPUT_ALT_UP | INPUT_ALT_UP2;
			break;
		}
		case 'S':
		{
			*InputBitfield |= INPUT_DOWN | INPUT_ALT_DOWN;
			break;
		}
		case 'O':
		{
			*InputBitfield |= INPUT_SHOOT;
			break;
		}
		case 'P':
		{
			*InputBitfield |= INPUT_JUMP;
			break;
		}
		case '0':
		{
			*InputBitfield |= INPUT_NEXTWEAPON;
			break;
		}
		case '9':
		{
			*InputBitfield |= INPUT_PREVIOUSWEAPON;
			break;
		}
		case VK_F1:
		{
			*InputBitfield |= INPUT_F1;
			break;
		}
		case VK_F2:
		{
			*InputBitfield |= INPUT_F2;
			break;
		}
		case VK_OEM_4:
		{
			*InputBitfield |= INPUT_INVENTORY;
			break;
		}
		case VK_F5:
		{
			*GamePadEnabled = 0x0;
			break;
		}
		default:
		{
			break;
		}
	}
}

void __stdcall HandleKeyRelease(const uint32_t key_code)
{
	switch(key_code)
	{
		case VK_ESCAPE:
		{
			*InputBitfield &= ~INPUT_QUIT;
			break;
		}
		case VK_OEM_6:
		{
			*InputBitfield &= ~INPUT_MAP;
			break;
		}
		case 'A':
		{
			*InputBitfield &= ~(INPUT_LEFT | INPUT_ALT_LEFT);
			break;
		}
		case 'D':
		{
			*InputBitfield &= ~(INPUT_RIGHT | INPUT_ALT_RIGHT);
			break;
		}
		case 'W':
		{
			*InputBitfield &= ~(INPUT_UP | INPUT_ALT_UP | INPUT_ALT_UP2);
			break;
		}
		case 'S':
		{
			*InputBitfield &= ~(INPUT_DOWN | INPUT_ALT_DOWN);
			break;
		}
		case 'O':
		{
			*InputBitfield &= ~INPUT_SHOOT;
			break;
		}
		case 'P':
		{
			*InputBitfield &= ~INPUT_JUMP;
			break;
		}
		case '0':
		{
			*InputBitfield &= ~INPUT_NEXTWEAPON;
			break;
		}
		case '9':
		{
			*InputBitfield &= ~INPUT_PREVIOUSWEAPON;
			break;
		}
		case VK_F1:
		{
			*InputBitfield &= ~INPUT_F1;
			break;
		}
		case VK_F2:
		{
			*InputBitfield &= ~INPUT_F2;
			break;
		}
		case VK_OEM_4:
		{
			*InputBitfield &= ~INPUT_INVENTORY;
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

void InitInput(void)
{
	// WASD controls
	WriteRelativeAddress(0x412CEE + 2, HandleKeyPress_caller);
	WriteRelativeAddress(0x412CBC + 2, HandleKeyRelease_caller);
}
