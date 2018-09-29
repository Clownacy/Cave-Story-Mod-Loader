#pragma once

#include <stdbool.h>

typedef struct pxtnService pxtnService;

pxtnService* Pxtone_Open(const char *file_path, bool loop);
void Pxtone_Close(pxtnService *decoder);
void Pxtone_Rewind(pxtnService *decoder, bool loop);
unsigned long Pxtone_GetSamples(pxtnService *decoder, void *buffer, unsigned long bytes_to_do);
