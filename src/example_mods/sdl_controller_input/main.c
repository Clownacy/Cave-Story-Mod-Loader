// SDL2 controller input mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "SDL2/SDL.h"

#include "cave_story.h"
#include "controls.h"
#include "mod_loader.h"

enum
{
	ID_MAP,
	ID_LEFT,
	ID_RIGHT,
	ID_UP,
	ID_DOWN,
	ID_SHOOT,
	ID_JUMP,
	ID_NEXTWEAPON,
	ID_PREVIOUSWEAPON,
	ID_INVENTORY,
	ID_MAX
};

const int input_masks[] = {
	INPUT_MAP,
	INPUT_LEFT | INPUT_ALT_LEFT,
	INPUT_RIGHT | INPUT_ALT_RIGHT,
	INPUT_UP | INPUT_ALT_UP | INPUT_ALT_UP2,
	INPUT_DOWN | INPUT_ALT_DOWN,
	INPUT_SHOOT,
	INPUT_JUMP,
	INPUT_NEXTWEAPON,
	INPUT_PREVIOUSWEAPON,
	INPUT_INVENTORY
};

SDL_Event event;
SDL_GameController *controller;
int controller_id;

static int input_totals[ID_MAX];

void SetInput(const int input_id)
{
	++input_totals[input_id];

	*input_bitfield |= input_masks[input_id];
}

void ClearInput(const int input_id)
{
	if (--input_totals[input_id] == 0)
		*input_bitfield &= ~input_masks[input_id];
}

void DoButton(const int input_id)
{
	if (event.cbutton.type == SDL_CONTROLLERBUTTONDOWN)
		SetInput(input_id);
	else
		ClearInput(input_id);
}

void DoTrigger(const int input_id, const int trigger_id)
{
	static int trigger_input[2];

	if (event.caxis.value >= (0x7FFF / 8))
	{
		if (!(trigger_input[trigger_id] & input_masks[input_id]))
		{
			trigger_input[trigger_id] |= input_masks[input_id];
			SetInput(input_id);
		}
	}
	else
	{
		if (trigger_input[trigger_id] & input_masks[input_id])
		{
			trigger_input[trigger_id] &= ~input_masks[input_id];
			ClearInput(input_id);
		}
	}
}

inline void DoLeftTrigger(const int input_id)
{
	DoTrigger(input_id, 0);
}

inline void DoRightTrigger(const int input_id)
{
	DoTrigger(input_id, 1);
}

void DoStick(const int input_id1, const int input_id2, const int stick_id)
{
	static int stick_input[1];

	if (event.caxis.value <= -(0x7FFF / 4))
	{
		if (!(stick_input[stick_id] & input_masks[input_id1]))
		{
			stick_input[stick_id] |= input_masks[input_id1];
			SetInput(input_id1);
		}
	}
	else
	{
		if (stick_input[stick_id] & input_masks[input_id1])
		{
			stick_input[stick_id] &= ~input_masks[input_id1];
			ClearInput(input_id1);
		}
	}

	if (event.caxis.value >= (0x7FFF / 4))
	{
		if (!(stick_input[stick_id] & input_masks[input_id2]))
		{
			stick_input[stick_id] |= input_masks[input_id2];
			SetInput(input_id2);
		}
	}
	else
	{
		if (stick_input[stick_id] & input_masks[input_id2])
		{
			stick_input[stick_id] &= ~input_masks[input_id2];
			ClearInput(input_id2);
		}
	}
}

inline void DoLeftStick(const int input_id1, const int input_id2)
{
	DoStick(input_id1, input_id2, 0);
}

__cdecl void ProcessControllerEvents(void)
{
	while (SDL_PollEvent(&event) != 0)
	{
		switch(event.type)
		{
			case SDL_CONTROLLERDEVICEADDED:
			{
				if (controller == NULL && SDL_IsGameController(event.cdevice.which))
				{
					controller = SDL_GameControllerOpen(event.cdevice.which);
					controller_id = event.cdevice.which;
				}
				break;
			}
			case SDL_CONTROLLERDEVICEREMOVED:
			{
				if (controller != NULL && controller_id == event.cdevice.which)
				{
					SDL_GameControllerClose(controller);
					controller = NULL;
				}
				break;
			}
			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_CONTROLLERBUTTONUP:
			{
				switch (event.cbutton.button)
				{
					case SDL_CONTROLLER_BUTTON_A:
					{
						DoButton(ID_JUMP);
						break;
					}
					case SDL_CONTROLLER_BUTTON_B:
					{
						DoButton(ID_SHOOT);
						break;
					}
					case SDL_CONTROLLER_BUTTON_X:
					{
						DoButton(ID_SHOOT);
						break;
					}
					case SDL_CONTROLLER_BUTTON_Y:
					{
						DoButton(ID_JUMP);
						break;
					}
					case SDL_CONTROLLER_BUTTON_BACK:
					{
						DoButton(ID_MAP);
						break;
					}
					case SDL_CONTROLLER_BUTTON_START:
					{
						DoButton(ID_INVENTORY);
						break;
					}
					case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
					{
						DoButton(ID_PREVIOUSWEAPON);
						break;
					}
					case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
					{
						DoButton(ID_NEXTWEAPON);
						break;
					}
					case SDL_CONTROLLER_BUTTON_DPAD_UP:
					{
						DoButton(ID_UP);
						break;
					}
					case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
					{
						DoButton(ID_DOWN);
						break;
					}
					case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
					{
						DoButton(ID_LEFT);
						break;
					}
					case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
					{
						DoButton(ID_RIGHT);
						break;
					}
				}
				break;
			}
			case SDL_CONTROLLERAXISMOTION:
			{
				switch (event.caxis.axis)
				{
					case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
					{
						DoLeftTrigger(ID_PREVIOUSWEAPON);
						break;
					}
					case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
					{
						DoRightTrigger(ID_NEXTWEAPON);
						break;
					}
					case SDL_CONTROLLER_AXIS_LEFTX:
					{
						DoLeftStick(ID_LEFT, ID_RIGHT);
						break;
					}
					case SDL_CONTROLLER_AXIS_LEFTY:
					{
						DoLeftStick(ID_UP, ID_DOWN);
						break;
					}
				}
				break;
			}
		}
	}
}

void InitMod(void)
{
	// This also initialises the SDL Event system
	// but it doesn't grab window events
	SDL_Init(SDL_INIT_GAMECONTROLLER);

	// Grab all controllers that were plugged-in before the game was started
	for (int i=0; i < SDL_NumJoysticks(); ++i)
	{
		if (SDL_IsGameController(i))
		{
			controller = SDL_GameControllerOpen(i);
			controller_id = i;
			break;
		}
	}

	// Fix door-opening bug, so I can map both down keys at once
	FixDoorEnterBug();
	// NOP-out call to DirectInput init function
	WriteWord(0x420EF6, 0x03EB);
	// Redirect controller update function call
	WriteRelativeAddress(0x4135CC + 1, ProcessControllerEvents);
}
