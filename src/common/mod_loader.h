// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#pragma once

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
extern void (*FixDoorEnterBug)(void);

// Mod loader helper functions
extern const char* const GetSetting(const char* const setting_name);
