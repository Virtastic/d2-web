// Fog CRT statics (batch 143). Callees/globals are injected via env structs, as in fog_sub_3a7a.cpp.
// Not verified: no test runner, no vector; transcribed from the cached decompile only.
#include <cstdint>
#include <cstring>

// 1.13c Fog+0x96fc
struct Fog96fcEnv { void (*sub_3c3f)(int32_t); };
extern "C" void fog_sub_96fc(const Fog96fcEnv* e) { e->sub_3c3f(0xD); }

// 1.13c Fog+0xb49d  (local_1c is the caller's [ebp-0x1c], passed in)
struct FogB49dEnv { void (*sub_3c3f)(int32_t); };
extern "C" void fog_sub_b49d(const FogB49dEnv* e, int32_t arg, int32_t callerLocal1c) {
    if (callerLocal1c != arg) e->sub_3c3f(arg);
}

// 1.13c Fog+0xa5b0  tab = &__fptrap[0] (Fog+0x7eaa8..0x7eabc), six 32-bit slots
struct FogA5b0Env { uint32_t cfltcvt, lab_b517, fassign, sub_b4db, lab_b562; };
extern "C" void fog_sub_a5b0(uint32_t tab[6], const FogA5b0Env* e) {
    tab[0] = e->cfltcvt; tab[1] = e->lab_b517; tab[2] = e->fassign;
    tab[3] = e->sub_b4db; tab[4] = e->lab_b562; tab[5] = e->cfltcvt;
}

// 1.13c Fog+0xb4db  _cfltcvt hook: move to the 'e' (first char whose tolower is 'e', skipping digits), insert decimal point
struct FogB4dbEnv { int (*tolower_)(int); int (*isdigit_)(int); char decimalPoint; };
extern "C" void fog_sub_b4db(const FogB4dbEnv* e, char* p) {
    bool isE = e->tolower_((int)*p) == 0x65;
    while (!isE) { p++; isE = e->isdigit_((int)*p) == 0; }
    char c = *p;
    *p = e->decimalPoint;
    char n;
    do { p++; n = *p; *p = c; c = n; } while (*p != '\0');
}

// 1.13c Fog+0xbd61 and 1.13c Fog+0xbda6  (identical bodies; callee bd35 vs bd4b is in the c2be arg path only)
struct FogStrtodEnv {
    void (*sub_c2be)(void* ld12, int32_t* out, const uint8_t* s, int, int, int, int);
    void (*ld12tod)(const void* ld12, void* dbl);
};
extern "C" void fog_sub_bd61(const FogStrtodEnv* e, void* dbl, const uint8_t* s) {
    uint8_t ld12[12]; int32_t t;
    e->sub_c2be(ld12, &t, s, 0, 0, 0, 0);
    e->ld12tod(ld12, dbl);
}
extern "C" void fog_sub_bda6(const FogStrtodEnv* e, void* dbl, const uint8_t* s) { fog_sub_bd61(e, dbl, s); }

// float formatters: bf1c = _fltout-style (fills strflt: sign at [0], decpt at [1]), fptostr, cftof2/cftoe2
struct FogFmtEnv {
    void (*sub_bf1c)(uint32_t lo, uint32_t hi, int32_t* strflt, uint32_t* buf);
    void (*fptostr)(char* dst, int32_t n, int32_t strflt, uint32_t pt);
    void (*cftof2)(char* b, int32_t n, char flag);
    void (*cftoe2)(int32_t n, int32_t caps, char flag);
};

// 1.13c Fog+0xb791  (strflt struct local_30: [0]=sign, [1]=decpt)
extern "C" char* fog_sub_b791(const FogFmtEnv* e, const uint32_t* dbl, char* out, uint32_t n) {
    int32_t st[2]; uint32_t buf[6];
    e->sub_bf1c(dbl[0], dbl[1], st, buf);
    e->fptostr(out + (st[0] == 0x2d), (int32_t)(st[1] + n), (int32_t)(intptr_t)st, dbl[0]);
    e->cftof2(out, (int32_t)n, 0);
    return out;
}

