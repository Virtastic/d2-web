// Probe for the join loading-screen frame routine: stdcall, one word.
// Arg p = r[0]. Buffers: r[14] is the hook log (2000 bytes = 49 records), r[16] holds the 14 pinned
// globals, r[18] is the pointer word that routine reads, r[20] is its scratch target.
// Every import call goes through HookEnv, so the log is the capture's hook log.
#include <emscripten.h>
#include <cstring>
#include "hookenv.h"
#include "../d2client/d2client_batch_c0450.h"

using namespace d2client_batch_c0450;

namespace {

HookEnv* H;
uint8_t* G;     // blob g
uint8_t* PB;    // blob pb

// The descriptor's gmap lines: VA = 0x6FAB0000 + RVA. pb marks the one entry that lives in blob pb.
struct Gm { uint32_t va, off; bool pb; };
const Gm GM[] = {
    {0x6FBCC394, 0x00, false}, {0x6FBC97FC, 0x04, false}, {0x6FBC9800, 0x08, false}, {0x6FBCB9D0, 0x0C, false},
    {0x6FBCBBB4, 0x10, false}, {0x6FBCBBB8, 0x14, false}, {0x6FBAAD78, 0x18, false}, {0x6FBCC4F8, 0x1C, false},
    {0x6FB8BC48, 0x20, false}, {0x6FB8BC4C, 0x24, false}, {0x6FBCC3DC, 0x28, false}, {0x6FBCC3E0, 0x2C, false},
    {0x6FBCC3F8, 0x30, false}, {0x6FBCC3FC, 0x34, false}, {0x6FBCB980, 0x00, true},
};

uint8_t* ptr(uint32_t va) {
    for (const Gm& m : GM) if (va >= m.va && va < m.va + 4) return (m.pb ? PB : G) + m.off + (va - m.va);
    return nullptr;   // an unpinned global reads 0 and a write is dropped
}
uint32_t ld(P a, int n) { uint8_t* p = ptr((uint32_t)a); uint32_t v = 0; if (p) memcpy(&v, p, n); return v; }
void st(P a, uint32_t v, int n) { uint8_t* p = ptr((uint32_t)a); if (p) memcpy(p, &v, n); }

uint32_t rd32f(P a) { return ld(a, 4); }
void wr32f(P a, uint32_t v) { st(a, v, 4); }
uint8_t rd8f(P a) { return (uint8_t)ld(a, 1); }
void wr8f(P a, uint8_t v) { st(a, v, 1); }

uint32_t getTickf() { return H->tick(5000); }
void f4b00f() { H->h(0x14b00, 0, 0, 0, 0, 0); }
int32_t o10043zf() { return (int32_t)H->h(0xd0a2, 0, 0, 0, 0, 0); }
void o10021zf(int32_t a) { H->h(0xd090, 0, 0, 0, 0, 0, (uint32_t)a); }
void o10062zf() { H->h(0xd3a2, 0, 0, 0, 0, 0); }
int32_t o10085f(int32_t a, int32_t b, int32_t c, int32_t d) {
    return (int32_t)H->h(0xd0a8, 0, 0, 0, 0, 0, (uint32_t)a, (uint32_t)b, (uint32_t)c, (uint32_t)d);
}
int32_t o10086f(uint32_t v) { return (int32_t)H->h(0xc0c4, 0, 0, 0, 0, 0, v); }
void o10040f() { H->h(0xd08a, 0, 0, 0, 0, 0); }
void o10014f(int32_t a, int32_t b, int32_t c, int32_t d, uint32_t e, int32_t f) {
    H->h(0xd0ba, 0, 0, 0, 0, 0, (uint32_t)a, (uint32_t)b, (uint32_t)c, (uint32_t)d, e, (uint32_t)f);
}
// 10041 takes a stack CellContext (its ctx pointer is the stack word 0). C1760 writes only word 0 (the frame): the
// record is masked to word 0, as the descriptor's `hookblk 0xd096 0 14 0x1` says (the other words are stack residue).
void o10041f(const int32_t* ctx, int32_t x, int32_t y, uint32_t pal, int32_t m, int32_t z) {
    H->h(0xd096, 0, 0, 0, 0, 0, 0xB3000000u, (uint32_t)x, (uint32_t)y, pal, (uint32_t)m, (uint32_t)z);
    uint32_t masked[14] = {};
    masked[0] = (uint32_t)ctx[0];
    H->blk(0xd096, (uint32_t)(uintptr_t)masked, 14);
}

}  // namespace

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_c1760(uint32_t* r) {
    static const uint32_t sc[] = {0xc0c4, 10, 0xd0a8, 1, 0};   // 10086 returns F = 10 frames; 10085 returns 1; the rest 0
    HookEnv h((uint32_t*)(uintptr_t)r[14], sc); H = &h; h.cap = 49;
    G = (uint8_t*)(uintptr_t)r[16]; PB = (uint8_t*)(uintptr_t)r[18];
    *(uint32_t*)PB = r[20];   // patch pb+0 @ptb

    Env e{};
    e.rd32 = rd32f; e.wr32 = wr32f; e.rd8 = rd8f; e.wr8 = wr8f;
    e.getTick = getTickf; e.f4b00 = f4b00f;
    e.o10043z = o10043zf; e.o10021z = o10021zf; e.o10062z = o10062zf;
    e.o10085 = o10085f; e.o10086 = o10086f; e.o10040 = o10040f;
    e.o10014 = o10014f; e.o10041 = o10041f;
    sub_C1760(e, (int32_t)r[0]);
    r[4] = 0;
}
