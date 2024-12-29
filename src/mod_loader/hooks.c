// Mod loader for Freeware Cave Story
// Copyright © 2018 Clownacy, 20kdc

#include "hooks.h"
#include "patch.h"
#include "log.h"
#include <windows.h>

typedef struct HookTableSubEntry
{
	MLHookCallback cb;
	void * ud;
	struct HookTableSubEntry * next;
} HookTableSubEntry;

typedef struct HookTableEntry
{
	// Intercepted point A
	char * address;
	unsigned int length;
	char * hookspace; // Backup of code
	char * hookcore; // Hook setup/teardown code
	int stage2; // Boolean
	HookTableSubEntry * targets;
	struct HookTableEntry * next;
} HookTableEntry;

static HookTableEntry * ht_first_entry = 0;

// NOTE: PLEASE KEEP UP TO DATE WITH mod_loader_hook.h CPU REGISTERS STUFF.
static unsigned char ht_global_hookcore[] = {
 // Critical Offsets
#define HT_HOOKCORE_PUSHADDR 2
#define HT_HOOKCORE_NEXUSCALL 8
#define HT_HOOKCORE_JMP 20
#define HT_HOOKCORE_SIZE 25
 0x60, // 1
 0x68, 0xFF, 0xFF, 0xFF, 0x01, // 5
 0x9C, 0x54, // 2
 0xE8, 0xF2, 0xFF, 0xFF, 0x01, // 5
 0x9D, 0x58, 0x61, 0x8B, 0x64, 0x24, 0xEC, // 7
 0xE9, 0x00, 0x00, 0x00, 0x00 // 5
/*
bits 32

pushad
push dword 0x01FFFFFF ; push hook address as IP
pushfd
push esp ; points to CPU structure
call [0x01FFFFFF] ; call nexus offset
popfd
pop eax
popad
mov esp, [esp-20]
; final 5 bytes patched in by the Nexus
db 0xE9, 0x00, 0x00, 0x00, 0x00
*/
};

static HookTableEntry * GetStackableHook(void * address, int remove) {
	HookTableEntry ** hte = &ht_first_entry;
	while (1) {
		HookTableEntry * current = *hte;
		if (!current)
			return 0;
		if (current->address == address) {
			if (remove)
				*hte = current->next;
			return current;
		}
		hte = &((*hte)->next);
	}
}

static int HookChain(HookTableSubEntry * target, MLHookCPURegisters * cpuData) {
	while (target) {
		if (target->cb(cpuData, target->ud))
			return 1;
		target = target->next;
	}
	return 0;
}

static void __stdcall HookNexus(MLHookCPURegisters * cpuData) {
	// We are currently running from within the game.
	HookTableEntry * hte = GetStackableHook((void *) (cpuData->eip), 0);
	if (!hte) {
		PrintMessageBoxError("Hook called but did not exist @ 0x%08x, cannot continue", cpuData->eip);
		ExitProcess(1);
		// must not be reachable
	}
	// Wipe the slate
	for (unsigned int i = 0; i < hte->length + 5; i++)
		WriteByte(hte->address + i, hte->hookspace[i]);
	if (!hte->stage2) {
		if (HookChain(hte->targets, cpuData)) {
			// Override default behavior entirely
			WriteJump(hte->address, hte->hookcore);
			WriteJump(hte->hookcore + HT_HOOKCORE_JMP, (void *) (cpuData->eip));
			hte->stage2 = 0;
			return;
		}
	}
	if (!hte->stage2) {
		WriteJump(hte->address + hte->length, hte->hookcore);
		WriteJump(hte->hookcore + HT_HOOKCORE_JMP, hte->address);
		hte->stage2 = 1;
	} else {
		WriteJump(hte->address, hte->hookcore);
		WriteJump(hte->hookcore + HT_HOOKCORE_JMP, hte->address + hte->length);
		hte->stage2 = 0;
	}
}

static HookTableEntry * PrependNewStackableHook(void * address, unsigned int length) {
	PrintPollution("Stackable Hook creation: %p length %d\n", address, length);
	char * hookcore = malloc(HT_HOOKCORE_SIZE);
	if (!hookcore)
		PrintDebug("The murderer was malloc hookcore");
	DWORD ign;
	VirtualProtect(hookcore, HT_HOOKCORE_SIZE, PAGE_EXECUTE_READWRITE, &ign);
	for (unsigned int i = 0; i < HT_HOOKCORE_SIZE; i++)
		hookcore[i] = ht_global_hookcore[i];
	WriteLong(hookcore + HT_HOOKCORE_PUSHADDR, (unsigned int) address);
	WriteCall(hookcore + HT_HOOKCORE_NEXUSCALL, HookNexus);
	// use a more conventional method for something that isn't madness
	HookTableEntry * hte = (HookTableEntry *) malloc(sizeof(HookTableEntry));
	if (!hte)
		PrintDebug("The murderer was malloc hte");
	char * hookspace = malloc(length + 5);
	if (!hookspace)
		PrintDebug("The murderer was malloc hookspace");
	for (unsigned int i = 0; i < length + 5; i++)
		hookspace[i] = ((char *) address)[i];
	hte->address = address;
	hte->length = length;
	hte->hookspace = hookspace;
	hte->hookcore = hookcore;
	hte->stage2 = 0;
	hte->targets = 0;
	hte->next = ht_first_entry;
	ht_first_entry = hte;
	// Finally, setup stage1
	WriteJump(address, (void *) hookcore);
	PrintPollution("Stackable Hook creation complete\n");
	return hte;
}

void AddStackableHook(void * address, unsigned int length, MLHookCallback cb, void * ud) {
	HookTableEntry * hte = GetStackableHook(address, 0);
	if (!hte)
		hte = PrependNewStackableHook(address, length);
	// we're apparently allowed to assume malloc never fails, so let's take advantage of that
	HookTableSubEntry * htse = (HookTableSubEntry *) malloc(sizeof(HookTableSubEntry));
	htse->cb = cb;
	htse->ud = ud;
	htse->next = hte->targets;
	hte->targets = htse;
}
