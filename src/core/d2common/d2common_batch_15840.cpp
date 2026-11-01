// D2Common implementation batch: 1.13c RVA 0x15840, 0x17EB0, 0x3DD40, 0x7E9F0 (outdoor DRLG helpers).
// Transcribed from cached Ghidra decompiles. Blobs are 1.13c x86-layout byte images whose pointer fields
// hold 32-bit address tokens; the Env resolves tokens and supplies unmapped callees and retail tables.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace {
inline uint8_t* B(void* p) { return static_cast<uint8_t*>(p); }
inline const uint8_t* B(const void* p) { return static_cast<const uint8_t*>(p); }
inline uint8_t R8(const void* p, uint32_t o) { uint8_t v; std::memcpy(&v, B(p) + o, 1); return v; }
inline uint32_t R32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, B(p) + o, 4); return v; }
inline int32_t RI32(const void* p, uint32_t o) { int32_t v; std::memcpy(&v, B(p) + o, 4); return v; }
inline void W32(void* p, uint32_t o, uint32_t v) { std::memcpy(B(p) + o, &v, 4); }
inline void WI32(void* p, uint32_t o, int32_t v) { std::memcpy(B(p) + o, &v, 4); }
}

struct D2CommonBatchDrlgEnv {
    void* (*resolve)(uint32_t token);                  // 32-bit retail token -> blob address
    uint32_t (*tokenOf)(const void* p);                // blob address -> 32-bit token
    void* (*alloc)(uint32_t size);                     // Fog alloc (Ordinal 10045), caller zeroes nothing
    // 0x15840: indirect call through PTR_FUN_6fdeb244 (register args not recovered; (level, x, y) assumed).
    void (*visitCell)(void* pLevel, int32_t x, int32_t y);
    // 0x3DD40: 0x3D4A0 CalculatePathCoordinates(level, srcVertex, dstVertex).
    void (*calcPathCoords)(void* pLevel, void* pSrc, void* pDst);
    const int32_t* tabX;                               // DAT_6fde4050, 4 ints
    const int32_t* tabY;                               // DAT_6fde4040, 4 ints
    // 0x17EB0: 0x17D00 pool search. Returns result node (or null); *poolUsed is the node-pool count.
    void* (*search)(void* nodeBuf, int32_t limit, void* pLevel, int32_t* endpoints, int32_t* poolUsed);
    const int32_t* dirTable;                           // DAT_6fddc320, 12-byte stride
    uint32_t nodeTableToken;                           // DAT_6fddcab8 address token
    const int32_t* trimDx;                             // 0x3D920: DAT_6fde4020, 4 ints
    const int32_t* trimDy;                             // 0x3D920: DAT_6fde4030, 4 ints
};


namespace {
// 1.13c D2Common+0x3E230 direction index (local copy; row 0x3E230 is separate).
int32_t dirIndex(int32_t x2, int32_t y2, int32_t y1, int32_t x1) {
    int32_t dx = x2 - x1, dy = y2 - y1;
    uint32_t a = (uint32_t)dx, b = (uint32_t)dy;
    int32_t ax = dx < 0 ? -dx : dx, ay = dy < 0 ? -dy : dy;
    if (ax < ay * 2) {
        if (ax * 2 <= ay) {
            if (dx < 0) { a = 0xffffffffu; goto L; }
            a = a & 1;
        }
    } else if (dy < 0) b = 0xffffffffu;
    else b = b & 1;
    if ((int32_t)a < -1) a = 0xfffffffeu; else if (1 < (int32_t)a) a = 2;
L:
    if ((int32_t)b < -1) return (int32_t)(a * 5 + 10);
    if (1 < (int32_t)b) b = 2;
    return (int32_t)(a * 5 + 0xc + b);
}

void* newVertex(const D2CommonBatchDrlgEnv* env) {
    void* v = env->alloc(0x14);
    std::memset(v, 0, 0x14);
    return v;
}
}


