// 1.13c D2Common+0x315E0 (UNITS_TestCollisionByCoordinates) with injectable callees. The callee struct lets a probe
// stand in for 0x2FE10, 0x30F80 and 0x30730 and log their calls; the real build wires them to the engine's own functions.
#pragma once
#include <cstdint>

struct D2Collision315E0Callees {
    void* ctx;
    uint32_t (*getRoom)(void* ctx, uint32_t unit);                                    // 0x2FE10: stdcall, one word (unit)
    uint32_t (*getSize)(void* ctx, uint32_t unit);                                    // 0x30F80: stdcall, one word (unit)
    uint32_t (*test)(void* ctx, uint32_t x, uint32_t ux, uint32_t uy, uint32_t size,
                     uint32_t y, uint32_t room, uint32_t flags);                     // 0x30730: EAX = x, ECX = ux, EDX = 2, stack uy, size, y, room, flags (ret 0x14)
};

extern "C" uint32_t D2Common_315E0_CollisionInjected(const D2Collision315E0Callees* c, uint32_t unit,
                                                    uint32_t x, uint32_t y, uint32_t flags);
