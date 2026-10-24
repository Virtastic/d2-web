// src/core/realm/d2mcpclient_n77.cpp -- the 14 STALLED 1.13c D2MCPClient.dll CRT rows, reimplemented from
// build-wasm/ghidra-d2mcpclient/decomp/0x<RVA>.c. Base 0x6FA20000. Retail's module globals are fields of
// state structs the caller owns; every kernel32 import, CRT callee outside the row and retail 32-bit
// address read is a host function-pointer seam (the wasm module has no PE import table, and wasm64
// pointers do not fit retail's dwords). Retail "addresses" are uint32 offsets from Mem::base.
#include <cstdint>
#include <cstring>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

struct N77Mem {
    uint8_t* base;
    uint32_t r32(uint32_t a) const { uint32_t v; memcpy(&v, base + a, 4); return v; }
    void w32(uint32_t a, uint32_t v) const { memcpy(base + a, &v, 4); }
    uint32_t r16(uint32_t a) const { uint16_t v; memcpy(&v, base + a, 2); return v; }
};

struct N77Locks {                       // 0x14EE _unlock / 0x15A3 __lock
    void (*lock)(uint32_t idx);
    void (*unlock)(uint32_t idx);
};

// ---- the 9-byte `push N; call 0x14EE; pop ecx; ret` thunks --------------------------------------------
// 1.13c D2MCPCLIENT+0x0159A
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_n77_159a(const N77Locks* l) { l->unlock(10); }
// 1.13c D2MCPCLIENT+0x01CEB
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_n77_1ceb(const N77Locks* l) { l->unlock(4); }
// 1.13c D2MCPCLIENT+0x02B52
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_n77_2b52(const N77Locks* l) { l->unlock(4); }
// 1.13c D2MCPCLIENT+0x034D5
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_n77_34d5(const N77Locks* l) { l->unlock(0xd); }

// ---- 0x2B9C __msize ------------------------------------------------------------------------------------
struct N77MsizeState { uint32_t sbhMode; uint32_t heap; };       // DAT_6fa324a4, DAT_6fa324a0
struct N77MsizeEnv {
    N77Locks locks;
    uint32_t (*sbh_find_block)(uint32_t mem);                    // 0x1750-side ___sbh_find_block
    uint32_t (*heap_size)(uint32_t heap, uint32_t flags, uint32_t mem);  // HeapSize
    N77Mem mem;
};
// 1.13c D2MCPCLIENT+0x02B9C
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_n77_2b9c(const N77MsizeState* s, const N77MsizeEnv* e, uint32_t p) {
    if (s->sbhMode == 3) {
        e->locks.lock(4);
        uint32_t hdr = e->sbh_find_block(p);
        uint32_t sz = hdr ? e->mem.r32(p - 4) - 9 : 0;
        e->locks.unlock(4);                                      // 0x2C09
        if (hdr) return sz;
    }
    return e->heap_size(s->heap, 0, p);
}

