#include "60fps.h"
#include "main.h"

void Init60FPS(void)
{
	// 60 FPS
	WriteByte(0x40B36F, 17);
	WriteByte(0x40B3A9, 17);
}
