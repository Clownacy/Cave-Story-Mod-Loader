#include "mod_loader_wrapper.h"

#include "sprite_resolution/sprite_resolution.h"
#include "upscale_window/upscale_window.h"
#include "widescreen/widescreen.h"

void InitMod(void)
{
	SetWidescreen();
	SetSpriteResolution(2);
	UpscaleWindow(2);
}
