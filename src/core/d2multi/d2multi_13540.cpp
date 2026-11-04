// 1.13c D2Multi+0x13540 (regs: EAX = name, EDI = source string; stack a, b; ret 8): builds the multiplayer game-list panel. If strchr-like 7BA8(name, '*')
// finds a '*' followed by more text the caption starts after it. Then as 13750: four 7C1A calls on the handles at DAT_6fa09150..5c, the vtable slot +0x78
// of DAT_6fa0917c, 7EA8(0x147B) into AA60, 7CCE, 8790. When DAT_6fa09118 is still 0 it creates the controls (7CAA by name into DAT_6fa09d64..74, da0, da4
// unless DAT_6fa0a094), tokenises the third local buffer on "\r\n" through 1274 (each token set as d74's text by 7CBC), runs 11600, 7C26(DAT_6fa0a0d0) when set,
// and copies EDI's string into DAT_6fa09dd4 with a and b into de4/de8.
#include "d2multi_env.h"

// h = the dwords at DAT_6fa09150..5c; w = the dwords at DAT_6fa09d64.. (0x8c bytes); g94 = DAT_6fa0a094; g118 = DAT_6fa09118; g0d0 = DAT_6fa0a0d0.
void d2multi_13540(const D2mEnv& e, uint32_t name, const char* src, uint32_t a, uint32_t b, const uint32_t* h, uint32_t* w,
                   const uint32_t* g94, uint32_t* g118, const uint32_t* g0d0) {
    uint8_t loc0[12], loc1[44], buf[160];
    const char delim[4] = { '\r', '\n', 0, 0 };
    uint32_t esi = name;
    uint32_t p = d2m_call(e, 0x7ba8, 0, 0, 0, 0, 0, name, 0x2a);
    if (p != 0 && *(const char*)(uintptr_t)(p + 1) != 0) esi = p + 1;
    d2m_call(e, 0x7c1a, 0, h[1]);
    d2m_call(e, 0x7c1a, 0, h[0]);
    d2m_call(e, 0x7c1a, 0, h[2]);
    d2m_call(e, 0x7c1a, 0, h[3]);
    d2m_call(e, 0x1000, 0, esi, 3, 0, 0, 1, D2M_MOD(0x19c28));
    *g118 = 0;
    uint32_t t = d2m_call(e, 0x7ea8, 0, 0x147b);
    d2m_call(e, 0xaa60, 0, 0, 0, D2M_MOD(0x84b0), 0, t);
    d2m_call(e, 0x7cce, 0, 0, 0, 0, 0, D2M_MOD(0x8450));
    d2m_call(e, 0x8790);
    if (*g118 != 0) return;
    d2m_call(e, 0xbe00);
    w[18] = 1;
    w[0] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1b650));
    w[1] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1b680));
    d2m_call(e, 0x7cbc, 0, w[1], esi);
    w[2] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1b6b0));
    d2m_call(e, 0x7cbc, 0, w[2], d2m_ptr(loc0));
    w[3] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1b6e0));
    d2m_call(e, 0x7cbc, 0, w[3], d2m_ptr(loc1));
    w[4] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1b710));
    uint32_t tok = d2m_call(e, 0x1274, 0, 0, 0, 0, 0, d2m_ptr(buf), d2m_ptr(delim));
    while (tok != 0) {
        d2m_call(e, 0x7cbc, 0, w[4], tok);
        tok = d2m_call(e, 0x1274, 0, 0, 0, 0, 0, 0, d2m_ptr(delim));
    }
    w[15] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1b830));
    d2m_call(e, 0x7c38, 0, 0, 0, 0, 0, w[15]);
    if (*g94 == 0) w[16] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1b860));
    d2m_call(e, 0x11600);
    if (*g0d0 != 0) d2m_call(e, 0x7c26, 0, 0, 0, 0, 0, *g0d0);
    char* dst = (char*)&w[28];
    char c;
    do { c = *src++; *dst++ = c; } while (c != 0);
    w[32] = a;
    w[33] = b;
}
