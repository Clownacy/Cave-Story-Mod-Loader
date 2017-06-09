// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <stdbool.h>

#include "cave_story.h"
#include "controls.h"
#include "patch.h"

static bool patch_already_applied = false;

static const int NORMAL_DIRECTION_KEY_MASK = ~(INPUT_LEFT | INPUT_RIGHT | INPUT_DOWN | INPUT_UP);
static const int ALTERNATE_DIRECTION_KEY_MASK = ~(INPUT_ALT_LEFT | INPUT_ALT_RIGHT | INPUT_ALT_DOWN | INPUT_ALT_UP | INPUT_ALT_UP2);

static bool TestDownKey(const int input_bitfield)
{
	const bool using_normal_controls = *down_key_mapping == INPUT_DOWN;	// If we're using alternate controls, the down key mask is set to 0x20000

	const int current_direction_keys_mask = using_normal_controls ? NORMAL_DIRECTION_KEY_MASK : ALTERNATE_DIRECTION_KEY_MASK;
	const int current_down_key_mask = using_normal_controls ? INPUT_DOWN : INPUT_ALT_DOWN;

	return ((input_bitfield & current_direction_keys_mask) == current_down_key_mask);
}

bool TestOnlyDownKeyIsPressedAndHeld(const int input_bitfield)
{
	return TestDownKey(*input_bitfield_held) && TestDownKey(*input_bitfield_newly_pressed);
}

bool TestOnlyDownKeyIsHeld(void)
{
	return TestDownKey(*input_bitfield_held);
}

__asm(
"	_TestOnlyDownKeyIsPressedAndHeld_asm:\n"
"	call	_TestOnlyDownKeyIsPressedAndHeld\n"
"	or	%eax, %eax\n"
"	jz	0x415826\n"
"	jmp	0x4157F3\n"
);
extern char TestOnlyDownKeyIsPressedAndHeld_asm;

__asm(
"_TestOnlyDownKeyIsHeld_asm:\n"
"	call	_TestOnlyDownKeyIsHeld\n"
"	or	%eax, %eax\n"
"	jz	0x415835\n"
"	jmp	0x4158B3\n"
);
extern char TestOnlyDownKeyIsHeld_asm;

__declspec(dllexport) void FixDoorEnterBug(void)
{
	if (!patch_already_applied)
	{
		WriteJump(0x4157D7, &TestOnlyDownKeyIsPressedAndHeld_asm);
		WriteJump(0x415826, &TestOnlyDownKeyIsHeld_asm);
		patch_already_applied = true;
	}
}
