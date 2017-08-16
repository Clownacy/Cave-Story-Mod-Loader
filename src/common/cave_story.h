// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#pragma once

#include <windows.h>

// Variables
static RECT* const clip_rect = (RECT*)0x48F91C;
static int* const down_key_mapping = (void*)0x49363C;
static int* const background_tile_width = (int*)0x499C78;
static int* const background_tile_height = (int*)0x499C7C;
static int* const dword_499C8C = (int*)0x499C8C;
static int* const window_padding_h = (int*)0x49CDA8;
static int* const window_padding_w = (int*)0x49CDAC;
static int* const window_surface_width = (int*)0x49D374;
static int* const window_surface_height = (int*)0x49D378;
static signed int* const camera_x_pos = (signed int*)0x49E1C8;
static signed int* const camera_y_pos = (signed int*)0x49E1CC;
static signed int* const camera_x_destination = (void*)0x49E1D0;
static signed int* const camera_y_destination = (void*)0x49E1D4;
static int* const input_bitfield_held = (void*)0x49E210;
static int* const input_bitfield_newly_pressed = (void*)0x49E214;
static int* const level_layout_buffer = (int*)0x49E480;
static char* const byte_49E484 = (char*)0x49E484;
static HWND* const hWnd = (HWND* const)0x49E458;
static int* const gamepad_enabled = (int* const)0x49E45C;
static unsigned short* const level_width = (unsigned short*)0x49E586;
static unsigned short* const level_height = (unsigned short*)0x49E588;
static int* const music_fade_flag = (void*)0x4A4E10;
static int* const current_room = (void*)0x4A57F0;
static int* const current_music = (int* const)0x4A57F4;
static int* const previous_song_last_position = (int* const)0x4A57F8;
static int* const previous_music = (int* const)0x4A57FC;

// String array
static const char* const * const org_playlist = (const char* const * const)0x4981E8;

// Functions
static void (* const LoadBackgroundSprite)(char*, int) = (void(*)(char*,int))0x402270;
static void (* const DrawWater)(int x_pos, int y_pos) = (void(*)(int,int))0x402830;
static void (* const DrawSprite_WithTransparency)(RECT*,int,int,RECT*,int) = (void(*)(RECT*,int,int,RECT*,int))0x40C3C0;
static void (* const DrawSprite_NoTransparency)(RECT*, int, int, RECT*, int) = (void(*)(RECT*, int, int, RECT*, int))0x40C5B0;
static void (* const DrawColourFill)(RECT *dst_rect, int colour) = (void(*)(RECT*, int))0x40C9E0;
static void (* const DrawNumber)(int x, int y, int, int) = (void(*)(int x, int y, int, int))0x40F380;
static void (* const DrawHUD_Air)(int x, int y) = (void(*)(int x, int y))0x41A350;
static void (* const LoadOrgMusic)(const char* const) = (void(*)(const char* const))0x41C6F0;
static void (* const SetOrgMusicPosition)(int) = (void(*)(int))0x41C730;
static int (* const GetOrgMusicPosition)(void) = (int(*)(void))0x41C770;
static void (* const StartOrgPlayback)(void) = (void(*)(void))0x41C790;
static void (* const SetOrgVolume)(int) = (void(*)(int))0x41C7C0;
static void (* const sub_41C7F0)(void) = (void(*)(void))0x41C7F0;
static void (* const FadeMusic)(void) = (void(*)(void))0x41C880;
static void (* const PlayMusic)(int music_id) = (void(*)(int))0x420EE0;
static void (* const PlayPreviousMusic)(void) = (void(*)(void))0x420F50;
