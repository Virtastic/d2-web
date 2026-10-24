// D2Multi batch 20: ED60, EEA0, F020, F170, F340. Globals are retail's, named by address.
// Register-passed args (retail's in_EAX/unaff_ESI/EBX) are passed as explicit parameters.
// Node layout of the DAT_6fa09ef8 list: +0 name[0x32], +0x32 text[0x12], +0x44 next (retail 4-byte slot; byte-offset access here).
#include <cstdint>
#include <cstdlib>
#include <cstring>

extern "C" {
int32_t  Ordinal_10003(void);
uint32_t Ordinal_10265(int32_t);
void     Ordinal_10024(void*, int32_t);
uint32_t Ordinal_10023(int32_t);
int32_t  Ordinal_10132(uint8_t, uint32_t, uint32_t, int32_t);
void     Ordinal_10082(int32_t, void*, void*, void*, void*, uint16_t, int32_t);
void*    Ordinal_10042(int32_t, int32_t);
void     Ordinal_10043(int32_t, int32_t);
uint16_t Ordinal_10085(const void*);
char*    Ordinal_571(const char*, int32_t);   // Storm SStrChr
void Ordinal_501(char*, const char*, int32_t); // Storm SStrCopy
extern uint8_t* DAT_6fa09ef8;
extern uint8_t* DAT_6fa09ef4;
extern int32_t DAT_6fa09efc;
extern char DAT_6f9e75d7[];
extern uint32_t* gdwBitMasks_exref;
size_t ustrlen(const uint16_t*);
uint16_t* ustrcpy(uint16_t*, const uint16_t*);
uint16_t* ustrcat(uint16_t*, const uint16_t*);
uint16_t* ustrncat(uint16_t*, const uint16_t*, int32_t);
void utoUnicode(uint16_t*, const char*, int32_t);
void uwin2Unicode(uint16_t*, const char*, int32_t);
int wsprintfA(char*, const char*, ...);
int __stricmp(const char*, const char*);
}

// 1.13c D2Multi+0xED60
void d2_multi_sub_ed60(uint16_t* dst, int32_t n) {
    uint16_t a[512] = {0}, num[8] = {0};
    char s[4];
    ustrcpy(a, (uint16_t*)Ordinal_10003());
    wsprintfA(s, "%d", n);
    utoUnicode(num, s, 2);
    ustrcat(a, num);
    if (dst[0] == 0) { ustrcpy(dst, a); return; }
    if (ustrlen(dst) > 499) { Ordinal_10024(DAT_6f9e75d7, Ordinal_10265(0xb60)); exit(-1); }
    ustrcat(dst, a);
}

// 1.13c D2Multi+0xEEA0
void d2_multi_sub_eea0(uint8_t* p1, char* out, const char* name) {
    *out = 0; *p1 = 0;
    if (*name == '*') { Ordinal_501(out, name, 0x7fffffff); return; }
    // ponytail: the two arg-less 571 calls are inferred as first/last '*' (disassembly not consulted)
    char* a = Ordinal_571(name, '*');
    const char* b = strrchr(name, '*');
    if (a && b && a < b) { Ordinal_501(out, name, 0x7fffffff); char* s = Ordinal_571(out, 0x2a); *s = 0; return; }
    for (uint8_t* n = DAT_6fa09ef8; n; n = *(uint8_t**)(n + 0x44)) {
        if (__stricmp((char*)n, name)) continue;
        for (uint8_t* q = DAT_6fa09ef4; q; q = *(uint8_t**)(q + 0xb0)) {
            if (__stricmp((char*)(q + 4), name)) continue;
            uint32_t m = Ordinal_10023(*(int32_t*)(q + 0x9c));
            if ((gdwBitMasks_exref[6] & m) || (gdwBitMasks_exref[7] & m))
                Ordinal_501((char*)0x2a, (char*)(intptr_t)Ordinal_10132(q[0x86], (m >> 5 & 1) * 3 + (m >> 7 & 1) * 2 + (m >> 6 & 1), m >> 12 & 1, 0x7fffffff), 0);
            break;
        }
        Ordinal_501(out, (char*)n + 0x32, 0x7fffffff);
        break;
    }
    char* star = Ordinal_571(name, '*');
    if (*name != '*' && star) { size_t k = star - name; memcpy(out, name, k); out[k] = 0; return; }
    Ordinal_501(out, "", 0x7fffffff);   // retail copies the (empty) fallback text
}

// 1.13c D2Multi+0xF020
void d2_multi_sub_f020(const char* name, uint8_t* prev) {
    uint8_t* n = DAT_6fa09ef8;
    char* t = Ordinal_571(name, '*');
    const char* key = (t && t[1]) ? t + 1 : name;
    for (; n; prev = n, n = *(uint8_t**)(n + 0x44)) {
        char* s = Ordinal_571((char*)n, 0x2a);
        const char* k2 = (s && s[1]) ? s + 1 : (char*)n;
        if (strcmp(k2, key) == 0) { strcpy((char*)n + 0x32, ""); return; }  // retail copies the incoming text to +0x32
    }
    if (DAT_6fa09efc > 0x3ff) {
        DAT_6fa09ef8 = *(uint8_t**)(DAT_6fa09ef8 + 0x44);
        Ordinal_10043(0x928, 0);
        DAT_6fa09efc--;
    }
    uint8_t* nn = (uint8_t*)Ordinal_10042(0x92c, 0);
    strcpy((char*)nn, name);
    strcpy((char*)nn + 0x32, (char*)prev);
    *(uint8_t**)(nn + 0x44) = 0;
    DAT_6fa09efc++;
}

