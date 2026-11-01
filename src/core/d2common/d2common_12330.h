#pragma once
#include <cstdint>
// D2Common 0x12330 seams (retail callees): 0x2FE10 room of a unit (stdcall), 0x121D0 room-in-box test (EAX = y,
// ECX = room, EDX = x, EDI = margin), 0x7F5E0 unit distance to (x, y) (stdcall), the caller's fastcall filter
// (ECX = candidate, EDX = source unit).
struct D2Common12330Env {
    uint32_t (*room)(uint32_t unit);
    uint32_t (*inBox)(uint32_t room, int32_t x, int32_t y, int32_t margin);
    int32_t (*dist)(uint32_t unit, int32_t x, int32_t y);
    uint32_t (*filter)(uint32_t cand, uint32_t src);
};
extern "C" uint32_t D2Common_12330_FindNearest(uint32_t unit, int32_t x, int32_t y, int32_t range, const D2Common12330Env* env);
