// src/core/d2sound/d2sound_crt_batch_k01.cpp -- D2Sound.dll CRT stubs. Locks no-op (single wasm thread); x87 status stubs return 0.
#include <cstdint>

void d2s_unlock(int);

// 1.13c D2sound+0x28D3  heap-lock release thunk: _unlock(4)
void sub_28D3() { d2s_unlock(4); }

// 1.13c D2sound+0x1AA1  _unlock(0xd)
void sub_1AA1() { d2s_unlock(0xd); }

// 1.13c D2sound+0x1AAD  _unlock(0xc)
void sub_1AAD() { d2s_unlock(0xc); }

// 1.13c D2sound+0xDF77  __statfp: x87 status word; no FPU status in wasm
int __statfp() { return 0; }

// 1.13c D2sound+0xDF82  __clrfp: x87 status word then clear; no FPU status in wasm
int __clrfp() { return 0; }

struct D2sSbhRegion { uint32_t pad[3]; uint32_t base; uint32_t pad2; };  // 0x14 bytes, base at +0xc
static D2sSbhRegion* g_sbhRegions; static uint32_t g_sbhCount;

// 1.13c D2sound+0x3564  ___sbh_find_block(p): region whose 1 MiB span holds p, else null
D2sSbhRegion* ___sbh_find_block(uint32_t p) {
    for (uint32_t i = 0; i < g_sbhCount; i++)
        if (p - g_sbhRegions[i].base < 0x100000) return &g_sbhRegions[i];
    return nullptr;
}
