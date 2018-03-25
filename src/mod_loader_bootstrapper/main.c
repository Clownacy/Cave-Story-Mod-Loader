// Mod loader bootstrapper for Freeware Cave Story
// Copyright © 2018 Clownacy

#include <stdbool.h>
#include <stdlib.h>
#include <windows.h>

#include "sprintfMalloc.h"

// Defined in the ASM below
extern char Init_New_DirectSoundCreate;
extern char Init_New_DirectSoundEnumerateA;
extern char Init_New_DirectSoundEnumerateW;
extern char Init_New_DllCanUnloadNow;
extern char Init_New_DllGetClassObject;
extern char Init_New_DirectSoundCaptureCreate;
extern char Init_New_DirectSoundCaptureEnumerateA;
extern char Init_New_DirectSoundCaptureEnumerateW;
extern char Init_New_GetDeviceID;
extern char Init_New_DirectSoundFullDuplexCreate;
extern char Init_New_DirectSoundCreate8;
extern char Init_New_DirectSoundCaptureCreate8;

// Point dsound.dll export pointers to init functions, since we can't call LoadLibrary from DLLMain
void *Original_DirectSoundCreate = &Init_New_DirectSoundCreate;
void *Original_DirectSoundEnumerateA = &Init_New_DirectSoundEnumerateA;
void *Original_DirectSoundEnumerateW = &Init_New_DirectSoundEnumerateW;
void *Original_DllCanUnloadNow = &Init_New_DllCanUnloadNow;
void *Original_DllGetClassObject = &Init_New_DllGetClassObject;
void *Original_DirectSoundCaptureCreate = &Init_New_DirectSoundCaptureCreate;
void *Original_DirectSoundCaptureEnumerateA = &Init_New_DirectSoundCaptureEnumerateA;
void *Original_DirectSoundCaptureEnumerateW = &Init_New_DirectSoundCaptureEnumerateW;
void *Original_GetDeviceID = &Init_New_GetDeviceID;
void *Original_DirectSoundFullDuplexCreate = &Init_New_DirectSoundFullDuplexCreate;
void *Original_DirectSoundCreate8 = &Init_New_DirectSoundCreate8;
void *Original_DirectSoundCaptureCreate8 = &Init_New_DirectSoundCaptureCreate8;

void LoadDLLModLoader(void)
{
	HMODULE mod_loader_hmodule = LoadLibrary("mods/mod_loader.dll");
	if (mod_loader_hmodule)
	{
		void (*ModInit)(void) = (void(*)(void))GetProcAddress(mod_loader_hmodule, "init");

		if (ModInit)
			ModInit();
	}
	else
	{
		MessageBox(NULL, "Could not find 'mods/mod_loader.dll'", "Mod Loader error", 0);
	}
}

__asm(
"_ASM_LoadDLLModLoader:\n"
"	call	_LoadDLLModLoader\n"
"	movl	(0x498B20),%eax\n"
"	ret\n"
);
extern char ASM_LoadDLLModLoader;

void LoadDSound(void)
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
}

BOOLEAN WINAPI DllMain(IN HINSTANCE hDllHandle, IN DWORD nReason, IN LPVOID Reserved)
{
	if (nReason == DLL_PROCESS_ATTACH)
	{
		if (!memcmp((void*)0x412429, (char[]){0xA1, 0x20, 0x8B, 0x49, 0x00}, 5))	// Make sure this is the right EXE (or that the code hasn't been tampered with)
		{
			// Write a call to ASM_LoadDLLModLoader to 0x412429
			const HANDLE handle = GetCurrentProcess();
			const unsigned int relative_address = (unsigned int)&ASM_LoadDLLModLoader - (0x412429 + 5);
			WriteProcessMemory(handle, (void*)0x412429, &(unsigned char){0xE8}, 1, NULL);
			WriteProcessMemory(handle, (void*)0x412429 + 1, &relative_address, 4, NULL);
		}
	}

	return TRUE;
}

