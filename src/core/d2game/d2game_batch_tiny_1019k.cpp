// src/core/d2game/d2game_batch_tiny_1019k.cpp -- five 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c) and the retail bytes, in RVA
// order: the exported constant-1 thunk at 0xB87C0 (export ordinal 10046) and four __fastcall wrappers
// (0xC2AF0, 0xC2F00, 0xC3620, 0xC3EB0) that forward ECX/EDX unchanged to the quest reward/GUID helper at
// 0x340C0. Stage A batch lane (OBJECTIVE v8.3): implementation + marker only, no spec, no vector.
//
// 0xB87C0 is `mov eax,1; ret 0x18`: six dword stack arguments, result 1, nothing else observable.
// The four wrappers are `push ebx; push esi; mov ebx,edx; mov esi,ecx; call 0x340C0; pop esi; pop ebx;
// ret`: __fastcall(ECX = pQuest, EDX = pQuestArg), no stack arguments and no return value. The decompile
// drops the register arguments and shows them as `FUN_6fc540c0()`; the callee is src's 0x340C0, which
// reads pQuest and pQuestArg from exactly those two registers. Modelled here as an Env seam in retail
// argument order, as the other batch files do for callees the file does not own.
//
// Prior art: src/core/d2game/d2game_batch_34e0.cpp (0x340C0, the callee and its ESI/EBX convention);
// reference/D2MOO/source/D2Game/src/QUESTS/ACT5/A5Q6.cpp (QUESTS_FastRemovePlayerGUID at the inlined
// reward-removal sites; 1.10f has no standalone wrapper).
#include "d2game_scalar_helpers.h"
#include <cstdint>
#include <emscripten.h>

// 1.13c D2Game+0xB87C0
// Exported constant-1 stub: six stack arguments, returns 1 unconditionally.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2game_export10046(uint32_t, uint32_t, uint32_t,
                                                uint32_t, uint32_t, uint32_t)
{
    return 1;
}
