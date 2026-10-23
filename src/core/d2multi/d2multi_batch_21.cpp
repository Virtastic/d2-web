// D2Multi batch 21: F750, FB40, FBB0, FC90, 10000. Globals are retail's, named by address.
#include <cstdint>
#include <cstring>
#include <cstdio>

extern "C" {
void     Ordinal_10140(int32_t, int32_t);
void     Ordinal_10194(int32_t, uint32_t);
void     Ordinal_10106(void);
void     Ordinal_10000(void);
char*    Ordinal_571(const char* s = nullptr, int c = 0);
extern uint8_t *DAT_6fa09ef4, *DAT_6fa09f08, *DAT_6fa09f14;
extern int32_t DAT_6fa09f04, DAT_6fa0a0c8, DAT_6fa0a0d4;
extern char DAT_6fa09198[];
extern uint32_t *gdwBitMasks_exref, *gdwInvBitMasks_exref;
void win2Unicode(uint16_t*, const char*, int32_t);   // Unicode::win2Unicode
void utf8ToUnicode(uint16_t*, const char*, int32_t); // Unicode::utf8ToUnicode
uint16_t* ustrcat(uint16_t*, const uint16_t*);            // Unicode::strcat
void d2_multi_sub_eb70(uint16_t*);
void d2_multi_sub_ec80(int32_t, uint16_t*);
void d2_multi_sub_ed60(uint16_t*, int32_t);
}

static inline uint8_t*& nxt(uint8_t* n) { return *(uint8_t**)(n + 0xb0); }
static inline uint32_t rank(uint8_t* n) {
    uint32_t f = *(uint32_t*)(n + 0x90);
    return f & 2 ? 4 : f & 8 ? 3 : f & 1 ? 2 : (f >> 2) & 1;
}
static inline uint32_t setm(uint32_t u, bool on, int i) { return on ? u | gdwBitMasks_exref[i] : u & gdwInvBitMasks_exref[i]; }

// 1.13c D2Multi+0xF750
void d2_multi_sub_f750(void) {
    if (DAT_6fa09ef4) {
        uint8_t* rest = nxt(DAT_6fa09ef4);
        nxt(DAT_6fa09ef4) = 0;
        while (rest) {  // insertion sort, rank descending
            uint32_t r = rank(rest);
            uint8_t *prev = 0, *cur = DAT_6fa09ef4, *after = nxt(rest);
            bool placed = false;
            for (; cur && !placed; ) {
                if (rank(cur) < r) {
                    nxt(rest) = cur;
                    if (!prev) DAT_6fa09ef4 = rest; else nxt(prev) = rest;
                    placed = true;
                } else { prev = cur; cur = nxt(cur); }
            }
            if (!placed) { nxt(prev) = rest; nxt(rest) = 0; }
            rest = after;
        }
    }
    int32_t y = 0x46;
    int32_t idx = 0;
    for (uint8_t* n = DAT_6fa09ef4; n; n = nxt(n), idx++) {
        bool in = !(idx < DAT_6fa09f04 || DAT_6fa09f04 + 0xb <= idx);
        uint32_t f = *(uint32_t*)(n + 0x90), u;
        uint16_t w = *(uint16_t*)(n + 0x8a);
        if (in) {
            Ordinal_10140(*(int32_t*)(n + 0x9c), y);
            u = (f & 9) ? gdwBitMasks_exref[11] | 0xf : gdwInvBitMasks_exref[11] & 0xf;
        } else u = (f & 9) ? gdwBitMasks_exref[11] : 0;
        u = setm(u, w & 8, 4); u = setm(u, w & 4, 5); u = setm(u, w & 0x20, 12);
        if (!in) {
            uint16_t v = *(uint16_t*)(DAT_6fa09f14 + 0x1eb);
            bool a4 = v & 4, a20 = v & 0x20, z0 = *(int32_t*)(n + 0xa0) == 0, z8 = *(int32_t*)(n + 0xa8) == 0;
            bool diff = (!a20 && !z8) || DAT_6fa09f14[0x3be] != n[0x94] || (a4 && z0) || (!a4 && !z0) || (a20 && z8);
            u = setm(u, diff, 13);
        }
        uint32_t c = (w >> 8) & 0x1f, k;
        if (!(w & 0x20)) k = c < 8 ? (c >= 4) : c < 12 ? 2 : 3;
        else k = c < 10 ? (c >= 5) : c < 15 ? 2 : 3;
        u = setm(u, k & 1, 6); u = setm(u, k & 2, 7);
        if (in && (idx & 1)) u |= gdwBitMasks_exref[8];
        u = setm(u, f & 0x20, 9); u = setm(u, n == DAT_6fa09f08, 10);
        if (in) y += 0x3c;
        Ordinal_10194(*(int32_t*)(n + 0x9c), u);
    }
}

// 1.13c D2Multi+0xFB40
int32_t d2_multi_sub_fb40(char* s) {
    uint16_t u[0x240] = {};
    win2Unicode(u, s, 0x240);
    if (DAT_6fa0a0d4) { Ordinal_10106(); Ordinal_10000(); }
    return 1;
}

// 1.13c D2Multi+0xFBB0
void d2_multi_sub_fbb0(char* a, char* b, int32_t p3, int32_t p4) {
    uint16_t u1[0x240] = {}, u2[0x240] = {};
    win2Unicode(u1, a, 0x240);
    win2Unicode(u2, b, 0x240);
    d2_multi_sub_eb70(u1);
    d2_multi_sub_eb70(u2);
    d2_multi_sub_ec80(p3, u1);  // dst is a register arg in retail (assumed u1)
    d2_multi_sub_ed60(u1, p4);
    ustrcat(u1, u2);
    if (DAT_6fa0a0c8) Ordinal_10000();
}

// 1.13c D2Multi+0xFC90
int32_t d2_multi_sub_fc90(char* s, int32_t kind) {
    uint16_t u[0x240] = {};
    utf8ToUnicode(u, s, 0x240);
    d2_multi_sub_eb70(u);
    if (DAT_6fa0a0c8) Ordinal_10000();
    return 1;
}

// 1.13c D2Multi+0x10000
// ponytail: sprintf args beyond `who` are not shown by the decompiler; assumed (who, DAT_6fa09198-copy) -- low confidence.
void d2_multi_sub_10000(char* who, const char* edi) {
    char buf[500];
    strcpy(DAT_6fa09198, edi);  // retail copies the in_EDI string here first
    const char* mine = edi;
    const char* tail = who;
    bool same = false;
    if (*who) {
        if (*who == '*') {
            char* p = Ordinal_571(); const char* a = (p && p[1]) ? p + 1 : mine;
            char* q = Ordinal_571(who, '*'); const char* b = (q && q[1]) ? q + 1 : who;
            same = strcmp(b, a) == 0;
        }
        if (!same) { snprintf(buf, sizeof buf, "%s (*%s) whispers: %s", who, mine, tail); goto out; }
    }
    snprintf(buf, sizeof buf, "%s whispers: %s", who, tail);
out:
    d2_multi_sub_fc90(buf, 2);
}
