// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#pragma once

extern void AddToModList(char *mod_path);
extern const char* const GetModPath(unsigned int mod_num);
extern unsigned int GetModCount(void);
