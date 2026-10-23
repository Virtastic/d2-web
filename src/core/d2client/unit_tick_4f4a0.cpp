// 1.13c D2Client+0x4F4A0 (578 B): per-unit client update, from the cached decompile.
// Unit = retail 32-bit layout (u32 words); every callee/global is a host seam (env struct).
#include <cstdint>
#include <emscripten.h>

typedef uint32_t u32;
struct D2ClientUnit4f4a0Env {
    u32 bitMask4;                          // gdwBitMasks[4]
    u32* playerGlobal;                     // DAT 0x6FBCBBFC (player unit ptr)
    u32 (*ord10494)();
    u32 (*s55440)();                       // D2Client+0xA5440
    u32* (*s4bff0)();                      // -> state (+0 kind, +8 mode)
    void (*s4f2c0)();
    u32 (*s2a700)();                       // D2Client+0x2A700 (FUN_6fada700)
    u32 (*s1e40)(u32);                     // D2Client+0x1E40
    u32 (*ord10392)(u32);
    void (*ord11163)(u32, u32*, u32*);
    u32 (*s55b40)(u32* unit, u32 ebx);     // D2Client+0xA5B40
    void (*ord10336)(u32, u32);
    void (*ord10340)();
    uint8_t* (*s1230)();                   // record ptr (+0xd flags)
    u32 (*s1510)(u32 v);
    u32 (*ord10407)();
    u32 (*s1330)(u32* p);
    u32 (*s1660)(u32* p);
    u32 (*s1630)(u32* p);
    void (*ord10739)(u32, u32, u32, u32);
    u32 (*s4ee40)(u32* state);
    u32 (*ord10426)();
    void (*s4f0b0)();
    void (*s212f0)(u32* unit, u32 a, u32 b);
    void (*ord10603)(u32);
    void (*s4da60)();
    void (*s57bf0)(u32, u32, u32, u32);    // D2Client+0xA7BF0
    u32 (*ord10276)();
    void (*s508e0)(u32* unit);             // D2Client+0xA08E0
    void (*s237e0)(u32* unit);
    void (*s23c00)(u32* unit);
    u32 ebx;                               // caller register (unaff_EBX)
};

// 1.13c D2Client+0x4F4A0  (EAX = unit)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_4f4a0(u32* u, D2ClientUnit4f4a0Env* e) {
    if (!u || u[0] != 1) return;
    if (e->ord10494() && !e->s55440()) return;
    u32 mode = u[4];
    if (mode > 0xf) return;
    u32* st = e->s4bff0();
    if (!st) return;
    e->s4f2c0();
    u32 flag = 0;
    if (st[0] == 1) flag = e->s2a700() & 0xff;
    if (mode == 1 && (e->s1e40(1) & 0xffff) == 0) {
        u32 h = u[0xb], a[3];
        bool skip = false;
        if (h && e->ord10392(h)) {
            e->ord11163(h, a, a);
            u32 r = e->s55b40(u, e->ebx);
            e->ord10336(h, r);
            skip = r == 0;
        }
        if (!skip) e->ord10340();
    }
    u32 id = u[1];
    uint8_t* rec = e->s1230();
    if (!rec) return;
    u32* pl = e->playerGlobal;
    if ((e->bitMask4 & rec[0xd] & 0xff) && e->s1510(0xd) == 0 && u[4] == 1 &&
        !((u[0x31] >> 9) & 1) && id != 0x219 && id != 0x21a && id != 0x21b && pl &&
        e->ord10407() < 7 && (int32_t)e->s1330(u + 8) < 10 && u[0xb]) {
        u32 y = e->s1660(pl), x = e->s1630(pl);
        e->ord10739(u[0xb], x, y, 0);
    }
    u32 v6 = e->s4ee40(st), r = 0;
    switch (st[2]) {
    case 1: r = flag; break;
    case 2: case 3: r = v6; break;
    case 4: if (flag && e->ord10426()) r = 1; break;
    }
    e->s4f0b0();
    e->s212f0(u, 0, 2);
    if (u[0xb]) e->ord10603(u[0xb]);
    if (r) e->s4da60();
    if (u[4] == 0 && u[1] == 0x210 && (u[0x31] & 1)) {
        u[0x31] &= ~1u;
        u32 y = e->s1660(u), x = e->s1630(u);
        e->s57bf0(0x1de, x, y, 2);
    }
    if (e->ord10276()) e->s508e0(u);
    e->s237e0(u);
    e->s23c00(u);
}
