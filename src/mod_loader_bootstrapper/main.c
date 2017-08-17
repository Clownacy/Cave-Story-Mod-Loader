// Mod loader bootstrapper for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <stdlib.h>
#define DllCanUnloadNow DllCanUnloadNow_thisiswhysuperheadersarebad
#define DllGetClassObject DllGetClassObject_thisiswhysuperheadersarebad
#include <windows.h>
#undef DllCanUnloadNow
#undef DllGetClassObject

#include "sprintfMalloc.h"

void *Original_DirectSoundCreate;
void *Original_DirectSoundEnumerateA;
void *Original_DirectSoundEnumerateW;
void *Original_DllCanUnloadNow;
void *Original_DllGetClassObject;
void *Original_DirectSoundCaptureCreate;
void *Original_DirectSoundCaptureEnumerateA;
void *Original_DirectSoundCaptureEnumerateW;
void *Original_GetDeviceID;
void *Original_DirectSoundFullDuplexCreate;
void *Original_DirectSoundCreate8;
void *Original_DirectSoundCaptureCreate8;

HMODULE mod_loader_hmodule;

void LoadDLLModLoader(void)
{
	void (*ModInit)(void) = (void(*)(void))GetProcAddress(mod_loader_hmodule, "init");

	if (ModInit)
		ModInit();
}

__asm(
"_ASM_LoadDLLModLoader:\n"
"	call	_LoadDLLModLoader\n"
"	movl	(0x498B20),%eax\n"
"	ret\n"
);
extern char ASM_LoadDLLModLoader;

BOOLEAN WINAPI DllMain(IN HINSTANCE hDllHandle, IN DWORD nReason, IN LPVOID Reserved)
{
	if (nReason == DLL_PROCESS_ATTACH)
	{
		char *windir = malloc(MAX_PATH);
		GetWindowsDirectory(windir, MAX_PATH);
		char *dsound_path = sprintfMalloc("%s\\System32\\dsound.dll", windir);
		free(windir);
		const HMODULE original_dll = LoadLibrary(dsound_path);
		free(dsound_path);

		Original_DirectSoundCreate = GetProcAddress(original_dll, "DirectSoundCreate");
		Original_DirectSoundEnumerateA = GetProcAddress(original_dll, "DirectSoundEnumerateA");
		Original_DirectSoundEnumerateW = GetProcAddress(original_dll, "DirectSoundEnumerateW");
		Original_DllCanUnloadNow = GetProcAddress(original_dll, "DllCanUnloadNow");
		Original_DllGetClassObject = GetProcAddress(original_dll, "DllGetClassObject");
		Original_DirectSoundCaptureCreate = GetProcAddress(original_dll, "DirectSoundCaptureCreate");
		Original_DirectSoundCaptureEnumerateA = GetProcAddress(original_dll, "DirectSoundCaptureEnumerateA");
		Original_DirectSoundCaptureEnumerateW = GetProcAddress(original_dll, "DirectSoundCaptureEnumerateW");
		Original_GetDeviceID = GetProcAddress(original_dll, "GetDeviceID");
		Original_DirectSoundFullDuplexCreate = GetProcAddress(original_dll, "DirectSoundFullDuplexCreate");
		Original_DirectSoundCreate8 = GetProcAddress(original_dll, "DirectSoundCreate8");
		Original_DirectSoundCaptureCreate8 = GetProcAddress(original_dll, "DirectSoundCaptureCreate8");

		mod_loader_hmodule = LoadLibrary("mods/mod_loader.dll");
		if (mod_loader_hmodule)
		{
			void (*WriteCall)(void*,void*) = (void(*)(void*,void*))GetProcAddress(mod_loader_hmodule, "WriteCall");
			if (WriteCall)
				WriteCall((void*)0x412429, (void*)&ASM_LoadDLLModLoader);
		}
	}

	return TRUE;
}

__asm(
".globl _DirectSoundCreate\n"
"_DirectSoundCreate:\n"
"	jmp	*_Original_DirectSoundCreate\n"

".globl _DirectSoundEnumerateA\n"
"_DirectSoundEnumerateA:\n"
"	jmp	*_Original_DirectSoundEnumerateA\n"

".globl _DirectSoundEnumerateW\n"
"_DirectSoundEnumerateW:\n"
"	jmp	*_Original_DirectSoundEnumerateW\n"

".globl _DllCanUnloadNow\n"
"_DllCanUnloadNow:\n"
"	jmp	*_Original_DllCanUnloadNow\n"

".globl _DllGetClassObject\n"
"_DllGetClassObject:\n"
"	jmp	*_Original_DllGetClassObject\n"

".globl _DirectSoundCaptureCreate\n"
"_DirectSoundCaptureCreate:\n"
"	jmp	*_Original_DirectSoundCaptureCreate\n"

".globl _DirectSoundCaptureEnumerateA\n"
"_DirectSoundCaptureEnumerateA:\n"
"	jmp	*_Original_DirectSoundCaptureEnumerateA\n"

".globl _DirectSoundCaptureEnumerateW\n"
"_DirectSoundCaptureEnumerateW:\n"
"	jmp	*_Original_DirectSoundCaptureEnumerateW\n"

".globl _GetDeviceID\n"
"_GetDeviceID:\n"
"	jmp	*_Original_GetDeviceID\n"

".globl _DirectSoundFullDuplexCreate\n"
"_DirectSoundFullDuplexCreate:\n"
"	jmp	*_Original_DirectSoundFullDuplexCreate\n"

".globl _DirectSoundCreate8\n"
"_DirectSoundCreate8:\n"
"	jmp	*_Original_DirectSoundCreate8\n"

".globl _DirectSoundCaptureCreate8\n"
"_DirectSoundCaptureCreate8:\n"
"	jmp	*_Original_DirectSoundCaptureCreate8\n"

".section .drectve\n"
".ascii \" -export:\\\"DirectSoundCreate\\\"\"\n"
".ascii \" -export:\\\"DirectSoundEnumerateA\\\"\"\n"
".ascii \" -export:\\\"DirectSoundEnumerateW\\\"\"\n"
".ascii \" -export:\\\"DllCanUnloadNow\\\"\"\n"
".ascii \" -export:\\\"DllGetClassObject\\\"\"\n"
".ascii \" -export:\\\"DirectSoundCaptureCreate\\\"\"\n"
".ascii \" -export:\\\"DirectSoundCaptureEnumerateA\\\"\"\n"
".ascii \" -export:\\\"DirectSoundCaptureEnumerateW\\\"\"\n"
".ascii \" -export:\\\"GetDeviceID\\\"\"\n"
".ascii \" -export:\\\"DirectSoundFullDuplexCreate\\\"\"\n"
".ascii \" -export:\\\"DirectSoundCreate8\\\"\"\n"
".ascii \" -export:\\\"DirectSoundCaptureCreate8\\\"\"\n"
);
