// Template to show how to use the mod loader

#include <windows.h>

#include "mod_loader.h"	// This contains all of the mod loader's helper functions

static int test_setting;

static void SetWindowName(HWND hWnd)
{
	char *window_name;

	switch (test_setting)
	{
		case 0:
			window_name = "Sonic the Bionic";
			break;
		case 1:
			window_name = "Cory Story";
			break;
		case 2:
			window_name = "Literally everything is Misery's fault";
			break;
		default:
			window_name = "Ball OS";
			break;
	}

	SetWindowTextA(hWnd, window_name);
}

// This is our entry-point, called when the game starts
void InitMod(void)
{
	// Get the setting
	test_setting = ModLoader_GetSettingInt("example_setting_int", 0);

	// Show an error message to the player
	ModLoader_PrintErrorMessageBox("test error");

	// Rename the config file to something else
	ModLoader_WriteLong((void*)0x40AD84 + 2, (unsigned long)"configuration.bin");

	// Hijack the set-window-name function, and replace it with our own
	ModLoader_WriteJump((void*)0x412320, (void*)SetWindowName);
}
