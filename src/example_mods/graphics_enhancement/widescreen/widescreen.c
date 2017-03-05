// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "mod_loader.h"

#include "../common.h"
#include "fix_subforeground_bug.h"
#include "patch_beetle_and_basu.h"
#include "patch_boss_explosion.h"
#include "patch_boss_health.h"
#include "patch_bute.h"
#include "patch_camera.h"
#include "patch_exit_screen.h"
#include "patch_fade.h"
#include "patch_gaudi.h"
#include "patch_inventory_screen.h"
#include "patch_ironhead.h"
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
#include "widescreen.h"

void SetWidescreen(void)
{
	// Patch clip rectangles
	// Default clip
	WriteLong(0x48F91C + 8, SCREEN_WIDTH);
	// Some other clip
	WriteLong(0x48F92C + 8, SCREEN_WIDTH);
	// SomethingHUDNumbers
	WriteLong(0x40F39D + 6, SCREEN_WIDTH);
	// GameLoop_StageSelect, for the screengrab
	WriteLong(0x41DA1F + 3, SCREEN_WIDTH);
	// This one I think has to do with text box drawing, but I'm not sure yet
//	WriteLong(0x498290 + 8, (0xD8 * SCREEN_WIDTH) / 320);

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

	// ??????? (sub_40F040) The function appears to be unused. Looks camera-related.
	//WriteLong(0x40F0C5 + 2, SCREEN_WIDTH);
	//WriteLong(0x40F0E0 + 2, SCREEN_WIDTH);

	FixSubForegroundBug();
	PatchBeetleAndBasu();
	PatchBossExplosion();
	PatchBossHealth();
	PatchBute();
	PatchCamera();
	PatchExitScreen();
	PatchFade();
	PatchGaudi();
	PatchInventoryScreen();
	PatchIronhead();
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
