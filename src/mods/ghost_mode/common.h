#pragma once

#include <stdbool.h>

typedef struct
{
	int x_pos;
	int y_pos;
	int frame_id : 4;
	bool facing_right : 1;
	bool should_display : 1;
	short current_room;
} GhostFrame;
