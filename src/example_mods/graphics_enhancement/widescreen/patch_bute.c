#include "mod_loader.h"

#include "../common.h"
#include "patch_bute.h"

void PatchBute(void)
{
	WriteLong(0x4637C3 + 2, ((SCREEN_WIDTH / 2) + 128) * 0x200);
	WriteLong(0x4637D7 + 1, ((SCREEN_WIDTH / 2) + 112) * 0x200);
	WriteLong(0x4637FB + 1, ((SCREEN_WIDTH / 2) + 128) * 0x200);
	WriteLong(0x46380E + 2, ((SCREEN_WIDTH / 2) + 112) * 0x200);

	WriteByte(0x461AE1 + 1, (((SCREEN_WIDTH / 2) + 64) / 16));
	WriteByte(0x461AE3 + 1, -(((SCREEN_WIDTH / 2) + 64) / 16));
	WriteByte(0x461AFE + 1, (((SCREEN_WIDTH / 2) + 16) / 16));
	WriteByte(0x461B00 + 1, -(((SCREEN_WIDTH / 2) + 16) / 16));
}
