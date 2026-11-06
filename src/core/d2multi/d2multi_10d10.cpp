// 1.13c D2Multi+0x10D10 -- find-or-create a channel/user list node by name. stdcall, three stack words
// (name, a2, a3), ret 0xC; returns the node. gv = the pinned globals: [0] list head 39EF4 (next at +0xB0), [1] counter 39F00, [2] seeded flag 3A0FC.
// A node whose name matches (a leading '*' skipped on both sides, by strchr: the text after the first '*') is refreshed by 10410
// (ESI = node, EAX = a2, EDX = a3, stack = name) and returned. Otherwise Fog #10042 (7BE4) allocates 0xB4 bytes, zeroed; name to +4,
// +0x90 = a3; F340 (EAX = a2, stack = node) parses it and answers a byte. A +0x8A flags word with 4 set and 8 clear and +0x90 bit 2
// clear asks 7C02: a non-zero answer or a parse byte < 5 opens 7CE6 (#10154) with 1, else 5. A null handle retries with +0x86 = 7,
// slot bytes +0x66 (byte 1 = 1) and +0x76 (all 0xFF); still null: 7BDE(0x6FA) / 7BD2 report and _exit(-1) (14BA). Then 7CF2, 7D1C,
// one srand(GetTickCount) (1A58), 7D16 with rand() % (7D34 << 8), F170, the list append, F020 (EBX = node+4), the "%s (%d)" title
// over 38D48 and the bumped counter (sprintf 0x1000), FB40, 104D0 (EAX = FB40's answer, EDX = 0), F750.
#include "d2multi_env.h"

static const uint8_t* m10d10_star(const uint8_t* s) {                     // strchr(s, '*') + 1, or s
    for (const uint8_t* p = s; *p; ++p) if (*p == '*') return p + 1;
    return s;
}
static int32_t m10d10_strcmp(const uint8_t* a, const uint8_t* b) {
    for (;;) {
        if (a[0] != b[0]) return a[0] < b[0] ? -1 : 1;
        if (a[0] == 0) return 0;
        if (a[1] != b[1]) return a[1] < b[1] ? -1 : 1;
        if (a[1] == 0) return 0;
        a += 2; b += 2;
    }
}
static inline uint32_t m10d10_rd32(uint32_t a) { uint32_t v; __builtin_memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void m10d10_wr32(uint32_t a, uint32_t v) { __builtin_memcpy((void*)(uintptr_t)a, &v, 4); }

extern "C" uint32_t d2multi_10d10(const D2mEnv& e, uint32_t* gv, uint32_t now, const uint8_t* name, uint32_t a2, uint32_t a3) {
    for (uint32_t n = gv[0]; n != 0; n = m10d10_rd32(n + 0xb0)) {
        if (m10d10_strcmp(m10d10_star((const uint8_t*)(uintptr_t)(n + 4)), m10d10_star(name)) == 0) {
            d2m_call(e, 0x10410, a2, 0, a3, 0, n, d2m_ptr(name));
            return n;
        }
    }
    const uint32_t n = d2m_call(e, 0x7be4, 0, 0xb4, D2M_MOD(0x17780), 0, 0, 0x6c6, 0);
    uint8_t* o = (uint8_t*)(uintptr_t)n;
    __builtin_memset(o, 0, 0xb4);
    { uint32_t i = 0; do { o[4 + i] = name[i]; } while (name[i++] != 0); }
    m10d10_wr32(n + 0x90, a3);
    const uint8_t val = (uint8_t)d2m_call(e, 0xf340, a2, 0, 0, 0, 0, n);
    const uint16_t fl = (uint16_t)(o[0x8a] | o[0x8b] << 8);
    uint32_t mode = 5;
    if ((fl & 4) && !(fl & 8) && !(o[0x90] & 2)) {
        if (d2m_call(e, 0x7c02) != 0 || val < 5) mode = 1;
    }
    uint32_t h = d2m_call(e, 0x7ce6, 0, 0, 0, 0, 0, val, mode, n + 0x66, n + 0x76);
    m10d10_wr32(n + 0x9c, h);
    if (h == 0) {
        o[0x86] = 7;
        for (uint32_t i = 0; i < 0x10; i++) { o[0x66 + i] = i == 1 ? 1 : 0; o[0x76 + i] = 0xff; }
        h = d2m_call(e, 0x7ce6, 0, 0, 0, 0, 0, o[0x86], 5, n + 0x66, n + 0x76);
        m10d10_wr32(n + 0x9c, h);
    }
    uint32_t a = m10d10_rd32(n + 0x9c);
    if (a == 0) {
        const uint32_t t = d2m_call(e, 0x7bde, 0, 0, 0, 0, 0, 0x6fa);
        d2m_call(e, 0x7bd2, 0, 0, 0, 0, 0, D2M_MOD(0x175d7), t);
        a = d2m_call(e, 0x14ba, 0, 0, 0, 0, 0, 0xffffffffu);                       // _exit(-1); a recorder returns
    }
    d2m_call(e, 0x7cf2, 0, 0, 0, 0, 0, a, 0x23b);
    d2m_call(e, 0x7d1c, 0, 0, 0, 0, 0, m10d10_rd32(n + 0x9c), 0);
    if (gv[2] == 0) {
        d2m_call(e, 0x1a58, 0, 0, 0, 0, 0, now);                                    // srand(GetTickCount())
        gv[2] = 1;
    }
    const int32_t span = (int32_t)(d2m_call(e, 0x7d34, 0, 0, 0, 0, 0, m10d10_rd32(n + 0x9c)) << 8);
    const int32_t r = (int32_t)d2m_call(e, 0x1a65);
    d2m_call(e, 0x7d16, 0, 0, 0, 0, 0, m10d10_rd32(n + 0x9c), (uint32_t)(r % span));
    d2m_call(e, 0xf170, 0, 0, 0, 0, 0, n);
    if (gv[0] == 0) gv[0] = n;
    else {
        uint32_t c = gv[0];
        while (m10d10_rd32(c + 0xb0) != 0) c = m10d10_rd32(c + 0xb0);
        m10d10_wr32(c + 0xb0, n);
    }
    d2m_call(e, 0xf020, 0, 0, 0, n + 4, 0, n + 0x36);
    gv[1] += 1;
    char buf[0x100];
    d2m_call(e, 0x1000, 0, 0, 0, 0, 0, d2m_ptr(buf), D2M_MOD(0x17774), D2M_MOD(0x38d48), gv[1]);
    const uint32_t w = d2m_call(e, 0xfb40, 0, 0, 0, 0, 0, d2m_ptr(buf));
    d2m_call(e, 0x104d0, w, 0, 0);
    d2m_call(e, 0xf750);
    return n;
}
