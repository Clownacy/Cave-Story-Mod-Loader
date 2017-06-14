// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#pragma once

#include <stdbool.h>

typedef struct Setting Setting;

extern Setting *mod_loader_settings;

extern Setting* ReadSettings(const char* const filename);
extern const char* const GetSettingString(const char* const setting_name, const Setting* const settings_list_head);
extern int GetSettingInt(const char* const setting_name, const Setting* const settings_list_head);
extern bool GetSettingBool(const char* const setting_name, const Setting* const settings_list_head);
