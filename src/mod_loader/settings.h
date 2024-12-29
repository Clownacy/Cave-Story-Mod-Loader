// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#pragma once

#include <stdbool.h>

typedef struct Setting Setting;

extern Setting *mod_loader_settings;

extern Setting* ReadSettings(const char* const filename);
__declspec(dllexport) extern const char* GetSettingString(const char* const setting_name, const char* const default_string, const Setting* const settings_list_head);
__declspec(dllexport) extern int GetSettingInt(const char* const setting_name, const int default_int, const Setting* const settings_list_head);
__declspec(dllexport) extern bool GetSettingBool(const char* const setting_name, const bool default_bool, const Setting* const settings_list_head);
