// 1.13c D2Client+0x87560 wardenLoadModule: ECX = packed module size, stack (src, rc4Key) (ret 8); returns 1 when the module
// is loaded and published in the retail slot 0x11BF50 (g50 = that slot's address here), else 0. The SEH frame only guards
// the 0x14-byte object constructor; it has no effect on a normal return and is not modelled.
#include "cl_env.h"
namespace {
const uint32_t kPath = 0xB2000000u | 0xD4938;   // "..\Source\D2Client\WARDEN\WardenClient.cpp"
uint32_t alloc(const cl::Env& e, uint32_t n, uint32_t line) { return e.H(0xbe66, n, kPath, line, 0); }   // Storm #401
void release(const cl::Env& e, uint32_t p, uint32_t line) { e.H(0xbe6c, p, kPath, line, 0); }             // Storm #403
void unload(const cl::Env& e, uint32_t m) { e.H(0x9e630, 0, 0, 0, 0, 0, 0, 0, 0, 0, m); }               // ESI = module object
}  // namespace
extern "C" uint32_t d2c_87560(uint32_t size, uint32_t src, uint32_t key, uint32_t g50, const cl::Env& e) {
    // the stack string "Maiev.mod", stored obfuscated and decoded by a byte multiply with 0xD3
    static const uint8_t kEnc[10] = { 0x5f, 0x7b, 0x53, 0xe7, 0xf2, 0x5a, 0xbf, 0x75, 0x8c, 0x00 };
    uint32_t name[3] = {};
    for (int i = 0; i < 10; ++i) name[i >> 2] |= (uint32_t)(uint8_t)(kEnc[i] * 0xd3) << ((i & 3) * 8);
    uint32_t rc4[0x42] = {};   // the 0x102-byte RC4 state on the stack
    const uint32_t ns = (uint32_t)(uintptr_t)name, st = (uint32_t)(uintptr_t)rc4;
    if (e.R(g50)) {
        unload(e, e.R(g50));
        const uint32_t old = e.R(g50);
        if (old) { unload(e, old); release(e, old, 0x117); }
        e.W(g50, 0);
    }
    const uint32_t p = alloc(e, size, 0x16e);
    e.H(0xbee4, p, src, size);                                   // Storm #491 copy (runs before the null test)
    if (!p) return 0;
    e.H(0xc78c0, key, 0x10); e.Ej(0xc78c0, st);                  // RC4 key schedule, EDI = state
    e.H(0x1920, p, size, 0, 0, 0, st);                           // RC4 over the copy, EAX = state
    const uint32_t ok = e.H(0x872d0, ns, p, 0xB2000000u | 0xD2CA8, 0xB2000000u | 0xD2DA8, 4, size, 0);
    e.Ej(0x872d0, 0x100);
    if (e.blk) e.blk(e.ctx, 0x872d0, ns, 3);
    if (!ok) { release(e, p, 0x17c); return 0; }
    uint32_t n = e.R(p);
    const uint32_t q = alloc(e, n, 0x182);
    if (!q) { release(e, p, 0x185); return 0; }
    const uint32_t rc = e.H(0xc8220, 0, 0, 0, 0, 0, p + 4, size - 4, q, 0, (uint32_t)(uintptr_t)&n);   // inflate p+4 into q
    if (rc) { release(e, p, 0x18d); release(e, q, 0x18e); return 0; }
    release(e, p, 0x193);
    const uint32_t m = alloc(e, 0x14, 0x10d);
    const uint32_t s = m ? e.H(0x19b0, m) : 0;
    if (!s) { release(e, q, 0x199); return 0; }
    if ((e.H(0x9e690, s, 0, 0, 0, 0, 0, q) & 0xff) == 0) {
        release(e, q, 0x1a0);
    } else {
        const uint32_t img = e.R(s);
        uint32_t entry = 0;
        if (img) {
            const uint32_t k = 1 - e.R(img + 0x18);
            if (k < e.R(img + 0x14)) entry = e.R(e.R(img + 0x10) + img + k * 4) + img;
        }
        e.W(s + 0x10, entry);
        if (entry) { release(e, q, 0x1b0); e.W(g50, s); return 1; }
        unload(e, s);
        release(e, q, 0x1aa);
    }
    unload(e, s);
    release(e, s, 0x117);
    return 0;
}
