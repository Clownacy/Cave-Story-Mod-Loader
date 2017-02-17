#pragma once

void (*WriteRelativeAddress)(const int instruction_address, const void* const new_destination);
void (*WriteByte)(const int instruction_address, const char value);
void (*WriteWord)(const int instruction_address, const short value);
void (*WriteLong)(const int instruction_address, const int value);
void (*WriteJump)(const int instruction_address, const void* const new_destination);
void (*WriteCall)(const int instruction_address, const void* const new_destination);
void (*FixDoorEnterBug)(void);
