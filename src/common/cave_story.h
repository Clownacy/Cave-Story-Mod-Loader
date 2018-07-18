// Mod loader for Freeware Cave Story
// Public domain

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <ddraw.h>
#include <windows.h>

// Enums
typedef enum CS_SurfaceID
{
	CS_SURFACE_ID_TITLE,
	CS_SURFACE_ID_PIXEL,
	CS_SURFACE_ID_LEVEL_TILESET,
	CS_SURFACE_ID_UNKNOWN_3,
	CS_SURFACE_ID_UNKNOWN_4,
	CS_SURFACE_ID_UNKNOWN_5,
	CS_SURFACE_ID_FADE,
	CS_SURFACE_ID_UNKNOWN_7,
	CS_SURFACE_ID_ITEM_IMAGE,
	CS_SURFACE_ID_MAP,
	CS_SURFACE_ID_SCREEN_GRAB,
	CS_SURFACE_ID_ARMS,
	CS_SURFACE_ID_ARMS_IMAGE,
	CS_SURFACE_ID_ROOM_NAME,
	CS_SURFACE_ID_STAGE_ITEM,
	CS_SURFACE_ID_LOADING,
	CS_SURFACE_ID_MY_CHAR,
	CS_SURFACE_ID_BULLET,
	CS_SURFACE_ID_UNKNOWN_12,
	CS_SURFACE_ID_CARET,
	CS_SURFACE_ID_NPC_SYM,
	CS_SURFACE_ID_LEVEL_SPRITESET_1,
	CS_SURFACE_ID_LEVEL_SPRITESET_2,
	CS_SURFACE_ID_NPC_REGU,
	CS_SURFACE_ID_UNKNOWN_18,
	CS_SURFACE_ID_UNKNOWN_19,
	CS_SURFACE_ID_TEXT_BOX,
	CS_SURFACE_ID_FACE,
	CS_SURFACE_ID_LEVEL_BACKGROUND,
	CS_SURFACE_ID_UNKNOWN_1D,
	CS_SURFACE_ID_UNKNOWN_1E,
	CS_SURFACE_ID_UNKNOWN_1F,
	CS_SURFACE_ID_UNKNOWN_20,
	CS_SURFACE_ID_UNKNOWN_21,
	CS_SURFACE_ID_UNKNOWN_22,
	CS_SURFACE_ID_UNKNOWN_23,
	CS_SURFACE_ID_CREDITS_IMAGE,
	CS_SURFACE_ID_CASTS,
	CS_SURFACE_ID_UNKNOWN_26,
	CS_SURFACE_ID_UNKNOWN_27
} CS_SurfaceID;

// Structs
typedef struct CS_ConfigData
{
	char magic_string[0x20];
	char font_name[0x40];
	unsigned long directional_keys;
	unsigned long jump_shoot_keys;
	unsigned long jump_shoot_keys_2;
	unsigned long window_size;
	unsigned long gamepad_enabled;
	unsigned long gamepad_buttons[8];
} CS_ConfigData;

// Variables
#define CS_window_upscale (*(int*)0x48F914)
#define CS_clip_rect_common (*(RECT*)0x48F91C)
#define CS_down_key_mapping (*(int*)0x49363C)
#define CS_background_tile_width (*(int*)0x499C78)
#define CS_background_tile_height (*(int*)0x499C7C)
#define CS_dword_499C8C (*(int*)0x499C8C)
#define CS_window_padding_h (*(int*)0x49CDA8)
#define CS_window_padding_w (*(int*)0x49CDAC)
#define CS_window_surface_width (*(int*)0x49D374)
#define CS_window_surface_height (*(int*)0x49D378)
#define CS_directdraw (*(IDirectDraw7**)0x49D37C)
#define CS_screen_primary_surface (*(IDirectDrawSurface7** const)0x49D380)
#define CS_screen_surface (*(IDirectDrawSurface7** const)0x49D384)
#define CS_surfaces (*(IDirectDrawSurface7*(*)[40])0x49D388)
#define CS_fade_counter (*(int*)0x49DB38)
#define CS_camera_x_pos (*(signed int*)0x49E1C8)
#define CS_camera_y_pos (*(signed int*)0x49E1CC)
#define CS_camera_x_destination (*(signed int*)0x49E1D0)
#define CS_camera_y_destination (*(signed int*)0x49E1D4)
#define CS_gamemode_flags (*(unsigned int*)0x49E1E8)
#define CS_input_bitfield_held (*(int*)0x49E210)
#define CS_input_bitfield_newly_pressed (*(int*)0x49E214)
#define CS_tsc_buffer (*(char**)0x4A5AD8)
#define CS_tsc_offset (*(unsigned int*)0x4A5AE0)
#define CS_pxm_buffer (*(unsigned char**)0x49E480)
#define CS_pxa_buffer ((unsigned char*)0x49E484)
#define CS_hWnd (*(HWND* const)0x49E458)
#define CS_gamepad_enabled (*(int*)0x49E45C)
#define CS_level_width (*(unsigned short*)0x49E586)
#define CS_level_height (*(unsigned short*)0x49E588)
#define CS_music_fade_flag (*(int*)0x4A4E10)
#define CS_current_room (*(int*)0x4A57F0)
#define CS_current_music (*(int*)0x4A57F4)
#define CS_previous_song_last_position (*(int*)0x4A57F8)
#define CS_previous_music (*(int*)0x4A57FC)

