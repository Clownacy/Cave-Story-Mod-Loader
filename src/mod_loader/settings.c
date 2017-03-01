#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "settings.h"

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

	char settings_path[strlen(filename) + 5 + 4 + 1];
	strcpy(settings_path, "mods/");
	strcat(settings_path, filename);
	strcat(settings_path, ".txt");

	FILE *settings_file = fopen(settings_path, "r");

	if (settings_file != NULL)
	{
		char setting_string[MAX_PATH];
		while (fgets(setting_string, MAX_PATH, settings_file) != NULL)
		{
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

			AddSetting(setting_name, setting_value, settings_list_head_ptr);
		}

		fclose(settings_file);
	}

	return settings_list_head_ptr;
}

__declspec(dllexport) const char* const GetSetting(const char* const setting_name, Setting **settings_list_head)
{
	for (Setting *setting = *settings_list_head; setting != NULL; setting = setting->next)
	{
		if (strcmp(setting->name, setting_name) == 0)
		{
			return setting->value;
		}
	}

	return NULL;
}
