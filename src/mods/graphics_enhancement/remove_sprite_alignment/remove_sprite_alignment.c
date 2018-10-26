// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "remove_sprite_alignment.h"

#include <stdbool.h>
#include <stdlib.h>
#include <windows.h>

#include "cave_story.h"
#include "mod_loader.h"

#include "../common.h"

void DrawColourFill_RawXY(RECT *dst_rect, int colour)
{
	DDBLTFX ddbltfx;
	memset(&ddbltfx, 0, sizeof(ddbltfx));
	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwFillColor = colour;

	RECT final_dst_rect;
	final_dst_rect.left = (dst_rect->left * CS_window_upscale) / 0x200;
	final_dst_rect.top = (dst_rect->top * CS_window_upscale) / 0x200;
	final_dst_rect.right = (dst_rect->right * CS_window_upscale) / 0x200;
	final_dst_rect.bottom = (dst_rect->bottom * CS_window_upscale) / 0x200;

	CS_screen_surface->lpVtbl->Blt(CS_screen_surface, &final_dst_rect, 0, 0, 0x1000400, &ddbltfx);
}

static void DrawSprite_RawXY(RECT *clip_rect, int x, int y, RECT *src_rect, CS_SurfaceID surface_id, bool transparency)
{
	RECT new_clip_rect;
	RECT new_src_rect;

	new_clip_rect.left = clip_rect->left * CS_window_upscale;
	new_clip_rect.top = clip_rect->top * CS_window_upscale;
	new_clip_rect.right = clip_rect->right * CS_window_upscale;
	new_clip_rect.bottom = clip_rect->bottom * CS_window_upscale;

	new_src_rect.left = src_rect->left * CS_window_upscale;
	new_src_rect.top = src_rect->top * CS_window_upscale;
	new_src_rect.right = src_rect->right * CS_window_upscale;
	new_src_rect.bottom = src_rect->bottom * CS_window_upscale;

	RECT final_src_rect_1;
	RECT final_dst_rect_1;

//	x = (x * CS_window_upscale) / 512;
//	y = (y * CS_window_upscale) / 512;

	if (!remove_sprite_alignment)
	{
		x -= x % CS_window_upscale;
		y -= y % CS_window_upscale;
	}

	final_src_rect_1.left = new_src_rect.left;
	final_src_rect_1.top = new_src_rect.top;
	final_src_rect_1.right = new_src_rect.right;
	final_src_rect_1.bottom = new_src_rect.bottom;

	if ( new_src_rect.right + x - new_src_rect.left > new_clip_rect.right )
		final_src_rect_1.right -= new_src_rect.right + x - new_src_rect.left - new_clip_rect.right;

	if ( x < new_clip_rect.left )
	{
		final_src_rect_1.left += new_clip_rect.left - x;
		x = new_clip_rect.left;
	}

	if ( new_src_rect.bottom + y - new_src_rect.top > new_clip_rect.bottom )
		final_src_rect_1.bottom -= new_src_rect.bottom + y - new_src_rect.top - new_clip_rect.bottom;

	if ( y < new_clip_rect.top )
	{
		final_src_rect_1.top += new_clip_rect.top - y;
		y = new_clip_rect.top;
	}

	final_dst_rect_1.left = x;
	final_dst_rect_1.top = y;
	final_dst_rect_1.right = (final_src_rect_1.right + x - final_src_rect_1.left);
	final_dst_rect_1.bottom = (final_src_rect_1.bottom + y - final_src_rect_1.top);

	CS_screen_surface->lpVtbl->Blt(CS_screen_surface, &final_dst_rect_1, CS_surfaces[surface_id], &final_src_rect_1, transparency? 0x1008000 : 0x1000000, 0);
}

void DrawSpriteWithTransparency_RawXY(RECT *clip_rect, int x, int y, RECT *src_rect, CS_SurfaceID surface_id)
{
	DrawSprite_RawXY(clip_rect, x, y, src_rect, surface_id, true);
}

void DrawSpriteNoTransparency_RawXY(RECT *clip_rect, int x, int y, RECT *src_rect, CS_SurfaceID surface_id)
{
	DrawSprite_RawXY(clip_rect, x, y, src_rect, surface_id, false);
}

