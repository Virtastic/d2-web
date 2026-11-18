// 1.13c D2Client wait-box state helpers 0x47810 and 0x4B420, callees through D2cHost (client_host.h) in retail's call order.
// Globals are passed in by the caller (offsets in); ordinals/thunks are host rvas.
#include <emscripten.h>
#include <cstdint>
#include "client_host.h"

static uint32_t hcall(D2cHost h, uint32_t rva, uint32_t eax = 0, uint32_t ecx = 0, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t* out0 = nullptr) {
    D2cCall c = {}; c.rva = rva; c.eax = eax; c.ecx = ecx; c.s[0] = s0; c.s[1] = s1; c.s[2] = s2; c.s[3] = s3;
    uint32_t r = h(&c); if (out0) *out0 = c.out[0]; return r;
}

// 1.13c D2Client+0x47810   g = {DAT 0xBC976F gate, 0xBC977B object, 0xBC9729 mode}; vfn = the object's vtable[0] (a host rva token)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_47810(D2cHost h, uint32_t* g, uint32_t vfn) {
    if (g[0]) {
        hcall(h, 0xA3D20);
        if (g[1]) hcall(h, vfn, 0, g[1], 1);
        g[1] = 0; g[0] = 0;
    }
    g[2] = 5;
    hcall(h, 0x47500);
}

// 1.13c D2Client+0x4B420   st: {0xBC975F pending, 0xBC975B dialog, 0xBC9663 (dword write), 0xBC9781 state (byte), 0xBB824, 0xBB828, tab[5] = 0xA3E14/3B/62/89,0xA416E}
struct D2c4B420 { uint32_t pending, dlg, g9663, state, g824, g828, tab[5]; };
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_4b420(D2cHost h, D2c4B420* s) {
    static const uint16_t msg[10] = { 0xd1e, 0xd1e, 0xd21, 0xd1c, 0xd20, 0xd22, 0xd21, 0xd21, 0xd23, 0xd1d };   // state 7..16
    if (s->pending) { hcall(h, 0xA3D20); s->pending = 0; }
    if (s->state == 5) hcall(h, 0x5AC00);
    else if (s->state != 6 && s->state != 3) {
        uint32_t idx = s->state - 7u, m = idx < 10 ? msg[idx] : 0xd21;
        s->g9663 = 0xc;
        if (s->dlg) { hcall(h, 0xBF92, 0, 0, 0x1413); hcall(h, 0xBF80); hcall(h, 0x3664, 0, 0, 0xffffffffu); return; }   // fatal: _exit(-1), not capturable
        s->dlg = hcall(h, 0xA3E40, 0, 0, s->g828, s->g824, 0xB2000000u | 0x4AAF0, 0);
        hcall(h, 0xA3340, s->dlg, 0, 1);
        uint32_t t = hcall(h, 0xD1EC, 0, m);
        hcall(h, 0xA36F0, 1, s->dlg, t, 0x15, 0, 1);
        t = hcall(h, 0xD1EC, 0, 0xd4b);
        hcall(h, 0xA36F0, 0, s->dlg, t, 0xf, 0, 1);
        return;
    }
    uint32_t cls = 0, u = hcall(h, 0x46150, 0, 0, 0, 0, 0, 0, &cls);   // out[0] = unit +4
    if (u && (cls == 0x109 || (cls >= 0xf4 && cls <= 0xf6) || cls == 0x208)) for (uint32_t& t : s->tab) t = 2;
    hcall(h, 0x48B20);
}
