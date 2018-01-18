// Mod loader bootstrapper for Freeware Cave Story
// Copyright © 2018 Clownacy

#include <stdlib.h>
#include <windows.h>

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
		GetSystemDirectory(windir, MAX_PATH);
		char *dsound_path = sprintfMalloc("%s\\dsound.dll", windir);
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
".globl _New_DirectSoundCreate\n"
"_New_DirectSoundCreate:\n"
"	jmp	*_Original_DirectSoundCreate\n"

".globl _New_DirectSoundEnumerateA\n"
"_New_DirectSoundEnumerateA:\n"
"	jmp	*_Original_DirectSoundEnumerateA\n"

".globl _New_DirectSoundEnumerateW\n"
"_New_DirectSoundEnumerateW:\n"
"	jmp	*_Original_DirectSoundEnumerateW\n"

".globl _New_DllCanUnloadNow\n"
"_New_DllCanUnloadNow:\n"
"	jmp	*_Original_DllCanUnloadNow\n"

".globl _New_DllGetClassObject\n"
"_New_DllGetClassObject:\n"
"	jmp	*_Original_DllGetClassObject\n"

".globl _New_DirectSoundCaptureCreate\n"
"_New_DirectSoundCaptureCreate:\n"
"	jmp	*_Original_DirectSoundCaptureCreate\n"

".globl _New_DirectSoundCaptureEnumerateA\n"
"_New_DirectSoundCaptureEnumerateA:\n"
"	jmp	*_Original_DirectSoundCaptureEnumerateA\n"

".globl _New_DirectSoundCaptureEnumerateW\n"
"_New_DirectSoundCaptureEnumerateW:\n"
"	jmp	*_Original_DirectSoundCaptureEnumerateW\n"

".globl _New_GetDeviceID\n"
"_New_GetDeviceID:\n"
"	jmp	*_Original_GetDeviceID\n"

".globl _New_DirectSoundFullDuplexCreate\n"
"_New_DirectSoundFullDuplexCreate:\n"
"	jmp	*_Original_DirectSoundFullDuplexCreate\n"

".globl _New_DirectSoundCreate8\n"
"_New_DirectSoundCreate8:\n"
"	jmp	*_Original_DirectSoundCreate8\n"

".globl _New_DirectSoundCaptureCreate8\n"
"_New_DirectSoundCaptureCreate8:\n"
"	jmp	*_Original_DirectSoundCaptureCreate8\n"

".section .drectve\n"
".ascii \" -export:DirectSoundCreate=New_DirectSoundCreate\"\n"
".ascii \" -export:DirectSoundEnumerateA=New_DirectSoundEnumerateA\"\n"
".ascii \" -export:DirectSoundEnumerateW=New_DirectSoundEnumerateW\"\n"
".ascii \" -export:DllCanUnloadNow=New_DllCanUnloadNow\"\n"
".ascii \" -export:DllGetClassObject=New_DllGetClassObject\"\n"
".ascii \" -export:DirectSoundCaptureCreate=New_DirectSoundCaptureCreate\"\n"
".ascii \" -export:DirectSoundCaptureEnumerateA=New_DirectSoundCaptureEnumerateA\"\n"
".ascii \" -export:DirectSoundCaptureEnumerateW=New_DirectSoundCaptureEnumerateW\"\n"
".ascii \" -export:GetDeviceID=New_GetDeviceID\"\n"
".ascii \" -export:DirectSoundFullDuplexCreate=New_DirectSoundFullDuplexCreate\"\n"
".ascii \" -export:DirectSoundCreate8=New_DirectSoundCreate8\"\n"
".ascii \" -export:DirectSoundCaptureCreate8=New_DirectSoundCaptureCreate8\"\n"
);