// ---- 0x2C12 __ValidateEH3RN ----------------------------------------------------------------------------
struct N77EhState {
    uint32_t stackHigh, stackLow;                // *(Self+4), *(Self+8): the thread's stack bounds
    int32_t  cacheN;                             // DAT_6fa2acb8
    uint32_t cache[16];                          // DAT_6fa2acc0 (page numbers already checked)
    int32_t  busy;                               // DAT_6fa2ad00 spin flag
};
struct N77EhEnv {
    N77Mem mem;
    // VirtualQuery(addr): 0 on failure, else fills allocBase, type, protect
    uint32_t (*virtual_query)(uint32_t addr, uint32_t* allocBase, uint32_t* type, uint32_t* protect);
};
static int32_t n77_xchg(int32_t* p, int32_t v) { int32_t o = *p; *p = v; return o; }  // InterlockedExchange
static void n77_front(N77EhState* s, int32_t upto, uint32_t page) {      // shift [0..upto] right, page to front
    for (int32_t i = 0; i <= upto; i++) { uint32_t t = s->cache[i]; s->cache[i] = page; page = t; }
}
// 1.13c D2MCPCLIENT+0x02C12
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_n77_2c12(N77EhState* s, const N77EhEnv* e, uint32_t rec) {
    const N77Mem& m = e->mem;
    uint32_t table = m.r32(rec + 8);
    if ((table & 3) != 0 || (s->stackLow <= table && table < s->stackHigh)) return 0;
    uint32_t last = m.r32(rec + 0xc);
    if (last == 0xffffffffu) return 1;
    uint32_t i = 0, handlers = 0, t = table;
    do {
        uint32_t prev = m.r32(t);
        if (prev != 0xffffffffu && i <= prev) return 0;
        if (m.r32(t + 4) != 0) handlers++;
        i++; t += 12;
    } while (i <= last);
    if (handlers != 0) { uint32_t f = m.r32(rec - 8); if (f < s->stackLow || rec <= f) return 0; }
    uint32_t page = table & 0xfffff000u;
    int32_t k = 0;
    for (; k < s->cacheN; k++) if (s->cache[k] == page) {
        if (k < 1) return 1;
        if (n77_xchg(&s->busy, 1) != 0) return 1;
        if (s->cache[k] != page) {                               // moved while we spun: search from the back
            k = s->cacheN - 1;
            while (k >= 0 && s->cache[k] != page) k--;
            if (k < 0) { if (s->cacheN < 16) s->cacheN++; k = s->cacheN - 1; }
            else if (k == 0) { n77_xchg(&s->busy, 0); return 1; }
        }
        n77_front(s, k, page);
        n77_xchg(&s->busy, 0);
        return 1;
    }
    uint32_t base, type, prot;
    if (e->virtual_query(table, &base, &type, &prot) == 0) return -1;
    if (type != 0x1000000) return -1;
    if ((prot & 0xcc) != 0) {
        if (m.r16(base) != 0x5a4d) return -1;
        uint32_t pe = m.r32(base + 0x3c) + base;
        if (m.r32(pe) != 0x4550 || m.r16(pe + 0x18) != 0x10b) return -1;
        uint32_t opt = m.r16(pe + 0x14);
        if (m.r16(pe + 6) == 0) return -1;
        uint32_t lo = m.r32(pe + opt + 0x24), off = table - base;
        if (lo <= off && off < m.r32(pe + opt + 0x20) + lo && (m.base[pe + opt + 0x3f] & 0x80)) return 0;
    }
    if (n77_xchg(&s->busy, 1) != 0) return 1;
    int32_t n = s->cacheN, j = n;
    while (j > 0 && s->cache[j - 1] != page) j--;
    if (j == 0) {
        int32_t top = n < 16 ? n : 15;
        n77_front(s, top, page);
        if (n < 16) s->cacheN = n + 1;
    }
    n77_xchg(&s->busy, 0);
    return 1;
}

