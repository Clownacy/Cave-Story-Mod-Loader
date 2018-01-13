// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#pragma once

#include <stdbool.h>

typedef struct Setting Setting;

extern const char* location_path;

// Mod loader DLL functions
extern void (*WriteRelativeAddress)(void* const address, const void* const new_destination);
extern void (*WriteByte)(void* const address, const char value);
extern void (*WriteWord)(void* const address, const short value);
extern void (*WriteLong)(void* const address, const int value);
extern void (*WriteWordBE)(void* const address, const short value);
extern void (*WriteLongBE)(void* const address, const int value);
extern void (*WriteJump)(void* const address, const void* const new_destination);
extern void (*WriteCall)(void* const address, const void* const new_destination);
extern void (*WriteNOPs)(void* const address, const unsigned int);
extern void (*FixDoorEnterBug)(void);
extern void (*PrintError)(const char* const format, ...);
extern void (*PrintDebug)(const char* const format, ...);

// Mod loader helper functions
extern const char* const GetSettingString(const char* const setting_name, const char* const default_string);
extern int GetSettingInt(const char* const setting_name, const int default_int);
extern bool GetSettingBool(const char* const setting_name, const bool default_bool);
