// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "sprite_resolution.h"

#include <stdbool.h>
#include <windows.h>

#include "cave_story.h"
#include "mod_loader.h"

#include "../common.h"

// LoadBackgroundSprite actually figures out the size of the image
// and tiles it accordingly. But since we're working with double-res,
// we need it to tile these at half-size.
static void LoadBackgroundSprite_hijack(char* filename, int something)
{
	CS_LoadBackgroundSprite(filename, something);

	CS_background_tile_width /= sprite_resolution_factor;
	CS_background_tile_height /= sprite_resolution_factor;
}

__asm (
"_LoadBMP_FromFile_SurfaceCreationHijack:\n"
"	pushl	%eax\n"

"	movl	_sprite_resolution_factor, %ecx\n"
"	movl	-0x8C(%ebp), %eax\n"
"	cdq\n"
"	divl	%ecx\n"
"	movl	%eax, -0x8C(%ebp)\n"
"	movl	-0x90(%ebp), %eax\n"
"	cdq\n"
"	divl	%ecx\n"
"	movl	%eax, -0x90(%ebp)\n"

"	popl	%eax\n"
"	call	*0x18(%eax)\n"
"	movl	$0, -0x10(%ebp)\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x40BC35\n"
);
extern char LoadBMP_FromFile_SurfaceCreationHijack;

__asm (
"_LoadBMP_StretchBlitHijack:\n"
"	movl	_sprite_resolution_factor, %ecx\n"
"	movl	0x10(%esp), %eax\n"
"	cdq\n"
"	divl	%ecx\n"
"	movl	%eax, 0x10(%esp)\n"
"	movl	0x14(%esp), %eax\n"
"	cdq\n"
"	divl	%ecx\n"
"	movl	%eax, 0x14(%esp)\n"
"	pushl	0x48C018\n"
"	ret\n"
);
extern char LoadBMP_StretchBlitHijack;

__asm (
"_LoadBMP_FromResource_SurfaceCreationHijack:\n"
"	pushl	%edx\n"

"	movl	_sprite_resolution_factor, %ecx\n"
"	movl	-0x8C(%ebp), %eax\n"
"	cdq\n"
"	divl	%ecx\n"
"	movl	%eax, -0x8C(%ebp)\n"
"	movl	-0x90(%ebp), %eax\n"
"	cdq\n"
"	divl	%ecx\n"
"	movl	%eax, -0x90(%ebp)\n"

"	popl	%edx\n"
"	call	*0x18(%edx)\n"
"	test	%eax, %eax\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x40B8E6\n"
);
extern char LoadBMP_FromResource_SurfaceCreationHijack;

void SetSpriteResolution(void)
{
	ModLoader_PrintDebug("Applying 'set sprite resolution' patch\n");

	// Fix background tile sizes
	ModLoader_WriteRelativeAddress((void*)0x420DAA + 1, LoadBackgroundSprite_hijack);

	ModLoader_WriteJump((void*)0x40BC2B, &LoadBMP_FromFile_SurfaceCreationHijack);
	ModLoader_WriteCall((void*)0x40BD04, &LoadBMP_StretchBlitHijack);
	ModLoader_WriteByte((void*)0x40BD04 + 5, 0x90);
	ModLoader_WriteCall((void*)0x40C142, &LoadBMP_StretchBlitHijack);
	ModLoader_WriteByte((void*)0x40C142 + 5, 0x90);

	ModLoader_WriteJump((void*)0x40B8E1, &LoadBMP_FromResource_SurfaceCreationHijack);
	ModLoader_WriteCall((void*)0x40B9C5, &LoadBMP_StretchBlitHijack);
	ModLoader_WriteByte((void*)0x40B9C5 + 5, 0x90);
	ModLoader_WriteCall((void*)0x40BF0A, &LoadBMP_StretchBlitHijack);
	ModLoader_WriteByte((void*)0x40BF0A + 5, 0x90);
}