__asm(
".globl _Init_New_DirectSoundCreate\n"
".globl _New_DirectSoundCreate\n"
"_Init_New_DirectSoundCreate:\n"
"	call	_LoadDSound\n"
"_New_DirectSoundCreate:\n"
"	jmp	*_Original_DirectSoundCreate\n"

".globl _Init_New_DirectSoundEnumerateA\n"
".globl _New_DirectSoundEnumerateA\n"
"_Init_New_DirectSoundEnumerateA:\n"
"	call	_LoadDSound\n"
"_New_DirectSoundEnumerateA:\n"
"	jmp	*_Original_DirectSoundEnumerateA\n"

".globl _Init_New_DirectSoundEnumerateW\n"
".globl _New_DirectSoundEnumerateW\n"
"_Init_New_DirectSoundEnumerateW:\n"
"	call	_LoadDSound\n"
"_New_DirectSoundEnumerateW:\n"
"	jmp	*_Original_DirectSoundEnumerateW\n"

".globl _Init_New_DllCanUnloadNow\n"
".globl _New_DllCanUnloadNow\n"
"_Init_New_DllCanUnloadNow:\n"
"	call	_LoadDSound\n"
"_New_DllCanUnloadNow:\n"
"	jmp	*_Original_DllCanUnloadNow\n"

".globl _Init_New_DllGetClassObject\n"
".globl _New_DllGetClassObject\n"
"_Init_New_DllGetClassObject:\n"
"	call	_LoadDSound\n"
"_New_DllGetClassObject:\n"
"	jmp	*_Original_DllGetClassObject\n"

".globl _Init_New_DirectSoundCaptureCreate\n"
".globl _New_DirectSoundCaptureCreate\n"
"_Init_New_DirectSoundCaptureCreate:\n"
"	call	_LoadDSound\n"
"_New_DirectSoundCaptureCreate:\n"
"	jmp	*_Original_DirectSoundCaptureCreate\n"

".globl _Init_New_DirectSoundCaptureEnumerateA\n"
".globl _New_DirectSoundCaptureEnumerateA\n"
"_Init_New_DirectSoundCaptureEnumerateA:\n"
"	call	_LoadDSound\n"
"_New_DirectSoundCaptureEnumerateA:\n"
"	jmp	*_Original_DirectSoundCaptureEnumerateA\n"

".globl _Init_New_DirectSoundCaptureEnumerateW\n"
".globl _New_DirectSoundCaptureEnumerateW\n"
"_Init_New_DirectSoundCaptureEnumerateW:\n"
"	call	_LoadDSound\n"
"_New_DirectSoundCaptureEnumerateW:\n"
"	jmp	*_Original_DirectSoundCaptureEnumerateW\n"

".globl _Init_New_GetDeviceID\n"
".globl _New_GetDeviceID\n"
"_Init_New_GetDeviceID:\n"
"	call	_LoadDSound\n"
"_New_GetDeviceID:\n"
"	jmp	*_Original_GetDeviceID\n"

".globl _Init_New_DirectSoundFullDuplexCreate\n"
".globl _New_DirectSoundFullDuplexCreate\n"
"_Init_New_DirectSoundFullDuplexCreate:\n"
"	call	_LoadDSound\n"
"_New_DirectSoundFullDuplexCreate:\n"
"	jmp	*_Original_DirectSoundFullDuplexCreate\n"

".globl _Init_New_DirectSoundCreate8\n"
".globl _New_DirectSoundCreate8\n"
"_Init_New_DirectSoundCreate8:\n"
"	call	_LoadDSound\n"
"_New_DirectSoundCreate8:\n"
"	jmp	*_Original_DirectSoundCreate8\n"

".globl _Init_New_DirectSoundCaptureCreate8\n"
".globl _New_DirectSoundCaptureCreate8\n"
"_Init_New_DirectSoundCaptureCreate8:\n"
"	call	_LoadDSound\n"
"_New_DirectSoundCaptureCreate8:\n"
"	jmp	*_Original_DirectSoundCaptureCreate8\n"

// Handles our exports, giving them their real names, and setting their ordinals
".section .drectve\n"
".ascii \" -export:DirectSoundCreate=New_DirectSoundCreate @1\"\n"
".ascii \" -export:DirectSoundEnumerateA=New_DirectSoundEnumerateA @2\"\n"
".ascii \" -export:DirectSoundEnumerateW=New_DirectSoundEnumerateW @3\"\n"
".ascii \" -export:DllCanUnloadNow=New_DllCanUnloadNow @4\"\n"
".ascii \" -export:DllGetClassObject=New_DllGetClassObject @5\"\n"
".ascii \" -export:DirectSoundCaptureCreate=New_DirectSoundCaptureCreate @6\"\n"
".ascii \" -export:DirectSoundCaptureEnumerateA=New_DirectSoundCaptureEnumerateA @7\"\n"
".ascii \" -export:DirectSoundCaptureEnumerateW=New_DirectSoundCaptureEnumerateW @8\"\n"
".ascii \" -export:GetDeviceID=New_GetDeviceID @9\"\n"
".ascii \" -export:DirectSoundFullDuplexCreate=New_DirectSoundFullDuplexCreate @10\"\n"
".ascii \" -export:DirectSoundCreate8=New_DirectSoundCreate8 @11\"\n"
".ascii \" -export:DirectSoundCaptureCreate8=New_DirectSoundCaptureCreate8 @12\"\n"
);
