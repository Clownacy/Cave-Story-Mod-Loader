// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#pragma once

__declspec(dllexport) extern void WriteRelativeAddress(void* const address, const void* const new_destination);
__declspec(dllexport) extern void WriteByte(void* const address, const unsigned char value);
__declspec(dllexport) extern void WriteWord(void* const address, const unsigned short value);
__declspec(dllexport) extern void WriteLong(void* const address, const unsigned int value);
__declspec(dllexport) extern void WriteWordBE(void* const address, const unsigned short value);
__declspec(dllexport) extern void WriteLongBE(void* const address, const unsigned int value);
__declspec(dllexport) extern void WriteJump(void* const address, const void* const new_destination);
__declspec(dllexport) extern void WriteCall(void* const address, const void* const new_destination);
__declspec(dllexport) extern void WriteNOPs(void* const address, const unsigned int count);
