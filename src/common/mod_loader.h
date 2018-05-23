// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

extern const char *ModLoader_path_to_dll;

// Mod loader DLL functions
extern void (*ModLoader_WriteRelativeAddress)(void* const address, const void* const new_destination);
extern void (*ModLoader_WriteByte)(void* const address, const unsigned char value);
extern void (*ModLoader_WriteWord)(void* const address, const unsigned short value);
extern void (*ModLoader_WriteLong)(void* const address, const unsigned int value);
extern void (*ModLoader_WriteWordBE)(void* const address, const unsigned short value);
extern void (*ModLoader_WriteLongBE)(void* const address, const unsigned int value);
extern void (*ModLoader_WriteJump)(void* const address, const void* const new_destination);
extern void (*ModLoader_WriteCall)(void* const address, const void* const new_destination);
extern void (*ModLoader_WriteNOPs)(void* const address, const unsigned int);
extern void (*ModLoader_FixDoorEnterBug)(void);
extern void (*ModLoader_PrintErrorMessageBox)(const char* const format, ...);
extern void (*ModLoader_PrintError)(const char* const format, ...);
extern void (*ModLoader_PrintDebug)(const char* const format, ...);

// Mod loader helper functions
extern const char* ModLoader_GetSettingString(const char* const setting_name, const char* const default_string);
extern int ModLoader_GetSettingInt(const char* const setting_name, const int default_int);
extern bool ModLoader_GetSettingBool(const char* const setting_name, const bool default_bool);

#ifdef __cplusplus
}
#endif
