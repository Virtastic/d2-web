// src/core/d2cmp/d2cmp_n81.cpp -- ten 1.13c D2CMP.dll rows from the cached Ghidra decompiles.
// Retail's register-passed args (EAX/ESI/EDI/EBX) are explicit parameters; the Fog/Storm ordinals and
// the process-wide globals come in through the state structs below (host seams). `fatal(code)` is the
// retail error tail: Ordinal_10265(code) -> Ordinal_10024(DAT_6fe2d30f, msg) -> _exit(-1); it does not return.
#include <cstdint>

typedef void (*D2CmpFatalFn)(uint32_t code);
static inline uint32_t* G(uint32_t a) { return (uint32_t*)(uintptr_t)a; }

// ---- 0x8EF0: assertion helper (ESI = object, EDI = expected value) ----
struct D2Cmp8ef0Env {
    int (*ord10104)(uint32_t* out108);   // Ordinal_10104; *out108 is the stack local read back (local_108)
    void (*ord276)();
    D2CmpFatalFn fatal;
};
// 1.13c D2CMP+0x08EF0
void d2cmp_8ef0(uint32_t esi, uint32_t edi, const D2Cmp8ef0Env* e) {
    uint32_t code, l108 = 0;
    if (esi == 0) code = 0x87;
    else if (e->ord10104(&l108) == 0) { e->ord276(); code = 0x8f; }
    else {
        if (edi == l108) return;
        code = 0x92;
    }
    e->fatal(code);
}

// ---- 0x9720 lives in src/core/d2moo/compat/d2cmp_tilelib.cpp (loadLibrary) ----

// ---- 0x9880: free the global node list (DAT_6fe88328) ----
struct D2Cmp9880Env {
    uint32_t* listHead;                                  // DAT_6fe88328
    void (*f16600)(void* ecx, uint32_t h, int z);        // 0x16600
    void (*f12d30)(uint32_t h);                          // 0x12D30
    void (*ord10043)(uint32_t a, uint32_t b);            // Ordinal_10043
};
// 1.13c D2CMP+0x09880 (__fastcall ECX = param_1)
void d2cmp_9880(void* ecx, const D2Cmp9880Env* e) {
    uint32_t n = *e->listHead;
    while (n) {
        uint32_t next = G(n)[0x10c / 4];
        e->f16600(ecx, G(n)[0x104 / 4], 0);
        G(n)[0x104 / 4] = 0;
        e->f12d30(G(n)[0x108 / 4]);
        G(n)[0x108 / 4] = 0;
        e->ord10043(0x2a, 0);
        n = next;
    }
    *e->listHead = 0;
}

// ---- 0xBAD0: byte size of a DT1 tile library (EAX = file name) ----
// Opens the file, reads the 0x114-byte header through 0x8EF0 (ESI = handle, EDI = 0x114), closes it and
// returns header.nTiles (+0x10C) * 0x60 + 0x114. A failed open logs and returns 0.
struct D2CmpBad0Env {
    int (*open)(const char* name, uint32_t* handle);                     // 0x8DB4 (ECX = name, EDX = &handle)
    void (*getLastError)();                                              // GetLastError, result unused
    void (*log)(const char* fmt, const char* name);                      // 0x8DBA (Fog log, cdecl)
    void (*read8ef0)(uint32_t handle, uint32_t expect, uint8_t* buf);    // 0x8EF0 (ESI, EDI, push buf)
    void (*close)(uint32_t handle);                                      // 0x8DC0 (ECX = handle)
    D2CmpFatalFn fatal;
};
// 1.13c D2CMP+0x0BAD0
int d2cmp_bad0(const char* name, const D2CmpBad0Env* e) {
    uint32_t handle = 0;
    uint8_t hdr[0x114];
    if (e->open(name, &handle) == 0) {
        e->getLastError();
        e->log("Error opening file: %s", name);
        return 0;
    }
    e->read8ef0(handle, 0x114, hdr);
    if (handle == 0) e->fatal(0x49);
    e->close(handle);
    int32_t n; __builtin_memcpy(&n, hdr + 0x10C, 4);
    return n * 0x60 + 0x114;
}

