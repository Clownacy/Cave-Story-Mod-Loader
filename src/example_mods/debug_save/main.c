#include "mod_loader_main.h"

void InitMod(void)
{
	// Stop Debug Save menu from being deleted
	WriteWord(0x412D4D, 0x0FEB);
}
