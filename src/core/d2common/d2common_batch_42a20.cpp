// 1.13c D2Common+0x42A20 batch. Wasm64 pointers travel as uint32_t guest addresses.
#include <cstdint>
#include <emscripten.h>

extern "C" int32_t* (*g_d2common_fn_44CE0)(uint32_t pDrlg, int32_t nLevelId);

// 1.13c D2Common+0x42A20: GUESS signature (thiscall, level in EAX lost by the decompiler):
// pDrlg=ECX (GUESS), pLevelA=EAX (GUESS), pLevelB=stack. Looks up the eight per-difficulty level ids
// of level A's id (+0x1D0) and finds level B's id (+0x1D0) among them; returns 1 << (index + 4), or 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_42a20(uint32_t pDrlg, uint32_t pLevelA, uint32_t pLevelB)
{
    if (!g_d2common_fn_44CE0 || !pLevelA || !pLevelB) return 0;
    const int32_t* ids = g_d2common_fn_44CE0(pDrlg, *reinterpret_cast<int32_t*>(static_cast<uintptr_t>(pLevelA) + 0x1D0));
    const int32_t want = *reinterpret_cast<int32_t*>(static_cast<uintptr_t>(pLevelB) + 0x1D0);
    for (int32_t i = 0; i < 8; ++i)
        if (ids[i] == want) return 1u << (i + 4);
    return 0;
}
