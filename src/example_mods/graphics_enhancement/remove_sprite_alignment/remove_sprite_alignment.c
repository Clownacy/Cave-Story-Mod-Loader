// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "remove_sprite_alignment.h"

#include <stdbool.h>
#include <stdlib.h>
#include <windows.h>

#include "cave_story.h"
#include "mod_loader.h"

#include "../common.h"

static void DrawColourFill_RawXY(RECT *dst_rect, int colour)
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

static void DrawSprite_RawXY(RECT *clip_rect, int x, int y, RECT *src_rect, SurfaceID surface_id, bool transparency)
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

	x = (x * CS_window_upscale) / 512;
	y = (y * CS_window_upscale) / 512;

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

static void DrawSpriteWithTransparency_RawXY(RECT *clip_rect, int x, int y, RECT *src_rect, SurfaceID surface_id)
{
	DrawSprite_RawXY(clip_rect, x, y, src_rect, surface_id, true);
}

static void DrawSpriteNoTransparency_RawXY(RECT *clip_rect, int x, int y, RECT *src_rect, SurfaceID surface_id)
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
	for (int y = -((camera_y_pos / 2) % (CS_background_tile_height * 0x200)); y < 240 * 0x200; y += CS_background_tile_height * 0x200)
	{
		for (int x = -((camera_x_pos / 2) % (CS_background_tile_width * 0x200)); x < SCREEN_WIDTH * 0x200; x += CS_background_tile_width * 0x200)
		{
			DrawSpriteNoTransparency_RawXY(&CS_clip_rect_common, x, y, &(RECT){0, 0, CS_background_tile_width, CS_background_tile_height}, SURFACE_ID_LEVEL_BACKGROUND);
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
	for (int y = -(camera_y_pos % (CS_background_tile_height * 0x200)); y < 240 * 0x200; y += CS_background_tile_height * 0x200)
	{
		for (int x = -(camera_x_pos % (CS_background_tile_width * 0x200)); x < SCREEN_WIDTH * 0x200; x += CS_background_tile_width * 0x200)
		{
			DrawSpriteNoTransparency_RawXY(&CS_clip_rect_common, x, y, &(RECT){0, 0, CS_background_tile_width, CS_background_tile_height}, SURFACE_ID_LEVEL_BACKGROUND);
		}
	}
}

__asm(
"_DrawValueViewNewCode_ASM:\n"
"	addl	%eax, %ecx\n"
"	subl	$0x200*4, %ecx\n"
"	ret\n"
);
extern char DrawValueViewNewCode_ASM;

__asm(
"_DrawValueViewNewCode2_ASM:\n"
"	sal	$8, %eax\n"
"	ret\n"
);
extern char DrawValueViewNewCode2_ASM;

void RemoveSpriteAlignment(void)
{
	// DrawWater
	WriteNOPs((void*)0x4028D2, 9);
	WriteNOPs((void*)0x4028D2 + 11, 3);
	WriteNOPs((void*)0x4028D2 + 17, 12);

	WriteWordBE((void*)0x4028F6, 0x03C8);
	WriteWordBE((void*)0x4028F6 + 2, 0x894D);
	WriteByte((void*)0x4028F6 + 4, 0xE8);
	WriteWordBE((void*)0x4028F6 + 5, 0x817D);
	WriteByte((void*)0x4028F6 + 7, 0xE8);
	WriteLong((void*)0x4028F6 + 8, -32 * 512);
	WriteNOPs((void*)0x4028F6 + 0xC, 9);
	WriteLong((void*)0x402912, 240 * 512);

	WriteNOPs((void*)0x40293F, 9);
	WriteNOPs((void*)0x40293F + 11, 3);
	WriteNOPs((void*)0x40293F + 17, 12);

	WriteRelativeAddress((void*)0x402974 + 1, DrawSpriteWithTransparency_RawXY);
	WriteRelativeAddress((void*)0x402995 + 1, DrawSpriteWithTransparency_RawXY);

	// DrawBullet
	WriteNOPs((void*)0x403F10, 9);
	WriteNOPs((void*)0x403F10 + 11, 3);
	WriteNOPs((void*)0x403F10 + 17, 12);

	WriteNOPs((void*)0x403F33, 9);
	WriteNOPs((void*)0x403F33 + 11, 3);
	WriteNOPs((void*)0x403F33 + 17, 12);

	WriteRelativeAddress((void*)0x403F58 + 1, DrawSpriteWithTransparency_RawXY);

	// GameLoop_IslandCrash
	WriteByte((void*)0x40DCED, 0x2D);
	WriteLong((void*)0x40DCED + 1, (0xC * 512) - 0xC);
	WriteNOPs((void*)0x40DCED + 5, 7);

	WriteByte((void*)0x40DD00, 0x2D);
	WriteLong((void*)0x40DD00 + 1, (20 * 512) - 20);
	WriteNOPs((void*)0x40DD00 + 5, 7);

	WriteRelativeAddress((void*)0x40DD14 + 1, DrawSpriteWithTransparency_RawXY);

	// DrawForegroundBack
	WriteWordBE((void*)0x413C0C, 0x69C9);
	WriteLongBE((void*)0x413C0C + 2, 0x00020000);
	WriteLong((void*)0x413C0C + 6, 0x90909090);
	WriteWord((void*)0x413C0C + 10, 0x9090);

	WriteWordBE((void*)0x413C27, 0x69C9);
	WriteLongBE((void*)0x413C27 + 2, 0x00020000);
	WriteLong((void*)0x413C27 + 6, 0x90909090);
	WriteWord((void*)0x413C27 + 10, 0x9090);

	WriteRelativeAddress((void*)0x413C3B + 1, DrawSpriteWithTransparency_RawXY);

	// DrawRushingWaterParticles
	WriteWordBE((void*)0x41406E, 0x69C9);
	WriteLongBE((void*)0x41406E + 2, 0x00020000);
	WriteLong((void*)0x41406E + 6, 0x90909090);
	WriteWord((void*)0x41406E + 10, 0x9090);

	WriteWordBE((void*)0x414089, 0x69C9);
	WriteLongBE((void*)0x414089 + 2, 0x00020000);
	WriteLong((void*)0x414089 + 6, 0x90909090);
	WriteWord((void*)0x414089 + 10, 0x9090);

	WriteRelativeAddress((void*)0x41409D + 1, DrawSpriteWithTransparency_RawXY);

	// DrawWhimsicalStar
	WriteByte((void*)0x42146F, 0x2D);
	WriteLong((void*)0x42146F + 1, (4 * 512) - 4);
	WriteNOPs((void*)0x42146F + 5, 4);

	WriteNOPs((void*)0x42146F + 0xB, 3);

	WriteNOPs((void*)0x421480, 0xC);

	WriteByte((void*)0x42149E, 0x2D);
	WriteLong((void*)0x42149E + 1, (4 * 512) - 4);
	WriteNOPs((void*)0x42149E + 5, 4);

	WriteNOPs((void*)0x42149E + 0xB, 3);

	WriteNOPs((void*)0x4214AF, 0xC);

	WriteRelativeAddress((void*)0x4214C6 + 1, DrawSpriteWithTransparency_RawXY);

	// DrawBoss
	// Y
	WriteNOPs((void*)0x4728D0, 9);
	WriteNOPs((void*)0x4728D0 + 0xB, 3);
	WriteNOPs((void*)0x4728E1, 0xC);
	// X
	WriteNOPs((void*)0x472902, 9);
	WriteNOPs((void*)0x47290D, 3);
	WriteLongBE((void*)0x472913, 0x0FBE55FF);
	WriteLongBE((void*)0x472913 + 4, 0x69D20002);
	WriteWordBE((void*)0x472913 + 8, 0x0000);
	WriteNOPs((void*)0x472913 + 10, 2);
	WriteNOPs((void*)0x472921, 4);

	WriteRelativeAddress((void*)0x47292D + 1, DrawSpriteWithTransparency_RawXY);

	// DrawParticles
	WriteNOPs((void*)0x40AC1E, 9);
	WriteNOPs((void*)0x40AC1E + 11, 3);
	WriteNOPs((void*)0x40AC1E + 17, 12);

	WriteNOPs((void*)0x40AC58, 9);
	WriteNOPs((void*)0x40AC58 + 11, 3);
	WriteNOPs((void*)0x40AC58 + 17, 12);

	WriteRelativeAddress((void*)0x40AC7D + 1, DrawSpriteWithTransparency_RawXY);

	// DrawValueView
	WriteCall((void*)0x42647D, &DrawValueViewNewCode2_ASM);

	WriteNOPs((void*)0x4264A2, 9);
	WriteNOPs((void*)0x4264A2 + 11, 3);
	WriteNOPs((void*)0x4264A2 + 0x1A, 11);
	WriteCall((void*)0x4264A2 + 0x1A + 11, &DrawValueViewNewCode_ASM);
	WriteNOPs((void*)0x4264CF, 12);

	WriteNOPs((void*)0x4264EC, 9);
	WriteNOPs((void*)0x4264EC + 11, 3);
	WriteNOPs((void*)0x4264EC + 0x14, 12);

	WriteRelativeAddress((void*)0x426514 + 1, DrawSpriteWithTransparency_RawXY);

	// DrawPlayerAndWeapon
	WriteLong((void*)0x4152FF + 3, -4 * 512);
	WriteLong((void*)0x41532D + 3, 4 * 512);

	WriteNOPs((void*)0x4153A7, 9);
	WriteNOPs((void*)0x4153A7 + 11, 3);
	WriteNOPs((void*)0x4153A7 + 17, 12);

	WriteNOPs((void*)0x4153D5, 9);
	WriteNOPs((void*)0x4153D5 + 11, 3);
	WriteNOPs((void*)0x4153D5 + 17, 6);
	WriteWordBE((void*)0x4153D5 + 23, 0x81E9);
	WriteLong((void*)0x4153D5 + 25, (8 * 512) - 8);

	WriteRelativeAddress((void*)0x4153FD + 1, DrawSpriteWithTransparency_RawXY);

	WriteNOPs((void*)0x415419, 9);
	WriteNOPs((void*)0x415419 + 11, 3);
	WriteNOPs((void*)0x415419 + 17, 12);

	WriteNOPs((void*)0x415447, 9);
	WriteNOPs((void*)0x415447 + 11, 3);
	WriteNOPs((void*)0x415447 + 17, 12);

	WriteRelativeAddress((void*)0x41546C + 1, DrawSpriteWithTransparency_RawXY);

	WriteNOPs((void*)0x4154E5, 9);
	WriteNOPs((void*)0x4154E5 + 11, 3);
	WriteNOPs((void*)0x4154E5 + 17, 12);

	WriteNOPs((void*)0x415510, 9);
	WriteNOPs((void*)0x415510 + 11, 3);
	WriteNOPs((void*)0x415510 + 17, 12);

	WriteRelativeAddress((void*)0x415535 + 1, DrawSpriteWithTransparency_RawXY);

	WriteNOPs((void*)0x4155CB, 9);
	WriteNOPs((void*)0x4155CB + 11, 3);
	WriteWordBE((void*)0x4155CB + 20, 0x81E9);
	WriteLong((void*)0x4155CB + 22, (0xC * 512) - 0xC);
	WriteNOPs((void*)0x4155CB + 26, 6);

	WriteNOPs((void*)0x4155F3, 9);
	WriteNOPs((void*)0x4155F3 + 11, 3);
	WriteWordBE((void*)0x4155F3 + 20, 0x81E9);
	WriteLong((void*)0x4155F3 + 22, (0xC * 512) - 0xC);
	WriteNOPs((void*)0x4155F3 + 26, 6);

	WriteRelativeAddress((void*)0x41561B + 1, DrawSpriteWithTransparency_RawXY);

	WriteNOPs((void*)0x415658, 9);
	WriteNOPs((void*)0x415658 + 11, 3);
	WriteWordBE((void*)0x415658 + 20, 0x81E9);
	WriteLong((void*)0x415658 + 22, (0xC * 512) - 0xC);
	WriteNOPs((void*)0x415658 + 26, 6);

	WriteNOPs((void*)0x415680, 9);
	WriteNOPs((void*)0x415680 + 11, 3);
	WriteWordBE((void*)0x415680 + 20, 0x81E9);
	WriteLong((void*)0x415680 + 22, (0xC * 512) - 0xC);
	WriteNOPs((void*)0x415680 + 26, 6);

	WriteRelativeAddress((void*)0x4156A8 + 1, DrawSpriteWithTransparency_RawXY);

	// DrawForegroundFront
	// The level
	WriteWordBE((void*)0x413DA1, 0x69C9);
	WriteLong((void*)0x413DA1 + 2, 0x200);
	WriteNOPs((void*)0x413DA1 + 6, 6);

	WriteWordBE((void*)0x413DBC, 0x69C9);
	WriteLong((void*)0x413DBC + 2, 0x200);
	WriteNOPs((void*)0x413DBC + 6, 6);

	WriteRelativeAddress((void*)0x413DD0 + 1, DrawSpriteWithTransparency_RawXY);
	// Tile 67
	WriteWordBE((void*)0x413DF0, 0x69C9);
	WriteLong((void*)0x413DF0 + 2, 0x200);
	WriteNOPs((void*)0x413DF0 + 6, 6);

	WriteWordBE((void*)0x413E0B, 0x69C9);
	WriteLong((void*)0x413E0B + 2, 0x200);
	WriteNOPs((void*)0x413E0B + 6, 6);

	WriteRelativeAddress((void*)0x413E1F + 1, DrawSpriteWithTransparency_RawXY);

	// DrawObjects
	WriteLong((void*)0x46F983, 0x90909090);
	WriteLong((void*)0x46F987, 0x90909090);
	WriteByte((void*)0x46F98B, 0x90);

	WriteWord((void*)0x46F98E, 0x9090);
	WriteByte((void*)0x46F990, 0x90);

	WriteLong((void*)0x46F994, 0x90909090);
	WriteLong((void*)0x46F998, 0x90909090);
	WriteLong((void*)0x46F99C, 0x90909090);

	WriteLong((void*)0x46F9B5, 0x90909090);
	WriteLong((void*)0x46F9B5 + 4, 0x90909090);
	WriteByte((void*)0x46F9B5 + 8, 0x90);

	WriteWord((void*)0x46F9C0, 0x9090);
	WriteByte((void*)0x46F9C0 + 2, 0x90);

	WriteLongBE((void*)0x46F9C6, 0x0FBE55FF);
	WriteLongBE((void*)0x46F9C6 + 4, 0x69D20002);
	WriteWordBE((void*)0x46F9C6 + 8, 0x0000);
	WriteWord((void*)0x46F9C6 + 10, 0x9090);
	WriteLong((void*)0x46F9D4, 0x90909090);

	WriteRelativeAddress((void*)0x46F9E0 + 1, DrawSpriteWithTransparency_RawXY);

	// UpdateCamera
	// Patch out the camera bounds checks so they don't round to 0x200
	WriteLong((void*)0x40EED6, 0x90909090);
	WriteLong((void*)0x40EEDA, 0x90909090);
	WriteLong((void*)0x40EEDE, 0x90909090);

	WriteLong((void*)0x40EEF5, 0x90909090);
	WriteLong((void*)0x40EEF9, 0x90909090);
	WriteLong((void*)0x40EEFD, 0x90909090);

	// DrawBackground
	WriteLong((void*)0x402809 + 4, (unsigned int)&BackgroundType1_Scroll_ASM);
	WriteLong((void*)0x402809 + 8, (unsigned int)&BackgroundType2_Scroll_ASM);

	// DrawFlash
	WriteRelativeAddress((void*)0x40EE39 + 1, DrawColourFill_RawXY);
	WriteRelativeAddress((void*)0x40EE4D + 1, DrawColourFill_RawXY);

	// UpdateBossExplosion
	WriteNOPs((void*)0x40EB15, 0xC);
	WriteNOPs((void*)0x40EB32, 0xC);
	WriteNOPs((void*)0x40EB4F, 0xC);
	WriteNOPs((void*)0x40EB6C, 0xC);
	WriteNOPs((void*)0x40EC77, 0xC);
	WriteNOPs((void*)0x40ECA1, 0xC);
	WriteLong((void*)0x40ECB0 + 3, 240 * 0x200);
	WriteLong((void*)0x40ECB9 + 3, 240 * 0x200);
	WriteLong((void*)0x40ED03 + 6, 320 * 0x200);
	WriteLong((void*)0x40EB95 + 3, 320 * 0x200);
	WriteLong((void*)0x40EB9E + 3, 320 * 0x200);
	WriteLong((void*)0x40EBA5 + 3, 240 * 0x200);
	WriteLong((void*)0x40EBAE + 3, 240 * 0x200);
	WriteLong((void*)0x40EBD0 + 6, 240 * 0x200); 
	WriteLong((void*)0x40EBE4 + 6, 320 * 0x200); 
}