// ---- 0x2F0B ___updatetlocinfo_lk / 0x2FCC ___updatetlocinfo --------------------------------------------
struct N77TlocState { uint32_t defaultInfo; uint32_t staticInfo; };     // PTR_DAT_6fa2a484 value, &DAT_6fa2a430
struct N77TlocEnv {
    N77Mem mem;
    N77Locks locks;
    uint32_t (*get_ptd_locinfo)();                                       // __getptd()->ptlocinfo
    void (*set_ptd_locinfo)(uint32_t v);
    void (*free_tlocinfo)(uint32_t info);                                // ___freetlocinfo 0x18DE
};
static void n77_dec(const N77Mem& m, uint32_t p) { if (p) m.w32(p, m.r32(p) - 1); }
static void n77_inc(const N77Mem& m, uint32_t p) { if (p) m.w32(p, m.r32(p) + 1); }
// 1.13c D2MCPCLIENT+0x02F0B
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_n77_2f0b(const N77TlocState* s, const N77TlocEnv* e) {
    const N77Mem& m = e->mem;
    uint32_t old = e->get_ptd_locinfo(), d = s->defaultInfo;
    if (old != d) {
        if (old) {
            m.w32(old, m.r32(old) - 1);
            n77_dec(m, m.r32(old + 0x2c)); n77_dec(m, m.r32(old + 0x34)); n77_dec(m, m.r32(old + 0x30));
            n77_dec(m, m.r32(old + 0x40));
            uint32_t c = m.r32(old + 0x4c) + 0xb4; m.w32(c, m.r32(c) - 1);
        }
        e->set_ptd_locinfo(d);
        m.w32(d, m.r32(d) + 1);
        n77_inc(m, m.r32(d + 0x2c)); n77_inc(m, m.r32(d + 0x34)); n77_inc(m, m.r32(d + 0x30));
        n77_inc(m, m.r32(d + 0x40));
        uint32_t c = m.r32(d + 0x4c) + 0xb4; m.w32(c, m.r32(c) + 1);
        if (old && m.r32(old) == 0 && old != s->staticInfo) e->free_tlocinfo(old);
    }
    return e->get_ptd_locinfo();
}
// 1.13c D2MCPCLIENT+0x02FCC
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_n77_2fcc(const N77TlocState* s, const N77TlocEnv* e) {
    e->locks.lock(0xc);
    uint32_t r = d2_mcp_n77_2f0b(s, e);
    e->locks.unlock(0xc);                                                // 0x2FFE
    return r;
}

// ---- multibyte code page: 0x31FA, 0x338E, 0x34DE ------------------------------------------------------
struct N77MbState {
    int32_t  userDefault;                    // DAT_6fa2ad10 (code page came from -2/-3/-4)
    uint32_t fallbackCp;                     // DAT_6fa2ad78
    uint32_t cachedBlock;                    // DAT_6fa3226c
    uint32_t cp;                             // DAT_6fa32384
    uint32_t isMb;                           // DAT_6fa32270
    uint32_t lcid;                           // DAT_6fa32268
    uint16_t info[6];                        // DAT_6fa32390 (3 dwords)
    uint8_t  ctype[0x101];                   // DAT_6fa32280
    uint8_t  caseMap[0x100];                 // DAT_6fa323a0
    int32_t  tableInited;                    // DAT_6fa325d4
    uint8_t  flagBits[8];                    // DAT_6fa2a5c0
    struct { uint32_t cp; uint32_t info[3]; uint8_t ranges[32]; } known[5];  // DAT_6fa2a5c8, 0x30 each
};
struct N77MbEnv {
    N77Mem mem;
    N77Locks locks;
    int32_t (*get_cp_info)(uint32_t cp, uint32_t* maxCharSize, uint8_t leadBytes[12]);   // GetCPInfo, 1 = ok
    uint32_t (*get_oemcp)();
    uint32_t (*get_acp)();
    uint32_t (*malloc_)(uint32_t n);
    void (*free_)(uint32_t p);
    void (*finish)(N77MbState* s);                                       // 0x305F: case/ctype tables
};
static void n77_clear(N77MbState* s) { memset(s->ctype, 0, sizeof s->ctype); }
static uint32_t n77_lcid(uint32_t cp) {                                  // 0x3007
    return cp == 0x3a4 ? 0x411 : cp == 0x3a8 ? 0x804 : cp == 0x3b5 ? 0x412 : cp == 0x3b6 ? 0x404 : 0;
}
// 1.13c D2MCPCLIENT+0x031FA
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_n77_31fa(N77MbState* s, const N77MbEnv* e, uint32_t cp) {
    if (cp != 0) {
        for (int i = 0; i < 5; i++) {
            if (s->known[i].cp != cp) continue;
            n77_clear(s);
            for (uint32_t q = 0; q < 4; q++)
                for (const uint8_t* r = s->known[i].ranges + q * 8; r[0] && r[1]; r += 2)
                    for (uint32_t c = r[0]; c <= r[1]; c++) s->ctype[c + 1] |= s->flagBits[q];
            s->cp = cp; s->isMb = 1; s->lcid = n77_lcid(cp);
            memcpy(s->info, s->known[i].info, 12);
            e->finish(s);
            return 0;
        }
        uint32_t maxc; uint8_t lead[12];
        if (e->get_cp_info(cp, &maxc, lead) == 1) {
            n77_clear(s);
            s->cp = cp; s->lcid = 0;
            if (maxc < 2) s->isMb = 0;
            else {
                for (int k = 0; k < 12 && lead[k]; k += 2) {
                    if (!lead[k + 1]) break;
                    for (uint32_t c = lead[k]; c <= lead[k + 1]; c++) s->ctype[c + 1] |= 4;
                }
                for (uint32_t c = 1; c < 0xff; c++) s->ctype[c + 1] |= 8;
                s->lcid = n77_lcid(cp); s->isMb = 1;
            }
            memset(s->info, 0, 12);
            e->finish(s);
            return 0;
        }
        if (s->userDefault == 0) return -1;
    }
    n77_clear(s);                                                        // setSBCS 0x3036
    s->cp = 0; s->isMb = 0; s->lcid = 0; memset(s->info, 0, 12);
    e->finish(s);
    return 0;
}

