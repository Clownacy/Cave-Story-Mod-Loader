// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2026 Clownacy

#include "font_fix.h"

#include <windows.h>

#include "mod_loader.h"

#include "../common.h"

void FixFontAntialiasing(void)
{
	ModLoader_PrintDebug("Applying 'font anti-aliasing fix' patch\n");

	ModLoader_WriteByte((void*)0x40CD90 + 1, NONANTIALIASED_QUALITY);
	ModLoader_WriteByte((void*)0x40CDC7 + 1, NONANTIALIASED_QUALITY);
}

void FixFontCharset(void)
{
	ModLoader_PrintDebug("Applying 'font charset fix' patch\n");

	ModLoader_WriteByte((void*)0x40CD96 + 1, SHIFTJIS_CHARSET);
	ModLoader_WriteByte((void*)0x40CDCD + 1, SHIFTJIS_CHARSET);
}
