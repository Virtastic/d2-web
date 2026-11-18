// Fog.dll CRT rows 0x8FE7..0x9CD0 from build-wasm/ghidra-fog/decomp/0x<RVA>.c (alpha lane).
// Image base 0x6FF50000. Win32/TIB/unrecovered callees are host seams passed as params; 32-bit links are uint32_t.
#include <cstdint>
#include <cstring>

static inline uint32_t* P32_b8(uint32_t a) { return (uint32_t*)(uintptr_t)a; }

// 1.13c Fog+0x8FE7  __security_check_cookie(cookie, expected); report_failure (0x8FB6) is the seam.
extern "C" void fog_security_check_cookie_b8(uint32_t cookie, uint32_t expected, void (*report_failure)(void)) {
    if (cookie == expected) return;
    report_failure();
}

// 1.13c Fog+0x91EA  __abnormal_termination; exception_list = fs:[0]
extern "C" int32_t fog_abnormal_termination_b8(uint32_t exception_list) {
    int32_t r = 0;
    if (P32_b8(exception_list)[1] == 0x6FF59160u &&
        (int32_t)P32_b8(exception_list)[2] == (int32_t)P32_b8(P32_b8(exception_list)[3])[3])
        r = 1;
    return r;
}

// 1.13c Fog+0x920D  __NLG_Notify1 (EAX/EBP are register inputs, passed as params)
uint32_t g_fog_nlg_9a0_b8, g_fog_nlg_9a4_b8, g_fog_nlg_9a8_b8;  // +0x2E9A0/A4/A8
extern "C" void fog_nlg_notify1_b8(uint32_t ecx, uint32_t eax, uint32_t ebp) {
    g_fog_nlg_9a4_b8 = ecx; g_fog_nlg_9a0_b8 = eax; g_fog_nlg_9a8_b8 = ebp;
}

// 1.13c Fog+0x9723  ___crtInitCritSecNoSpinCount@8
extern "C" uint32_t fog_crtInitCritSecNoSpinCount_b8(uint32_t cs, void (*InitializeCriticalSection)(uint32_t)) {
    InitializeCriticalSection(cs);
    return 1;
}