// 1.13c D2MCPCLIENT+0x0338E
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_n77_338e(N77MbState* s, const N77MbEnv* e, int32_t cpArg) {
    int32_t rc = -1;
    e->locks.lock(0xd);
    s->userDefault = 0;
    uint32_t cp = (uint32_t)cpArg;
    if (cpArg == -2) { s->userDefault = 1; cp = e->get_oemcp(); }
    else if (cpArg == -3) { s->userDefault = 1; cp = e->get_acp(); }
    else if (cpArg == -4) { s->userDefault = 1; cp = s->fallbackCp; }
    if (cp == s->cp) rc = 0;
    else {
        N77Mem m = e->mem;
        uint32_t blk = s->cachedBlock;
        if (blk == 0 || m.r32(blk) != 0) blk = e->malloc_(0x220);
        if (blk != 0 && (rc = d2_mcp_n77_31fa(s, e, cp)) == 0) {
            m.w32(blk, 0); m.w32(blk + 4, s->cp); m.w32(blk + 8, s->isMb); m.w32(blk + 12, s->lcid);
            memcpy(m.base + blk + 0x10, s->info, 10);
            memcpy(m.base + blk + 0x1c, s->ctype, 0x101);
            memcpy(m.base + blk + 0x11d, s->caseMap, 0x100);
            s->cachedBlock = blk;
        }
        if (rc == -1 && blk != s->cachedBlock && blk != 0) e->free_(blk);
    }
    e->locks.unlock(0xd);                                                // 0x34D5
    return rc;
}

// 1.13c D2MCPCLIENT+0x034DE
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_n77_34de(N77MbState* s, const N77MbEnv* e) {
    if (s->tableInited == 0) { d2_mcp_n77_338e(s, e, -3); s->tableInited = 1; }
    return 0;
}

// ---- small-block heap: 0x3887, 0x393E, 0x3D23 -----------------------------------------------------------
struct N77SbhState {
    uint32_t entry, count, regions, max, lastRegion, capacity, seq, heap;  // DAT_6fa3224c..324a0
};
struct N77SbhEnv {
    N77Mem mem;
    uint32_t (*heap_alloc)(uint32_t heap, uint32_t flags, uint32_t bytes);
    uint32_t (*heap_realloc)(uint32_t heap, uint32_t flags, uint32_t p, uint32_t bytes);
    void (*heap_free)(uint32_t heap, uint32_t flags, uint32_t p);
    uint32_t (*virtual_alloc)(uint32_t addr, uint32_t size, uint32_t type, uint32_t protect);
};
// 1.13c D2MCPCLIENT+0x03887
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_n77_3887(N77SbhState* s, const N77SbhEnv* e) {
    const N77Mem& m = e->mem;
    if (s->count == s->capacity) {
        uint32_t g = e->heap_realloc(s->heap, 0, s->regions, (s->capacity * 5 + 0x50) * 4);
        if (!g) return 0;
        s->capacity += 0x10; s->regions = g;
    }
    uint32_t r = s->regions + s->count * 0x14;
    uint32_t ctl = e->heap_alloc(s->heap, 8, 0x41c4);
    m.w32(r + 0x10, ctl);
    if (!ctl) return 0;
    uint32_t base = e->virtual_alloc(0, 0x100000, 0x2000, 4);
    m.w32(r + 0xc, base);
    if (!base) { e->heap_free(s->heap, 0, ctl); return 0; }
    m.w32(r + 8, 0xffffffffu); m.w32(r, 0); m.w32(r + 4, 0);
    s->count++;
    m.w32(ctl, 0xffffffffu);
    return r;
}

