// D2Game sub_2F300 -- unit-state set (row s3334), transcribed from the cached decompile.
// Register args explicit: unit = ESI, game = EBX, value = stack slot stored at +0xC. Records are opaque
// host blocks read at retail offsets; callees are Env seams. Fatal: Ordinal 10265(code)->10024(text)->_exit(-1).
#include <cstdint>
#include <cstring>
#include <unistd.h>

static inline uint32_t r32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void w32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

struct D2GameSub2F300Env {
    void*    (*ptr)(uint32_t v);
    void*    (*ord10296)();                       // current-context record
    uint32_t (*ord10265)(uint32_t code);
    void     (*ord10024)(uint32_t text, uint32_t code);
    uint32_t fatalText;                           // DAT_6fd19ff9
    void     (*sub8E880)(void* unit);
    uint32_t (*sub1180)(void* p);
    void     (*ord11090)();
    void     (*sub2E7D0)();
    void     (*ord10169)();
    uint32_t handlers[0x50];                      // DAT_6fd2e050 retail fn pointers (0 = none)
    bool     (*isBadCodePtr)(uint32_t fn);
    void     (*callHandler)(uint32_t fn);
};

[[noreturn]] static void fatal(const D2GameSub2F300Env* e, uint32_t code) {
    e->ord10024(e->fatalText, e->ord10265(code));
    _exit(-1);
}

// 1.13c D2Game+0x2F300
void D2Game_sub_2F300(const D2GameSub2F300Env* e, void* unit, void* game, uint32_t value) {
    uint8_t* c = (uint8_t*)e->ord10296();
    if (!unit) fatal(e, 0xa48);
    uint8_t* u = (uint8_t*)unit;
    int32_t idx = (int32_t)r32(u, 0x10);
    uint32_t f = r32(u, 0xc4);
    uint32_t g = f & 0xfffffff7;
    w32(u, 0xc, value); u[0x78] = 0; w32(u, 0xc4, g); w32(u, 0xa4, 0);
    if (idx >= 8) fatal(e, 0x240);
    g = c[0x128] ? (g | 4) : (f & 0xfffffff3);
    w32(u, 0xc4, g);
    ((uint8_t*)e->ptr(r32(u, 0x14)))[4] = 0;
    uint16_t cls = (uint16_t)r32(u, 4);
    if (r32(u, 0xdc)) e->sub8E880(u);
    w32(u, 0xdc, 0);
    if (cls >= 0x23d) fatal(e, 0x260);
    uint8_t k = c[0x1b1];
    if (k >= 0x50) fatal(e, 0x262);
    uint32_t puVar2 = r32(game, 0x10f0);
    uint32_t fn = e->handlers[k];
    if (fn) {
        if (e->isBadCodePtr(fn)) fatal(e, 0x26a);
        e->callHandler(fn);
    }
    f = r32(u, 0xc4);
    w32(u, 0xc4, c[idx + 0xc4] ? (f | 2) : (f & 0xfffffffd));
    if (c[0x13d] && !((r32(u, 0xc4) >> 7) & 1)) {
        if (e->sub1180(e->ptr(puVar2)) == 0) e->ord11090();
    }
    e->sub2E7D0();
    e->ord10169();
}
