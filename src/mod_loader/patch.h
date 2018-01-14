// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#pragma once

extern void WriteRelativeAddress(void* const address, const void* const new_destination);
extern void WriteByte(void* const address, const unsigned char value);
extern void WriteWord(void* const address, const unsigned short value);
extern void WriteLong(void* const address, const unsigned int value);
extern void WriteWordBE(void* const address, const unsigned short value);
extern void WriteLongBE(void* const address, const unsigned int value);
extern void WriteJump(void* const address, const void* const new_destination);
extern void WriteCall(void* const address, const void* const new_destination);
extern void WriteNOPs(void* const address, const unsigned int count);