// 1.13c D2MCPCLIENT+0x0393E
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_n77_393e(const N77SbhState*, const N77SbhEnv* e, uint32_t region) {
    const N77Mem& m = e->mem;
    uint32_t ctl = m.r32(region + 0x10);
    int32_t g = 0;
    for (uint32_t b = m.r32(region + 8); (int32_t)b >= 0; b <<= 1) g++;
    uint32_t bk = (uint32_t)g * 0x204 + 0x144 + ctl;
    for (uint32_t it = bk, n = 0x3f; n; n--, it += 8) { m.w32(it + 8, it); m.w32(it + 4, it); }
    uint32_t lp = (uint32_t)g * 0x8000 + m.r32(region + 0xc);
    if (!e->virtual_alloc(lp, 0x8000, 0x1000, 4)) return -1;
    for (uint32_t p = lp + 0x10, n = 8; n; n--, p += 0x1000) {
        m.w32(p - 8, 0xffffffffu); m.w32(p + 0xfec, 0xffffffffu);
        m.w32(p, p + 0xffc); m.w32(p - 4, 0xff0);
        m.w32(p + 4, p - 0x1004); m.w32(p + 0xfe8, 0xff0);
    }
    m.w32(bk + 0x1fc, lp + 0xc);  m.w32(lp + 0x14, bk + 0x1f8);
    m.w32(bk + 0x200, lp + 0x700c); m.w32(lp + 0x7010, bk + 0x1f8);
    m.w32(ctl + 0x44 + g * 4, 0); m.w32(ctl + 0xc4 + g * 4, 1);
    uint8_t c = m.base[ctl + 0x43]++;
    if (c == 0) m.w32(region + 4, m.r32(region + 4) | 1);
    m.w32(region + 8, m.r32(region + 8) & ~(0x80000000u >> (g & 31)));
    return g;
}

