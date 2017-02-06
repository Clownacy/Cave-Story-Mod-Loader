#include "controls.h"
#include "patch.h"

const int* const input_down = (void*)0x49363C;
const int* const input_bitfield_held = (void*)0x49E210;
const int* const input_bitfield_newly_pressed = (void*)0x49E214;

const int NORM_MASK = ~(INPUT_LEFT | INPUT_RIGHT | INPUT_DOWN | INPUT_UP);
const int ALT_MASK = ~(INPUT_ALT_LEFT | INPUT_ALT_RIGHT | INPUT_ALT_DOWN | INPUT_ALT_UP | INPUT_ALT_UP2);

__stdcall const int TestOnlyDownPressed(void)
{
	const int current_mask = (*input_down == 8) ? ALT_MASK : NORM_MASK;
	const int current_down_input = (*input_down == 8) ? INPUT_DOWN : INPUT_ALT_DOWN;

	return ((*input_bitfield_newly_pressed & current_mask) == current_down_input);
}

__stdcall const int TestOnlyDownHeld(void)
{
	const int current_mask = (*input_down == 8) ? ALT_MASK : NORM_MASK;
	const int current_down_input = (*input_down == 8) ? INPUT_DOWN : INPUT_ALT_DOWN;

	return ((*input_bitfield_held & current_mask) == current_down_input);
}

__asm(
"_TestOnlyDownPressedAndHeld_asm:\n"
"call	_TestOnlyDownPressed@0\n"
"or	%eax, %eax\n"
"jz	0x415826\n"
"call	_TestOnlyDownHeld@0\n"
"or	%eax, %eax\n"
"jz	0x415826\n"
"jmp	0x4157F3\n"
);
extern void TestOnlyDownPressedAndHeld_asm(void);

__asm(
"_TestOnlyDownHeld_asm:\n"
"call	_TestOnlyDownHeld@0\n"
"or	%eax, %eax\n"
"jz	0x415835\n"
"jmp	0x4158B3\n"
);
extern void TestOnlyDownHeld_asm(void);

__declspec(dllexport) void FixDoorEnterBug(void)
{
	WriteJump(0x4157D7, (void*)TestOnlyDownPressedAndHeld_asm);
	WriteJump(0x415826, (void*)TestOnlyDownHeld_asm);
}
