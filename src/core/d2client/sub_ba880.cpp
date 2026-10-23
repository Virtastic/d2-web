// 1.13c D2Client+0xBA880: p (param_1/ECX) = unit. Owner lookup (FUN_6fb560d0); none -> B17C0 only. Else Ordinal_10818(p)==0 ->
// Ordinal_11018(p), B5380 (owner), seed step, n = seed%0x23+10. Then Ordinal_10019(p, n-1) and B17C0(p).
// Ordinals unverified (named as in the decompile). When 10818 != 0 the decompile passes its (nonzero) return as n.
#include <cstdint>
#include <emscripten.h>
struct Ba880Env {
    uint32_t  (*rd32)(uintptr_t);  void (*wr32)(uintptr_t, uint32_t);
    uintptr_t (*owner)(uintptr_t);          // FUN_6fb560d0
    int32_t   (*o10818)(uintptr_t); void (*o11018)(uintptr_t); void (*o10019)(uintptr_t, int32_t);
    void      (*b5380)(uintptr_t);          // FUN_6fb65380(owner)
    void      (*b17c0)(uintptr_t);          // FUN_6fb617c0
};
static Ba880Env g_ba880;
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_ba880_set_env(const Ba880Env* e) { g_ba880 = *e; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_ba880(uintptr_t p) {
    const Ba880Env& E = g_ba880;
    uintptr_t o = E.owner(p);
    if (!o) { E.b17c0(p); return; }
    int32_t n = E.o10818(p);
    if (n == 0) {
        E.o11018(p);
        E.b5380(o);
        uint64_t s = (uint64_t)E.rd32(p + 0x20) * 0x6AC690C5u + E.rd32(p + 0x24);
        E.wr32(p + 0x20, (uint32_t)s); E.wr32(p + 0x24, (uint32_t)(s >> 32));
        n = (int32_t)((uint32_t)s % 0x23) + 10;
    }
    E.o10019(p, n - 1);
    E.b17c0(p);
}
