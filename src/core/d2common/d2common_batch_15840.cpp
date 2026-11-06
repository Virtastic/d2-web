// D2Common implementation batch: 1.13c RVA 0x15840, 0x17EB0, 0x7E9F0 (outdoor DRLG helpers).
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

}


// 1.13c D2Common+0x7E9F0 -- 1 if column x (and x+1) has no cell with bit1 set in any row, else 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_7e9f0_env(void* pLevel, int32_t x, const D2CommonBatchDrlgEnv* env) { // 1.13c D2Common+0x7E9F0
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

