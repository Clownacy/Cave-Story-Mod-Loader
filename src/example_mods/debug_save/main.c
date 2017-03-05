// Debug Save enable mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "mod_loader.h"

void InitMod(void)
{
	// Stop Debug Save menu from being deleted
	WriteWord(0x412D4D, 0x0FEB);
}
