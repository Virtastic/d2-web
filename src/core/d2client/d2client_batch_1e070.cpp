// src/core/d2client/d2client_batch_1e070.cpp -- the 1.13c D2Client.dll rows 0x212F0 and 0x21400 from the
// decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000; every RVA below is retail's.
// (The other rows of this batch live in their stand-alone d2client_<rva>.cpp files.)
//
// Unrecovered internal callees and imported ordinals are host seams (the
// src/core/d2client/d2client_batch_1e70.cpp pattern): the function takes a D2ClientBatch1de0Env AFTER
// its retail arguments, which keep their order.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

#include "d2client_batch_1e070_env.h"

// 1.13c D2Client+0x212f0: cdecl(obj, ecxArg, base), callee pops 12. Live type-1 object with obj[5] set:
// for each of the nine index bytes at obj[5]+0x1C call table[base + byte*5] (when non-null) as
// __fastcall(ECX = obj, or ecxArg when base == 4; EDX = byte; stack = bit 3 of obj[5]+0x16).
// Retail does not range-check the index; the table here is sized for the largest reachable index so
// out-of-range slots read as empty instead of past the array.
typedef void (*AnimCb)(uint32_t ecx, uint32_t edx, uint32_t flag);
static AnimCb g_animCb[1280];        // +0x0F9FE0 static table, 5-dword stride

// Retail's static initialiser: slots 47, 52, 57 hold the D2Client callbacks at +0x20D20, +0x20C40,
// +0x21210. Their code is reached through the host, so it hands the three entry points over once.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_212f0_init(AnimCb s47, AnimCb s52, AnimCb s57) {
    g_animCb[47] = s47;
    g_animCb[52] = s52;
    g_animCb[57] = s57;
}

extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_212f0(int32_t* param_1, uint32_t param_2, int32_t param_3) {
    if (param_1 == nullptr || *param_1 != 1) return;
    int32_t iVar2 = param_1[5];
    if (iVar2 == 0) return;
    uint8_t flag = (uint8_t)((*(uint8_t*)(iVar2 + 0x16) >> 3) & 1);
    uint32_t recv = (param_3 == 4) ? param_2 : (uint32_t)(uintptr_t)param_1;
    for (int32_t i = 0; i < 9; ++i) {
        uint8_t idx = *(uint8_t*)(iVar2 + 0x1C + i);
        AnimCb fn = g_animCb[(uint32_t)param_3 + (uint32_t)idx * 5];
        if (fn != nullptr) fn(recv, idx, flag);
    }
}

// 1.13c D2Client+0x021400: EDI = unit, EBX = key, stack (param_1, param_2), ret 8. Walks the event list at unit+0x90 (retail
// addresses; next at +0x1C); every node whose first dword equals the key calls node+0x14 with ECX = key, EDX = unit and the
// stack words (param_1, param_2, node+0xC, node+0x10); the walk continues after a hit and the last return wins.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_21400(uint32_t param_1, uint32_t param_2, void* obj,
                                              int32_t key, const D2ClientBatch1de0Env* env) {
    uint32_t result = 0;
    if (obj == nullptr) return result;
    auto rd = [&](const uint8_t* p, uint32_t off) { uint32_t v; std::memcpy(&v, p + off, 4); return v; };
    uint32_t addr = rd((const uint8_t*)obj, 0x90);
    while (addr != 0) {
        const uint8_t* node = env->resolve(addr);
        uint32_t next = rd(node, 0x1c);
        if (rd(node, 0) == (uint32_t)key)
            result = env->invoke1410(rd(node, 0x14), (uint32_t)key, (uint32_t)(uintptr_t)obj, param_1, param_2,
                                     rd(node, 0xc), rd(node, 0x10));
        addr = next;
    }
    return result;
}
