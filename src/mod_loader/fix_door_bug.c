// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <stdbool.h>

#include "cave_story.h"
#include "controls.h"
#include "patch.h"

bool done = false;

const int NORM_MASK = ~(INPUT_LEFT | INPUT_RIGHT | INPUT_DOWN | INPUT_UP);
const int ALT_MASK = ~(INPUT_ALT_LEFT | INPUT_ALT_RIGHT | INPUT_ALT_DOWN | INPUT_ALT_UP | INPUT_ALT_UP2);

const int TestOnlyDownPressed(void)
{
	const int current_mask = (*down_key_input_mask == 8) ? ALT_MASK : NORM_MASK;
	const int current_down_input = (*down_key_input_mask == 8) ? INPUT_DOWN : INPUT_ALT_DOWN;

	return ((*input_bitfield_newly_pressed & current_mask) == current_down_input);
}

const int TestOnlyDownHeld(void)
{
	const int current_mask = (*down_key_input_mask == 8) ? ALT_MASK : NORM_MASK;
	const int current_down_input = (*down_key_input_mask == 8) ? INPUT_DOWN : INPUT_ALT_DOWN;

	return ((*input_bitfield_held & current_mask) == current_down_input);
}

__asm(
"	_TestOnlyDownPressedAndHeld_asm:\n"
"	call	_TestOnlyDownPressed\n"
"	or	%eax, %eax\n"
"	jz	0x415826\n"
"	call	_TestOnlyDownHeld\n"
"	or	%eax, %eax\n"
"	jz	0x415826\n"
"	jmp	0x4157F3\n"
);
extern char TestOnlyDownPressedAndHeld_asm;

__asm(
"_TestOnlyDownHeld_asm:\n"
"	call	_TestOnlyDownHeld\n"
"	or	%eax, %eax\n"
"	jz	0x415835\n"
"	jmp	0x4158B3\n"
);
extern char TestOnlyDownHeld_asm;

__declspec(dllexport) void FixDoorEnterBug(void)
{
	if (!done)
	{
		WriteJump(0x4157D7, &TestOnlyDownPressedAndHeld_asm);
		WriteJump(0x415826, &TestOnlyDownHeld_asm);
		done = true;
	}
}
