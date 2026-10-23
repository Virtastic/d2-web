// D2Client sub_4B8A0: seam-based port (callees via D2cHost, globals via G).
#include "client_host.h"
#include <cstdint>
#include <cstring>

struct S4b8a0G {                       // globals, RVA = VA-0x6faf0000
    uint32_t* d_bbfc;                  // DAT_6fbcbbfc (ptr to struct; [0],[5],[0x18])
    uint8_t  d9660, d9661; uint16_t d966f; uint8_t d9671; uint8_t d97a6, d97a8;
    uint32_t d3b10, d971d, d9721, d9725, daada0, d8d5d0, d973b;
    uint32_t d46c4, d46c8, dc3dc, dc3e0, dc3f8, dc3fc, dc2f8, dc2f4;
    uint32_t dbf94, dbf98;
    uint32_t tbl;                      // &DAT_6fbba608 (entries 0x80 each)
};
// host ids (rva): 54E20 54DA0 F71F0 C43E0 F6A60 F6070 B1760 55600 6E350 2D3C0 F5D30 F5FF0 C4910
// 55540 3BBF0 FA8C0 F5C50 F7B20 72790 FA4F0 6E2E0 F92F0 ; imports 0x10795 10778 11017 10167 10159 10227 10265 10024 ; 0xFFFFF=_exit
static uint32_t H(D2cHost h, uint32_t rva, uint32_t eax=0, uint32_t ecx=0, uint32_t s0=0, uint32_t s1=0, uint32_t s2=0, uint32_t s3=0) {
    D2cCall c; memset(&c, 0, sizeof c); c.rva = rva; c.eax = eax; c.ecx = ecx; c.s[0]=s0; c.s[1]=s1; c.s[2]=s2; c.s[3]=s3;
    return h(&c);
}

// 1.13c D2Client+0x4B8A0  (AL = al, stack p1,p2,p3)
extern "C" void d2c_sub_4b8a0(D2cHost h, S4b8a0G* g, uint8_t al, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t retaddr) {
    uint32_t* ent;
    if (al == 6) { H(h, 0x10795, 0, 0, g->d973b, p2, 0x60, 0); return; }
    uint32_t e = H(h, 0x54E20, al, 0, p1, g->tbl + al * 0x80, al);
    ent = (uint32_t*)(uintptr_t)e;
    if (!ent) {
        uint32_t* b = g->d_bbfc;
        if (b && b[0] == 0 && b[5]) { uint8_t* q = (uint8_t*)(uintptr_t)b[5]; memset(q + 0x150, 0, 16); }
        uint8_t m[12]; m[0] = '0'; uint32_t a = p3 & 0xff, bb = p1; memcpy(m + 1, &a, 4); memcpy(m + 5, &bb, 4);
        H(h, 0xC43E0, 0, 0, (uint32_t)(uintptr_t)m);
        H(h, 0xF6A60, 0, 0, p1);
        return;
    }
    H(h, 0x54DA0, 0, 0, g->tbl + 0x200);
    ent[0x31] |= 2;
    H(h, 0x10795, 0, 0, g->d973b, p2, 0x60, 0);
    H(h, 0xF71F0);
    { uint8_t m[12]; m[0] = '/'; uint32_t a = al; memcpy(m + 1, &a, 4); memcpy(m + 5, &p1, 4); H(h, 0xC43E0, 0, 0, (uint32_t)(uintptr_t)m); }
    g->d46c8 = g->d46c4 = 0x10; g->dc3dc = g->dc3e0 = g->dc3f8 = g->dc3fc = g->dc2f8 = g->dc2f4 = 0;
    if (g->d9660) { g->d9661 = 1; H(h, 0xF6070); ent[0x31] &= ~2u; return; }
    if (g->d3b10 == 0) { H(h, 0x10265, 0, 0, 0x1060); H(h, 0x10024, 0, 0, 0, 0); H(h, 0xFFFFF); return; }
    uint16_t w = (uint16_t)H(h, 0x10778, 0, 0, g->d3b10, 0);
    uint32_t id = ent[1];
    if (H(h, 0xB1760, 0, 0, 9) && id != 0x219 && id != 0x21a && id != 0x21b && id != 0x20f) H(h, 0x55600);
    if (H(h, 0x11017, 0, 0, g->d_bbfc[0x18]) == 0) {
        g->d97a6 = 0;
        if (w != 0xffff) {
            H(h, 0x6E350, 0, 0, 0, 0);
            g->d966f = w;
            H(h, 0x2D3C0, 0, 0, (uint32_t)(uintptr_t)ent, w);
            H(h, 0xF5D30, 0, 0, w);
            g->d9671 = (uint8_t)H(h, 0xF5FF0);
            H(h, 0xC4910, 0, 0, ent[3], w);
            g->d9661 = 1; g->dbf94 = 0xFB230; g->dbf98 = 0;
            H(h, 0xF6070);
            g->d97a8 = 0; return;
        }
        int16_t sv = (int16_t)H(h, 0x10167, 0, 0, retaddr);
        if (ent[1] != 0x20f) H(h, 0x55540);
        if (ent[0xb]) H(h, 0x10159, 0, 0, ent[0xb]);
        if (sv != -1) {
            H(h, 0x3BBF0); H(h, 0x6E350, 0, 0, 0, 0);
            g->d966f = (uint16_t)sv; g->d9671 = (uint8_t)H(h, 0xF5FF0);
            H(h, 0xFA8C0, 0, 0, (uint32_t)(uintptr_t)ent);
            g->d97a8 = 0; return;
        }
        if (H(h, 0xB1760, 0, 0, 9) == 0) { H(h, 0xF7B20); H(h, 0x72790, 0, 0, 8, 1, 0); g->d97a8 = 0; return; }
        if ((uint8_t)H(h, 0xF5C50) == 0) {
            H(h, 0x3BBF0); H(h, 0x6E350, 0, 0, 0, 0); H(h, 0xFA4F0, 0, 0, (uint32_t)(uintptr_t)ent);
            g->d97a8 = 0; return;
        }
    }
    g->d971d = ent[3]; g->d9725 = ent[1];
    g->d9721 = 1;
    H(h, 0xF7B20);
    uint32_t keep = g->daada0;
    H(h, 0x10227);
    if (g->d8d5d0 == 0x26) {
        g->daada0 = 0;
        if (keep == 0) return;
        H(h, 0x6E2E0); H(h, 0xF92F0); return;
    }
    H(h, 0x10265, 0, 0, 0x139b); H(h, 0x10024, 0, 0, 0, 0); H(h, 0xFFFFF);
}
