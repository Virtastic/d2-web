// D2Client+0x47970: EAX = packet (+1 type byte, +2 unit id dword); env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2c47970Env {
    int32_t* (*fA4E20)(int32_t id, void* tbl, uint32_t type);  // D2Client+0xA4E20 unit lookup
    void* tbl;                                                   // D2Client+0xBA608 (+type*0x80)
    void (*f88A70)(int32_t* u, int a, int b, int c);             // D2Client+0x88A70
    int32_t* (*f46150)();                                        // D2Client+0x46150
    void (*f6B300)(int32_t* u, int a, int b, int c, int d);      // D2Client+0x6B300
    const uint8_t* g_bc973b;                                     // DAT_6fbc973b (dword)
    uint8_t* g_cc33e;                                            // DAT_6fbcc33e
    int32_t (*ord10174)(int32_t h, int a, int b);                // Ordinal_10174
    void (*fA5360)();                                           // D2Client+0xA5360
};
extern "C" {
// 1.13c D2Client+0x47970
EMSCRIPTEN_KEEPALIVE void d2c_47970(const D2c47970Env* e, const uint8_t* pkt) {
    uint32_t t = pkt[1];
    int32_t id; __builtin_memcpy(&id, pkt + 2, 4);
    int32_t* u = e->fA4E20(id, (uint8_t*)e->tbl + t * 0x80, t);
    if (!u) return;
    if (u[1] == 0x216) { e->f88A70(u, 0, 0, 0); return; }
    if (u == e->f46150()) return;
    e->f6B300(u, 3, 0, 0, 0);
    if (u[1] != 0x14b || e->f46150()) return;
    int32_t h; __builtin_memcpy(&h, e->g_bc973b, 4);
    if (!h || e->ord10174(h, 0xc, 8) != 0 || e->ord10174(h, 0xc, 1) != 0) return;
    *e->g_cc33e = 0;
    e->fA5360();
    if (*e->g_cc33e == 0) e->f88A70(u, 0, 0, 0);
}
}
