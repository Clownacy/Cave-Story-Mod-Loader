// Mod loader for Freeware Cave Story
// Copyright © 2018 Clownacy, 20kdc

#pragma once

#include <mod_loader_hooks.h>

// Adds a stackable hook.
// The conditions are:
// 1. The length must be above or equal to 5.
// 2. Stacked hooks must share the same address and length.
// 3. The code starting at address and continuing for length bytes must be,
//     by itself, valid opcodes (instruction boundaries on either side),
//     and must never branch away without eventually reaching the position at address + length.
//    The hook is inactive until this position is reached.
// 4. For the purposes of accounting and preventing corruption of function pointer tables,
//     the area patched by this code starts at address, and extends for length + 5 bytes.
// 5. The entire hook system is single-threaded.
//    If hooks are created or executed on anything other than the main thread,
//     expect total failure.
__declspec(dllexport) void AddStackableHook(void * address, unsigned int length, MLHookCallback cb, void * ud);
