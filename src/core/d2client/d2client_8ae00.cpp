// 1.13c D2Client+0x8AE00 (448 B, no args): star-field draw and step. First call (0x11BF24 clear): 0xC634 (ECX = seed ptr 0xE1304),
// 0xC9B2 (ECX = 0x10B9CC) whose result seeds 0xC3B2 (ECX = seed ptr, EDX = it); 8 palette bytes (0x10EAE0) from 0xD390 (r, g, b):
// each channel (seed roll & 0x3F) - 0x20 + acc/7 + 0x80 clamped to 0xFF, acc = 0, 0x80 .. 0x380; then 0x8AC40 (EAX = 0, EDI = i)
// for the 256 stars; the flag is set. Every call: 0xD102 (x, y, x, y, colour byte 0x10B440[i], 0xFF) per star, x 0x10EAF0[i],
// y 0x10B5A0[i]. When now - 0x10B598 > 0x28: x += dx (0x10EEF0[i]), a negative x respawns via 0x8AC40 (EAX = 1, EDI = i), and
// 0x10B598 = now.
#include "cl_env.h"
extern "C" void d2c_8ae00(uint32_t now, const cl::Env& e) {
    constexpr uint32_t B = 0x6FAB0000u, kInit = B + 0x11BF24, kSeed = B + 0xE1304, kPal = B + 0x10EAE0, kX = B + 0x10EAF0,
                       kY = B + 0x10B5A0, kCol = B + 0x10B440, kDx = B + 0x10EEF0, kLast = B + 0x10B598;
    if (!e.R(kInit)) {
        e.H(0xc634, 0, 0, 0, 0, 0, 0, e.R(kSeed));
        const uint32_t v = e.H(0xc9b2, 0, 0, 0, 0, 0, 0, e.R(B + 0x10B9CC));
        e.H(0xc3b2, 0, 0, 0, 0, 0, 0, e.R(kSeed), v);
        uint32_t i = 0;
        for (int32_t acc = 0; acc < 0x400; acc += 0x80, ++i) {
            const int32_t base = acc / 7 + 0x80;
            int32_t c[3];
            for (int k = 0; k < 3; ++k) { c[k] = (int32_t)(e.Lcg(e.R(kSeed)) & 0x3f) - 0x20 + base; if (c[k] > 0xff) c[k] = 0xff; }
            const uint32_t b = e.H(0xd390, (uint32_t)c[0], (uint32_t)c[1], (uint32_t)c[2]) & 0xff;
            const uint32_t q = (kPal + i) & ~3u, sh = ((kPal + i) & 3u) * 8;
            e.W(q, (e.R(q) & ~(0xffu << sh)) | b << sh);
        }
        for (uint32_t n = 0; n < 0x100; ++n) { e.H(0x8ac40, 0, 0, 0, 0, 0, 0); e.Ej(0x8ac40, n); }
        e.W(kInit, 1);
    }
    for (uint32_t n = 0; n < 0x100; ++n) {
        const uint32_t x = e.R(kX + 4 * n), y = e.R(kY + 4 * n), c = e.R((kCol + n) & ~3u) >> (((kCol + n) & 3u) * 8) & 0xff;
        if (e.xs) e.xs(e.ctx, 0xff, 0);
        e.H(0xd102, x, y, x, y, c);
        const uint32_t s[6] = { x, y, x, y, c, 0xff };
        if (e.blk) e.blk(e.ctx, 0xd102, (uint32_t)(uintptr_t)s, 6);
    }
    if (now - e.R(kLast) > 0x28) {
        for (uint32_t n = 0; n < 0x100; ++n) {
            const uint32_t v = e.R(kX + 4 * n) + e.R(kDx + 4 * n);
            e.W(kX + 4 * n, v);
            if ((int32_t)v < 0) { e.H(0x8ac40, 0, 0, 0, 0, 0, 1); e.Ej(0x8ac40, n); }
        }
        e.W(kLast, now);
    }
}
