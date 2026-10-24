// D2Multi batch 22: 10410 104D0 10AA0 10B70 10D10 (channel player list). Globals are retail's, named by address.
// ponytail: decompiler hides some register args (ECX/EBX/ESI); assumed from callers, low confidence on 10B70/10D10 param roles.
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>

extern "C" {
void*    Ordinal_10042(int, int);
void     Ordinal_10043(int32_t, int32_t);
int32_t  Ordinal_10054(void);
int32_t  Ordinal_10154(...);
int32_t Ordinal_10179(void);
int32_t Ordinal_10080(int32_t, int32_t);
int32_t  Ordinal_10121(int32_t);
int32_t Ordinal_10172(int32_t, int32_t);
int32_t  Ordinal_10227(void);
void     Ordinal_10168(int32_t, uint32_t, uint32_t, void*, void*);
void     Ordinal_10147(void);
void Ordinal_501(void*, const void*, int32_t);
extern uint8_t *DAT_6fa09ef4, *DAT_6fa09ef8, *DAT_6fa09f08;
extern int32_t DAT_6fa09efc, DAT_6fa09f00, DAT_6fa09f04, DAT_6fa0a090, DAT_6fa0a0fc;
extern char DAT_6fa08d48[];
void d2_multi_sub_e860(void);
void d2_multi_sub_f020(const char*, uint8_t*);
void d2_multi_sub_f170(int32_t*);
uint32_t d2_multi_sub_f340(const char*, uint32_t, uint8_t*);
void d2_multi_sub_f750(void);
int32_t d2_multi_sub_fb40(char*);
int32_t d2_multi_sub_fc90(char*, int32_t);
void d2_multi_sub_104d0(int32_t, int32_t);
void d2_multi_sub_1a58(uint32_t);
}

static inline uint8_t*& nx(uint8_t* n) { return *(uint8_t**)(n + 0xb0); }
// name compare ignoring a leading "...*" prefix
static bool same_name(const char* a, const char* b) {
    const char* p = strchr(a, '*'); if (p) a = p + 1;
    p = strchr(b, '*'); if (p) b = p + 1;
    return strcmp(a, b) == 0;
}
static void update_count(void) {
    char b[256];
    snprintf(b, sizeof b, "%s (%d)", DAT_6fa08d48, DAT_6fa09f00);
    d2_multi_sub_fb40(b);
}

// 1.13c D2Multi+0x10410
void d2_multi_sub_10410(int32_t p1, uint32_t flags, const char* name, uint8_t* o) {
    *(uint32_t*)(o + 0x90) = flags;
    uint32_t r = d2_multi_sub_f340((const char*)(intptr_t)p1, flags, o);
    Ordinal_501(o + 4, name, 0x7fffffff);
    uint32_t k = 5;
    if ((*(uint16_t*)(o + 0x8a) & 4) && !(*(uint16_t*)(o + 0x8a) & 8))
        if (Ordinal_10227() != 0 || (uint8_t)r < 5) k = 1;
    Ordinal_10168(*(int32_t*)(o + 0x9c), r & 0xff, k, o + 0x66, o + 0x76);
    d2_multi_sub_f170((int32_t*)o);
    d2_multi_sub_f750();
}

// 1.13c D2Multi+0x104D0
void d2_multi_sub_104d0(int32_t, int32_t delta) {
    int32_t old = DAT_6fa09f04;
    if (DAT_6fa09f00 < 0xc) DAT_6fa09f04 = 0;
    else {
        int32_t s = DAT_6fa09f04 + delta;
        DAT_6fa09f04 = s < 1 ? 0 : s;
        if (DAT_6fa09f00 - 0xb <= DAT_6fa09f04) DAT_6fa09f04 = DAT_6fa09f00 - 0xb;
    }
    if (DAT_6fa09f04 != old) d2_multi_sub_f750();
    Ordinal_10147(); Ordinal_10147(); Ordinal_10147(); Ordinal_10147();
}

