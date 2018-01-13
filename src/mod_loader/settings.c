// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "settings.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "inih/ini.h"
#include "log.h"
#include "sprintfMalloc.h"

typedef struct Setting
{
	const char *name;
	const char *value;
	struct Setting *next;
} Setting;

Setting *mod_loader_settings;

static int INICallback(void *user, const char *setting_section, const char *setting_name, const char *setting_value)
{
	Setting **settings_list_head = (Setting**)user;

	PrintDebug("      Setting name: '%s'\n      Setting value: '%s'\n", setting_name, setting_value);

	Setting *setting = malloc(sizeof(Setting));
	setting->name = strdup(setting_name);
	setting->value = strdup(setting_value);

	setting->next = *settings_list_head;
	*settings_list_head = setting;

	return 1;
}

Setting* ReadSettings(const char* const filename)
{
	Setting *settings_list_head = NULL;

	FILE *settings_file;
	if (filename)
	{
		char *settings_path = sprintfMalloc("mods/%s/settings.ini", filename);
		settings_file = fopen(settings_path, "r");
		free(settings_path);
	}
	else
	{
		settings_file = fopen("mods/settings.ini", "r");
	}

	if (settings_file != NULL)
	{
		if (filename)
			PrintDebug("    Mod has settings file!\n");

		ini_parse_file(settings_file, INICallback, &settings_list_head);

		fclose(settings_file);
	}

	return settings_list_head;
}

__declspec(dllexport) const char* const GetSettingString(const char* const setting_name, const Setting* const settings_list_head)
{
	const char *setting_value = "";

	for (const Setting *setting = settings_list_head; setting != NULL; setting = setting->next)
	{
		if (strcasecmp(setting->name, setting_name) == 0)
		{
			setting_value = setting->value;
		}
	}

	return setting_value;
}

__declspec(dllexport) int GetSettingInt(const char* const setting_name, const Setting* const settings_list_head)
{
	return strtol(GetSettingString(setting_name, settings_list_head), NULL, 0);
}

__declspec(dllexport) bool GetSettingBool(const char* const setting_name, const Setting* const settings_list_head)
{
	return (strcasecmp(GetSettingString(setting_name, settings_list_head), "true") == 0);
}
