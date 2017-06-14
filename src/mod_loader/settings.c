// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "settings.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "log.h"
#include "sprintfMalloc.h"

typedef struct Setting
{
	char *name;
	char *value;
	struct Setting *next;
} Setting;

Setting *mod_loader_settings;

void AddSetting(char *setting_name, char *setting_value, Setting **settings_list_head)
{
	Setting *setting = malloc(sizeof(Setting));
	setting->name = setting_name;
	setting->value = setting_value;

	setting->next = *settings_list_head;
	*settings_list_head = setting;
}

Setting* ReadSettings(const char* const filename)
{
	Setting *settings_list_head = NULL;

	FILE *settings_file;
	if (filename)
	{
		char *settings_path = sprintfMalloc("mods/%s/settings.txt", filename);
		settings_file = fopen(settings_path, "r");
		free(settings_path);
	}
	else
	{
		settings_file = fopen("mods/settings.txt", "r");
	}

	if (settings_file != NULL)
	{
		if (filename)
			PrintDebug("    Mod has settings file!\n");

		char setting_string[MAX_PATH];
		while (fgets(setting_string, MAX_PATH, settings_file) != NULL)
		{
			setting_string[strcspn(setting_string, "\r\n")] = '\0';	// Trim newline characters
			// Get setting name
			size_t setting_name_length = strcspn(setting_string, ":= \t");
			char* setting_name = malloc(setting_name_length + 1);
			strncpy(setting_name, setting_string, setting_name_length);
			setting_name[setting_name_length] = '\0';

			// Get setting value
			char* setting_value_string = setting_string + setting_name_length + strspn(setting_string + setting_name_length, ":= \t");
			size_t setting_value_length = strcspn(setting_value_string, ":= \t");
			char* setting_value = malloc(setting_value_length + 1);
			strncpy(setting_value, setting_value_string, setting_value_length);
			setting_value[setting_value_length] = '\0';

			if (filename)
				PrintDebug("      Setting name: '%s'\n      Setting value: '%s'\n", setting_name, setting_value);

			AddSetting(setting_name, setting_value, &settings_list_head);
		}

		fclose(settings_file);
	}

	return settings_list_head;
}

__declspec(dllexport) const char* const GetSettingString(const char* const setting_name, const Setting* const settings_list_head)
{
	for (const Setting *setting = settings_list_head; setting != NULL; setting = setting->next)
	{
		if (strcmp(setting->name, setting_name) == 0)
		{
			return setting->value;
		}
	}

	return "";
}

__declspec(dllexport) int GetSettingInt(const char* const setting_name, const Setting* const settings_list_head)
{
	return strtol(GetSettingString(setting_name, settings_list_head), NULL, 10);
}

__declspec(dllexport) bool GetSettingBool(const char* const setting_name, const Setting* const settings_list_head)
{
	return (strcmp(GetSettingString(setting_name, settings_list_head), "true") == 0);
}
