// src/core/d2game/d2game_batch_eb440.cpp -- 1.13c D2Game.dll 0xEB440 (player-save section: three stat-pair table copies)
// transcribed from the cached Ghidra decompile. in_EAX = buffer end, EBX = cursor slot, param_1 = unit.
// ord10920 is the retail Ordinal_10920 seam; it returns a record whose +0x60/+0x64/+0x68 are double-indirect
// tables of 2 x {u32,u32} destinations.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static void put(uint8_t* rec, int off, const uint8_t* s, int a, int b, int c, int d) {
    // table at *(rec+off): [0]->ptr->ptr->dst0, [1]->ptr->dst1 (decompile shape)
    uint8_t* t = (uint8_t*)(uintptr_t)*(uint32_t*)(rec + off);
    uint32_t p0, p1, d0, d1;
    std::memcpy(&p0, t, 4); std::memcpy(&d0, (void*)(uintptr_t)p0, 4);
    std::memcpy((void*)(uintptr_t)d0, s + a, 4); std::memcpy((uint8_t*)(uintptr_t)d0 + 4, s + b, 4);
    std::memcpy(&p1, t + 4, 4); std::memcpy(&d1, (void*)(uintptr_t)p1, 4);
    std::memcpy((void*)(uintptr_t)d1, s + c, 4); std::memcpy((uint8_t*)(uintptr_t)d1 + 4, s + d, 4);
}

extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_EB440(uint32_t in_eax, uint32_t* ebx, void* unit, uint8_t* (*ord10920)(void*)) {  // 1.13c D2Game+0xEB440
    uint8_t* s = (uint8_t*)(uintptr_t)*ebx;
    int16_t tag; std::memcpy(&tag, s, 2);
    if (!((uintptr_t)(s + 0x34) <= in_eax && tag == 0x7701)) return 0x11;
    uint8_t* r = ord10920(unit);
    put(r, 0x60, s, 4, 8, 0x1c, 0x20);
    put(r, 0x64, s, 12, 16, 0x24, 0x28);
    put(r, 0x68, s, 20, 24, 0x2c, 0x30);
    *ebx = (uint32_t)(uintptr_t)(s + 0x34);
    return 0;
}
