// D2CMP SpriteCache: move a node to the LRU tail. ESI = cache, EDI = node in retail; both are 32-bit retail
// addresses of the SpriteCache structs (flat guest image). Win32 and the in-module unlink are seams.
#include <cstdint>
#include <cstring>

struct D2CmpCae0Env {
    void (*enter)(uint32_t cs);                  // EnterCriticalSection([cache+0x30])
    void (*leave)(uint32_t cs);                  // LeaveCriticalSection([cache+0x30])
    void (*detach)(uint32_t node, uint32_t c);   // 0xC9F0: EAX = node, ECX = cache
    uint32_t (*tick)();                          // GetTickCount
    void (*fatal)(uint32_t code);                // Ordinal_10265 / Ordinal_10024 / _exit(-1)
};

static inline uint32_t rdc(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void wrc(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }

// 1.13c D2CMP+0x0CAE0
void d2cmp_cae0(uint32_t c, uint32_t n, const D2CmpCae0Env* e)
{
    if (!rdc(c + 0xc)) return;
    wrc(c + 0x38, rdc(c + 0x38) + 1);
    e->enter(rdc(c + 0x30));
    if (!n) { e->fatal(0xa4a); return; }
    e->detach(n, c);
    wrc(n + 0xc, rdc(n + 0xc) & ~1u);
    const uint32_t tail = rdc(c + 0x18);
    if (!tail) wrc(c + 0x14, n);
    else { wrc(tail + 0x14, n); wrc(n + 0x18, rdc(c + 0x18)); }
    wrc(c + 0x18, n);
    wrc(n + 0x10, e->tick());
    e->leave(rdc(c + 0x30));
}