// 1.13c Fog+0x97BE  __ValidateEH3RN
uint32_t g_fog_eh3_count_b8;        // +0x30578 DAT_6ff80578
uint32_t g_fog_eh3_cache_b8[16];    // +0x30580
uint32_t g_fog_eh3_lock_b8;         // +0x305C0
struct FogEh3Env_b8 {
    uint32_t tib;                                              // Self: +4 stack base, +8 stack limit
    int32_t (*interlocked_xchg)(uint32_t*, int32_t);           // InterlockedExchange
    uint32_t (*virtual_query)(uint32_t addr, uint32_t* mbi);   // VirtualQuery(addr, &mbi, 0x1c)
};
extern "C" uint32_t fog_ValidateEH3RN_b8(uint32_t frame, const FogEh3Env_b8* env) {
    uint32_t pvVar3 = frame;
    uint32_t puVar8 = P32_b8(frame)[2];
    uint32_t local_8;
    if ((puVar8 & 3) != 0) return 0;
    local_8 = P32_b8(env->tib)[2];
    if (local_8 <= puVar8 && puVar8 < P32_b8(env->tib)[1]) return 0;
    uint32_t* puVar1 = P32_b8(frame + 0xc);
    if (*puVar1 == 0xffffffffu) return 1;
    uint32_t uVar10 = 0, cnt = 0;
    uint32_t* puVar4 = P32_b8(puVar8);
    do {
        if (*puVar4 != 0xffffffffu && uVar10 <= *puVar4) return 0;
        if (puVar4[1] != 0) cnt++;
        uVar10++;
        puVar4 += 3;
    } while (uVar10 <= *puVar1);
    if (cnt != 0) {
        uint32_t p = P32_b8(pvVar3 - 8)[0];
        if (p < local_8 || pvVar3 <= p) return 0;
    }
    uVar10 = puVar8 & 0xfffff000u;
    int32_t iVar13 = 0, iVar11, iVar12;
    uint32_t uVar9;
    if (0 < (int32_t)g_fog_eh3_count_b8) {
        do {
            if (g_fog_eh3_cache_b8[iVar13] == uVar10) {
                if (iVar13 < 1) return 1;
                if (env->interlocked_xchg(&g_fog_eh3_lock_b8, 1) != 0) return 1;
                if (g_fog_eh3_cache_b8[iVar13] == uVar10) goto LAB_b9;
                iVar13 = (int32_t)g_fog_eh3_count_b8 - 1;
                if (iVar13 < 0) goto LAB_a7;
                goto LAB_97;
            }
            iVar13++;
        } while (iVar13 < (int32_t)g_fog_eh3_count_b8);
    }
    {
        uint32_t mbi[7];  // BaseAddress, AllocationBase, AllocationProtect, RegionSize, State, Protect, Type
        if (env->virtual_query(puVar8, mbi) == 0) return 0xffffffffu;
        if (mbi[6] != 0x1000000u) return 0xffffffffu;
        if ((mbi[5] & 0xcc) != 0) {
            uint32_t base = mbi[1];
            if (*(uint16_t*)(uintptr_t)base != 0x5a4d) return 0xffffffffu;
            uint32_t pe = *P32_b8(base + 0x3c) + base;
            if (*P32_b8(pe) != 0x4550u || *(int16_t*)(uintptr_t)(pe + 0x18) != 0x10b) return 0xffffffffu;
            uVar9 = *(uint16_t*)(uintptr_t)(pe + 0x14);
            if (*(int16_t*)(uintptr_t)(pe + 6) == 0) return 0xffffffffu;
            uint32_t uVar2 = *P32_b8(pe + uVar9 + 0x24);
            uint32_t off = puVar8 - base;
            if (uVar2 <= off && off < *P32_b8(pe + uVar9 + 0x20) + uVar2 &&
                (*(uint8_t*)(uintptr_t)(pe + uVar9 + 0x3f) & 0x80) != 0)
                return 0;
        }
    }
    iVar13 = (int32_t)g_fog_eh3_count_b8;
    if (env->interlocked_xchg(&g_fog_eh3_lock_b8, 1) != 0) return 1;
    iVar11 = (int32_t)g_fog_eh3_count_b8;
    if (0 < (int32_t)g_fog_eh3_count_b8) {
        uint32_t* p = &g_fog_eh3_cache_b8[g_fog_eh3_count_b8 - 1];  // &DAT_6ff8057c + count*4
        do {
            if (*p == uVar10) break;
            iVar11--; p--;
        } while (0 < iVar11);
    }
    if (iVar11 == 0) {
        iVar11 = 0xf;
        if ((int32_t)g_fog_eh3_count_b8 < 0x10) iVar11 = (int32_t)g_fog_eh3_count_b8;
        iVar12 = 0;
        if (-1 < iVar11) {
            do {
                uint32_t* p = &g_fog_eh3_cache_b8[iVar12];
                uVar9 = *p; iVar12++; *p = uVar10; uVar10 = uVar9;
            } while (iVar12 <= iVar11);
        }
        if (iVar13 < 0x10) g_fog_eh3_count_b8 = iVar13 + 1;
    }
    env->interlocked_xchg(&g_fog_eh3_lock_b8, 0);
    return 1;
    while (iVar13--, -1 < iVar13) {
LAB_97:
        if (g_fog_eh3_cache_b8[iVar13] == uVar10) break;
    }
    if (iVar13 < 0) {
LAB_a7:
        if ((int32_t)g_fog_eh3_count_b8 < 0x10) g_fog_eh3_count_b8++;
        iVar13 = (int32_t)g_fog_eh3_count_b8 - 1;
    } else if (iVar13 == 0) goto LAB_d1;
LAB_b9:
    iVar11 = 0;
    if (-1 < iVar13) {
        do {
            uint32_t* p = &g_fog_eh3_cache_b8[iVar11];
            uVar9 = *p; iVar11++; *p = uVar10; uVar10 = uVar9;
        } while (iVar11 <= iVar13);
    }
LAB_d1:
    env->interlocked_xchg(&g_fog_eh3_lock_b8, 0);
    return 1;
}

// 1.13c Fog+0x99E7  __getbuf; FILE: ptr 0, cnt 4, base 8, flag 0xC, file 0x10, charbuf 0x14, bufsiz 0x18
uint32_t g_fog_bufcount_b8;  // +0x2E3D8 _DAT_6ff803d8
extern "C" void fog_getbuf_b8(uint32_t file, uint32_t (*malloc_)(uint32_t)) {
    uint32_t* f = P32_b8(file);
    g_fog_bufcount_b8++;
    uint32_t buf = malloc_(0x1000);
    f[2] = buf;
    if (buf == 0) {
        f[3] |= 4;
        f[2] = file + 0x14;
        f[6] = 2;
    } else {
        f[3] |= 8;
        f[6] = 0x1000;
    }
    f[1] = 0;
    f[0] = f[2];
}

