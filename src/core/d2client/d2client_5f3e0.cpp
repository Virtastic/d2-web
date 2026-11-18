// 1.13c D2Client+0x5F3E0: open this character's automap slot file. int(void), returns the
// handle of "<dir><name>.ma<slot>" or -1. "<dir><name>.map" holds a 0x18-byte header {0xC, next slot, owner ids[4]}: a known
// owner keeps its slot, a new one takes `next` (next = (next+1) % 4, C remainder) and the stale .ma file is removed; a short or
// wrong-version header is rebuilt and all four .ma files removed. Every string/file operation is a callee (thunks and 0x3146).
#include <cstdint>
#include "cl_env.h"
namespace {
constexpr uint32_t G(uint32_t rva) { return 0x6FAB0000u + rva; }
inline uint32_t P(const volatile void* p) { return (uint32_t)(uintptr_t)p; }
constexpr uint32_t kT = 0xB2000000u;
constexpr uint32_t kName = kT | 0x119928, kSub = kT | 0x119860;
constexpr uint32_t kFmtDir = kT | 0xD5374, kFmtMap = kT | 0xD5368, kFmtMa = kT | 0xD535C;
// a cdecl call whose argument words the vector logs whole (hookblk RVA k 5)
uint32_t callK(const cl::Env& e, uint32_t rva, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4) {
    volatile uint32_t a[5] = { s0, s1, s2, s3, s4 };
    const uint32_t r = e.H(rva, s0, s1, s2, s3, s4);
    if (e.blk) e.blk(e.ctx, rva, P(a), 5);
    return r;
}
uint32_t openF(const cl::Env& e, uint32_t path) { return e.H(0xC02E, 0, 0, 4, 0x80, 0, 0, path, 0xC0000000u); }
}
extern "C" uint32_t d2c_5f3e0(const cl::Env& e) {
    volatile uint32_t got = 0, hdr[6] = {};
    volatile char B[0x104] = {}, A[0x104] = {}, C[0x104] = {};
    const uint32_t a = P(A), b = P(B), c = P(C), hp = P(hdr), gp = P(&got);
    e.H(0xBF98, 0, 0, 0, 0, 0, 0, a, 0x104);                       // save directory into A
    uint32_t h = 0xFFFFFFFFu;
    if (e.R8(G(0x119860))) {
        callK(e, 0xBEA2, c, 0x104, kFmtDir, a, kSub);              // C = A + subdir + "\"
        e.H(0xC022, 0, 0, 0, 0, 0, 0, c);                           // create it
        callK(e, 0xBEA2, b, 0x104, kFmtMap, c, kName);             // B = C + name + ".map"
        h = openF(e, b);
        if (h != 0xFFFFFFFFu) e.H(0xBE96, a, c, 0x104);             // A = C
    }
    if (h == 0xFFFFFFFFu) {
        // four-argument call: word 4 is the caller's saved EDI (a stack pointer under the capture harness)
        callK(e, 0x3146, b, kFmtMap, a, kName, gp - 0x10);
        h = openF(e, b);
        if (h == 0xFFFFFFFFu) return h;
    }
    e.H(0xC034, 0x18, gp, 0, 0, 0, 0, h, hp);                     // read the header
    const uint32_t owner = e.R(e.R(G(0x11C3BC)));
    int32_t slot;
    if (got == 0x18 && hdr[0] == 0xC) {
        int32_t i = 0;
        while (i < 4 && hdr[2 + i] != owner) ++i;
        if (i != 4) {
            slot = i;
            e.H(0xC03A, 0, 0, 0, 0, 0, 0, h);
            callK(e, 0x3146, b, kFmtMa, a, kName, (uint32_t)slot);
            return openF(e, b);
        }
        slot = (int32_t)hdr[1];
        hdr[1] = (uint32_t)((slot + 1) % 4);
        if (slot >= 0 && slot < 4) hdr[2 + slot] = owner;   // retail writes [hdr+8+4*slot] unchecked; the file only ever holds 0..3
        callK(e, 0x3146, b, kFmtMa, a, kName, (uint32_t)slot);
        e.H(0xC040, 0, 0, 0, 0, 0, 0, b);                           // remove the stale slot file
    } else {
        hdr[0] = 0xC; hdr[1] = 1; hdr[2] = owner;
        for (uint32_t i = 0; i < 4; ++i) { callK(e, 0x3146, b, kFmtMa, a, kName, i); e.H(0xC040, 0, 0, 0, 0, 0, 0, b); }
        slot = 0;
    }
    e.H(0xC028, 0, 0, 0, 0, 0, 0, h, 0);                            // seek 0
    e.H(0xC046, 0x18, gp, 0, 0, 0, 0, h, hp);                       // write the header back
    if (e.blk) e.blk(e.ctx, 0xC046, hp, 6);
    e.H(0xC03A, 0, 0, 0, 0, 0, 0, h);
    callK(e, 0x3146, b, kFmtMa, a, kName, (uint32_t)slot);
    return openF(e, b);
}
