#include "in.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "cave_story.h"

#include "common.h"

typedef struct GhostIn
{
	FILE *file;
	GhostFrame frames[60 * 60];
	unsigned int index;
	unsigned int remaining;
	unsigned int current;
	bool enabled;
} GhostIn;

static const RECT spriterects[] = {
	{0, 0, 16, 16},
	{16, 0, 32, 16},
	{0, 0, 16, 16},
	{32, 0, 48, 16},
	{0, 0, 16, 16},
	{48, 0, 64, 16},
	{64, 0, 80, 16},
	{48, 0, 64, 16},
	{80, 0, 96, 16},
	{48, 0, 64, 16},
	{96, 0, 112, 16},
	{112, 0, 128, 16}
};

static void SeekGhostData(GhostIn *this, unsigned int frame)
{
	fseek(this->file, sizeof(unsigned int) + (frame * sizeof(GhostFrame)), SEEK_SET);
	this->remaining = 0;
	this->current = frame;
}

GhostIn* GhostIn_Open(const char *filename)
{
	GhostIn *this = NULL;

	FILE *file = fopen(filename, "r+b");

	if (file)
	{
		this = malloc(sizeof(GhostIn));

		this->file = file;
		this->index = 0;
		this->remaining = 0;
		fread(&this->current, 1, sizeof(unsigned int), file);
		this->enabled = true;
	}

	return this;
}

void GhostIn_Reset(GhostIn *this)
{
	SeekGhostData(this, 0);
}

void GhostIn_Resume(GhostIn *this)
{
	SeekGhostData(this, this->current);
}

void GhostIn_Save(GhostIn *this)
{
	const long old_pos = ftell(this->file);
	rewind(this->file);
	fwrite(&this->current, 1, sizeof(unsigned int), this->file);
	fseek(this->file, old_pos, SEEK_SET);
}

void GhostIn_Update(GhostIn *this, int camera_x, int camera_y)
{
	if (this->enabled)
	{
		if (this->remaining == 0)
		{
			this->index = 0;

			this->remaining = fread(this->frames, sizeof(GhostFrame), 60 * 60, this->file);

			if (this->remaining == 0)
			{
				this->enabled = false;
				return;
			}
		}

		GhostFrame *ghost_frame = &this->frames[this->index];

		++this->index;
		++this->current;
		--this->remaining;

		RECT selected_rect = spriterects[ghost_frame->frame_id];
		if (ghost_frame->facing_right)
		{
			selected_rect.top += 16;
			selected_rect.bottom += 16;
		}

//		static bool flicker;
//		flicker = !flicker;

		if (ghost_frame->should_display && ghost_frame->current_room == CS_current_room)// && flicker)
			CS_DrawSprite_WithTransparency(&CS_clip_rect_common, (ghost_frame->x_pos / 0x200) - (camera_x / 0x200), (ghost_frame->y_pos / 0x200) - (camera_y / 0x200), &selected_rect, CS_SURFACE_ID_MY_CHAR);
	}
}
