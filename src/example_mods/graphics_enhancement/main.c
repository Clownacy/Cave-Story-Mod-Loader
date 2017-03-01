#include <stdlib.h>

#include "mod_loader_main.h"

#include "sprite_resolution/sprite_resolution.h"
#include "upscale_window/upscale_window.h"
#include "widescreen/widescreen.h"

void InitMod(void)
{
	SetWidescreen();

	int sprite_resolution_factor = strtol(GetSetting("sprite_resolution"), NULL, 10);
	if (sprite_resolution_factor == 0)
		sprite_resolution_factor = 1;

	SetSpriteResolution(sprite_resolution_factor);

	int window_upscale_factor = strtol(GetSetting("window_upscale"), NULL, 10);
	if (window_upscale_factor == 0)
		window_upscale_factor = 1;

	UpscaleWindow(window_upscale_factor);
}
