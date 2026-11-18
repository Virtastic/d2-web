// 1.13c D2Client+0xA9100 (887 B): light-record grid fill. ESI = light node (+0x10 x, +0x14 y, +0x18 radius, +0x2C done flag, +0x30 out buffer), no stack args.
// Radius r must be 0 < r < 0x100. Clears the 0x400-dword grid (D2Client+0xFCDE8, row stride 0x40 dwords), asks 0xA8D30(rec) for the unit and 0xC1F0 for its room,
// fills the 64x64 occlusion map (D2Client+0x100DE8) from 0xC4A8(room, x, y, 0x22) around the unit (0x1630/0x1660 with ECX = unit), and, when r>>3 >= 2, runs the
// ring sweep: 8 pairs of 0xA8470(EAX = node, EDX = a, stack = b) then 0xA8310(EAX, ECX). Finally copies (M+1)^2 grid dwords, M = (2r)>>3, from row (0x20 - (r>>3))
// (stride 0x41 dwords for the first row, then 0x40) into the node's out buffer and sets the done flag. Fatal exits (0x414 no room, 0x454 done set, 0x455 no buffer) call `fatal`.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static uint32_t R32(uintptr_t p) { uint32_t v; std::memcpy(&v, (const void*)p, 4); return v; }
static void W32(uintptr_t p, uint32_t v) { std::memcpy((void*)p, &v, 4); }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a9100(
    uintptr_t rec, uintptr_t grid, uintptr_t occl, uintptr_t (*unit_a8d30)(uintptr_t rec), uint32_t (*room_c1f0)(uintptr_t unit),
    int32_t (*ux_1630)(uintptr_t unit), int32_t (*uy_1660)(uintptr_t unit), uint32_t (*c4a8)(uint32_t room, int32_t x, int32_t y, uint32_t mask),
    void (*a8470)(uintptr_t rec, int32_t edx, int32_t stk), void (*a8310)(int32_t eax, int32_t ecx), void (*fatal)(uint32_t msg)) {
    int32_t r = (int32_t)R32(rec + 0x18);
    if (r <= 0 || r >= 0x100) return;
    int32_t M = (2 * r) >> 3;
    for (uint32_t i = 0; i < 0x400; i++) W32(grid + i * 4, 0);
    int32_t lim = (int32_t)R32(rec + 0x18) >> 3, X = (int32_t)R32(rec + 0x10) >> 3, Y = (int32_t)R32(rec + 0x14) >> 3;
    uintptr_t u = unit_a8d30(rec);
    if (!u) return;
    uint32_t room = room_c1f0(u);
    if (!room) fatal(0x414);
    int32_t ux = ux_1630(u) - 0x20, uy = uy_1660(u) - 0x20;
    for (int32_t j = 0; j < 0x40; j++)
        for (int32_t i = 0; i < 0x40; i++) W32(occl + (j * 0x40 + i) * 4, (int16_t)c4a8(room, i + ux, j + uy, 0x22) ? 0x10 : 0);
    if (lim >= 2) {
        int32_t a24 = Y * 8 + 0x14, a20 = X * 8 + 0x14, b2c = Y * 8 - 0xc, b28 = X * 8 - 0xc, v6 = 0x1e, ebp = 0x22;
        do {
            int32_t c14 = Y * 8 + 4, c1c = c14, c10 = X * 8 + 4, c18 = c10, ebx = 0x20, edi = 0x20;
            for (int32_t n = ebp - 0x1f; n != 0; n--) {
                a8470(rec, c18, b2c); a8310(v6, edi);
                a8470(rec, c10, b2c); a8310(v6, ebx);
                a8470(rec, c18, a24); a8310(ebp, edi);
                a8470(rec, c10, a24); a8310(ebp, ebx);
                a8470(rec, a20, c1c); a8310(edi, ebp);
                a8470(rec, a20, c14); a8310(ebx, ebp);
                a8470(rec, b28, c1c); a8310(edi, v6);
                a8470(rec, b28, c14); a8310(ebx, v6);
                c10 += 8; c14 += 8; c18 -= 8; c1c -= 8; edi--; ebx++;
            }
            a20 += 8; a24 += 8; b28 -= 8; b2c -= 8; v6--; ebp++;
        } while (ebp - 0x20 <= lim);
    }
    if (R32(rec + 0x2C)) fatal(0x454);
    uintptr_t dst = R32(rec + 0x30);
    if (!dst) fatal(0x455);
    if (M >= 0) {
        uintptr_t row = grid + (uint32_t)(0x20 - lim) * 0x104;
        for (int32_t k = M + 1; k != 0; k--, row += 0x100)
            for (int32_t i = 0; i <= M; i++, dst += 4) W32(dst, R32(row + i * 4));
    }
    W32(rec + 0x2C, 1);
}
