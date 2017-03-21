#pragma once

#include <windows.h>

// Variables
static int* const current_music = (int* const)0x4A57F4;
static int* const previous_song_last_position = (int* const)0x4A57F8;
static int* const previous_music = (int* const)0x4A57FC;
static int* const input_bitfield = (int* const)0x49E210;
static int* const gamepad_enabled = (int* const)0x49E45C;
static int* const music_fade_flag = (void*)0x4A4E10;
static int* const background_tile_width = (int*)0x499C78;
static int* const background_tile_height = (int*)0x499C7C;
static unsigned short *level_width = (unsigned short*)0x49E586;
static unsigned short *level_height = (unsigned short*)0x49E588;
static char *byte_49E484 = (char*)0x49E484;
static int *dword_49E480 = (int*)0x49E480;
static signed int *camera_x_pos = (signed int*)0x49E1C8;
static signed int *camera_y_pos = (signed int*)0x49E1CC;
static int *dword_499C8C = (int*)0x499C8C;
static RECT *clip_rect = (RECT*)0x48F91C;

// String array
static const char* const * const org_playlist = (const char* const * const)0x4981E8;

// Functions
static void (*LoadOrgMusic)(const char* const) = (void(*)(const char* const))0x41C6F0;
static void (*SetOrgMusicPosition)(int) = (void(*)(int))0x41C730;
static int (*GetOrgMusicPosition)(void) = (int(*)(void))0x41C770;
static void (*sub_41C790)(void) = (void(*)(void))0x41C790;
static void (*SetOrgVolume)(int) = (void(*)(int))0x41C7C0;
static void (*sub_41C7F0)(void) = (void(*)(void))0x41C7F0;
static void (*DrawSprite_WithTransparency)(void*,int,int,void*,int) = (void(*)(void*,int,int,void*,int))0x40C3C0;
static void (*LoadBackgroundSprite)(char*, int) = (void(*)(char*,int))0x402270;
static void (*DrawColourFillOntoScreen)(RECT *dst_rect, int colour) = (void(*)(RECT*, int))0x40C9E0;
static void (*DrawWater)(int x_pos, int y_pos) = (void(*)(int,int))0x402830;
static void (*DrawSprite1)(void*, int x, int y, void*, int) = (void(*)(void*, int x, int y, void*, int))0x40C3C0;
static void (*SomethingHUDNumbers)(int x, int y, int, int) = (void(*)(int x, int y, int, int))0x40F380;
static void (*DrawSprite2)(RECT*, int, int, RECT*, int) = (void(*)(RECT*, int, int, RECT*, int))0x40C5B0;
static void (*DrawHUD_Air)(int x, int y) = (void(*)(int x, int y))0x41A350;
static void (*DrawSprite)(RECT* clip, int x, int y, RECT* src, int slot) = (void(*)(RECT* clip, int x, int y, RECT* src, int slot))0x40C3C0;
static void (*ClearScreen)(RECT* rect, int flags) = (void(*)(RECT* rect, int flags))0x40C9E0;








