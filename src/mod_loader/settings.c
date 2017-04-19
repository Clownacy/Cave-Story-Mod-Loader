// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "settings.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "log.h"

typedef struct Setting
{
	char *name;
	char *value;
	struct Setting *next;
} Setting;

void AddSetting(char *setting_name, char *setting_value, Setting **settings_list_head)
{
	Setting *setting = malloc(sizeof(Setting));
	setting->name = setting_name;
	setting->value = setting_value;

	Setting **settings_next_pointer = settings_list_head;

	while (*settings_next_pointer != NULL)
	{
		settings_next_pointer = &(*settings_next_pointer)->next;
	}

	*settings_next_pointer = setting;
}

void* ReadSettings(const char* const filename)
{
	Setting **settings_list_head_ptr = malloc(sizeof(void*));
	*settings_list_head_ptr = NULL;

	char settings_path[5 + strlen(filename) + 13 + 1];
	sprintf(settings_path, "mods/%s/settings.txt", filename);

	FILE *settings_file = fopen(settings_path, "r");

	if (settings_file != NULL)
	{
		PrintDebug("  Mod has settings file!\n");
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

			PrintDebug("    Setting name: '%s'\n    Setting value: '%s'\n", setting_name, setting_value);
			AddSetting(setting_name, setting_value, settings_list_head_ptr);
		}

		fclose(settings_file);
	}

	return settings_list_head_ptr;
}

__declspec(dllexport) const char* const GetSetting(const char* const setting_name, const Setting* const * const settings_list_head)
{
	for (const Setting *setting = *settings_list_head; setting != NULL; setting = setting->next)
	{
		if (strcmp(setting->name, setting_name) == 0)
		{
			return setting->value;
		}
	}

	return NULL;
}