__asm(
"_BackgroundType1_Scroll_ASM:\n"
"	mov	0xC(%ebp), %eax\n"
"	push	%eax\n"
"	mov	0x8(%ebp), %eax\n"
"	push	%eax\n"
"	call	_BackgroundType1_Scroll\n"
"	add	$0x8, %esp\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x402805\n"
);
extern char BackgroundType1_Scroll_ASM;

void BackgroundType1_Scroll(int camera_x_pos, int camera_y_pos)
{
	for (int y = -(ConvertCoordMacro(camera_y_pos / 2) % ConvertPixelMacro(CS_background_tile_height)); y < ConvertPixelMacro(240); y += ConvertPixelMacro(CS_background_tile_height))
	{
		for (int x = -(ConvertCoordMacro(camera_x_pos / 2) % ConvertPixelMacro(CS_background_tile_width)); x < ConvertPixelMacro(SCREEN_WIDTH); x += ConvertPixelMacro(CS_background_tile_width))
		{
			DrawSpriteNoTransparency_RawXY(&CS_clip_rect_common, x, y, &(RECT){0, 0, CS_background_tile_width, CS_background_tile_height}, CS_SURFACE_ID_LEVEL_BACKGROUND);
		}
	}
}

__asm(
"_BackgroundType2_Scroll_ASM:\n"
"	mov	0xC(%ebp), %eax\n"
"	push	%eax\n"
"	mov	0x8(%ebp), %eax\n"
"	push	%eax\n"
"	call	_BackgroundType2_Scroll\n"
"	add	$0x8, %esp\n"
"	jmp	*1f\n"
"1:\n"
"	.long	0x402805\n"
);
extern char BackgroundType2_Scroll_ASM;

void BackgroundType2_Scroll(int camera_x_pos, int camera_y_pos)
{
	for (int y = -(ConvertCoordMacro(camera_y_pos) % ConvertPixelMacro(CS_background_tile_height)); y < ConvertPixelMacro(240); y += ConvertPixelMacro(CS_background_tile_height))
	{
		for (int x = -(ConvertCoordMacro(camera_x_pos) % ConvertPixelMacro(CS_background_tile_width)); x < ConvertPixelMacro(SCREEN_WIDTH); x += ConvertPixelMacro(CS_background_tile_width))
		{
			DrawSpriteNoTransparency_RawXY(&CS_clip_rect_common, x, y, &(RECT){0, 0, CS_background_tile_width, CS_background_tile_height}, CS_SURFACE_ID_LEVEL_BACKGROUND);
		}
	}
}

__asm(
"_DrawValueViewNewCode_ASM:\n"
"	addl	%eax, %ecx\n"
"	push	%eax\n"
"	mov	$4*0x200, %eax\n"
"	call	_ConvertCoordASM\n"
"	sub	%eax, %ecx\n"
"	pop	%eax\n"
"	ret\n"
);
extern char DrawValueViewNewCode_ASM;

__asm(
"_ConvertCoordASM:\n"
"	imul	0x48F914, %eax\n"	// CS_window_upscale
"	cdq\n"
"	and	$0x1FF, %edx\n"
"	add	%edx, %eax\n"
"	sar	$9, %eax\n"
"	ret\n"
);
extern char ConvertCoordASM;

static void PatchCommonCoordCode(void *address)
{
	ModLoader_WriteCall(address, &ConvertCoordASM);
	ModLoader_WriteNOPs(address + 5, 4);
	ModLoader_WriteNOPs(address + 11, 3);
	ModLoader_WriteCall(address + 17, &ConvertCoordASM);
	ModLoader_WriteNOPs(address + 22, 7);
}

__asm(
"_PatchCommonTileCoordCode:\n"
"	call	_ConvertCoordASM\n"
"	push	%eax\n"
"	mov	%ecx, %eax\n"
"	sal	$9, %eax\n"
"	call	_ConvertCoordASM\n"
"	mov	%eax, %ecx\n"
"	pop	%eax\n"
"	ret\n"
);
extern char PatchCommonTileCoordCode;

