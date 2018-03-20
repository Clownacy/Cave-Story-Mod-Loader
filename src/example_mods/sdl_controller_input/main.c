// SDL2 controller input mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <stddef.h>
#include <stdlib.h>

#include "SDL2/SDL.h"

#include "cave_story.h"
#include "controls.h"
#include "mod_loader.h"
#include "sprintfMalloc.h"

typedef struct ControllerMeta
{
	struct ControllerMeta *next;

	SDL_GameController *controller_id;
	unsigned int joystick_id;
} ControllerMeta;

typedef enum InputID
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
} InputID;

static const int input_masks[] = {
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

static SDL_Event event;

static unsigned int input_totals[ID_MAX];

static ControllerMeta *controller_list_head;

static void AddController(unsigned int joystick_id)
{
	if (SDL_IsGameController(joystick_id))
	{
		ControllerMeta *controller = malloc(sizeof(ControllerMeta));
		controller->next = controller_list_head;
		controller_list_head = controller;

		controller->controller_id = SDL_GameControllerOpen(joystick_id);
		controller->joystick_id = joystick_id;
	}
}

static void RemoveController(unsigned int joystick_id)
{
	for (ControllerMeta **controller_pointer = &controller_list_head; *controller_pointer != NULL; controller_pointer = &(*controller_pointer)->next)
	{
		ControllerMeta *controller = *controller_pointer;

		if (controller->joystick_id == joystick_id)
		{
			// We found the disconnected controller
			SDL_GameControllerClose(controller->controller_id);

			ControllerMeta *next = controller->next;
			free(controller);
			*controller_pointer = next;
			break;
		}
	}
}

static void SetInput(const int input_id)
{
	++input_totals[input_id];

	CS_input_bitfield_held |= input_masks[input_id];
}

static void ClearInput(const InputID input_id)
{
	if (--input_totals[input_id] == 0)
		CS_input_bitfield_held &= ~input_masks[input_id];
}

static void DoButton(const InputID input_id)
{
	if (event.cbutton.type == SDL_CONTROLLERBUTTONDOWN)
		SetInput(input_id);
	else
		ClearInput(input_id);
}

static void DoTrigger(const InputID input_id, const int trigger_id)
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

static inline void DoLeftTrigger(const InputID input_id)
{
	DoTrigger(input_id, 0);
}

static inline void DoRightTrigger(const InputID input_id)
{
	DoTrigger(input_id, 1);
}

static void DoStick(const InputID input_id1, const InputID input_id2, const int stick_id)
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

static inline void DoLeftStick(const InputID input_id1, const InputID input_id2)
{
	DoStick(input_id1, input_id2, 0);
}

static void ProcessControllerEvents(void)
{
	while (SDL_PollEvent(&event) != 0)
	{
		switch(event.type)
		{
			case SDL_CONTROLLERDEVICEADDED:
			{
				AddController(event.cdevice.which);
				break;
			}
			case SDL_CONTROLLERDEVICEREMOVED:
			{
				RemoveController(event.cdevice.which);
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
	// but it doesn't grab window events, since SDL2 didn't create the window
	SDL_Init(SDL_INIT_GAMECONTROLLER);

	// Load DInput controller mappings
	char *controller_mappings_path = sprintfMalloc("%s/gamecontrollerdb.txt", location_path);
	if (SDL_GameControllerAddMappingsFromFile(controller_mappings_path) == -1)
		PrintError("sdl_controller_input: Could not load 'gamecontrollerdb.txt'. DInput devices will not be supported\n");

	free(controller_mappings_path);

	// Grab all controllers that were plugged-in before the game was started
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
		AddController(i);

	// Fix door-opening bug, so I can map both down keys at once
	FixDoorEnterBug();
	// Skip call to DirectInput init function
	WriteJump((void*)0x412B18, (void*)0x412B3B);
	// Redirect DirectInput controller update function call to our new one
	WriteRelativeAddress((void*)0x4135CC + 1, ProcessControllerEvents);
}
