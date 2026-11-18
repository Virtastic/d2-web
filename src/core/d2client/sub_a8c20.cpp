// 1.13c D2Client+0xA8C20 (265 B): allocate a 0x34-byte light node and push it on the list head (DAT_6fbcbbec, next = +0x28); count in DAT_6fbcbbf0.
// Retail convention (disassembly): EDI = unit (or 0), EBX = kind, stdcall stack s0 = radius, s1..s4 = amp, r, g, b (ret 0x14). Alloc is thunk 0xBF7A
// (ECX = size, EDX = source file string, stack = line, 0); 0xC1AE/0xC232 (stdcall unit) give the unit's x/y. Returns the node (0 for radius <= 0).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern "C" EMSCRIPTEN_KEEPALIVE uintptr_t d2_client_a8c20(
    int32_t radius, uint32_t amp, uint32_t rr, uint32_t gg, uint32_t bb, uint32_t kind, uintptr_t unit, uint32_t* head, uint32_t* count,
    uintptr_t (*alloc)(uint32_t size, uint32_t line), uint32_t (*unit_x_c1ae)(uintptr_t), uint32_t (*unit_y_c232)(uintptr_t)) {
    if (radius <= 0) return 0;
    int32_t n = radius < 0x12 ? radius : 0x12;
    ++*count;
    uint8_t* p = (uint8_t*)alloc(0x34, 0xff);
    uint32_t t = 6, id = 0xffffffffu, fl = 0;
    if (unit) { std::memcpy(&t, (const void*)unit, 4); std::memcpy(&id, (const void*)(unit + 0xc), 4); uint32_t f; std::memcpy(&f, (const void*)(unit + 0xc4), 4); fl = f >> 0x15 & 1; }
    auto W = [&](uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); };
    W(0, t); W(4, id);
    p[8] = fl ? 1 : 0;
    W(0x10, (unit_x_c1ae(unit) >> 13) + 4);
    W(0x14, (unit_y_c232(unit) >> 13) + 4);
    p[0x24] = (uint8_t)amp;
    W(0x18, (uint32_t)n * 8); W(0x1c, (uint32_t)n * 8);
    p[0x25] = (uint8_t)rr; p[0x26] = (uint8_t)gg; p[0x27] = (uint8_t)bb;
    W(0xc, kind); W(0x20, 0); W(0x28, *head); W(0x2c, 0);
    uintptr_t cells = 0;
    if (kind == 2) { uint32_t w = (uint32_t)(2 * n + 1); cells = alloc(w * w * 4, 0x116); }
    W(0x30, (uint32_t)cells);
    *head = (uint32_t)(uintptr_t)p;
    return (uintptr_t)p;
}
