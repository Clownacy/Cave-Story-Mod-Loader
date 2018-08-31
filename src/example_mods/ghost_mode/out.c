#include "out.h"

#include <stdio.h>
#include <stdlib.h>

#include "cave_story.h"

#include "common.h"

static bool record_ghost = true;

static GhostFrame ghost_frames[60 * 60];
static unsigned int ghost_frames_index;

void GhostOut_Reset(void)
{
	remove("ghosttemp.dat");

	ghost_frames_index = 0;
}

void GhostOut_Load(void)
{
	FILE *ghost_file = fopen("outghost.dat", "rb");
	fseek(ghost_file, 4, SEEK_SET);

	if (ghost_file)
	{
		fseek(ghost_file, 0, SEEK_END);
		const long ghost_file_size = ftell(ghost_file);
		rewind(ghost_file);

		unsigned char *ghost_file_buffer = malloc(ghost_file_size);
		fread(ghost_file_buffer, 1, ghost_file_size, ghost_file);
		fclose(ghost_file);

		FILE *temp_file = fopen("ghosttemp.dat", "wb");
		fwrite(ghost_file_buffer, 1, ghost_file_size, temp_file);
		fclose(temp_file);

		free(ghost_file_buffer);

		ghost_frames_index = 0;
	}
	else
	{
		record_ghost = false;
	}
}

void GhostOut_Save(void)
{
	FILE *ghost_file = fopen("outghost.dat", "wb");
	fseek(ghost_file, sizeof(unsigned int), SEEK_SET);

	FILE *temp_file = fopen("ghosttemp.dat", "rb");
	if (temp_file)
	{
		fseek(temp_file, 0, SEEK_END);
		const long temp_file_size = ftell(temp_file);
		rewind(temp_file);

		unsigned char *temp_file_buffer = malloc(temp_file_size);
		fread(temp_file_buffer, 1, temp_file_size, temp_file);
		fclose(temp_file);

		fwrite(temp_file_buffer, 1, temp_file_size, ghost_file);

		free(temp_file_buffer);
	}

	fwrite(ghost_frames, sizeof(GhostFrame), ghost_frames_index, ghost_file);
	fclose(ghost_file);
}

void GhostOut_Write(void)
{
	ghost_frames[ghost_frames_index].x_pos = CS_quote_x_pos - CS_quote_sprite_half_width;
	ghost_frames[ghost_frames_index].y_pos = CS_quote_y_pos - CS_quote_sprite_half_height;
	ghost_frames[ghost_frames_index].frame_id = CS_quote_frame_id;
	ghost_frames[ghost_frames_index].facing_right = CS_quote_facing_right;
	ghost_frames[ghost_frames_index].should_display = (CS_quote_display_flags & 0x80) && !(CS_quote_display_flags & 2);
	ghost_frames[ghost_frames_index].current_room = CS_current_room;

	if (++ghost_frames_index == 60 * 60)
	{
		ghost_frames_index = 0;

		FILE *file = fopen("ghosttemp.dat", "ab");
		fwrite(ghost_frames, sizeof(GhostFrame), 60 * 60, file);
		fclose(file);
	}
}
