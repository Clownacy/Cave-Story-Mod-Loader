// Mod loader for Freeware Cave Story
// Public domain

#pragma once

#include <stdbool.h>

#include "mod_loader_hooks.h"

extern const char *mod_loader_path_to_dll;

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
extern void (*ModLoader_AddStackableHook)(void * address, unsigned int length, MLHookCallback cb, void * ud);

// Mod loader helper functions
const char* ModLoader_GetSettingString(const char* const setting_name, const char* const default_string);
int ModLoader_GetSettingInt(const char* const setting_name, const int default_int);
bool ModLoader_GetSettingBool(const char* const setting_name, const bool default_bool);