// 1.13c D2Multi+0xF170
void d2_multi_sub_f170(int32_t* p) {
    uint16_t a[102] = {0}, b[34] = {0}, c[102] = {0}, d[64] = {0}, e[100] = {0};
    char* s = (char*)(p + 1);
    char pre[0x20] = {0};
    if (*s == '*') {
        uwin2Unicode(a, s, 0x32); b[0] = 0;
    } else {
        Ordinal_501(pre, s, 0x7fffffff);
        char* st = Ordinal_571(s, 0x2a);
        if (st) { size_t k = st - s; memcpy(pre, s, k); pre[k] = 0; }
        char* t = Ordinal_571(s, 0x2a);
        uwin2Unicode(e, (t && t[1]) ? t + 1 : s, 0x32);
        uwin2Unicode(a, "*", 0x32);
        ustrncat(a, e, 0x32);
        uwin2Unicode(b, pre, 0x10);
    }
    int32_t id = p[0];
    uint16_t v = 0;
    if (id == 0x44324456 || id == 0x44325354 || id == 0x44325850) v = (uint16_t)p[0x22];
    Ordinal_10082(p[0x27], a, pre, c, d, v, id);
}

// 1.13c D2Multi+0xF340
uint32_t d2_multi_sub_f340(const char* in, uint32_t flags, uint8_t* o) {
    uint8_t res = 7;
    int len = (int)strlen(in);
    *(uint16_t*)(o + 0x8a) = 0;
    bool gen = false;
    auto unk = [&] { memcpy(o, "NKNU", 4); strcpy((char*)o + 0x36, in + 4); };
    auto tail = [&]() -> uint32_t {
        for (int i = 0; i < 0x10; i++) { o[0x66 + i] = i == 1 ? 1 : 0; o[0x76 + i] = 0xff; }
        return res;
    };
    if (flags & 2) { res = 0x13; }
    else {
        if (flags & 8) { res = 0x14; return tail(); }
        if (flags & 1) { res = 0x12; return tail(); }
        if (flags & 4) { res = 0x17; return tail(); }
        if (flags & 0x101000) { res = 0x15; return tail(); }
        if (len < 4) { res = 7; return tail(); }
        gen = true;
    }
    if (len < 4) return tail();
    uint32_t id; memcpy(&id, in, 4);
    memcpy(o, &id, 4);
    const char* p = in + 4;
    if (id == 0x4a535452 || id == 0x53534852 || id == 0x53544152) { if (gen) res = 0xd; return tail(); }
    if (id == 0x43484154) { if (gen) res = 0x16; return tail(); }
    if (id == 0x4452544c || id == 0x44534852 || id == 0x44545354) { if (gen) res = 0xa; return tail(); }
    if (id == 0x5732424e) { if (gen) res = 0xf; return tail(); }
    if (id == 0x53455850) { if (gen) res = 0xe; return tail(); }
    if (id == 0x57335850 || id == 0x57415233) { if (gen) res = 0x10; return tail(); }
    if (id == 0x44324456 || id == 0x44325354 || id == 0x44325850) {
        if (*p == 0) { if (res == 7) res = 0x18; unk(); return tail(); }
        if (len - 4 < 1) { Ordinal_10024(DAT_6f9e75d7, Ordinal_10265(0x56b)); exit(-1); }
        char* w = (char*)o + 0x46;
        while (*p != ',') *w++ = *p++;
        *w = 0; p++;
        w = (char*)o + 0x36;
        while (*p != ',') *w++ = *p++;
        *w = 0; p++;
        Ordinal_10085(p);
        p += 2;
        int i = 0;
        for (; i < 0xb; i++) o[0x66 + i] = *p++;  // retail: last byte read stays in p-1
        for (; i < 0x10; i++) o[0x66 + i] = 0xff;
        if (o[0x66]) {
            const uint8_t* q = (const uint8_t*)p;   // retail's pcVar15 is the last copied byte; q[0] is that byte
            o[0x86] = q[0] - 1;
            const uint8_t* r = q + 1;
            for (i = 0; i < 0xb; i++) o[0x76 + i] = *r++;
            for (; i < 0x10; i++) o[0x76 + i] = 0xff;
            const uint8_t* z = r - 1;
            *(uint16_t*)(o + 0x88) = z[1];
            uint16_t f = Ordinal_10085(z + 2);
            *(uint16_t*)(o + 0x8a) = f;
            *(uint32_t*)(o + 0xa0) = f >> 2 & 1; *(uint32_t*)(o + 0xa4) = f >> 3 & 1;
            *(uint32_t*)(o + 0xa8) = f >> 5 & 1; *(uint32_t*)(o + 0xac) = f >> 6 & 1;
            *(uint16_t*)(o + 0x8c) = Ordinal_10085(z + 4);
            for (int k = 0; k < 3; k++) { uint8_t b = z[6 + k]; o[0x94 + k] = b == 0xff ? 0 : b; }
            if (gen) return res;
        }
        return tail();
    }
    if (gen) { unk(); res = 7; }
    return tail();
}
