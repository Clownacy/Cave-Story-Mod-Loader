#pragma once

__declspec(dllexport) extern void WriteRelativeAddress(const int instruction_address, const void* const new_destination);
__declspec(dllexport) extern void WriteByte(const int instruction_address, const char value);
__declspec(dllexport) extern void WriteJump(const int instruction_address, const void* const new_destination);
__declspec(dllexport) extern void WriteCall(const int instruction_address, const void* const new_destination);
