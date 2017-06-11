// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

/*
This here fixes a minor bug in the game's input handling:

When you open a door, the game expects the down key to be the only
mappable direction key pressed. Keyword: mappable. This means, if you
were to press the down arrow key while holding, say, the '0' key, you'd
still go through the door.

The bug here is that the alternate directional keys are still considered
mapped, even though they're never used, so if you hold '>' (the alternate
down key) while pressing the actual down key, you won't go through the
door.

This is an issue because my WASD and SDL2 controller mods set both the
normal and alternate keys at the same time (that is to say, pressing
down on the controller will set the normal down key and the alternate
down key, so the controller works no matter what setting you've set in
DoConfig). The result is the player being completely unable to go through
doors, when using said mods.
*/

#include <stdbool.h>

#include "cave_story.h"
#include "controls.h"
#include "patch.h"

static bool patch_already_applied = false;

static const int NORMAL_DIRECTION_KEY_MASK = ~(INPUT_ALT_LEFT | INPUT_ALT_RIGHT | INPUT_ALT_DOWN | INPUT_ALT_UP | INPUT_ALT_UP2);
static const int ALTERNATE_DIRECTION_KEY_MASK = ~(INPUT_LEFT | INPUT_RIGHT | INPUT_DOWN | INPUT_UP);

static bool TestDownKey(const int input_bitfield)
{
	const bool using_normal_controls = *down_key_mapping == INPUT_DOWN;	// If we're using alternate controls, the down key mask is set to 0x20000

	const int current_direction_keys_mask = using_normal_controls ? NORMAL_DIRECTION_KEY_MASK : ALTERNATE_DIRECTION_KEY_MASK;
	const int current_down_key_mask = using_normal_controls ? INPUT_DOWN : INPUT_ALT_DOWN;

	return (input_bitfield & current_direction_keys_mask) == current_down_key_mask;
}

bool TestOnlyDownKeyIsPressedAndHeld(void)
{
	return TestDownKey(*input_bitfield_held) && TestDownKey(*input_bitfield_newly_pressed);
}

bool TestOnlyDownKeyIsHeld(void)
{
	return TestDownKey(*input_bitfield_held);
}

__asm(
"_TestOnlyDownKeyIsPressedAndHeld_asm:\n"
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