// 1.13c Fog+0x9A55  ___wctomb_mt(locinfo, dst, wc); WideCharToMultiByte(cp, 0, &wc, 1, dst, mbcurmax, 0, &usedDefault)
struct FogWctombEnv_b8 {
    int32_t (*wc2mb)(uint32_t cp, uint32_t flags, const uint16_t* wc, int32_t n, uint32_t dst, int32_t dstlen, uint32_t def, uint32_t* used);
    void (*set_errno)(int32_t);  // *FUN_6ff56a5a() = v
};
extern "C" int32_t fog_wctomb_mt_b8(uint32_t loc, uint32_t dst, uint16_t wc, const FogWctombEnv_b8* env) {
    if (dst == 0) return 0;
    if (P32_b8(loc)[0x14 / 4] == 0) {
        if (wc < 0x100) { *(uint8_t*)(uintptr_t)dst = (uint8_t)wc; return 1; }
    } else {
        uint32_t used = 0;
        int32_t r = env->wc2mb(P32_b8(loc)[1], 0, &wc, 1, dst, (int32_t)P32_b8(loc)[0x28 / 4], 0, &used);
        if (r != 0 && used == 0) return r;
    }
    env->set_errno(0x2a);
    return -1;
}

// 1.13c Fog+0x9AB5  _wctomb; seams: __getptd (0x38CB) tfpecode field, ___updatetlocinfo (0x3857), g_loc = PTR_DAT_6ff7e0d4
extern "C" int32_t fog_wctomb_b8(uint32_t dst, uint16_t wc, uint32_t cur_loc, uint32_t g_loc,
                                 uint32_t (*updatetlocinfo)(void), const FogWctombEnv_b8* env) {
    if (cur_loc != g_loc) cur_loc = updatetlocinfo();
    return fog_wctomb_mt_b8(cur_loc, dst, wc, env);
}

// 1.13c Fog+0x9AE0  __aulldvrm: quotient returned, remainder via *rem (retail: quotient edx:eax, remainder ebx:ecx)
extern "C" uint64_t fog_aulldvrm_b8(uint32_t lo, uint32_t hi, uint32_t dlo, uint32_t dhi, uint64_t* rem) {
    uint32_t qh, ql;
    if (dhi == 0) {
        qh = hi / dlo;
        ql = (uint32_t)((((uint64_t)(hi % dlo) << 32) | lo) / dlo);
    } else {
        uint32_t d = dhi, uh = hi, ul = lo, dd = dlo;
        do {
            dd = dd >> 1 | ((d & 1) << 31);
            ul = ul >> 1 | ((uh & 1) << 31);
            d >>= 1; uh >>= 1;
        } while (d != 0);
        uint64_t q = (((uint64_t)uh << 32) | ul) / dd;
        ql = (uint32_t)q;
        uint64_t m = (uint64_t)dlo * ql;
        uint32_t mh = (uint32_t)(m >> 32);
        uint32_t t = mh + ql * dhi;
        if (t < mh || hi < t || (hi <= t && lo < (uint32_t)m)) ql--;
        qh = 0;
    }
    uint64_t qq = ((uint64_t)qh << 32) | ql;
    if (rem) *rem = (((uint64_t)hi << 32) | lo) - qq * (((uint64_t)dhi << 32) | dlo);
    return qq;
}

// 1.13c Fog+0x9B75  _ungetc / __ungetc_lk; FILE layout as __getbuf; getbuf = 0x99E7 seam
extern "C" int32_t fog_ungetc_b8(int32_t ch, uint32_t file, void (*getbuf)(uint32_t)) {
    if (ch == -1) return -1;
    uint32_t* f = P32_b8(file);
    uint32_t fl = f[3];
    if ((fl & 1) != 0 || ((int8_t)fl < 0 && (fl & 2) == 0)) {
        if (f[2] == 0) getbuf(file);
        if (f[0] == f[2]) {
            if (f[1] != 0) return -1;
            f[0]++;
        }
        f[0]--;
        uint8_t* p = (uint8_t*)(uintptr_t)f[0];
        if ((f[3] & 0x40) == 0) *p = (uint8_t)ch;
        else if (*p != (uint8_t)ch) { f[0]++; return -1; }
        f[1]++;
        f[3] = (f[3] & 0xffffffefu) | 1;
        return ch & 0xff;
    }
    return -1;
}

// 1.13c Fog+0x9CA1  _mbtowc; ___mbtowc_mt = 0x9BE1 seam
extern "C" int32_t fog_mbtowc_b8(uint32_t dstch, uint32_t src, uint32_t n, uint32_t cur_loc, uint32_t g_loc,
                                 uint32_t (*updatetlocinfo)(void),
                                 int32_t (*mbtowc_mt)(uint32_t, uint32_t, uint32_t, uint32_t)) {
    if (cur_loc != g_loc) cur_loc = updatetlocinfo();
    return mbtowc_mt(cur_loc, dstch, src, n);
}

// 1.13c Fog+0x9CD0  __allmul
extern "C" int64_t fog_allmul_b8(uint32_t a, int32_t ah, uint32_t b, int32_t bh) {
    if (ah == 0 && bh == 0) return (int64_t)((uint64_t)a * b);
    uint64_t p = (uint64_t)a * b;
    return (int64_t)(((uint64_t)(uint32_t)((int32_t)(p >> 32) + ah * (int32_t)b + (int32_t)a * bh) << 32) | (uint32_t)p);
}