// 1.13c Fog+0xb685
extern "C" int32_t fog_sub_b685(const FogFmtEnv* e, const uint32_t* dbl, int32_t out, int32_t n, int32_t caps) {
    int32_t st[4]; uint32_t buf[6];
    e->sub_bf1c(dbl[0], dbl[1], st, buf);
    e->fptostr((char*)(intptr_t)((n > 0) + (st[0] == 0x2d) + out), n + 1, (int32_t)(intptr_t)st, dbl[0]);
    e->cftoe2(n, caps, 0);
    return out;
}

// 1.13c Fog+0xb7f8  (%g)
extern "C" void fog_sub_b7f8(const FogFmtEnv* e, const uint32_t* dbl, char* out, int32_t n, int32_t caps) {
    int32_t st[2]; uint32_t buf[6];
    e->sub_bf1c(dbl[0], dbl[1], st, buf);
    int32_t before = st[1] - 1;
    char* p = out + (st[0] == 0x2d);
    e->fptostr(p, n, (int32_t)(intptr_t)st, dbl[0]);
    st[1] -= 1;
    if (st[1] < -4 || n <= st[1]) {
        e->cftoe2(n, caps, 1);
    } else {
        if (before < st[1]) { while (*p++ != '\0') {} p[-2] = '\0'; }
        e->cftof2(out, n, 1);
    }
}

// 1.13c Fog+0x3162  DllMain/CRT entry
struct FogEntryEnv {
    int32_t* haveInit;                 // DAT_6ff8024c
    int (*userInit)(uint32_t, int32_t, uint32_t);  // DAT_6ffa828c, may be null
    int (*crtInit)(uint32_t, int32_t); // __CRT_INIT@12
    int (*sub_fb70)();
};
extern "C" int32_t fog_entry_3162(const FogEntryEnv* e, uint32_t hinst, int32_t reason, uint32_t reserved) {
    int32_t r = 1;
    if (reason == 0 && *e->haveInit == 0) return 0;
    if (reason == 1 || reason == 2) {
        if (e->userInit) r = e->userInit(hinst, reason, reserved);
        if (r == 0 || e->crtInit(hinst, reason) == 0) return 0;
    }
    r = e->sub_fb70();
    if (reason == 1 && r == 0) e->crtInit(hinst, 0);
    if (reason == 0 || reason == 3) {
        if (e->crtInit(hinst, reason) == 0) r = 0;
        if (r != 0 && e->userInit) r = e->userInit(hinst, reason, reserved);
    }
    return r;
}

// Locale state (setlocale code-page tables). ctype = DAT_6ffa6f00 (0x101 bytes, [0] unused), upper/lower map = DAT_6ffa7020.
struct FogCpState {
    uint8_t ctype[0x101];   // 6ffa6f00; flag at ctype[c+1]
    uint8_t casemap[0x100]; // 6ffa7020
    uint32_t cp;            // 6ffa7004
    uint32_t cpA;           // 6ffa6ee4
    uint32_t mb;            // 6ffa6eec
    uint32_t t0, t1, t2;    // 6ffa7010/14/18
};
struct FogCpInfo { uint32_t maxCharSize; uint8_t defaultChar[2]; uint8_t leadByte[12]; };
struct FogCpEnv {
    int (*getCPInfo)(uint32_t cp, FogCpInfo* out);                       // returns 1 on success
    void (*getStringTypeA)(const char* s, uint16_t* out, uint32_t cp, uint32_t cpA);   // sub_8fe7-style ___crtGetStringTypeA (type 1, 0x100)
    void (*lcmap)(uint32_t cpA, uint32_t flags, const char* s, int n, int32_t* out, int m, uint32_t cp, int z); // Fog+0x3279
    uint32_t (*sub_922e)();
    void (*sub_9286)();
    void (*setSBCS)();
    int32_t sbcsFallbackFlag;                                            // DAT_6ff80570
    const uint8_t* cpTable;   // 6ff7e9b0 : [0..15]=bit flags@+0 (4 bytes), table rows of 0x30 at +0x8, id at +0x8 (6ff7e9b8)
};

