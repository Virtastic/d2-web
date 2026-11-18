// 1.13c D2Client+0x5CE30 (63 B, ESI = slot record, plain ret): slot [esi+0] = handle, [esi+8] = state block. With a handle:
// when a block exists, Ordinal 10032 (thunk 0xD30C, ECX = handle) is stored at block+0x41 (unaligned dword); then Ordinal
// 10006 (thunk 0xD336, ECX = handle). A block is then reset (+0x30 := 2, +0x2C := 0) and detached ([esi+8] := 0).
#include <emscripten.h>
#include <cstdint>
#include <cstring>
struct D2c5ce30Env {
    uint32_t (*cd30c)(uint32_t ecx);
    void (*cd336)(uint32_t ecx);
};
extern "C" void d2c_port_5ce30(uint8_t* esi, const D2c5ce30Env* e) {
    auto rd = [](const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; };
    auto wr = [](uint8_t* p, uint32_t v) { std::memcpy(p, &v, 4); };
    uint32_t h = rd(esi);
    if (h != 0) {
        uint8_t* blk = (uint8_t*)(uintptr_t)rd(esi + 8);
        if (blk != nullptr) {
            uint32_t r = e->cd30c(h);
            wr((uint8_t*)(uintptr_t)rd(esi + 8) + 0x41, r);
        }
        e->cd336(rd(esi));
    }
    uint8_t* blk = (uint8_t*)(uintptr_t)rd(esi + 8);
    if (blk != nullptr) {
        wr(blk + 0x30, 2);
        wr((uint8_t*)(uintptr_t)rd(esi + 8) + 0x2C, 0);
        wr(esi + 8, 0);
    }
}
