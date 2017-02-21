#include "mod_loader.h"

#include "common.h"
#include "patch_fade.h"

char fade_buffer_bool[SCREEN_WIDTH * 240];
char fade_buffer_int[SCREEN_WIDTH * 240];

void ProcessFade(void)
{
	int *fade_in_or_out = (int*)0x49DB30;
	int *fade_complete = (int*)0x49DB34;
	int *fade_counter = (int*)0x49DB38;
	int *fade_type = (int*)0x49DD94;

	const int tile_width = ScreenWidthToTiles(16);

	if (*fade_in_or_out == 1)
	{
		*fade_complete = 0;
		switch (*fade_type)
		{
			case 0:
			{
				for (int i=0; i < 15; ++i)
				{
					for (int j=0; j < tile_width; ++j)
					{
						if ((tile_width - 1) - *fade_counter == j)
						{
							fade_buffer_bool[(tile_width * i) + j] = 1;
						}
					}
				}
				break;
			}
			case 2:
			{
				for (int i=0; i < 15; ++i)
				{
					for (int j=0; j < tile_width; ++j)
					{
						if (*fade_counter == j)
						{
							fade_buffer_bool[(tile_width * i) + j] = 1;
						}
					}
				}
				break;
			}
			case 1:
			{
				for (int i=0; i < 15; ++i)
				{
					for (int j=0; j < tile_width; ++j)
					{
						if (14 - *fade_counter == i)
						{
							fade_buffer_bool[(tile_width * i) + j] = 1;
						}
					}
				}
				break;
			}
			case 3:
			{
				for (int i=0; i < 15; ++i)
				{
					for (int j=0; j < tile_width; ++j)
					{
						if (*fade_counter == i)
						{
							fade_buffer_bool[(tile_width * i) + j] = 1;
						}
					}
				}
				break;
			}
			case 4:
			{
				for (int i=0; i < 7; ++i)
				{
					for (int j=0; j < tile_width / 2; ++j)
					{
						if ((tile_width - 1) - *fade_counter == i + j)
						{
							fade_buffer_bool[(tile_width * i) + j] = 1;
						}
					}
				}
				for (int i=0; i < 7; ++i)
				{
					for (int j=tile_width / 2; j < tile_width; ++j)
					{
						if ((tile_width - 1) - *fade_counter == i + (tile_width - 1) - j)
						{
							fade_buffer_bool[(tile_width * i) + j] = 1;
						}
					}
				}
				for (int i=7; i < 15; ++i)
				{
					for (int j=0; j < tile_width / 2; ++j)
					{
						if ((tile_width - 1) - *fade_counter == j + 14 - i)
						{
							fade_buffer_bool[(tile_width * i) + j] = 1;
						}
					}
				}
				for (int i=7; i < 15; ++i)
				{
					for (int j=tile_width / 2; j < tile_width; ++j)
					{
						if ((tile_width - 1) - *fade_counter == 14 - i + (tile_width - 1) - j)
						{
							fade_buffer_bool[(tile_width * i) + j] = 1;
						}
					}
				}
				break;
			}
		}
		for (int i=0; i < 15; ++i)
		{
			for (int j=0; j < tile_width; ++j)
			{
				if (fade_buffer_int[(tile_width * i) + j] > 0 && fade_buffer_bool[(tile_width * i) + j])
				{
					--fade_buffer_int[(tile_width * i) + j];
				}
			}
		}
		if (++*fade_counter > 15 + tile_width + 1)
		{
			*fade_in_or_out = 0;
		}
	}
	else if (*fade_in_or_out == 2)
	{
		switch (*fade_type)
		{
			case 0:
			{
				for (int i=0; i < 15; ++i)
				{
					for (int j=0; j < tile_width; ++j)
					{
						if ((tile_width - 1) - *fade_counter == j)
						{
							fade_buffer_bool[(tile_width * i) + j] = 1;
						}
					}
				}
				break;
			}
			case 2:
			{
				for (int i=0; i < 15; ++i)
				{
					for (int j=0; j < tile_width; ++j)
					{
						if (*fade_counter == j)
						{
							fade_buffer_bool[(tile_width * i) + j] = 1;
						}
					}
				}
				break;
			}
			case 1:
			{
				for (int i=0; i < 15; ++i)
				{
					for (int j=0; j < tile_width; ++j)
					{
						if (14 - *fade_counter == i)
						{
							fade_buffer_bool[(tile_width * i) + j] = 1;
						}
					}
				}
				break;
			}
			case 3:
			{
				for (int i=0; i < 15; ++i)
				{
					for (int j=0; j < tile_width; ++j)
					{
						if (*fade_counter == i)
						{
							fade_buffer_bool[(tile_width * i) + j] = 1;
						}
					}
				}
				break;
			}
			case 4:
			{
				for (int i=0; i < 7; ++i)
				{
					for (int j=0; j < tile_width / 2; ++j)
					{
						if (*fade_counter == i + j)
						{
							fade_buffer_bool[(tile_width * i) + j] = 1;
						}
					}
				}
				for (int i=0; i < 7; ++i)
				{
					for (int j=tile_width / 2; j < tile_width; ++j)
					{
						if (*fade_counter == i + (tile_width - 1) - j)
						{
							fade_buffer_bool[(tile_width * i) + j] = 1;
						}
					}
				}
				for (int i=7; i < 15; ++i)
				{
					for (int j=0; j < tile_width / 2; ++j)
					{
						if (*fade_counter == j + 14 - i)
						{
							fade_buffer_bool[(tile_width * i) + j] = 1;
						}
					}
				}
				for (int i=7; i < 15; ++i)
				{
					for (int j=tile_width / 2; j < tile_width; ++j)
					{
						if (*fade_counter == 14 - i + (tile_width - 1) - j)
						{
							fade_buffer_bool[(tile_width * i) + j] = 1;
						}
					}
				}
				break;
			}
		}
		for (int i=0; i < 15; ++i)
		{
			for (int j=0; j < tile_width; ++j)
			{
				if (fade_buffer_int[(tile_width * i) + j] < 15 && fade_buffer_bool[(tile_width * i) + j])
				{
					++fade_buffer_int[(tile_width * i) + j];
				}
			}
		}
		if (++*fade_counter > 15 + tile_width + 1)
		{
			*fade_complete = 1;
			*fade_in_or_out = 0;
		}
	}
}

void PatchFade(void)
{
	// Redirect ProcessFade
	WriteJump(0x40DFE0, ProcessFade);
	// BeginFadeOut
	WriteLong(0x40DF25 + 3, (int)fade_buffer_int);
	WriteLong(0x40DF36 + 3, (int)fade_buffer_bool);
	WriteByte(0x40DF16 + 3, ScreenWidthToTiles(16));
	WriteByte(0x40DF1F + 2, ScreenWidthToTiles(16));
	WriteByte(0x40DF30 + 2, ScreenWidthToTiles(16));
	// BeginFadeIn
	WriteLong(0x40DFB5 + 3, (int)fade_buffer_int);
	WriteLong(0x40DFC6 + 3, (int)fade_buffer_bool);
	WriteByte(0x40DFA6 + 3, ScreenWidthToTiles(16));
	WriteByte(0x40DFAF + 2, ScreenWidthToTiles(16));
	WriteByte(0x40DFC0 + 2, ScreenWidthToTiles(16));
	// DrawFade
	WriteLong(0x40E7E9 + 4, (int)fade_buffer_int);
	WriteByte(0x40E7DA + 3, ScreenWidthToTiles(16));
	WriteByte(0x40E7E3 + 2, ScreenWidthToTiles(16));
}