// 1.13c Fog+0x9286
extern "C" void fog_sub_9286(const FogCpEnv* e, FogCpState* s) {
    FogCpInfo ci;
    uint32_t i;
    if (e->getCPInfo(s->cp, &ci) == 1) {
        char buf[256]; uint16_t ty[256]; int32_t up[64], lo[64];
        for (i = 0; i < 0x100; i++) buf[i] = (char)i;
        buf[0] = ' ';
        for (const uint8_t* p = ci.leadByte; p[0] != 0; p += 2)
            for (uint32_t c = p[0]; c <= p[1]; c++) buf[c] = ' ';
        e->getStringTypeA(buf, ty, s->cp, s->cpA);
        e->lcmap(s->cpA, 0x100, buf, 0x100, up, 0x100, s->cp, 0);
        e->lcmap(s->cpA, 0x200, buf, 0x100, lo, 0x100, s->cp, 0);
        for (i = 0; i < 0x100; i++) {
            if (ty[i] & 1) { s->ctype[i + 1] |= 0x10; s->casemap[i] = ((uint8_t*)up)[i]; }
            else if (ty[i] & 2) { s->ctype[i + 1] |= 0x20; s->casemap[i] = ((uint8_t*)lo)[i]; }
            else s->casemap[i] = 0;
        }
    } else {
        for (i = 0; i < 0x100; i++) {
            if (i >= 0x41 && i <= 0x5a) { s->ctype[i + 1] |= 0x10; s->casemap[i] = (uint8_t)(i + 0x20); }
            else if (i > 0x60 && i < 0x7b) { s->ctype[i + 1] |= 0x20; s->casemap[i] = (uint8_t)(i - 0x20); }
            else s->casemap[i] = 0;
        }
    }
}

// 1.13c Fog+0x9421  (_setmbcp)
// table: 4 rows of 0x30 bytes; row r: id uint32 at +0x08 (6ff7e9b8), t0/t1/t2 at +0x0c/+0x10/+0x14, lead-byte ranges at +0x18 (6ff7e9c8; four 8-byte groups)
extern "C" int32_t fog_sub_9421(const FogCpEnv* e, FogCpState* s, uint32_t cp) {
    if (cp != 0) {
        for (uint32_t r = 0; r < 5; r++) {
            const uint8_t* row = e->cpTable + 8 + r * 0x30;
            uint32_t id; memcpy(&id, row, 4);
            if (id != cp) continue;
            memset(s->ctype, 0, sizeof s->ctype);
            for (uint32_t k = 0; k < 4; k++) {
                const uint8_t* p = row + 0x10 + k * 8;
                uint8_t flag = e->cpTable[k];
                while (p[0] != 0 && p[1] != 0) {
                    for (uint32_t c = p[0]; c <= p[1]; c++) s->ctype[c + 1] |= flag;
                    p += 2;
                }
            }
            s->cp = cp; s->mb = 1; s->cpA = e->sub_922e();
            memcpy(&s->t0, row + 4, 4); memcpy(&s->t1, row + 8, 4); memcpy(&s->t2, row + 12, 4);
            e->sub_9286();
            return 0;
        }
        FogCpInfo ci;
        if (e->getCPInfo(cp, &ci) == 1) {
            memset(s->ctype, 0, sizeof s->ctype);
            s->cp = cp; s->cpA = 0;
            if (ci.maxCharSize < 2) s->mb = 0;
            else {
                for (const uint8_t* p = ci.leadByte; p[0] != 0 && p[1] != 0; p += 2)
                    for (uint32_t c = p[0]; c <= p[1]; c++) s->ctype[c + 1] |= 4;
                for (uint32_t c = 1; c < 0xff; c++) s->ctype[c + 1] |= 8;
                s->cpA = e->sub_922e();
                s->mb = 1;  // ponytail: decompile shows extraout_ECX (unrecovered register); 1 assumed
            }
            s->t0 = s->t1 = s->t2 = 0;
            e->sub_9286();
            return 0;
        }
        if (e->sbcsFallbackFlag == 0) return -1;
    }
    e->setSBCS();
    e->sub_9286();
    return 0;
}
