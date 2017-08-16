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

static char* TrimSettingString(char *string, unsigned int string_length)
{
	if (string_length != 0)
	{
		unsigned int start_padding = 0;
		while (string[start_padding] == ' ' || string[start_padding] == '\t')
			if (++start_padding == string_length)
				break;

		unsigned int end_padding = string_length;
		while (string[end_padding - 1] == ' ' || string[end_padding - 1] == '\t')
			if (--end_padding == 0)
				break;

		char *string_trimmed = malloc(end_padding - start_padding + 1);
		strncpy(string_trimmed, &string[start_padding], end_padding - start_padding);
		string_trimmed[end_padding - start_padding] = '\0';

		return string_trimmed;
	}
	else
	{
		return "\0";
	}
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

			char* setting_name = TrimSettingString(setting_string, strcspn(setting_string, ":="));
			char* setting_value = TrimSettingString(setting_string + strcspn(setting_string, ":=") + 1, strlen(setting_string + strcspn(setting_string, ":=") + 1));

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
