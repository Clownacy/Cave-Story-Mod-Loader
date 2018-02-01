// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#pragma once

#include <ddraw.h>
#include <windows.h>

// Enums
typedef enum SurfaceID
{
	SURFACE_ID_TITLE,
	SURFACE_ID_PIXEL,
	SURFACE_ID_LEVEL_TILESET,
	SURFACE_ID_UNKNOWN_3,
	SURFACE_ID_UNKNOWN_4,
	SURFACE_ID_UNKNOWN_5,
	SURFACE_ID_FADE,
	SURFACE_ID_UNKNOWN_7,
	SURFACE_ID_ITEM_IMAGE,
	SURFACE_ID_MAP,
	SURFACE_ID_SCREEN_GRAB,
	SURFACE_ID_ARMS,
	SURFACE_ID_ARMS_IMAGE,
	SURFACE_ID_ROOM_NAME,
	SURFACE_ID_STAGE_ITEM,
	SURFACE_ID_LOADING,
	SURFACE_ID_MY_CHAR,
	SURFACE_ID_BULLET,
	SURFACE_ID_UNKNOWN_12,
	SURFACE_ID_CARET,
	SURFACE_ID_NPC_SYM,
	SURFACE_ID_LEVEL_SPRITESET_1,
	SURFACE_ID_LEVEL_SPRITESET_2,
	SURFACE_ID_NPC_REGU,
	SURFACE_ID_UNKNOWN_18,
	SURFACE_ID_UNKNOWN_19,
	SURFACE_ID_TEXT_BOX,
	SURFACE_ID_FACE,
	SURFACE_ID_LEVEL_BACKGROUND,
	SURFACE_ID_UNKNOWN_1D,
	SURFACE_ID_UNKNOWN_1E,
	SURFACE_ID_UNKNOWN_1F,
	SURFACE_ID_UNKNOWN_20,
	SURFACE_ID_UNKNOWN_21,
	SURFACE_ID_UNKNOWN_22,
	SURFACE_ID_UNKNOWN_23,
	SURFACE_ID_CREDITS_IMAGE,
	SURFACE_ID_CASTS,
	SURFACE_ID_UNKNOWN_26,
	SURFACE_ID_UNKNOWN_27
} SurfaceID;

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
#define CS_screen_surface (*(IDirectDrawSurface7** const)0x49D384)
#define CS_surfaces (*(IDirectDrawSurface7*(*)[40])0x49D388)
#define CS_camera_x_pos (*(signed int*)0x49E1C8)
#define CS_camera_y_pos (*(signed int*)0x49E1CC)
#define CS_camera_x_destination (*(signed int*)0x49E1D0)
#define CS_camera_y_destination (*(signed int*)0x49E1D4)
#define CS_input_bitfield_held (*(int*)0x49E210)
#define CS_input_bitfield_newly_pressed (*(int*)0x49E214)
#define CS_level_layout_buffer (*(int*)0x49E480)
#define CS_byte_49E484 ((char*)0x49E484)
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
static void (* const CS_LoadBackgroundSprite)(char*, int) = (void(*)(char*,int))0x402270;
static void (* const CS_DrawWater)(int x_pos, int y_pos) = (void(*)(int,int))0x402830;
static void (* const CS_DrawSprite_WithTransparency)(RECT*,int,int,RECT*,SurfaceID) = (void(*)(RECT*,int,int,RECT*,SurfaceID))0x40C3C0;
static void (* const CS_DrawSprite_NoTransparency)(RECT*, int, int, RECT*, SurfaceID) = (void(*)(RECT*, int, int, RECT*, SurfaceID))0x40C5B0;
static void (* const CS_DrawColourFill)(RECT *dst_rect, int colour) = (void(*)(RECT*, int))0x40C9E0;
static void (* const CS_DrawNumber)(int x, int y, int, int) = (void(*)(int x, int y, int, int))0x40F380;
static void (* const CS_DrawHUD_Air)(int x, int y) = (void(*)(int x, int y))0x41A350;
static void (* const CS_LoadOrgMusic)(const char* const) = (void(*)(const char* const))0x41C6F0;
static void (* const CS_SetOrgMusicPosition)(int) = (void(*)(int))0x41C730;
static int (* const CS_GetOrgMusicPosition)(void) = (int(*)(void))0x41C770;
static void (* const CS_StartOrgPlayback)(void) = (void(*)(void))0x41C790;
static void (* const CS_SetOrgVolume)(int) = (void(*)(int))0x41C7C0;
static void (* const CS_sub_41C7F0)(void) = (void(*)(void))0x41C7F0;
static void (* const CS_FadeMusic)(void) = (void(*)(void))0x41C880;
static void (* const CS_PlayMusic)(int music_id) = (void(*)(int))0x420EE0;
static void (* const CS_PlayPreviousMusic)(void) = (void(*)(void))0x420F50;
