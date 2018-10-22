// Ikachan cursor enable mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "mod_loader.h"

static const char* const cursor_filename = "CURSOR_IKA";

void InitMod(void)
{
	ModLoader_WriteLong((void*)0x41272B + 1, (int)cursor_filename);
}
