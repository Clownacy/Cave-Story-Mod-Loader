#pragma once

extern void WriteRelativeAddress(const int instruction_address, const void* const new_destination);
extern void WriteByte(const int instruction_address, const char value);
extern void WriteJump(const int instruction_address, const void* const new_destination);
extern void WriteCall(const int instruction_address, const void* const new_destination);
