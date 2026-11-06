// 1.13c D2Client+0x7C770 (382 B): build the two-label window. Globals +0x113A70 (window), +0x113A74 (object), +0xDBC48/4C (width, height).
// Window already set: nothing. Else create (0xA3E40, 11 stdcall words), store; NULL -> fatal 0x529. Label 1 (string 0xFC3, ECX), label 2 (0x1030)
// through 0xA36F0 (EAX = 4 / 0, ECX = window, six stack words). Object = Ordinal_401(0xC, "new", -1, 0) then 0x67F20 (ctor, ten stdcall words)
// when the allocation is non-null (else the object global := 0 and return). Then 0xA3400 (ESI = window, one word) and 0x7AEA0; the window global
// is re-read (every fatal path falls into the final [EAX+0x44] = -1 with EAX = _exit's return, which never comes back in retail; the capture's _exit hook returns a window): NULL -> fatal 0x4f2; [win+0x50] > -1 (signed) -> [win+0x44] = -1, else fatal 0x4f8. win+0x5c = 1 right after the create.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2c7C770Env {
    uint32_t cb;      // the 0x6FB2AC90 code address handed to the create and the second label (an opaque handle)
    uint32_t newStr;  // the "new" literal at 0xD04F0 handed to Ordinal_401
    uint32_t (*create)(uint32_t a0, uint32_t a1, uint32_t cb, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t w, uint32_t h, uint32_t a9, uint32_t a10);   // 0xA3E40
    uint32_t (*str)(uint32_t id);                                                                 // Ordinal_10003 (ECX = id)
    void (*label)(uint32_t eax, uint32_t win, uint32_t s, uint32_t y, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);   // 0xA36F0 (EAX, ECX, six words)
    uint32_t (*alloc)(uint32_t size, uint32_t name, uint32_t line, uint32_t z);                   // Ordinal_401 (0xBE66)
    uint32_t (*ctor)(uint32_t obj, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8, uint32_t a9);   // 0x67F20
    void (*a3400)(uint32_t esi, uint32_t obj);                                                    // 0xA3400
    void (*a7aea0)(void);                                                                         // 0x7AEA0
    uint32_t (*fatal)(uint32_t code);                                                             // 0xBF92, 0xBF80, _exit(-1): its EAX; never returns in retail
};

static uint32_t rd(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static void wr(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 4); }

extern "C" EMSCRIPTEN_KEEPALIVE
void d2c_port_7c770(const D2c7C770Env* e, uint32_t* pwin /*+0x113A70*/, uint32_t* pobj /*+0x113A74*/, int32_t w /*+0xDBC48*/, int32_t h /*+0xDBC4C*/) {
    if (*pwin) return;
    *pwin = e->create(5, 0x1e, e->cb, 0, 0, 0, 0, (uint32_t)(w - 0x14), (uint32_t)(h - 0x82), 0, 0);
    if (!*pwin) { wr(e->fatal(0x529), 0x44, 0xffffffffu); return; }
    wr(*pwin, 0x5c, 1);
    uint32_t s = e->str(0xfc3);
    e->label(4, *pwin, s, 0x15, 0, 1, 0, 0);
    s = e->str(0x1030);
    e->label(0, *pwin, s, (uint32_t)(h - 0xa5), 0, 1, e->cb, 1);
    const uint32_t m = e->alloc(0xc, e->newStr, 0xffffffffu, 0);
    *pobj = m ? e->ctor(m, 5, 0x41, (uint32_t)(w - 0x14), (uint32_t)(h - 200), 2, 0, 0xd, 0, 1) : 0;
    if (!*pobj) return;
    e->a3400(*pwin, *pobj);
    e->a7aea0();
    if (!*pwin) { wr(e->fatal(0x4f2), 0x44, 0xffffffffu); return; }
    if ((int32_t)rd(*pwin, 0x50) > -1) { wr(*pwin, 0x44, 0xffffffffu); return; }
    wr(e->fatal(0x4f8), 0x44, 0xffffffffu);
}
