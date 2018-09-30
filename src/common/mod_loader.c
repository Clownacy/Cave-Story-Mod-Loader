// Mod loader for Freeware Cave Story
// Public domain

#include "mod_loader.h"

#include <stdbool.h>
#include <windows.h>

typedef struct Settings Settings;

extern void InitMod(void);

void (*ModLoader_WriteRelativeAddress)(void* const address, const void* const new_destination);
void (*ModLoader_WriteByte)(void* const address, const unsigned char value);
void (*ModLoader_WriteWord)(void* const address, const unsigned short value);
void (*ModLoader_WriteLong)(void* const address, const unsigned int value);
void (*ModLoader_WriteWordBE)(void* const address, const unsigned short value);
void (*ModLoader_WriteLongBE)(void* const address, const unsigned int value);
void (*ModLoader_WriteJump)(void* const address, const void* const new_destination);
void (*ModLoader_WriteCall)(void* const address, const void* const new_destination);
void (*ModLoader_WriteNOPs)(void* const address, const unsigned int count);
void (*ModLoader_FixDoorEnterBug)(void);
void (*ModLoader_PrintErrorMessageBox)(const char* const format, ...);
void (*ModLoader_PrintError)(const char* const format, ...);
void (*ModLoader_PrintDebug)(const char* const format, ...);
void (*ModLoader_AddStackableHook)(void * address, unsigned int length, MLHookCallback cb, void * ud);

const char *mod_loader_path_to_dll;

static const char* (*GetSettingString)(const char* const filename, const char* const default_string, const Settings* const settings);
static int (*GetSettingInt)(const char* const filename, const int default_int, const Settings* const settings);
static bool (*GetSettingBool)(const char* const filename, const bool default_bool, const Settings* const settings);

static const Settings *settings;

const char* ModLoader_GetSettingString(const char* const setting_name, const char* const default_string)
{
	return GetSettingString(setting_name, default_string, settings);
}

int ModLoader_GetSettingInt(const char* const setting_name, const int default_int)
{
	return GetSettingInt(setting_name, default_int, settings);
}

bool ModLoader_GetSettingBool(const char* const setting_name, const bool default_bool)
{
	return GetSettingBool(setting_name, default_bool, settings);
}

#ifdef __cplusplus
extern "C"
#endif
__declspec(dllexport) void ModEntry(const HMODULE mod_loader_hmodule, const Settings* const settings_p, const char* const path_to_dll)
{
	settings = settings_p;
	mod_loader_path_to_dll = path_to_dll;

	ModLoader_WriteRelativeAddress = (void (*)(void* const, const void* const))GetProcAddress(mod_loader_hmodule, "WriteRelativeAddress");
	ModLoader_WriteByte = (void (*)(void* const, const unsigned char))GetProcAddress(mod_loader_hmodule, "WriteByte");
	ModLoader_WriteWord = (void (*)(void* const, const unsigned short))GetProcAddress(mod_loader_hmodule, "WriteWord");
	ModLoader_WriteLong = (void (*)(void* const, const unsigned int))GetProcAddress(mod_loader_hmodule, "WriteLong");
	ModLoader_WriteWordBE = (void (*)(void* const, const unsigned short))GetProcAddress(mod_loader_hmodule, "WriteWordBE");
	ModLoader_WriteLongBE = (void (*)(void* const, const unsigned int))GetProcAddress(mod_loader_hmodule, "WriteLongBE");
	ModLoader_WriteJump = (void (*)(void* const, const void* const))GetProcAddress(mod_loader_hmodule, "WriteJump");
	ModLoader_WriteCall = (void (*)(void* const, const void* const))GetProcAddress(mod_loader_hmodule, "WriteCall");
	ModLoader_WriteNOPs = (void (*)(void* const, const unsigned int))GetProcAddress(mod_loader_hmodule, "WriteNOPs");
	ModLoader_FixDoorEnterBug = (void (*)(void))GetProcAddress(mod_loader_hmodule, "FixDoorEnterBug");

	GetSettingString = (const char* (*)(const char* const, const char* const, const Settings* const))GetProcAddress(mod_loader_hmodule, "GetSettingString");
	GetSettingInt = (int (*)(const char* const, const int, const Settings* const))GetProcAddress(mod_loader_hmodule, "GetSettingInt");
	GetSettingBool = (bool (*)(const char* const, const bool, const Settings* const))GetProcAddress(mod_loader_hmodule, "GetSettingBool");

	ModLoader_PrintErrorMessageBox = (void (*)(const char* const format, ...))GetProcAddress(mod_loader_hmodule, "PrintMessageBoxError");
	ModLoader_PrintError = (void (*)(const char* const format, ...))GetProcAddress(mod_loader_hmodule, "PrintError");
	ModLoader_PrintDebug = (void (*)(const char* const format, ...))GetProcAddress(mod_loader_hmodule, "PrintDebug");

	ModLoader_AddStackableHook = (void (*)(void * address, unsigned int length, MLHookCallback cb, void * ud))GetProcAddress(mod_loader_hmodule, "AddStackableHook");

	InitMod();
}
