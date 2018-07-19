// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2018 Clownacy

#include "patch_beetle_and_basu.h"

#include "cave_story.h"
#include "mod_loader.h"

#include "../common.h"

void SpawnPoof(int x, int y)
{
	CS_CreateDustClouds(x, y, 0, 2);
}

__asm(
"_SpawnPoof_ASM:\n"
"	mov	0xC(%eax),%ecx\n"
"	push	%ecx\n"
"	mov	8(%eax),%ecx\n"
"	push	%ecx\n"
"	call	_SpawnPoof\n"
"	add	$8,%esp\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x4281B6\n"
);
extern char SpawnPoof_ASM;

__asm(
"_SpawnPoof2_ASM:\n"
"	mov	0xC(%edx),%ecx\n"
"	push	%ecx\n"
"	mov	8(%edx),%ecx\n"
"	push	%ecx\n"
"	call	_SpawnPoof\n"
"	add	$8,%esp\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x4321E7\n"
);
extern char SpawnPoof2_ASM;

__asm(
"_SpawnPoof3_ASM:\n"
"	mov	0xC(%eax),%ecx\n"
"	push	%ecx\n"
"	mov	8(%eax),%ecx\n"
"	push	%ecx\n"
"	call	_SpawnPoof\n"
"	add	$8,%esp\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x4506B6\n"
);
extern char SpawnPoof3_ASM;

__asm(
"_SpawnPoof4_ASM:\n"
"	mov	0xC(%edx),%ecx\n"
"	push	%ecx\n"
"	mov	8(%edx),%ecx\n"
"	push	%ecx\n"
"	call	_SpawnPoof\n"
"	add	$8,%esp\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x4506B6\n"
);
extern char SpawnPoof4_ASM;

void PatchBeetleAndBasu(void)
{
	// Beetle (object 8)
	ModLoader_WriteJump((void*)0x428087, &SpawnPoof_ASM);
	// Basu (object 58)
	ModLoader_WriteJump((void*)0x431DC4, &SpawnPoof2_ASM);
	// Beetle2 (object 210)
	ModLoader_WriteJump((void*)0x450587, &SpawnPoof3_ASM);
	// Basu2 (object 208)
	ModLoader_WriteJump((void*)0x44FE54, &SpawnPoof4_ASM);
}
