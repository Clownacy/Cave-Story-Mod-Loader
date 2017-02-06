#include "mod_loader.h"

void InitMod(void)
{
	// Stop Debug Save menu from being deleted
	WriteByte(0x412D4D, 0xEB);
	WriteByte(0x412D4D + 1, 0x0F);
}