// ---- 0xD680: Ordinal 10068, D2CMP init ----
struct D2CmpD680State {
    uint32_t* inited;      // DAT_6fe87ef4
    uint32_t* flag;        // DAT_6fe87f00
    uint32_t* z240;        // DAT_6fe76240 (0x400 words)
    uint32_t* z27c;        // DAT_6fe7727c (0xd words)
    uint32_t* z2b0;        // DAT_6fe772b0 (0x4000 words)
    uint32_t* z240b;       // DAT_6fe77240 (0xf words)
    void (*initCs)();      // InitializeCriticalSection(&DAT_6fe6fba8)
    uint32_t (*ord10022)();   // returns struct ptr (0 on failure)
    void (*f275c0)();
    uint32_t* g32cc4;      // DAT_6fe32cc4, passed in EAX to 0x27270
    void (*f27270)(uint32_t ecx, uint32_t edx, uint32_t eax);   // ESI = DAT_6fe7727c, pushes LAB_6fe1d5e0
    void (*fce40)();                                            // ESI = DAT_6fe77240
    void (*fc0b0)(uint32_t ecx, uint32_t eax);                  // EDX = LAB_6fe1d600
    D2CmpFatalFn fatal;
};
// 1.13c D2CMP+0x0D680
void d2cmp_d680(uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, const D2CmpD680State* s) {
    if (*s->inited == 0) { s->initCs(); *s->inited = 1; }
    if (p4 == 0) {
        uint32_t r = s->ord10022();
        if (r == 0) s->fatal(0x10d);
        p4 = G(r)[3] - 0xb00000;
    }
    *s->flag = p4 < 0x4000000;
    for (int i = 0; i < 0x400; i++) s->z240[i] = 0;
    s->f275c0();
    for (int i = 0; i < 0xd; i++) s->z27c[i] = 0;
    s->f27270(p1, p3, *s->g32cc4);
    for (int i = 0; i < 0x4000; i++) s->z2b0[i] = 0;
    s->fce40();
    for (int i = 0; i < 0xf; i++) s->z240b[i] = 0;
    s->fc0b0(p1, p2);
}

