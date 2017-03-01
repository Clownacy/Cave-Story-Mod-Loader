#include <ddraw.h>
#include <stdbool.h>
#include <windows.h>

#include "mod_loader.h"

#include "../common.h"
#include "sprite_resolution.h"

int sprite_resolution_factor;

// LoadBackgroundSprite actually figures out the size of the image
// and tiles it accordingly. But since we're working with double-res,
// we need it to tiles these at half-size.
void LoadBackgroundSprite_hijack(char* filename, int something)
{
	int *background_tile_width = (int*)0x499C78;
	int *background_tile_height = (int*)0x499C7C;
	void (*LoadBackgroundSprite)(char*, int) = (void(*)(char*,int))0x402270;

	LoadBackgroundSprite(filename, something);

	*background_tile_width /= sprite_resolution_factor;
	*background_tile_height /= sprite_resolution_factor;
}

__asm (
"_LoadBMP_FromFile_SurfaceCreationHijack:\n"
"	pushl	%eax\n"
"	movl	-0x8C(%ebp), %eax\n"	// Divide surface size by 2
"	cdq\n"
"	movl	_sprite_resolution_factor, %ecx\n"
"	divl	%ecx\n"
"	movl	%eax, -0x8C(%ebp)\n"
"	movl	-0x90(%ebp), %eax\n"
"	cdq\n"
"	movl	_sprite_resolution_factor, %ecx\n"
"	divl	%ecx\n"
"	movl	%eax, -0x90(%ebp)\n"
"	popl	%eax\n"
"	call	*0x18(%eax)\n"
"	movl	$0, -0x10(%ebp)\n"
"	jmp	0x40BC35\n"
);
void LoadBMP_FromFile_SurfaceCreationHijack(void);

__asm (
"_LoadBMP_FromFile_StretchBlitHijack:\n"
"	movl	0x10(%esp), %eax\n"
"	cdq\n"
"	movl	_sprite_resolution_factor, %ecx\n"
"	divl	%ecx\n"
"	movl	%eax, 0x10(%esp)\n"
"	movl	0x14(%esp), %eax\n"
"	cdq\n"
"	movl	_sprite_resolution_factor, %ecx\n"
"	divl	%ecx\n"
"	movl	%eax, 0x14(%esp)\n"
"	jmp	*0x48C018\n"
);
void LoadBMP_FromFile_StretchBlitHijack(void);

void SetSpriteResolution(const int factor)
{
	sprite_resolution_factor = factor;

	// Fix background tile sizes
	WriteRelativeAddress(0x420DAA + 1, LoadBackgroundSprite_hijack);

	WriteJump(0x40BC2B, LoadBMP_FromFile_SurfaceCreationHijack);
	WriteCall(0x40BD04, LoadBMP_FromFile_StretchBlitHijack);
	WriteByte(0x40BD04 + 5, 0x90);
	WriteCall(0x40C142, LoadBMP_FromFile_StretchBlitHijack);
	WriteByte(0x40C142 + 5, 0x90);
}
