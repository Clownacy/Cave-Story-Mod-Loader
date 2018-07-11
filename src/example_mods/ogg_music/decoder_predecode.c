#include "decoder_predecode.h"

#include <stddef.h>
#include <stdlib.h>

#include "decoder_common.h"
#include "memory_file.h"

typedef struct DecoderPredecode
{
	MemoryFile *file;
} DecoderPredecode;

DecoderPredecode* Decoder_Predecode_Open(const char *file_path, DecoderInfo *info, DecoderBackend *backend)
{
	DecoderPredecode *this = NULL;

	void *backend_object = backend->Open(file_path, info, backend->backend);

	if (backend_object)
	{
		this = malloc(sizeof(DecoderPredecode));

		unsigned char *buffer = malloc(info->decoded_size);
		backend->GetSamples(backend_object, buffer, info->decoded_size);
		this->file = MemoryFile_fopen_from(buffer, info->decoded_size);
		backend->Close(backend_object);
		free(backend);
	}

	return this;
}

void Decoder_Predecode_Close(DecoderPredecode *this)
{
	MemoryFile_fclose(this->file);
	free(this);
}

void Decoder_Predecode_Rewind(DecoderPredecode *this)
{
	MemoryFile_fseek(this->file, 0, SEEK_SET);
}


void Decoder_Predecode_Loop(DecoderPredecode *this)
{
	Decoder_Predecode_Rewind(this);
}

unsigned long Decoder_Predecode_GetSamples(DecoderPredecode *this, void *output_buffer, unsigned long bytes_to_do)
{
	return MemoryFile_fread(output_buffer, 1, bytes_to_do, this->file);
}