// 1.13c D2MCPCLIENT+0x03D23
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_n77_3d23(N77SbhState* s, const N77SbhEnv* e, uint32_t req) {
    const N77Mem& m = e->mem;
    auto R = [&](uint32_t a) { return m.r32(a); };
    auto W = [&](uint32_t a, uint32_t v) { m.w32(a, v); };
    uint32_t size = (req + 0x17) & 0xfffffff0u;
    int32_t idx = ((int32_t)(req + 0x17) >> 4) - 1;
    uint32_t end = s->regions + s->count * 0x14, rg = s->lastRegion;
    uint32_t mLo, mHi;                                                   // masks for buckets >= idx
    if (idx < 0x20) { mLo = 0xffffffffu >> (idx & 31); mHi = 0xffffffffu; }
    else { mLo = 0; mHi = 0xffffffffu >> ((idx - 0x20) & 31); }
    for (; rg < end && (R(rg + 4) & mHi) == 0 && (R(rg) & mLo) == 0; rg += 0x14) {}
    if (rg == end) {
        uint32_t q = s->regions;
        for (; q < s->lastRegion && (R(q + 4) & mHi) == 0 && (R(q) & mLo) == 0; q += 0x14) {}
        rg = q;
        if (q == s->lastRegion) {
            for (; q < end && R(q + 8) == 0; q += 0x14) {}
            uint32_t q2 = s->regions;
            rg = q;
            if (q == end) {
                for (; q2 < s->lastRegion && R(q2 + 8) == 0; q2 += 0x14) {}
                rg = q2;
                if (q2 == s->lastRegion) { rg = d2_mcp_n77_3887(s, e); if (!rg) return 0; }
            }
            int32_t g = d2_mcp_n77_393e(s, e, rg);
            W(R(rg + 0x10), (uint32_t)g);
            if ((int32_t)R(R(rg + 0x10)) == -1) return 0;
        }
    }
    uint32_t ctl = R(rg + 0x10);
    int32_t grp = (int32_t)R(ctl);
    if (grp == -1 || ((R(ctl + (grp + 0x31) * 4) & mHi) == 0 && (R(ctl + (grp + 0x11) * 4) & mLo) == 0)) {
        grp = 0;
        uint32_t lo = ctl + 0x44;
        if ((R(ctl + 0xc4) & mHi) == 0 && (R(lo) & mLo) == 0) {
            uint32_t hiw, lop = lo;
            do { grp++; lop += 4; hiw = lop + 0x80; } while ((R(hiw) & mHi) == 0 && (R(lop) & mLo) == 0);
        }
    }
    uint32_t grpBase = ctl + (uint32_t)grp * 0x204 + 0x144;              // piVar5 + grp*0x81 + 0x51
    int32_t b = 0;
    uint32_t bits = R(ctl + 0x44 + grp * 4) & mLo;
    if (bits == 0) { bits = R(ctl + 0xc4 + grp * 4) & mHi; b = 0x20; }
    for (; (int32_t)bits >= 0; bits <<= 1) b++;
    uint32_t blk = R(grpBase + b * 8 + 4);
    int32_t rest = (int32_t)(R(blk) - size);
    int32_t nb = (rest >> 4) - 1; if (nb > 0x3f) nb = 0x3f;
    s->lastRegion = rg;
    if (nb != b) {
        if (R(blk + 4) == R(blk + 8)) {
            uint32_t cnt = ctl + 4 + b;
            if (b < 0x20) {
                uint32_t k = ~(0x80000000u >> (b & 31));
                W(ctl + 0x44 + grp * 4, k & R(ctl + 0x44 + grp * 4));
                if (--m.base[cnt] == 0) W(rg, R(rg) & k);
            } else {
                uint32_t k = ~(0x80000000u >> ((b - 0x20) & 31));
                W(ctl + 0xc4 + grp * 4, R(ctl + 0xc4 + grp * 4) & k);
                if (--m.base[cnt] == 0) W(rg + 4, R(rg + 4) & k);
            }
        }
        W(R(blk + 8) + 4, R(blk + 4));
        W(R(blk + 4) + 8, R(blk + 8));
        if (rest != 0) {
            uint32_t pn = grpBase + nb * 8, tail = R(pn + 4);
            W(blk + 8, pn); W(blk + 4, tail); W(pn + 4, blk); W(R(blk + 4) + 8, blk);
            if (R(blk + 4) == R(blk + 8)) {
                uint8_t c = m.base[ctl + 4 + nb]++;
                if (nb < 0x20) {
                    if (c == 0) W(rg, R(rg) | (0x80000000u >> (nb & 31)));
                    W(ctl + 0x44 + grp * 4, R(ctl + 0x44 + grp * 4) | (0x80000000u >> (nb & 31)));
                } else {
                    if (c == 0) W(rg + 4, R(rg + 4) | (0x80000000u >> ((nb - 0x20) & 31)));
                    W(ctl + 0xc4 + grp * 4, R(ctl + 0xc4 + grp * 4) | (0x80000000u >> ((nb - 0x20) & 31)));
                }
            }
        }
    }
    if (rest != 0) { W(blk, (uint32_t)rest); W(blk + (uint32_t)rest - 4, (uint32_t)rest); }
    uint32_t out = blk + (uint32_t)rest;
    W(out, size + 1); W(out + size - 4, size + 1);
    uint32_t used = R(grpBase);
    W(grpBase, used + 1);
    if (used == 0 && rg == s->entry && grp == (int32_t)s->seq) s->entry = 0;
    W(ctl, (uint32_t)grp);
    return out + 4;
}