// 1.13c D2Common+0x3DD40 -- place the second vertex set (outdoors+0x1e8..) for each path start, then
// convert from the first set (outdoors+0x80) via CalculatePathCoordinates.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_3dd40(void* pLevel, const D2CommonBatchDrlgEnv* env) { // 1.13c D2Common+0x3DD40
    if (!pLevel || !env || !env->resolve) return;
    uint8_t* od = B(env->resolve(R32(pLevel, 0x14)));
    const int32_t lx = RI32(pLevel, 0x1c), ly = RI32(pLevel, 0x20);
    int32_t gx = -1, gy = -1;
    int32_t n = RI32(od, 0x260);
    bool found = false;
    if (R8(od, 0) & 0x10) {
        // 0x7EA40: search column 1..w-2 (at fixed row = EDI) for cell type 0x1c with (byte2 & 0xf)==1.
        // EDI is an unrecovered register input; the decompile gives no source for it, so it is 0.
        const int32_t* g0 = static_cast<const int32_t*>(env->resolve(R32(od, 4)));
        const int32_t* r0 = static_cast<const int32_t*>(env->resolve(R32(od, 8)));
        const uint8_t* f2 = B(env->resolve(R32(od, 0x2c)));
        const int32_t* r2 = static_cast<const int32_t*>(env->resolve(R32(od, 0x30)));
        const int32_t w = RI32(od, 0x5c) - 1;
        for (int32_t i = 1; i < w; ++i) {
            if (g0[r0[i] + 0] == 0x1c && (f2[2 + (r2[i] + 0) * 4] & 0xf) == 1) { gx = 0; gy = i; found = true; break; }
        }
    }
    if (found) {
        const int32_t px = lx + 3 + gx * 8;
        for (int32_t i = 0; i < RI32(od, 0x260); ++i) {
            uint8_t* v = od + 0x1e8 + i * 0x14;
            WI32(v, 4, ly + 3 + gy * 8);
            if (px < RI32(od, 0x80 + i * 0x14)) { WI32(v, 0, px + 8); v[8] = 0; }
            else { WI32(v, 0, px); v[8] = 2; }
        }
    } else if (n > 0) {
        const int32_t* flags = static_cast<const int32_t*>(env->resolve(R32(od, 0x2c)));
        const int32_t* rows = static_cast<const int32_t*>(env->resolve(R32(od, 0x30)));
        int32_t cx = gx, cy = gy;
        for (int32_t i = 0; i < n; ++i) {
            uint8_t* v = od + 0x1e8 + i * 0x14;
            if (i == 0) {
                if (n == 1) { cx = RI32(od, 0x5c) / 2; cy = RI32(od, 0x60) / 2; }
                else {
                    int32_t sx = 0, sy = 0;
                    for (int32_t j = 0; j < n; ++j) {
                        sx += RI32(od, 0x80 + j * 0x14) - lx;
                        sy += RI32(od, 0x84 + j * 0x14) - ly;
                    }
                    cx = sx / (RI32(od, 0x260) << 3);
                    cy = sy / (RI32(od, 0x260) << 3);
                }
                int32_t px = gx, py = gy;
                bool ok = false;
                for (int32_t r = 0; r < 8 && !ok; ++r) {
                    for (int k = 0; k < 4; ++k) {
                        px = env->tabX[k] * r + cx;
                        py = env->tabY[k] * r + cy;
                        if (px > -1 && px < RI32(od, 0x5c) && py > -1 && py < RI32(od, 0x60) &&
                            (R32(flags, (rows[py] + px) * 4) & 0x1b81) == 0) { ok = true; break; }
                    }
                }
                WI32(od, 0x1e8, lx + 3 + px * 8);
                WI32(od, 0x1ec, ly + 3 + py * 8);
                od[0x1f0] = 4;
            } else {
                WI32(v, 0, RI32(od, 0x1e8));
                WI32(v, 4, RI32(od, 0x1ec));
                v[8] = 4;
            }
            n = RI32(od, 0x260);
        }
    }
    for (int32_t i = 0; i < RI32(od, 0x260); ++i)
        env->calcPathCoords(pLevel, od + 0x170 + i * 0x14, od + 0x1e8 + i * 0x14);
}