// ---- 0xE1D0: Ordinal 10005, load a cel's frame (thiscall) ----
// 0x17020 reports two counts through out-pointers (EDI = &A, EDX = &B); Ghidra folded them to 0 and dropped
// the rescale and the post-load 0xE090 paths, so this follows the disassembly.
struct D2CmpE1d0Env {
    uint32_t (*f222e0)(void* self, uint32_t* p);                                   // 0x122E0
    uint8_t* (*fd350)(void* self, uint32_t* p, char* name1, char* buf2);           // 0xD350 (EAX = buf2)
    uint32_t (*f27020)(uint32_t tbl, uint8_t* a, uint32_t* p, uint32_t* outA, uint32_t* outB);   // 0x17020
    uint32_t (*f221c0)(uint32_t v, uint32_t n, uint32_t b, uint32_t flag);         // 0x121C0
    uint32_t (*ord10092)(uint32_t h);                                              // 0x8DD8 (ECX = h)
    void (*fcae0)(uint32_t edi, uint32_t esiTbl);                                  // 0xCAE0
    void (*fcfc0)(uint8_t* a, uint32_t* p, uint32_t cel);                          // 0xCFC0 (EAX = cel)
    uint32_t (*fd310)(uint32_t cel, uint32_t* p);                                  // 0xD310 (EAX, EDI)
    int (*fc2d0)(char* name1, char* buf2, uint32_t* p);                            // 0xC2D0 (EBX, ESI)
    uint32_t (*fddc0)(uint32_t* p, uint8_t* a, uint32_t A, uint32_t B, char* name1);           // 0xDDC0
    void (*fe090)(uint32_t* p, char* buf2, uint32_t A, uint32_t B, uint8_t* a, char* name1, int p3);   // 0xE090 (ECX, EAX)
    uint32_t* g88398;                                                              // DAT_6fe88398
};
// 1.13c D2CMP+0x0E1D0
uint32_t d2cmp_e1d0(void* self, uint32_t* p, int p2, int p3, const D2CmpE1d0Env* e) {
    char name1[0x104], buf2[0x104];
    if (!p) return 0;
    p[0xf] = 0;
    if (p[0xd]) { p[0xf] = e->f222e0(self, p); return 1; }
    uint8_t* a = e->fd350(self, p, name1, buf2);
    uint32_t A = 0, B = 0;
    uint32_t t = e->f27020(0x6fe772b0, a, p, &A, &B);
    uint32_t R = 1, D = 0;
    if (B > 1 && A != 0) {
        bool rescale = true;
        if (t) {
            uint32_t h = G(G(t)[0x20 / 4])[0xb0 / 4];
            if (!h) return 0;
            D = e->ord10092(h);
            if (D) rescale = false;
        }
        if (rescale && !(a[0] & 1)) {
            uint32_t n = *(uint32_t*)(a + 0x14);
            uint32_t r = e->f221c0((p[0x10] * n) >> 6, n, B, ((uint8_t*)p)[4] & 4);
            uint32_t n2 = *(uint32_t*)(a + 0x14);
            p[0x10] = n2 > 1 ? (r << 6) / n2 : 1;
            t = e->f27020(0x6fe772b0, a, p, &A, &B);
        }
    }
    if (t == 0) {
        if (p2) return 0;
        if (name1[0] == 0 && e->fc2d0(name1, buf2, p) == 0) return 0;
        R = e->fddc0(p, a, A, B, name1);
        if (R) e->fe090(p, buf2, A, B, a, name1, p3);
        return R;
    }
    uint32_t cel = G(t)[0x20 / 4];
    if (!cel || (((uint8_t*)G(cel))[0x20] & 1) || !G(cel)[0xb0 / 4]) return 0;
    if (p2) return e->ord10092(G(cel)[0xb0 / 4]) != 0;
    e->fcae0(G(t)[0x24 / 4], 0x6fe77240);
    if (G(cel)[0x28 / 4] == 0) e->fcfc0(a, p, cel);
    uint32_t v = e->fd310(cel, p);
    p[0xf] = v;
    if (!v) { if (*e->g88398 == 0) *e->g88398 = 1; return 0; }
    if (D || A || B <= 1) return R;
    if (name1[0] == 0 && e->fc2d0(name1, buf2, p) == 0) return 0;
    e->fe090(p, buf2, 0, B, a, name1, p3);
    return R;
}

// ---- 0x11890: Ordinal 10015, clipped blit dispatch ----
typedef void (*D2CmpBlitFn)(uint32_t dst /* EDX */, int, int, int, uint32_t, int, int, uint32_t, uint32_t, uint32_t);
struct D2Cmp11890Env {
    D2CmpBlitFn const* table;   // PTR_FUN_6fe2d5c0
    D2CmpFatalFn fatal;
};
// 1.13c D2CMP+0x11890
void d2cmp_11890(uint32_t* p1, int p2, int p3, int p4, int p5, int p6, int p7,
                 uint32_t p8, uint32_t p9, uint32_t p10, uint32_t p11, const D2Cmp11890Env* e) {
    uint32_t* a = G(p1[0x34 / 4]);
    if (a) {
        if (a[0] != 6) e->fatal(0x452);
        if (((uint8_t*)a)[4] & 4) e->fatal(0x453);
    }
    if (p6 == 0) e->fatal(0x456);
    uint32_t* b = G(p1[0x3c / 4]);
    if (!b) e->fatal(0x459);
    int h = (int)b[2];
    int y = p3 + (int)b[4];
    int x = p2 + (int)b[3];
    int skip = 0;
    if (y - h < 0) h = y + 1;
    if (h <= 0) return;
    if (p7 <= y) {
        skip = y - p7 + 1;
        h -= skip;
        if (h < 1) return;
        y = p7 - 1;
    }
    if (p4 != 0 || p5 != 0 || (x >= 0 && y >= 0)) {
        uint32_t idx = a ? a[2] : 0;
        if (!e->table[idx]) e->fatal(0x481);
        uint32_t dst = (uint32_t)y * p8 + (uint32_t)x + (uint32_t)p6;   // EDX: p6 is the destination base, p8 the pitch
        e->table[idx](dst, x, p4, p5, p8, skip, h, p9, p10, p11);
    }
}

