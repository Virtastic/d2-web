// 1.13c D2Client+0x4BEC0: ESI = unit. Posts two 0x2020-type requests about a player unit through 0xB30F0 (ECX = a 23-dword block built on the
// stack). Seams: D2Common ord 10494 (thunk 0xC1FC, stdcall 2 words), 0x1260 (EAX = unit class), 0xB30F0 (ECX = block, called twice with the same block).
#include <emscripten.h>
#include <cstdint>
#include <cstring>
struct D2c4bec0Env {
    int32_t (*o10494)(const int32_t* unit, int32_t k);
    const uint8_t* (*f1260)(int32_t cls);
    void (*b30f0)(uint32_t* blk);
};
extern "C" void d2_client_4bec0(const int32_t* u, const D2c4bec0Env* e) {
    if (!u || u[0] != 1) return;
    uint32_t blk[23];
    auto build = [&](uint32_t code) {
        memset(blk, 0, sizeof blk);
        blk[0] = 0x2020; blk[1] = (uint32_t)(uintptr_t)u; blk[2] = (uint32_t)(uintptr_t)u; blk[4] = code; blk[12] = 1;   // [esp+0x18] and [esp+0x38], block at [esp+0x8]
    };
    if (e->o10494(u, 0xb8) && !e->o10494(u, 1)) {
        build(0x1ab); e->b30f0(blk);
        blk[4] = 0x1aa; e->b30f0(blk);
        return;
    }
    const uint8_t* rec = e->f1260(u[1]);
    build((uint32_t)rec[0xb] + 0x10f); e->b30f0(blk);
    blk[4] = 0x113; e->b30f0(blk);
}
