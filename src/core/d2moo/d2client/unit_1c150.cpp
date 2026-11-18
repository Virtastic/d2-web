// 1.13c D2Client+0x1C150: per-unit mode-change handler, ret 4 (stdcall pUnit).
// Callees are injected: D2Common ordinals by their D2Client thunk, D2Client internals by RVA.
#include <cstdint>
#include <cstring>
struct Client1C150Callees {
    uint32_t (*c1fc)(uint32_t, uint32_t), (*c3e2)(uint32_t, uint32_t), (*c310)(uint32_t), (*c628)(uint32_t), (*c46c)(uint32_t),
        (*c3be)(uint32_t), (*c2ec)(uint32_t), (*c5c2)(uint32_t, uint32_t), (*c1f0)(uint32_t), (*c208)(uint32_t),
        (*c43c)(uint32_t, uint32_t), (*cffa)(uint32_t), (*c78a)(uint32_t), (*a700)(uint32_t);
    void (*c60a)(uint32_t, uint32_t), (*cdcc)(uint32_t), (*cd2a)(uint32_t), (*cd78)(uint32_t), (*c778)(uint32_t, uint32_t),
        (*cfee)(uint32_t, uint32_t), (*cd84)(uint32_t), (*a0b0)(uint32_t), (*b24480)(uint32_t), (*b078f0)(uint32_t),
        (*c8920)(uint32_t), (*d3c00)(uint32_t), (*b53000)(uint32_t), (*c97d0)(uint32_t, uint32_t), (*b58630)(uint32_t),
        (*a6c0)(uint32_t), (*a1b0)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t), (*a590)(uint32_t, uint32_t),
        (*d4490)(uint32_t, uint32_t, uint32_t), (*b56210)(uint32_t), (*b1a220)(uint32_t), (*cbe90)(uint32_t),
        (*c9d80)(uint32_t), (*b508e0)(uint32_t), (*b540)(uint32_t);
};
static uint32_t rd(const void* p, uint32_t o) { uint32_t v; memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static void wr(void* p, uint32_t o, uint32_t v) { memcpy((uint8_t*)p + o, &v, 4); }
static uint8_t* at(uint32_t a) { return (uint8_t*)(uintptr_t)a; }

// u: the unit in memory, ua its address; tab = 3 dwords per mode (module 0xF8418); g9824 = [0x119824]; gUnit = [0x11BBFC] (the local player)
extern "C" void d2_client_1c150(uint8_t* u, uint32_t ua, const uint32_t* tab, uint32_t g9824, uint32_t gUnit, const Client1C150Callees& f) {
    const uint32_t pd0 = rd(u, 0x2c), mode = rd(u, 0x10);
    uint32_t r = 0;
    if (tab[mode * 3] == 1) {                                           // 1C166
        f.a0b0(ua);
        if (f.c1fc(ua, 0xd)) f.b24480(ua);
        if (f.c1fc(ua, 0x16)) f.b078f0(ua);
        r = f.c3e2(ua, g9824) == 0;
        if (ua == gUnit) f.c8920(ua);
        f.d3c00(ua);
    }
    if (tab[mode * 3] == 2) {                                           // 1C1D7
        uint32_t p = f.c310(ua);
        if (p) {
            uint32_t fl = f.c628(p), keep = 1;
            if (fl & 1) {
                if (f.a700(ua)) { f.c60a(p, fl | 2); f.b53000(ua); keep = 0; }
                if (ua == gUnit) f.c8920(ua);
            }
            uint32_t b = u[0x4e];
            if (keep && !((rd(u, 0xc4) >> 6) & 1) && (b == 1 || b == 2 || b == 3)) f.b53000(ua);
        }
    }
    switch (tab[mode * 3 + 1]) {                                        // 1C26F jump table: 0 CDCC; 1 C46C?-:CDCC; 2 C46C?{+44,+4c}:CDCC; 3 {+44,+4c}
    case 0: f.cdcc(ua); break;
    case 1: if (!f.c46c(ua)) f.cdcc(ua); break;
    case 2: if (f.c46c(ua)) { wr(u, 0x44, rd(u, 0x48) - 0x100); memset(u + 0x4c, 0, 2); } else f.cdcc(ua); break;
    case 3: wr(u, 0x44, 0x100); memset(u + 0x4c, 0, 2); break;
    }
    if (mode == 0x12 && ((rd(u, 0xc4) >> 0xe) & 1)) f.b1a220(ua);
    int32_t v2 = (int32_t)tab[mode * 3 + 2];
    uint32_t flag = 0;
    if (v2 == 1) flag = r;
    else if (v2 >= 2 && v2 <= 3) flag = f.c46c(ua);
    if ((v2 >= 1 && v2 <= 3) && flag) {                                 // 1C302
        const uint32_t type = rd(u, 0);
        if (mode == 0x13) {
            const bool direct = type == 2 || (type >= 4 && type <= 5);
            uint32_t a, b, pd = rd(u, 0x2c);
            a = direct ? rd(at(pd), 0xc) : (pd ? f.c3be(pd) : 0);
            b = direct ? rd(at(pd), 0x10) : (pd ? f.c2ec(pd) : 0);
            f.cd2a(pd0); f.cd78(pd0);
            uint32_t x = f.c5c2(ua, 0);
            if (x) f.b58630(x);
            f.c97d0(ua, ua == gUnit);
            f.c778(ua, 0);
            pd = rd(u, 0x2c);
            f.cd2a(pd); f.cd78(pd);
            f.c208(f.c1f0(ua));
            wr(u, 0xb0, 0);
            f.a6c0(ua);
            f.a1b0(a, b, 1, 0, 0);
        } else {
            uint32_t pl = rd(u, 0x14);
            if (type == 0 && pl && rd(at(pl), 0x154)) wr(at(pl), 0x150, 1);
            else {
                uint32_t x = f.c5c2(ua, 0);
                if (x) f.b58630(x);
                f.c97d0(ua, ua == gUnit);
                f.c778(ua, 0);
                f.cd78(rd(u, 0x2c));
                uint32_t e = f.c208(f.c1f0(ua)) ? 5 : 1;
                f.b56210(ua);
                wr(u, 0xc4, (rd(u, 0xc4) & 0xffffffbfu) | 2);
                if (f.c43c(ua, e)) { f.b1a220(ua); f.a590(e, ua); f.d4490(ua, e, 0); }
            }
        }
    }
    if (f.cffa(ua)) { f.cfee(ua, 0); if (ua == gUnit) f.cbe90(ua); }    // 1C4E1
    f.cd84(pd0);
    if (mode == 3) f.c9d80(ua);
    if (f.c78a(ua)) f.b508e0(ua);
    f.b540(ua);
}
