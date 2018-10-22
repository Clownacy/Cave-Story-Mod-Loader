#pragma once

typedef struct GhostIn GhostIn;

GhostIn* GhostIn_Open(const char *filename);
void GhostIn_Reset(GhostIn *this);
void GhostIn_Resume(GhostIn *this);
void GhostIn_Save(GhostIn *this);
void GhostIn_Update(GhostIn *this, int camera_x, int camera_y);

