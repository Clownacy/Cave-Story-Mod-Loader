// Mod loader for Freeware Cave Story
// Public domain

#pragma once

typedef struct MLHookCPURegisters
{
	// NOTE: PLEASE KEEP UP TO DATE WITH hooks.c MACHINECODE BLOB.
	// Keep in mind the reversed order due to the way push works
	unsigned int eflags, eip;
	// Last bunch of stuff is a PUSHAD
	unsigned int edi, esi, ebp, esp;
	unsigned int ebx, edx, ecx, eax; // not out of order
} MLHookCPURegisters;

// address, userdata, registers
// Return value is a boolean.
// If it's zero, then the "bi-stage" hook cleanup method occurs.
// If it's non-zero,
typedef int (*MLHookCallback)(MLHookCPURegisters *, void * cb);
