#include "SDL2/SDL.h"

#include "controls.h"
#include "mod_loader_wrapper.h"

int* const InputBitfield = (int* const)0x49E210;

SDL_Event event;
SDL_GameController *controller = NULL;
int controller_id;

void DoButton(const int input_id)
{
	if (event.cbutton.type == SDL_CONTROLLERBUTTONDOWN)
		*InputBitfield |= input_id;
	else
		*InputBitfield &= ~input_id;
}

void DoTrigger(const int input_id)
{
	if (event.caxis.value >= (0x7FFF / 8))
		*InputBitfield |= input_id;
	else
		*InputBitfield &= ~input_id;
}

void DoStick(const int input_id1, const int input_id2)
{
	if (event.caxis.value <= -(0x7FFF / 4))
		*InputBitfield |= input_id1;
	else
		*InputBitfield &= ~input_id1;

	if (event.caxis.value >= (0x7FFF / 4))
		*InputBitfield |= input_id2;
	else
		*InputBitfield &= ~input_id2;
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
						DoButton(INPUT_JUMP);
						break;
					}
					case SDL_CONTROLLER_BUTTON_B:
					{
						DoButton(INPUT_SHOOT);
						break;
					}
					case SDL_CONTROLLER_BUTTON_X:
					{
						DoButton(INPUT_SHOOT);
						break;
					}
					case SDL_CONTROLLER_BUTTON_Y:
					{
						DoButton(INPUT_JUMP);
						break;
					}
					case SDL_CONTROLLER_BUTTON_BACK:
					{
						DoButton(INPUT_MAP);
						break;
					}
					case SDL_CONTROLLER_BUTTON_START:
					{
						DoButton(INPUT_INVENTORY);
						break;
					}
					case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
					{
						DoButton(INPUT_PREVIOUSWEAPON);
						break;
					}
					case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
					{
						DoButton(INPUT_NEXTWEAPON);
						break;
					}
					case SDL_CONTROLLER_BUTTON_DPAD_UP:
					{
						DoButton(INPUT_UP | INPUT_ALT_UP | INPUT_ALT_UP2);
						break;
					}
					case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
					{
						DoButton(INPUT_DOWN | INPUT_ALT_DOWN);
						break;
					}
					case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
					{
						DoButton(INPUT_LEFT | INPUT_ALT_LEFT);
						break;
					}
					case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
					{
						DoButton(INPUT_RIGHT | INPUT_ALT_RIGHT);
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
						DoTrigger(INPUT_PREVIOUSWEAPON);
						break;
					}
					case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
					{
						DoTrigger(INPUT_NEXTWEAPON);
						break;
					}
					case SDL_CONTROLLER_AXIS_LEFTX:
					{
						DoStick(INPUT_LEFT, INPUT_RIGHT);
						break;
					}
					case SDL_CONTROLLER_AXIS_LEFTY:
					{
						DoStick(INPUT_UP, INPUT_DOWN);
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
	WriteLong(0x420EF6, 0x90909090);
	WriteByte(0x420EF6 + 4, 0x90);
	// Redirect controller update function call
	WriteRelativeAddress(0x4135CC + 1, ProcessControllerEvents);
}
