// 1.13c D2Multi+0x13750 (stdcall, ret 0xC: str, a, b): builds the multiplayer lobby's text panel. Reads the four widget handles at
// DAT_6fa09150..5c, calls 7C1A (ECX = handle, EDX = 0, ESI = game state + 0x89) on each in the order 54, 50, 58, 5C, then the vtable slot +0x78
// of DAT_6fa0917c (ECX = ESI, EDX = 3, stack 1 and the "profile_sex" string), GetStringByIndex(0x147B) into AA60 (EBX = 0x84B0), the 8450 callback
// through 7CCE and 8790. When DAT_6fa09118 is still 0 it creates the controls (7CAA by name into DAT_6fa09d78..8c, da0, da4 unless
// DAT_6fa0a094), reads three captions into local buffers (7C44), sizes them (7CC8 8, 0x28, 0xC8), runs 11600 and three 7CC2 links, then
// 7C26 on d80 (all three buffers empty) or on d88 plus 7C80, and copies the string into DAT_6fa09dd4 with a and b into de4/de8.
#include "d2multi_env.h"

// h = the dwords at DAT_6fa09150..5c; w = the dwords at DAT_6fa09d78.. (0x78 bytes); g94 = DAT_6fa0a094; g118 = DAT_6fa09118.
void d2multi_13750(const D2mEnv& e, uint32_t esi, const uint32_t* h, uint32_t* w, const uint32_t* g94, uint32_t* g118,
                   const char* str, uint32_t a, uint32_t b) {
    uint8_t b0[12], b1[44], b2[160];
    d2m_call(e, 0x7c1a, 0, h[1], 0, 0, esi);
    d2m_call(e, 0x7c1a, 0, h[0], 0, 0, esi);
    d2m_call(e, 0x7c1a, 0, h[2], 0, 0, esi);
    d2m_call(e, 0x7c1a, 0, h[3], 0, 0, esi);
    d2m_call(e, 0x1000, 0, esi, 3, 0, 0, 1, D2M_MOD(0x19c28));
    *g118 = 0;
    uint32_t t = d2m_call(e, 0x7ea8, 0, 0x147b);
    d2m_call(e, 0xaa60, 0, 0, 0, D2M_MOD(0x84b0), 0, t);
    d2m_call(e, 0x7cce, 0, 0, 0, 0, 0, D2M_MOD(0x8450));
    d2m_call(e, 0x8790);
    if (*g118 != 0) return;
    d2m_call(e, 0xbe00);
    w[13] = 2;
    w[0] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1b560));
    w[1] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1b590));
    d2m_call(e, 0x7cbc, 0, w[1], esi);
    w[2] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1b5c0));
    d2m_call(e, 0x7c44, 0, w[2], d2m_ptr(b0));
    d2m_call(e, 0x7cc8, 0, w[2], 8);
    w[3] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1b5f0));
    d2m_call(e, 0x7c44, 0, w[3], d2m_ptr(b1));
    d2m_call(e, 0x7cc8, 0, w[3], 0x28);
    w[4] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1b620));
    d2m_call(e, 0x7c44, 0, w[4], d2m_ptr(b2));
    d2m_call(e, 0x7cc8, 0, w[4], 0xc8);
    w[5] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1b800));
    d2m_call(e, 0x7c38, 0, 0, 0, 0, 0, w[5]);
    w[10] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1b830));
    if (*g94 == 0) w[11] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1b860));
    d2m_call(e, 0x11600);
    d2m_call(e, 0x7cc2, 0, w[4], w[2]);
    d2m_call(e, 0x7cc2, 0, w[2], w[3]);
    d2m_call(e, 0x7cc2, 0, w[3], w[4]);
    if (b0[0] == 0 && b1[0] == 0 && b2[0] == 0) {
        d2m_call(e, 0x7c26, 0, 0, 0, 0, 0, w[2]);
    } else {
        d2m_call(e, 0x7c26, 0, 0, 0, 0, 0, w[4]);
        d2m_call(e, 0x7c80, 0, w[4]);
    }
    char* dst = (char*)&w[23];
    char c;
    do { c = *str++; *dst++ = c; } while (c != 0);
    w[27] = a;
    w[28] = b;
}
