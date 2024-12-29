// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#pragma once

extern void InitLogging(void);
__declspec(dllexport) extern void PrintMessageBoxError(const char* const format, ...);
__declspec(dllexport) extern void PrintError(const char* const format, ...);
__declspec(dllexport) extern void PrintDebug(const char* const format, ...);
__declspec(dllexport) extern void PrintPollution(const char* const format, ...);