// 1.13c D2Multi+0x10AA0
void d2_multi_sub_10aa0(void) {
    d2_multi_sub_e860();
    for (uint8_t* n = DAT_6fa09ef4; n;) {
        uint8_t* next = nx(n);
        Ordinal_10054();
        *(int32_t*)(n + 0x9c) = 0;
        Ordinal_10043(0x814, 0);
        n = next;
    }
    DAT_6fa09ef4 = 0;
    for (uint8_t* n = DAT_6fa09ef8; n;) { n = *(uint8_t**)(n + 0x44); Ordinal_10043(0x81c, 0); }
    DAT_6fa09ef8 = 0; DAT_6fa09efc = 0; DAT_6fa09f00 = 0;
    update_count();
    DAT_6fa09f04 = 0;
    d2_multi_sub_104d0(0, 0);
}

// 1.13c D2Multi+0x10B70
void d2_multi_sub_10b70(const char* name) {
    uint8_t *prev = 0, *n = DAT_6fa09ef4;
    for (; n; prev = n, n = nx(n)) {
        if (!same_name((const char*)(n + 4), name)) continue;
        if (DAT_6fa0a090) {
            const char* shown = (const char*)(n + 0x36);
            if (!*shown) shown = (const char*)(n + 4);
            char b[500];
            snprintf(b, sizeof b, "%s has left the channel.", shown);
            d2_multi_sub_fc90(b, 5);
        }
        if (!prev) DAT_6fa09ef4 = nx(n); else nx(prev) = nx(n);
        if (DAT_6fa09f08 == n) d2_multi_sub_e860();
        Ordinal_10054();
        *(int32_t*)(n + 0x9c) = 0;
        Ordinal_10043(0x7f0, 0);
        break;
    }
    DAT_6fa09f00--;
    update_count();
    d2_multi_sub_104d0(0, 0);
    d2_multi_sub_f750();
}

// 1.13c D2Multi+0x10D10
uint8_t* d2_multi_sub_10d10(const char* name, int32_t, uint32_t flags) {
    for (uint8_t* n = DAT_6fa09ef4; n; n = *(uint8_t**)(n + 0xb0)) {
        if (same_name((const char*)(n + 4), name)) { d2_multi_sub_10410(0, flags, name, n); return n; }
    }
    uint8_t* o = (uint8_t*)Ordinal_10042(0, 0);
    memset(o, 0, 0xb4);
    strcpy((char*)o + 4, name);
    *(uint32_t*)(o + 0x90) = flags;
    d2_multi_sub_f340(0, flags, o);
    if ((*(uint16_t*)(o + 0x8a) & 4) && !(*(uint16_t*)(o + 0x8a) & 8) && !(flags & 2)) Ordinal_10227();
    int32_t h = Ordinal_10154();
    *(int32_t*)(o + 0x9c) = h;
    if (!h) {
        o[0x86] = 7;
        for (int i = 0; i < 0x10; i++) { o[0x66 + i] = (i == 1); o[0x76 + i] = 0xff; }
        h = Ordinal_10154(o[0x86], 5, o + 0x66);
        *(int32_t*)(o + 0x9c) = h;
    }
    if (!h) { exit(-1); }  // retail: error dialog, then _exit(-1)
    Ordinal_10179();
    Ordinal_10080(h, 0);
    if (!DAT_6fa0a0fc) { d2_multi_sub_1a58(0); DAT_6fa0a0fc = 1; }  // srand(GetTickCount())
    int32_t m = Ordinal_10121(h);
    Ordinal_10172(h, rand() % (m << 8));
    d2_multi_sub_f170((int32_t*)o);
    if (DAT_6fa09ef4) { uint8_t* t = DAT_6fa09ef4; while (nx(t)) t = nx(t); nx(t) = o; } else DAT_6fa09ef4 = o;
    d2_multi_sub_f020(name, 0);
    DAT_6fa09f00++;
    update_count();
    d2_multi_sub_104d0(0, 0);
    d2_multi_sub_f750();
    return o;
}