// 1.13c D2Common+0x7E9F0 -- 1 if column x (and x+1) has no cell with bit1 set in any row, else 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_7e9f0(void* pLevel, int32_t x, const D2CommonBatchDrlgEnv* env) { // 1.13c D2Common+0x7E9F0
    if (!pLevel || !env || !env->resolve) return 1;
    const uint8_t* od = B(env->resolve(R32(pLevel, 0x14)));
    const uint8_t* flags = B(env->resolve(R32(od, 0x2c)));
    const int32_t* rows = static_cast<const int32_t*>(env->resolve(R32(od, 0x30)));
    for (int32_t y = 0; y < RI32(od, 0x60); ++y) {
        const uint32_t o = (uint32_t)(rows[y] + x) * 4;
        if ((flags[o] & 2) || (flags[o + 4] & 2)) return 0;
    }
    return 1;
}

// 1.13c D2Common+0x3D920 -- (EAX = path index, ESI = level) trim one path's vertex chain: stamp the head
// with the first-set endpoints, then scale each interior vertex to world coordinates with a seeded jitter.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_3d920(void* pLevel, int32_t idx, const D2CommonBatchDrlgEnv* env) { // 1.13c D2Common+0x3D920
    if (!pLevel || !env || !env->resolve) return;
    uint8_t* od = B(env->resolve(R32(pLevel, 0x14)));
    auto step = [&]() { // seed = lo*0x6ac690c5 + hi, stored as 64 bits at level+0x1c4
        uint64_t v = (uint64_t)R32(pLevel, 0x1c4) * 0x6ac690c5u + R32(pLevel, 0x1c8);
        W32(pLevel, 0x1c4, (uint32_t)v); W32(pLevel, 0x1c8, (uint32_t)(v >> 32));
        return (uint32_t)v;
    };
    uint8_t* head = B(env->resolve(R32(od, 0x68 + idx * 4)));
    uint32_t c = step() & 3;
    if (!head) return;
    uint8_t* e = od + idx * 0x14;
    if (R8(od, 0x1f0 + idx * 0x14) != 4) {
        uint8_t* n = B(env->alloc(0x14));
        std::memset(n, 0, 0x14);
        W32(n, 0, R32(e, 0x1e8)); W32(n, 4, R32(e, 0x1ec)); W32(n, 0x10, env->tokenOf(head));
        W32(od, 0x68 + idx * 4, env->tokenOf(n));
    }
    W32(head, 0, R32(e, 0x170)); W32(head, 4, R32(e, 0x174));
    uint8_t* v = B(env->resolve(R32(head, 0x10)));
    if (!v) return;
    while (R32(v, 0x10) != 0) {
        const uint32_t s1 = step(); const int32_t dx = env->trimDx[c];
        const uint32_t s2 = step(); const int32_t dy = env->trimDy[c];
        c = (c + 1) & 3;
        const int32_t y0 = RI32(v, 4), x0 = RI32(v, 0);
        WI32(v, 0, RI32(pLevel, 0x1c) + x0 * 8 + 3 + (int32_t)((s1 & 1) + 2) * dx);
        WI32(v, 4, RI32(pLevel, 0x20) + y0 * 8 + 3 + (int32_t)((s2 & 1) + 2) * dy);
        v = B(env->resolve(R32(v, 0x10)));
        if (!v) return;
    }
    W32(v, 0, R32(e, 0xf8)); W32(v, 4, R32(e, 0xfc));
    uint8_t* n = B(env->alloc(0x14));
    std::memset(n, 0, 0x14);
    W32(n, 0, R32(e, 0x80)); W32(n, 4, R32(e, 0x84));
    W32(v, 0x10, env->tokenOf(n));
}