// ---- 0x12AA0 / 0x12B20: CelDataHash name table (512 buckets at base+0x800, chain next at +0x2c) ----
struct D2CmpHashEnv {
    void* (*ord10045)(const char* file, int line, int z);   // alloc
    uint32_t (*ord502)(const char* s, int a, int b);        // string hash
};
// 1.13c D2CMP+0x12AA0 (EBX = value, EDI = name; param_1 = table base)
uint32_t* d2cmp_12aa0(uint32_t* base, uint32_t ebx, const char* name, const D2CmpHashEnv* e) {
    uint32_t* n = (uint32_t*)e->ord10045("..\\Source\\D2CMP\\SRC\\CelDataHash.cpp", 0x88, 0);
    n[9] = ebx;
    uint32_t h = e->ord502(name, 1, 0);
    uint32_t b = ((h >> 8 ^ h) >> 16 ^ h >> 8 ^ h) & 0x1ff;
    n[10] = 0;
    uint32_t head = base[0x800 / 4 + b];
    n[0xb] = head;
    if (head) G(head)[10] = (uint32_t)(uintptr_t)n;
    n[0] = b;
    char* d = (char*)(n + 1);
    while ((*d++ = *name++) != 0) {}
    base[0x800 / 4 + b] = (uint32_t)(uintptr_t)n;
    return n;
}

// 1.13c D2CMP+0x12B20 (EBX = table base)
uint32_t d2cmp_12b20(const uint32_t* base, const uint8_t* name, const D2CmpHashEnv* e) {
    if (*name == 0) return 0;
    uint32_t h = e->ord502((const char*)name, 1, 0);
    uint32_t n = base[0x800 / 4 + ((((h >> 8 ^ h) >> 8 ^ h) >> 8 ^ h) & 0x1ff)];
    while (n) {
        const uint8_t* a = name; const uint8_t* b = (const uint8_t*)(uintptr_t)n + 4;
        while (*a == *b && *a) { a++; b++; }
        if (*a == *b) return n;
        n = G(n)[0x2c / 4];
    }
    return 0;
}

// ---- 0x16E80: record hash (EAX = record, ESI = out, EDI = second struct of 0x16C90) ----
extern uint32_t d2cmp_16c90(const uint8_t* ctx, const uint8_t* edi, uint32_t* esi);
struct D2Cmp16e80Env {
    int (*ord502)(uint32_t str, int a, int b);
    D2CmpFatalFn fatal;
};
// 1.13c D2CMP+0x16E80
uint32_t d2cmp_16e80(const uint8_t* rec, const uint8_t* edi, uint32_t* out, const D2Cmp16e80Env* e) {
    const uint32_t* r = (const uint32_t*)rec;
    uint32_t v = d2cmp_16c90(rec, edi, out);
    *out = v;
    int sum;
    int mode = (int)r[2];
    if ((mode == 6 && (rec[4] & 2)) || mode == 3 || mode == 4) {
        if (r[0x2c / 4] == 0) e->fatal(0x2c);
        sum = e->ord502(r[0x2c / 4], 1, 0);
        v = *out;
    } else {
        sum = (int)(r[0x28 / 4] + r[0x24 / 4] + r[0x20 / 4] + r[0x1c / 4] + r[0x18 / 4]);
    }
    v = (uint32_t)sum + v;
    return ((v >> 8 ^ v) >> 16 ^ v >> 8 ^ v) & 0x1fff;
}
