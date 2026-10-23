// 1.13c D2Client+0xC6060: set a control's flags/state by mode, then maybe redraw.
// Retail passes this=ESI, mode=EBP, flagA=EBX, flagB=[esp] in registers/stack; here they are explicit.
// The Ordinal_* callees are D2Win/D2Gfx imports (ordinals UNVERIFIED), reached through an env seam.
#include <cstdint>
struct C6060Env {
    void* (*o10331)();                   // returns the control handle (iVar2)
    void (*o10123)();
    void (*o11073)(void*);
    void (*o11056)(int32_t, int32_t);
    void (*o11164)(int32_t, int32_t);
    void (*o10647)(int32_t, int32_t);
    void (*o10084)();
};
extern "C" void d2_client_c6060(const C6060Env* e, const int32_t* s, int32_t mode, int32_t flagA, int32_t flagB) {
    int32_t h = s[11];                   // this+0x2c
    void* c = e->o10331();
    if (!h || !c) return;
    const bool one = mode == 1;
    if (flagA && flagB) { e->o10123(); e->o11073(c); }
    e->o11056(h, one ? 0x100 : 0x80);
    e->o11164(h, one ? 0x3c01 : 0x1c09);
    e->o10647(h, one ? 2 : 7);
    if (!(s[0x31] & 0x10000)) {          // this+0xc4
        const int32_t k = s[4];
        if (s[0] == 0) { if (k != 0 && k != 0x11) return; }
        else if (s[0] == 1) { if (k != 0 && k != 0xc) return; }
        else return;
    }
    e->o10084();
}