// String array
#define CS_org_playlist (*(char*(*)[42])0x4981E8)

// Functions
static void (* const CS_LoadBackgroundSprite)(char *background_filename, int background_type) = (void*)0x402270;
static void (* const CS_IncrementBGScroll)(void) = (void*)0x402370;
static void (* const CS_DrawBackground)(int camera_x_pos, int camera_y_pos) = (void*)0x4023D0;
static void (* const CS_DrawWater)(int x_pos, int y_pos) = (void*)0x402830;
static int (* const CS_LoadConfigFile)(CS_ConfigData *config_memory) = (void*)0x40AD60;
static void (* const CS_DrawSprite_WithTransparency)(RECT*,int,int,RECT*,CS_SurfaceID) = (void*)0x40C3C0;
static void (* const CS_DrawSprite_NoTransparency)(RECT*, int, int, RECT*, CS_SurfaceID) = (void*)0x40C5B0;
static void (* const CS_DrawColourFill)(RECT *dst_rect, int colour) = (void*)0x40C9E0;
static int (* const CS_RegenerateSurfaces)(void) = (void*)0x40CB60;
static int (* const CS_RandomNumber)(int min, int max) = (void*)0x40F350;
static void (* const CS_DrawNumber)(int x, int y, int, int) = (void*)0x40F380;
static void (* const CS_DrawHUD_Air)(int x, int y) = (void*)0x41A350;
static void (* const CS_LoadOrgMusic)(const char* const) = (void*)0x41C6F0;
static void (* const CS_SetOrgMusicPosition)(int) = (void*)0x41C730;
static int (* const CS_GetOrgMusicPosition)(void) = (void*)0x41C770;
static void (* const CS_StartOrgPlayback)(void) = (void*)0x41C790;
static void (* const CS_SetOrgVolume)(int) = (void*)0x41C7C0;
static void (* const CS_sub_41C7F0)(void) = (void*)0x41C7F0;
static void (* const CS_FadeMusic)(void) = (void*)0x41C880;
static void (* const CS_PlayMusic)(int music_id) = (void*)0x420EE0;
static void (* const CS_PlayPreviousMusic)(void) = (void*)0x420F50;
// Good name for this would be 'ExecuteTSC'
static void (* const CS_sub_422510)(void) = (void*)0x422510;
static void (* const CS_CreateObject)(int object_ID, int x_pos, int y_pos, int a4, int a5, int facing_right, int a7, int object_RAM_index) = (void*)0x46EFD0;
static void (* const CS_CreateDustClouds)(int x, int y, signed int range, int count) = (void*)0x46F150;
// Good name for this would be 'UpdateAllNPCs'
static void (* const CS_sub_46FA00)(void) = (void*)0x46FA00;

// Hookspaces & Hookjumps

// COMPATIBILITY NOTES:
// TSC+'s code starts at 4225CB, but the first 2 instructions are identical to Vanilla.
// On unknown command error, TSC+ jumps to 425244.
// If you're still using NICE-Lua at this point:
// Most people using NICE-Lua asked me (20kdc) to do their work, so this shouldn't be an issue, but it operates at
//  a start of 4225ED, and 6 bytes. (This was incompatible with TSC+, and this mistake is avoided here.)
// Theoretically, either of these could be used, but CSH_tsc_start allows the user to override an existing command,
//  and seems to be close enough to the start that it shouldn't get affected.
#define CSH_tsc_start (void*) 0x4225D5, 12
#define CSH_tsc_end (void*) 0x425244, 12
// Aka 4252A7, or "ParserEnd", but minus some indirection.
#define CSJ_tsc_done 0x4225CB

#ifdef __cplusplus
}
#endif