__asm(
"_PatchDrawBoss:\n"
"	movsx	-1(%ebp), %edx\n"
"	sal	$9, %edx\n"
"	push	%eax\n"
"	mov	%edx, %eax\n"
"	call	_ConvertCoordASM\n"
"	mov	%eax, %edx\n"
"	pop	%eax\n"
"	sub	%eax, %ecx\n"
"	add	%edx, %ecx\n"
"	ret\n"
);
extern char PatchDrawBoss;

__asm(
"_DrawValueViewNewCode2_ASM:\n"
"	sal	$8, %eax\n"
"	call	_ConvertCoordASM\n"
"	ret\n"
);
extern char DrawValueViewNewCode2_ASM;

void RemoveSpriteAlignment(void)
{
	// DrawWater
	PatchCommonCoordCode((void*)0x4028D2);

	ModLoader_WriteCall((void*)0x4028F6, &ConvertCoordASM);
	ModLoader_WriteWordBE((void*)0x4028F6 + 5, 0x03C8);	// add     ecx, eax
	ModLoader_WriteWordBE((void*)0x4028F6 + 7, 0x894D);	// mov     [ebp+y_pos], ecx
	ModLoader_WriteByte((void*)0x4028F6 + 9, 0xE8);
	ModLoader_WriteWordBE((void*)0x4028F6 + 10, 0x817D);	// cmp     [ebp+y_pos], -32h*200h
	ModLoader_WriteByte((void*)0x4028F6 + 12, 0xE8);
	ModLoader_WriteLong((void*)0x4028F6 + 13, ConvertPixelMacro(-32));
	ModLoader_WriteNOPs((void*)0x4028F6 + 17, 4);
	ModLoader_WriteLong((void*)0x402912, ConvertPixelMacro(240));

	PatchCommonCoordCode((void*)0x40293F);

	ModLoader_WriteRelativeAddress((void*)0x402974 + 1, DrawSpriteWithTransparency_RawXY);
	ModLoader_WriteRelativeAddress((void*)0x402995 + 1, DrawSpriteWithTransparency_RawXY);

	// DrawBullet
	PatchCommonCoordCode((void*)0x403F10);

	PatchCommonCoordCode((void*)0x403F33);

	ModLoader_WriteRelativeAddress((void*)0x403F58 + 1, DrawSpriteWithTransparency_RawXY);

	// GameLoop_IslandCrash
	ModLoader_WriteCall((void*)0x40DCED, &ConvertCoordASM);
	ModLoader_WriteByte((void*)0x40DCED + 5, 0x2D);	// sub
	ModLoader_WriteLong((void*)0x40DCED + 6, ConvertPixelMacro(0xC) - 0xC);
	ModLoader_WriteNOPs((void*)0x40DCED + 10, 2);

	ModLoader_WriteCall((void*)0x40DD00, &ConvertCoordASM);
	ModLoader_WriteByte((void*)0x40DD00 + 5, 0x2D);	// sub
	ModLoader_WriteLong((void*)0x40DD00 + 6, ConvertPixelMacro(20) - 20);
	ModLoader_WriteNOPs((void*)0x40DD00 + 10, 2);

	ModLoader_WriteRelativeAddress((void*)0x40DD14 + 1, DrawSpriteWithTransparency_RawXY);

	// DrawForegroundBack
	ModLoader_WriteCall((void*)0x413C0C, &PatchCommonTileCoordCode);
	ModLoader_WriteNOPs((void*)0x413C0C + 5, 7);

	ModLoader_WriteCall((void*)0x413C27, &PatchCommonTileCoordCode);
	ModLoader_WriteNOPs((void*)0x413C27 + 5, 7);

	ModLoader_WriteRelativeAddress((void*)0x413C3B + 1, DrawSpriteWithTransparency_RawXY);

	// DrawRushingWaterParticles
	ModLoader_WriteCall((void*)0x41406E, &PatchCommonTileCoordCode);
	ModLoader_WriteNOPs((void*)0x41406E + 5, 7);

	ModLoader_WriteCall((void*)0x414089, &PatchCommonTileCoordCode);
	ModLoader_WriteNOPs((void*)0x414089 + 5, 7);

	ModLoader_WriteRelativeAddress((void*)0x41409D + 1, DrawSpriteWithTransparency_RawXY);

	// DrawWhimsicalStar
	ModLoader_WriteCall((void*)0x42146F, &ConvertCoordASM);
	ModLoader_WriteByte((void*)0x42146F + 5, 0x2D);	// sub
	ModLoader_WriteLong((void*)0x42146F + 6, ConvertPixelMacro(4) - 4);
	ModLoader_WriteWordBE((void*)0x42146F + 10, 0x8BC8);	// mov     ecx, eax
	ModLoader_WriteNOPs((void*)0x42146F + 12, 2);

	ModLoader_WriteCall((void*)0x421480, &ConvertCoordASM);
	ModLoader_WriteNOPs((void*)0x421480 + 5, 7);

	ModLoader_WriteCall((void*)0x42149E, &ConvertCoordASM);
	ModLoader_WriteByte((void*)0x42149E + 5, 0x2D);	// sub
	ModLoader_WriteLong((void*)0x42149E + 6, ConvertPixelMacro(4) - 4);
	ModLoader_WriteWordBE((void*)0x42149E + 10, 0x8BC8);	// mov     ecx, eax
	ModLoader_WriteNOPs((void*)0x42149E + 12, 2);

	ModLoader_WriteCall((void*)0x4214AF, &ConvertCoordASM);
	ModLoader_WriteNOPs((void*)0x4214AF + 5, 7);

	ModLoader_WriteRelativeAddress((void*)0x4214C6 + 1, DrawSpriteWithTransparency_RawXY);

	// DrawBoss
	// Y
	ModLoader_WriteCall((void*)0x4728D0, &ConvertCoordASM);
	ModLoader_WriteNOPs((void*)0x4728D0 + 5, 4);
	ModLoader_WriteNOPs((void*)0x4728D0 + 0xB, 3);
	ModLoader_WriteCall((void*)0x4728E1, &ConvertCoordASM);
	ModLoader_WriteNOPs((void*)0x4728E1 + 5, 7);
	// X
	ModLoader_WriteCall((void*)0x472902, &ConvertCoordASM);
	ModLoader_WriteNOPs((void*)0x472902 + 5, 4);
	ModLoader_WriteNOPs((void*)0x47290D, 3);
	ModLoader_WriteCall((void*)0x472913, &ConvertCoordASM);
	ModLoader_WriteCall((void*)0x472913 + 5, &PatchDrawBoss);
	ModLoader_WriteNOPs((void*)0x472913 + 10, 10);

	ModLoader_WriteRelativeAddress((void*)0x47292D + 1, DrawSpriteWithTransparency_RawXY);

	// DrawParticles
	PatchCommonCoordCode((void*)0x40AC1E);

	PatchCommonCoordCode((void*)0x40AC58);

	ModLoader_WriteRelativeAddress((void*)0x40AC7D + 1, DrawSpriteWithTransparency_RawXY);

	// DrawValueView
	ModLoader_WriteCall((void*)0x42647D, &DrawValueViewNewCode2_ASM);

	ModLoader_WriteCall((void*)0x4264A2, &ConvertCoordASM);
	ModLoader_WriteNOPs((void*)0x4264A2 + 5, 4);
	ModLoader_WriteNOPs((void*)0x4264A2 + 11, 3);

	ModLoader_WriteCall((void*)0x4264BC, &ConvertCoordASM);
	ModLoader_WriteNOPs((void*)0x4264BC + 5, 6);
	ModLoader_WriteCall((void*)0x4264BC + 0xB, &DrawValueViewNewCode_ASM);

	ModLoader_WriteCall((void*)0x4264CF, &ConvertCoordASM);
	ModLoader_WriteNOPs((void*)0x4264CF + 5, 7);

	ModLoader_WriteCall((void*)0x4264EC, &ConvertCoordASM);
	ModLoader_WriteNOPs((void*)0x4264EC + 5, 4);
	ModLoader_WriteNOPs((void*)0x4264EC + 11, 3);

	ModLoader_WriteCall((void*)0x426500, &ConvertCoordASM);
	ModLoader_WriteNOPs((void*)0x426500 + 5, 7);

	ModLoader_WriteRelativeAddress((void*)0x426514 + 1, DrawSpriteWithTransparency_RawXY);

	// DrawPlayerAndWeapon
	ModLoader_WriteLong((void*)0x4152FF + 3, ConvertPixelMacro(-4));
	ModLoader_WriteLong((void*)0x41532D + 3, ConvertPixelMacro(4));

	PatchCommonCoordCode((void*)0x4153A7);

	PatchCommonCoordCode((void*)0x4153D5);

	ModLoader_WriteWordBE((void*)0x4153D5 + 23, 0x81E9);
	ModLoader_WriteLong((void*)0x4153D5 + 25, ConvertPixelMacro(8) - 8);

	ModLoader_WriteRelativeAddress((void*)0x4153FD + 1, DrawSpriteWithTransparency_RawXY);

	PatchCommonCoordCode((void*)0x415419);

	PatchCommonCoordCode((void*)0x415447);

	ModLoader_WriteRelativeAddress((void*)0x41546C + 1, DrawSpriteWithTransparency_RawXY);

	PatchCommonCoordCode((void*)0x4154E5);

	PatchCommonCoordCode((void*)0x415510);

	ModLoader_WriteRelativeAddress((void*)0x415535 + 1, DrawSpriteWithTransparency_RawXY);

	ModLoader_WriteCall((void*)0x4155CB, &ConvertCoordASM);
	ModLoader_WriteNOPs((void*)0x4155CB + 5, 4);
	ModLoader_WriteNOPs((void*)0x4155CB + 11, 3);

	ModLoader_WriteWordBE((void*)0x4155CB + 20, 0x81E9);
	ModLoader_WriteLong((void*)0x4155CB + 22, ConvertPixelMacro(0xC) - 0xC);
	ModLoader_WriteCall((void*)0x4155CB + 26, &ConvertCoordASM);
	ModLoader_WriteNOPs((void*)0x4155CB + 31, 1);

	ModLoader_WriteCall((void*)0x4155F3, &ConvertCoordASM);
	ModLoader_WriteNOPs((void*)0x4155F3 + 5, 4);
	ModLoader_WriteNOPs((void*)0x4155F3 + 11, 3);

	ModLoader_WriteWordBE((void*)0x4155F3 + 20, 0x81E9);
	ModLoader_WriteLong((void*)0x4155F3 + 22, ConvertPixelMacro(0xC) - 0xC);
	ModLoader_WriteCall((void*)0x4155F3 + 26, &ConvertCoordASM);
	ModLoader_WriteNOPs((void*)0x4155F3 + 31, 1);

	ModLoader_WriteRelativeAddress((void*)0x41561B + 1, DrawSpriteWithTransparency_RawXY);

	ModLoader_WriteCall((void*)0x415658, &ConvertCoordASM);
	ModLoader_WriteNOPs((void*)0x415658 + 5, 4);
	ModLoader_WriteNOPs((void*)0x415658 + 11, 3);

	ModLoader_WriteWordBE((void*)0x415658 + 20, 0x81E9);
	ModLoader_WriteLong((void*)0x415658 + 22, ConvertPixelMacro(0xC) - 0xC);
	ModLoader_WriteCall((void*)0x415658 + 26, &ConvertCoordASM);
	ModLoader_WriteNOPs((void*)0x415658 + 31, 1);

	ModLoader_WriteCall((void*)0x415680, &ConvertCoordASM);
	ModLoader_WriteNOPs((void*)0x415680 + 5, 4);
	ModLoader_WriteNOPs((void*)0x415680 + 11, 3);

	ModLoader_WriteWordBE((void*)0x415680 + 20, 0x81E9);
	ModLoader_WriteLong((void*)0x415680 + 22, ConvertPixelMacro(0xC) - 0xC);
	ModLoader_WriteCall((void*)0x415680 + 26, &ConvertCoordASM);
	ModLoader_WriteNOPs((void*)0x415680 + 31, 1);

	ModLoader_WriteRelativeAddress((void*)0x4156A8 + 1, DrawSpriteWithTransparency_RawXY);

	// DrawForegroundFront
	// The level
	ModLoader_WriteCall((void*)0x413DA1, &PatchCommonTileCoordCode);
	ModLoader_WriteNOPs((void*)0x413DA1 + 5, 7);

	ModLoader_WriteCall((void*)0x413DBC, &PatchCommonTileCoordCode);
	ModLoader_WriteNOPs((void*)0x413DBC + 5, 7);

	ModLoader_WriteRelativeAddress((void*)0x413DD0 + 1, DrawSpriteWithTransparency_RawXY);
	// Tile 67
	ModLoader_WriteCall((void*)0x413DF0, &PatchCommonTileCoordCode);
	ModLoader_WriteNOPs((void*)0x413DF0 + 5, 7);

	ModLoader_WriteCall((void*)0x413E0B, &PatchCommonTileCoordCode);
	ModLoader_WriteNOPs((void*)0x413E0B + 5, 7);

	ModLoader_WriteRelativeAddress((void*)0x413E1F + 1, DrawSpriteWithTransparency_RawXY);

	// DrawObjects
	ModLoader_WriteCall((void*)0x46F983, &ConvertCoordASM);
	ModLoader_WriteNOPs((void*)0x46F983 + 5, 4);
	ModLoader_WriteNOPs((void*)0x46F98E, 3);

	ModLoader_WriteCall((void*)0x46F994, &ConvertCoordASM);
	ModLoader_WriteNOPs((void*)0x46F994 + 5, 7);

	ModLoader_WriteCall((void*)0x46F9B5, &ConvertCoordASM);
	ModLoader_WriteNOPs((void*)0x46F9B5 + 5, 4);
	ModLoader_WriteNOPs((void*)0x46F9C0, 3);

	ModLoader_WriteCall((void*)0x46F9C6, &ConvertCoordASM);
	ModLoader_WriteCall((void*)0x46F9C6 + 5, &PatchDrawBoss);
	ModLoader_WriteNOPs((void*)0x46F9C6 + 10, 10);

	ModLoader_WriteRelativeAddress((void*)0x46F9E0 + 1, DrawSpriteWithTransparency_RawXY);

	// UpdateCamera
	// Patch out the camera bounds checks so they don't round to 0x200 (maybe investigate this)
	ModLoader_WriteNOPs((void*)0x40EED6, 12);

	ModLoader_WriteNOPs((void*)0x40EEF5, 12);

	// DrawBackground
	ModLoader_WriteLong((void*)0x402809 + 4, (unsigned int)&BackgroundType1_Scroll_ASM);
	ModLoader_WriteLong((void*)0x402809 + 8, (unsigned int)&BackgroundType2_Scroll_ASM);

	// DrawFlash
	ModLoader_WriteRelativeAddress((void*)0x40EE39 + 1, DrawColourFill_RawXY);
	ModLoader_WriteRelativeAddress((void*)0x40EE4D + 1, DrawColourFill_RawXY);

	// UpdateBossExplosion
	ModLoader_WriteNOPs((void*)0x40EB15, 0xC);
	ModLoader_WriteNOPs((void*)0x40EB32, 0xC);
	ModLoader_WriteNOPs((void*)0x40EB4F, 0xC);
	ModLoader_WriteNOPs((void*)0x40EB6C, 0xC);
	ModLoader_WriteNOPs((void*)0x40EC77, 0xC);
	ModLoader_WriteNOPs((void*)0x40ECA1, 0xC);
	ModLoader_WriteLong((void*)0x40ECB0 + 3, 240 * 0x200);
	ModLoader_WriteLong((void*)0x40ECB9 + 3, 240 * 0x200);
	ModLoader_WriteLong((void*)0x40ED03 + 6, SCREEN_WIDTH * 0x200);
	ModLoader_WriteLong((void*)0x40EB95 + 3, SCREEN_WIDTH * 0x200);
	ModLoader_WriteLong((void*)0x40EB9E + 3, SCREEN_WIDTH * 0x200);
	ModLoader_WriteLong((void*)0x40EBA5 + 3, 240 * 0x200);
	ModLoader_WriteLong((void*)0x40EBAE + 3, 240 * 0x200);
	ModLoader_WriteLong((void*)0x40EBD0 + 6, 240 * 0x200);
	ModLoader_WriteLong((void*)0x40EBE4 + 6, SCREEN_WIDTH * 0x200);

	// UpdateRapidScreenFlash
	ModLoader_WriteLong((void*)0x40ED7C + 6, 240 * 0x200);
	ModLoader_WriteLong((void*)0x40ED90 + 6, SCREEN_WIDTH * 0x200);
}
