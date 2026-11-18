// 1.13c D2Client+0xADD70 (320 B, written from the disassembly): ECX = packet (byte +1 index, dword +2, byte +6 length, byte +7 type, bit data at +8).
// 0xA4E20(EAX = dword&0x7F, ECX = dword, EDX = 0x10A608 table + index*0x200, stack index) returns the state block (0: return).
// 0x73760(type); Fog bit reader on a 20-byte stack reader: #10126(reader, packet+8, length-8); loop: n = #10130(reader, 9); n >= 0x1FF: leave;
// n < 0 or n >= *(dt+0xBD4) or no States row (dt+0xBCC, stride 0x144) or row byte +8 == 0: return. Byte +9 != 0: b = #10129(reader, row+9) else 0.
// Byte +8 < 0x20 and (byte [bm+4] & row byte +4): x = #10129(reader, row+8) else x = #10130(reader, row+8). acc = 0x72610(EAX = n, ECX = type, EDX = block; acc, x, b).
// After the 0x1FF terminator: 0x72530(EAX = block; type).
#include "cl_env.h"
extern "C" void d2c_add70(const cl::Env& e, uint32_t pk) {
    const uint32_t id1 = e.R8(pk + 1), v = e.R(pk + 2);
    const uint32_t blk = e.H(0xa4e20, id1, 0, 0, 0, 0, v & 0x7f, v, 0x6fbba608u + id1 * 0x200);
    if (!blk) return;
    const uint32_t ty = e.R8(pk + 7);
    e.H(0x73760, ty);
    volatile uint32_t rd[5] = {ty, 0, 0, 0, 0};
    const uint32_t rp = (uint32_t)(uintptr_t)&rd[0];
    e.H(0xbf9e, rp, pk + 8, e.R8(pk + 6) - 8);
    uint32_t acc = 0;
    int32_t n = (int32_t)e.H(0xbfb0, rp, 9);
    while (n < 0x1ff) {
        if (n < 0) return;
        if ((int32_t)e.R(e.dt + 0xbd4) <= n) return;
        const uint32_t row = (uint32_t)n * 0x144 + e.R(e.dt + 0xbcc);
        if (!row) return;
        if (e.R8(row + 8) == 0) return;
        uint32_t b = 0;
        if (e.R8(row + 9) != 0) b = e.H(0xbfaa, rp, e.R8(row + 9));
        const uint32_t w = e.R8(row + 8);
        uint32_t x;
        if (w < 0x20 && (e.R8(e.bm + 4) & e.R8(row + 4)) != 0) x = e.H(0xbfaa, rp, w);
        else x = e.H(0xbfb0, rp, w);
        acc = e.H(0x72610, acc, x, b, 0, 0, (uint32_t)n, ty, blk);
        n = (int32_t)e.H(0xbfb0, rp, 9);
    }
    e.H(0x72530, ty, 0, 0, 0, 0, blk);
}
