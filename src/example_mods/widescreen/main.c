#include <stdbool.h>
#include <stdio.h>

#include "mod_loader.h"

#define ASPECT_RATIO_X 21
#define ASPECT_RATIO_Y 9

const int native_width = (240. / ASPECT_RATIO_Y) * ASPECT_RATIO_X;

typedef struct DrawRectangle {
	int left;
	int up;
	int right;
	int down;
} DrawRectangle;

void __stdcall ScrollClouds(const int scroll_type)
{
	void (*DrawSprite2)(DrawRectangle*, int, int, DrawRectangle*, int) = (void(*)(DrawRectangle*, int, int, DrawRectangle*, int))0x40C5B0;
	int *dword_499C8C = (int*)0x499C8C;

	DrawRectangle clip_rect = {0, 0, native_width, 240};
	DrawRectangle src_rect = {0, 0, 320, 88};
	int sprite_width = (src_rect.right - src_rect.left);
	DrawSprite2(&clip_rect, 0, src_rect.up, &src_rect, 28);

	if (scroll_type == 6)
	{
		// bkMoon
		src_rect.left = 0;
		src_rect.right = 240;
	}
	else// if (scroll_type == 6)
	{
		// bkFog
		src_rect.left = 108;
		src_rect.right = 320;
	}

	sprite_width = (src_rect.right - src_rect.left);
	for (int i=0; i <= ((native_width - 320) / sprite_width); ++i)
	{
		DrawSprite2(&clip_rect, 320 + (sprite_width * i), src_rect.up, &src_rect, 28);
	}

	src_rect.left = 0;
	src_rect.right = 320;
	src_rect.up = 88;
	src_rect.down = 123;
	sprite_width = (src_rect.right - src_rect.left);
	for (int i=0; i <= (native_width / sprite_width) + 1; ++i)
	{
		DrawSprite2(&clip_rect, (sprite_width * i) - ((*dword_499C8C / 2) % sprite_width), src_rect.up, &src_rect, 28);
	}

	src_rect.up = 123;
	src_rect.down = 146;
	for (int i=0; i <= (native_width / sprite_width) + 1; ++i)
	{
		DrawSprite2(&clip_rect, (sprite_width * i) - (*dword_499C8C % sprite_width), src_rect.up, &src_rect, 28);
	}

	src_rect.up = 146;
	src_rect.down = 176;
	for (int i=0; i <= (native_width / sprite_width) + 1; ++i)
	{
		DrawSprite2(&clip_rect, (sprite_width * i) - ((*dword_499C8C * 2) % sprite_width), src_rect.up, &src_rect, 28);
	}

	src_rect.up = 176;
	src_rect.down = 240;
	for (int i=0; i <= (native_width / sprite_width) + 1; ++i)
	{
		DrawSprite2(&clip_rect, (sprite_width * i) - ((*dword_499C8C * 4) % sprite_width), src_rect.up, &src_rect, 28);
	}
}

__asm(
"_ScrollClouds_asm:\n"
"	push	%eax\n"
"	call	_ScrollClouds@4\n"
"	jmp	0x402805\n"
);
void ScrollClouds_asm(void);

bool __stdcall UpdateCamera_extra(const unsigned int level_width, const int level_height)
{
	int *camera_x_pos = (int*)0x49E1C8;
	int *camera_y_pos = (int*)0x49E1CC;

	if ((level_width * 16) > native_width)
		return false;

	*camera_x_pos = -(((native_width / 2) - ((level_width * 16) / 2)) * 0x200);

	if ((*camera_y_pos / 0x200) < 0)
		*camera_y_pos = 0;
	else if (*camera_y_pos > ((((level_height - 1) * 16) - 240) * 0x200))
		*camera_y_pos = (((level_height - 1) * 16) - 240) * 0x200;

	return true;
}

__asm(
"_UpdateCamera_extra_asm:\n"
"	push	%edx\n"
"	push	-4(%ebp)\n"
"	push	-8(%ebp)\n"
"	call	_UpdateCamera_extra@8\n"
"	pop	%edx\n"
"	test	%eax, %eax\n"
"	jnz	0x40EF76\n"
"	movl	0x49E1C8, %eax\n"
"	jmp	0x40EED6\n"
);
void UpdateCamera_extra_asm(void);

