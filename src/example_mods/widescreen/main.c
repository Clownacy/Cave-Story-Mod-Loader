#include <stdbool.h>

#include <windows.h>

#include "mod_loader_wrapper.h"

#include "aspect_ratio.h"
#include "centre_camera.h"
#include "centre_text_box.h"
#include "fix_subforeground_bug.h"
#include "scroll_clouds.h"

void InitMod(void)
{
	// Widescreen
//	char text_box_x = native_width - (320 - 244) / 2);
//	WriteByte(0x422013 + 1, 242);
//	WriteByte(0x42220D + 0, 0x90);
//	WriteByte(0x42220D + 1, 0x90);
//	WriteByte(0x42220D + 2, 0x90);
//	WriteByte(0x42220D + 3, 0x90);
//	WriteByte(0x42220D + 4, 0x90);
//	WriteByte(0x421F10, 0xC3);
	const int native_width_unk = native_width * (216. / 320);
	const int native_width_unk2 = native_width * (268. / 320);

	// Patch level drawers
	//  DrawBackground
	WriteLong(0x402445 + 3, native_width);
	WriteLong(0x4024D7 + 3, native_width);
	WriteLong(0x40255F + 3, native_width);
	WriteLong(0x4025D9 + 3, native_width);
//	WriteLong(0x40261B + 3, native_width);
//	WriteLong(0x402654 + 3, native_width);
	WriteLong(0x40268C + 1, native_width);
	WriteLong(0x402693 + 1, native_width);
	WriteLong(0x4026BC + 1, native_width);
//	WriteLong(0x4026C6 + 3, native_width);
	WriteLong(0x4026F9 + 1, native_width);
	WriteLong(0x402700 + 1, native_width);
	WriteLong(0x40272B + 1, native_width);
//	WriteLong(0x402735 + 3, native_width);
	WriteLong(0x402770 + 1, native_width);
	WriteLong(0x402777 + 1, native_width);
	WriteLong(0x4027A3 + 1, native_width);
//	WriteLong(0x4027AD + 3, native_width);
	WriteLong(0x4027E9 + 1, native_width);
	WriteLong(0x4027F0 + 1, native_width);

	//  DrawForegroundBG
	WriteLong(0x413AF6 + 3, (native_width / 16) + 1);
	//  DrawForegroundFG
	WriteLong(0x413C82 + 3, (native_width / 16) + 1);
	//  ???
	WriteLong(0x413E55 + 3, (native_width / 16) + 1);

	// Patch memory rectangles
	//WriteLong(0x48F91C + 0, -native_width);
	WriteLong(0x48F91C + 8, native_width);
	WriteLong(0x48F92C + 8, native_width);
	WriteLong(0x493650 + 8, native_width / 2);
	WriteLong(0x498290 + 8, native_width_unk);

	// Unknown camera(?) function
//	WriteLong(0x40EB95 + 3, native_width);
//	WriteLong(0x40EB9E + 3, native_width);
//	WriteLong(0x40EBE4 + 6, native_width);
//	WriteLong(0x40ED03 + 6, native_width);

	// Something inventory(?) related
//	WriteLong(0x421A0D + 6, native_width_unk2);

	// Related to credits. May be the characters ("casts")
	WriteLong(0x40D575, native_width / 2);

	// Patch WinMain (window creation and other stuff)
	WriteLong(0x4126E5 + 3, native_width);
	WriteLong(0x4127BC + 6, native_width);
	WriteLong(0x4127D2 + 6, native_width * 2);
	WriteLong(0x41293F + 6, native_width * 2);
	WriteLong(0x412A81 + 6, native_width);

	// InitDirectDraw
	WriteLong(0x40B53F + 2, native_width);
	WriteLong(0x40B571 + 2, native_width);
	WriteLong(0x40B594 + 2, native_width);
	WriteLong(0x40B62F + 2, native_width);

	// ??????? (sub_40F040) The function appears to be unused
	//WriteLong(0x40F0C5 + 2, native_width);
	//WriteLong(0x40F0E0 + 2, native_width);
/*
	// Fading part 1
	WriteByte(0x40E7DA + 3, native_width / 16);

	// Fading part 2
	WriteByte(0x40E04B + 3, native_width / 16);
	WriteByte(0x40E0A5 + 3, native_width / 16);
	WriteByte(0x40E0FA + 3, native_width / 16);
	WriteByte(0x40E154 + 3, native_width / 16);
	WriteByte(0x40E1FC + 3, native_width / 16);
	WriteByte(0x40E2AC + 3, native_width / 16);
	WriteByte(0x40E30B + 3, native_width / 16);
	WriteByte(0x40E3E3 + 3, native_width / 16);
	WriteByte(0x40E43D + 3, native_width / 16);
	WriteByte(0x40E492 + 3, native_width / 16);
	WriteByte(0x40E4EC + 3, native_width / 16);
	WriteByte(0x40E59B + 3, native_width / 16);
	WriteByte(0x40E659 + 3, native_width / 16);
	WriteByte(0x40E6BF + 3, native_width / 16);

	// 3
	WriteByte(0x40DF16 + 3, native_width / 16);
	WriteByte(0x40DFA6 + 3, native_width / 16);
*/
	// sub_473000
	//WriteByte(0x47303B + 3, native_width / 16);

	// Patch water to cover screen
	WriteByte(0x402896 + 2, (native_width / 32) + 1);

	// Patch IncrementBGScroll
	//WriteLong(0x4023B9 + 1, native_width * 2);
	//WriteLong(0x402396 + 2, 0xC00 * 2);

	// Hijack scrolling cloud background code
	PatchScrollClouds();
	FixSubForegroundBug();
	PatchCamera();
	CentreTextBox();
}
