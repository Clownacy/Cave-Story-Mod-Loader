// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch_fps_counter.h"

#include "cave_story.h"
#include "mod_loader.h"

#include "../common.h"

void PatchFPSCounter(void)
{
	ModLoader_WriteLong((void*)0x41238D + 1, SCREEN_WIDTH - 40);
}
