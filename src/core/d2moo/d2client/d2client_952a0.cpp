// 1.13c D2Client+0x952A0, written from the disassembly. EAX = unit, no stack arguments. key = [unit+0x60]
// (zero: nothing). For each of the 11 overlay slots (table 0x11BD80, 0x14 bytes each: x +0, y +8, panel w +0x10 byte, h +0x11 byte) the item
// is 0xC262(key, i); a present item gets its cell size (w, h bytes) from 0xC7F6(item, &w, &h, "CmnItems.h", 0x6DA), the centred position
// x = (slot.w - w*scale)/2 + slot.x, y likewise (division toward zero), is handed to 0x6B5A0 (EBX = item), and when the item is the cursor
// item [0x11BC38] with 0xC90A nonzero, 0xC97C's low byte nonzero and 0xC1A8(item, 0x800, 0x30B7, "inv.cpp") nonzero, 0x95140 draws its
// socket marks: 0x95140(item, 0x10B418, x, y). w and h are bytes of two dword out-slots (retail's adjacent byte locals); only the low byte is read.
#include "cl_env.h"
// 1.13c D2Client+0x952A0
extern "C" void d2c_952a0(uint32_t unit, uint32_t tab, uint32_t scale, uint32_t cur, const cl::Env& e) {
    const auto P = [](const void* p) { return (uint32_t)(uintptr_t)p; };
    const uint32_t key = e.R(unit + 0x60);
    if (!key) return;
    uint32_t t = tab;
    for (uint32_t i = 0; t < tab + 0xdc; i++, t += 0x14) {
        const uint32_t it = e.H(0xc262, key, i);
        if (!it) continue;
        uint32_t lw = 0, lh = 0;
        e.H(0xc7f6, it, P(&lw), P(&lh), 0xB20D478Cu, 0x6da);
        const int32_t sc = (int32_t)(e.R(scale) & 0xff);
        const int32_t xa = (int32_t)(e.R(t + 0x10) & 0xff) - (int32_t)(lw & 0xff) * sc;
        const int32_t ya = (int32_t)(e.R(t + 0x10) >> 8 & 0xff) - (int32_t)(lh & 0xff) * sc;
        const uint32_t x = (uint32_t)(xa / 2) + e.R(t), y = (uint32_t)(ya / 2) + e.R(t + 8);
        e.H(0x6b5a0, x, y, 0, 0, 0, 0, 0, 0, it, 0);
        if (e.R(cur) == it && e.H(0xc90a, it) && (e.H(0xc97c, it) & 0xff) && e.H(0xc1a8, it, 0x800, 0x30b7, 0xB20D476Cu))
            e.H(0x95140, it, 0xB2000000u | 0x10B418u, x, y);
    }
}
