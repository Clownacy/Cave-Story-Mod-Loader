#include <windows.h>

#include "mod_loader_wrapper.h"

#include "common.h"
#include "fix_subforeground_bug.h"
#include "patch_beetle_and_basu.h"
#include "patch_boss_explosion.h"
#include "patch_boss_health.h"
#include "patch_camera.h"
#include "patch_exit_screen.h"
#include "patch_fade.h"
#include "patch_inventory_screen.h"
#include "patch_island_crash.h"
#include "patch_loading_screen.h"
#include "patch_map_screen.h"
#include "patch_room_name_print.h"
#include "patch_screen_flash.h"
#include "patch_scrolling_clouds.h"
#include "patch_teleport_screen.h"
#include "patch_text_box.h"
#include "patch_tile_drawers.h"
#include "patch_title_screen.h"

void InitMod(void)
{
//	WriteByte(0x40EE39 + 0, 0x90);
//	WriteByte(0x40EE39 + 1, 0x90);
//	WriteByte(0x40EE39 + 2, 0x90);
//	WriteByte(0x40EE39 + 3, 0x90);
//	WriteByte(0x40EE39 + 4, 0x90);
//	WriteByte(0x41A0B0, 0xC3);

	// Patch clip rectangles
	//WriteLong(0x48F91C + 0, -SCREEN_WIDTH);
	// Default clip
	WriteLong(0x48F91C + 8, SCREEN_WIDTH);
	// Some other clip
	WriteLong(0x48F92C + 8, SCREEN_WIDTH);
	// SomethingHUDNumbers
	WriteLong(0x40F39D + 6, SCREEN_WIDTH);
//	WriteLong(0x498290 + 8, native_width_unk);

	// Unknown camera(?) function
//	WriteLong(0x40EB95 + 3, SCREEN_WIDTH);
//	WriteLong(0x40EB9E + 3, SCREEN_WIDTH);
//	WriteLong(0x40EBE4 + 6, SCREEN_WIDTH);
//	WriteLong(0x40ED03 + 6, SCREEN_WIDTH);

	// Something inventory(?) related
//	WriteLong(0x421A0D + 6, native_width_unk2);

	// Related to credits. May be the characters ("casts")
//	WriteLong(0x40D575, SCREEN_WIDTH / 2);

	// Patch WinMain (window creation and other stuff)
	WriteLong(0x4126E5 + 3, SCREEN_WIDTH);
	WriteLong(0x4127BC + 6, SCREEN_WIDTH);
	WriteLong(0x4127D2 + 6, SCREEN_WIDTH * 2);
	WriteLong(0x41293F + 6, SCREEN_WIDTH * 2);

	// InitDirectDraw
	WriteLong(0x40B53F + 2, SCREEN_WIDTH);
	WriteLong(0x40B571 + 2, SCREEN_WIDTH);
	WriteLong(0x40B594 + 2, SCREEN_WIDTH);
	WriteLong(0x40B62F + 2, SCREEN_WIDTH);

	// ??????? (sub_40F040) The function appears to be unused
	//WriteLong(0x40F0C5 + 2, SCREEN_WIDTH);
	//WriteLong(0x40F0E0 + 2, SCREEN_WIDTH);

	// sub_473000
	//WriteByte(0x47303B + 3, SCREEN_WIDTH / 16);

	// Patch IncrementBGScroll
	//WriteLong(0x4023B9 + 1, SCREEN_WIDTH * 2);
	//WriteLong(0x402396 + 2, 0xC00 * 2);

	FixSubForegroundBug();
	PatchBeetleAndBasu();
	PatchBossExplosion();
	PatchBossHealth();
	PatchCamera();
	PatchExitScreen();
	PatchFade();
	PatchInventoryScreen();
	PatchIslandCrash();
	PatchLoadingScreen();
	PatchMapMenu();
	PatchRoomNamePrint();
	PatchScreenFlash();
	PatchScrollingClouds();
	PatchTeleportScreen();
	PatchTextBox();
	PatchTileDrawers();
	PatchTitleScreen();
}