__asm(
"_DrawForegroundBG_patch:\n"
"	cmp	$0, -0x28(%ebp)\n"
"	jz	0x413B68\n"
"	cmp	$0x20, -0x28(%ebp)\n"
"	jl	0x413BAF\n"
"	jmp	0x413B68\n"
);
void DrawForegroundBG_patch(void);

int __stdcall DrawBackground_Type1(int camera_x_pos, int camera_y_pos, DrawRectangle *src_rect)
{
	void (*DrawSprite2)(DrawRectangle*, int, int, DrawRectangle*, int) = (void(*)(DrawRectangle*, int, int, DrawRectangle*, int))0x40C5B0;

	int *background_tile_width = (int*)0x499C78;
	int *background_tile_height = (int*)0x499C7C;

	DrawRectangle clip_rect = {0, 0, native_width, 240};

	for (int i = (((camera_y_pos / 2) / 0x200) % *background_tile_height); i < 240; i += *background_tile_height)
	{
		for (int j =  (((camera_x_pos / 2) / 0x200) % *background_tile_width); j < native_width; j += *background_tile_width)
		{
			DrawSprite2(&clip_rect, j, i, src_rect, 28);
		}
	}
}

__asm(
"_DrawBackground_Type1_asm:\n"
"	lea	-0x10(%ebp), %eax\n"
"	push	%eax\n"
"	push	-0xC(%ebp)\n"
"	push	-0x8(%ebp)\n"
"	call	_DrawBackground_Type1@12\n"
"	jmp	0x402805\n"
);
void DrawBackground_Type1_asm(void);


int __stdcall DrawBackground_Type2(int camera_x_pos, int camera_y_pos, DrawRectangle *src_rect)
{
	void (*DrawSprite2)(DrawRectangle*, int, int, DrawRectangle*, int) = (void(*)(DrawRectangle*, int, int, DrawRectangle*, int))0x40C5B0;

	int *background_tile_width = (int*)0x499C78;
	int *background_tile_height = (int*)0x499C7C;

	DrawRectangle clip_rect = {0, 0, native_width, 240};

	for (int i = ((camera_y_pos / 0x200) % *background_tile_height); i < 240; i += *background_tile_height)
	{
		for (int j =  ((camera_x_pos / 0x200) % *background_tile_width); j < native_width; j += *background_tile_width)
		{
			DrawSprite2(&clip_rect, j, i, src_rect, 28);
		}
	}
}

__asm(
"_DrawBackground_Type2_asm:\n"
"	lea	-0x10(%ebp), %eax\n"
"	push	%eax\n"
"	push	-0xC(%ebp)\n"
"	push	-0x8(%ebp)\n"
"	call	_DrawBackground_Type2@12\n"
"	jmp	0x402805\n"
);
void DrawBackground_Type2_asm(void);

void InitMod(void)
{
	// Widescreen
	//WriteByte(0x4023D0, 0xC3);
	const int native_width_unk = native_width * (216. / 320);
	const int native_width_unk2 = native_width * (268. / 320);

	// Patch level drawers
	//  DrawBackground
	WriteLong(0x402445 + 3, native_width);
	WriteLong(0x4024D7 + 3, native_width);
	WriteLong(0x40255F + 3, native_width);
	WriteLong(0x4025D9 + 3, native_width);
//	WriteLong(0x40261B + 3, native_width);
//	WriteLong(0x402654 + 3, native_width);
	WriteLong(0x40268C + 1, native_width);
	WriteLong(0x402693 + 1, native_width);
	WriteLong(0x4026BC + 1, native_width);
//	WriteLong(0x4026C6 + 3, native_width);
	WriteLong(0x4026F9 + 1, native_width);
	WriteLong(0x402700 + 1, native_width);
	WriteLong(0x40272B + 1, native_width);
//	WriteLong(0x402735 + 3, native_width);
	WriteLong(0x402770 + 1, native_width);
	WriteLong(0x402777 + 1, native_width);
	WriteLong(0x4027A3 + 1, native_width);
//	WriteLong(0x4027AD + 3, native_width);
	WriteLong(0x4027E9 + 1, native_width);
	WriteLong(0x4027F0 + 1, native_width);

	//  DrawForegroundBG
	WriteLong(0x413AF6 + 3, (native_width / 16) + 1);
	//  DrawForegroundFG
	WriteLong(0x413C82 + 3, (native_width / 16) + 1);
	//  ???
	WriteLong(0x413E55 + 3, (native_width / 16) + 1);

	// Patch memory rectangles
	//WriteLong(0x48F91C + 0, -native_width);
	WriteLong(0x48F91C + 8, native_width);
	WriteLong(0x48F92C + 8, native_width);
	WriteLong(0x493650 + 8, native_width / 2);
	WriteLong(0x498290 + 8, native_width_unk);

	// Unknown camera(?) function
//	WriteLong(0x40EB95 + 3, native_width);
//	WriteLong(0x40EB9E + 3, native_width);
//	WriteLong(0x40EBE4 + 6, native_width);
//	WriteLong(0x40ED03 + 6, native_width);

	// Something inventory(?) related
//	WriteLong(0x421A0D + 6, native_width_unk2);

	// Related to credits. May be the characters ("casts")
	WriteLong(0x40D575, native_width / 2);

	// Patch WinMain (window creation and other stuff)
	WriteLong(0x4126E5 + 3, native_width);
	WriteLong(0x4127BC + 6, native_width);
	WriteLong(0x4127D2 + 6, native_width * 2);
	WriteLong(0x41293F + 6, native_width * 2);
	WriteLong(0x412A81 + 6, native_width);

	// InitDirectDraw
	WriteLong(0x40B53F + 2, native_width);
	WriteLong(0x40B571 + 2, native_width);
	WriteLong(0x40B594 + 2, native_width);
	WriteLong(0x40B62F + 2, native_width);

	// Patch camera to centre properly
	WriteLong(0x40EE90 + 1, (native_width / 2) * 0x200);	// Patch camera so Quote is still centred
	WriteLong(0x40EF19 + 2, native_width);
	WriteLong(0x40EF34 + 2, native_width);
	WriteJump(0x40EED1, UpdateCamera_extra_asm);
	// SetCameraUponEnterRoom - This function is what decides where the camera starts off when you enter a room
	WriteLong(0x40F15B + 2, (native_width / 2) * 0x200);
	WriteLong(0x40F1BE + 1, native_width);
	WriteLong(0x40F1D8 + 2, native_width);

	// ??????? (sub_40F040) The function appears to be unused
	//WriteLong(0x40F0C5 + 2, native_width);
	//WriteLong(0x40F0E0 + 2, native_width);
/*
	// Fading part 1
	WriteByte(0x40E7DA + 3, native_width / 16);

	// Fading part 2
	WriteByte(0x40E04B + 3, native_width / 16);
	WriteByte(0x40E0A5 + 3, native_width / 16);
	WriteByte(0x40E0FA + 3, native_width / 16);
	WriteByte(0x40E154 + 3, native_width / 16);
	WriteByte(0x40E1FC + 3, native_width / 16);
	WriteByte(0x40E2AC + 3, native_width / 16);
	WriteByte(0x40E30B + 3, native_width / 16);
	WriteByte(0x40E3E3 + 3, native_width / 16);
	WriteByte(0x40E43D + 3, native_width / 16);
	WriteByte(0x40E492 + 3, native_width / 16);
	WriteByte(0x40E4EC + 3, native_width / 16);
	WriteByte(0x40E59B + 3, native_width / 16);
	WriteByte(0x40E659 + 3, native_width / 16);
	WriteByte(0x40E6BF + 3, native_width / 16);

	// 3
	WriteByte(0x40DF16 + 3, native_width / 16);
	WriteByte(0x40DFA6 + 3, native_width / 16);
*/
	// sub_473000
	//WriteByte(0x47303B + 3, native_width / 16);

	// Patch water to cover screen
	WriteByte(0x402896 + 2, (native_width / 32) + 1);

	// Patch IncrementBGScroll
	//WriteLong(0x4023B9 + 1, native_width * 2);
	//WriteLong(0x402396 + 2, 0xC00 * 2);

	// Hijack scrolling cloud background code
	WriteLong(0x402809 + (4 * 6), (int)ScrollClouds_asm);
	WriteLong(0x402809 + (4 * 7), (int)ScrollClouds_asm);
	WriteJump(0x413BA7, DrawForegroundBG_patch);
	WriteLong(0x402809 + (4 * 1), (int)DrawBackground_Type1_asm);
	WriteLong(0x402809 + (4 * 2), (int)DrawBackground_Type2_asm);
}
