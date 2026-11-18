// src/core/fog/fog_batch_z29.cpp -- twelve UNMAPPED 1.13c Fog.dll rows transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-fog/decomp/0x<RVA>.c). Alpha batch lane: implementation +
// marker only, no spec, no vector. Retail base 0x6FF50000. MSVC 2003 CRT bodies (heap init, __fptostr,
// tolower/toupper, tlocinfo, _getptd, _freefls, _flsbuf), plus three SEH unwind thunks and the
// type_info destructor. Retail globals and callees are seams on Z29Env; 32-bit links stay uint32_t
// and are resolved through env->mem(). Ghidra's register-only arguments are explicit parameters.
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <emscripten.h>

struct Z29Env {
    uint8_t* (*mem)(uint32_t addr);                    // guest 32-bit address -> host pointer
    // heap
    uint32_t heap;                                     // +0x5A280 DAT_6ffa8280
    uint32_t heap_mode;                                // +0x5A284 DAT_6ffa8284
    uint32_t (*HeapCreate)(uint32_t flags, uint32_t init, uint32_t max);
    void     (*HeapDestroy)(uint32_t heap);
    uint32_t (*heap_select)(void);                     // ___heap_select +0x49C5
    int32_t  (*sbh_heap_init)(uint32_t threshold);     // ___sbh_heap_init
    // locks / free
    void (*lock)(int32_t n);                           // __lock +0x3DEB
    void (*unlock)(int32_t n);                         // the per-site unlock leaves
    void (*free)(uint32_t p);                          // _free +0x1FAD
    // ctype / locale
    uint32_t (*isctype_mt)(uint32_t ptloci, uint32_t c, uint32_t mask);          // ___isctype_mt
    uint32_t (*crtLCMapStringA)(uint32_t lcid, uint32_t flags, const void* src, uint32_t srclen,
                                void* dst, uint32_t dstlen, uint32_t cp, int32_t err);  // +0x3279
    uint32_t (*updatetlocinfo_lk)(void);               // ___updatetlocinfo_lk
    void     (*freetlocinfo)(uint32_t p);              // ___freetlocinfo
    // getptd
    uint32_t (*GetLastError)(void);
    void     (*SetLastError)(uint32_t e);
    uint32_t (*GetCurrentThreadId)(void);
    uint32_t (*flsGetValue)(uint32_t idx);             // DAT_6ff80274
    int32_t  (*flsSetValue)(uint32_t idx, uint32_t v); // DAT_6ff80278
    uint32_t tls_index;                                // DAT_6ff7e20c
    uint32_t (*calloc)(uint32_t n, uint32_t sz);       // +0x7E41
    void     (*amsg_exit)(int32_t code);
    uint32_t initaddr;                                 // &DAT_6ff7e760
    // freefls
    uint32_t lconv_c;                                  // DAT_6ffa6ee8
    uint32_t locinfo_a;                                // PTR_DAT_6ff7e0d4
    uint32_t locinfo_b;                                // &DAT_6ff7e080
    // flsbuf
    uint32_t stdout_addr, stderr_addr;                 // &DAT_6ff7e360 / &DAT_6ff7e380
    uint32_t ioinfo_tab;                               // DAT_6ffa7140 (array of block ptrs)
    uint32_t ioinfo_dummy;                             // &DAT_6ff7e728
    int32_t  (*isatty)(uint32_t fh);
    void     (*getbuf)(uint32_t file);
    int32_t  (*write)(uint32_t fh, const void* buf, uint32_t n);
    int32_t  (*lseek)(uint32_t fh, int32_t off, int32_t whence);
};

static inline uint32_t z29_rd(const Z29Env* e, uint32_t a) { uint32_t v; memcpy(&v, e->mem(a), 4); return v; }
static inline void z29_wr(const Z29Env* e, uint32_t a, uint32_t v) { memcpy(e->mem(a), &v, 4); }

// 1.13c Fog+0x48F5
// __heap_init(mode_arg): HeapCreate(arg==0, 0x1000, 0); 0 on failure. heap_mode = __heap_select();
// mode 3 also needs ___sbh_heap_init(0x3F8), else destroy the heap and fail.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t z29_heap_init(int32_t arg, Z29Env* e) {  // 1.13c Fog+0x48F5
    e->heap = e->HeapCreate(arg == 0, 0x1000, 0);
    if (e->heap == 0) return 0;
    e->heap_mode = e->heap_select();
    if (e->heap_mode == 3 && e->sbh_heap_init(0x3F8) == 0) { e->HeapDestroy(e->heap); return 0; }
    return 1;
}

// 1.13c Fog+0xBDEB
// __fptostr(buf, count, flt): flt+4 = decpt, flt+0xC = digit string. buf[0]='0', copy count digits
// (zero-padded past the NUL) into buf+1, round up on the next digit > '4' propagating 9s, then if
// buf[0] became '1' bump decpt else shift the string down over buf[0].
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t z29_fptostr(uint32_t buf, int32_t count, uint32_t flt, Z29Env* e) {  // 1.13c Fog+0xBDEB
    uint32_t digits = z29_rd(e, flt + 0xC);
    uint8_t* b = e->mem(buf);
    uint8_t* p = b + 1;
    int32_t sz = count;
    b[0] = '0';
    if (0 < count) {
        sz = 0;
        for (int32_t n = count; n != 0; n--) {
            uint8_t c = *e->mem(digits);
            if (c == 0) c = '0'; else digits++;
            *p++ = c;
        }
    }
    *p = 0;
    if (-1 < sz && *e->mem(digits) > '4') {
        for (--p; *p == '9'; --p) *p = '0';
        *p += 1;
    }
    if (b[0] == '1') {
        z29_wr(e, flt + 4, z29_rd(e, flt + 4) + 1);
        return (uint32_t)(p - b) + buf;
    }
    size_t n = strlen((char*)b + 1);
    memmove(b, b + 1, n + 1);
    return buf;
}

// 1.13c Fog+0x24B00
// SEH unwind thunk: _free-class funclet FUN_6ff516b0(*(ebp+4)). Seam: f.
extern "C" EMSCRIPTEN_KEEPALIVE void z29_unwind_24B00(uint32_t ebp, void (*f)(uint32_t), Z29Env* e) {  // 1.13c Fog+0x24B00
    f(z29_rd(e, ebp + 4));
}
// 1.13c Fog+0x24B20
// SEH unwind thunk: FUN_6ff51630(*(ebp+4)).
extern "C" EMSCRIPTEN_KEEPALIVE void z29_unwind_24B20(uint32_t ebp, void (*f)(uint32_t), Z29Env* e) {  // 1.13c Fog+0x24B20
    f(z29_rd(e, ebp + 4));
}
// 1.13c Fog+0x24B2A
// SEH unwind thunk: FUN_6ff51400(*(ebp+4)).
extern "C" EMSCRIPTEN_KEEPALIVE void z29_unwind_24B2A(uint32_t ebp, void (*f)(uint32_t), Z29Env* e) {  // 1.13c Fog+0x24B2A
    f(z29_rd(e, ebp + 4));
}

// shared body of ___tolower_mt / ___toupper_mt: kind 0 = lower (class 1, LCMAP 0x100, 'A'..'Z'),
// 1 = upper (class 2, LCMAP 0x200, 'a'..'z'). loc = ptloci: +4 codepage, +0x14 lcid, +0x24 C-locale
// flag, +0x28 mb_cur_max, +0x48 ctype table.
static uint32_t z29_case_mt(uint32_t loc, uint32_t c, int kind, Z29Env* e) {
    const uint32_t cls = kind ? 2 : 1, lo = kind ? 0x60 : 0x40, hi = kind ? 0x7B : 0x5B;
    if (z29_rd(e, loc + 0x14) == 0 || (z29_rd(e, loc + 0x24) != 0 && c < 0x80)) {
        if ((int32_t)lo < (int32_t)c && (int32_t)c < (int32_t)hi) return kind ? c - 0x20 : c + 0x20;
        return c;
    }
    const uint8_t* tab = e->mem(z29_rd(e, loc + 0x48));
    if (c < 0x100) {
        uint32_t t = ((int32_t)z29_rd(e, loc + 0x28) < 2) ? (tab[c * 2] & cls) : e->isctype_mt(loc, c, cls);
        if (t == 0) return c;
    }
    uint8_t src[4] = {0, 0, 0, 0}, dst[4] = {0, 0, 0, 0};
    uint32_t len;
    if ((tab[1 + ((c >> 8) & 0xFF) * 2] & 0x80) == 0) { src[0] = (uint8_t)c; len = 1; }
    else { src[0] = (uint8_t)(c >> 8); src[1] = (uint8_t)c; len = 2; }
    uint32_t r = e->crtLCMapStringA(z29_rd(e, loc + 0x14), kind ? 0x200 : 0x100, src, len, dst, 3,
                                    z29_rd(e, loc + 4), 1);
    if (r == 0) return c;
    if (r != 1) return ((uint32_t)dst[0] << 8) | dst[1];
    return dst[0];
}

// 1.13c Fog+0x184D
// ___tolower_mt(ptloci, c): see z29_case_mt.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t z29_tolower_mt(uint32_t loc, uint32_t c, Z29Env* e) {  // 1.13c Fog+0x184D
    return z29_case_mt(loc, c, 0, e);
}
// 1.13c Fog+0x1937
// ___toupper_mt(ptloci, c): see z29_case_mt.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t z29_toupper_mt(uint32_t loc, uint32_t c, Z29Env* e) {  // 1.13c Fog+0x1937
    return z29_case_mt(loc, c, 1, e);
}

// 1.13c Fog+0x2B5D
// type_info::~type_info(this): lock 0xE, free *(this+4) if set, unlock (FUN_6ff52b9a). The vftable
// store at *this is a guest-memory write of the type_info vftable (seam value).
extern "C" EMSCRIPTEN_KEEPALIVE void z29_type_info_dtor(uint32_t self, uint32_t vftable, Z29Env* e) {  // 1.13c Fog+0x2B5D
    z29_wr(e, self, vftable);
    e->lock(0xE);
    uint32_t p = z29_rd(e, self + 4);
    if (p != 0) e->free(p);
    e->unlock(0xE);
}

// 1.13c Fog+0x3857
// ___updatetlocinfo(): __lock(0xC); r = ___updatetlocinfo_lk(); unlock(0xC); return r.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t z29_updatetlocinfo(Z29Env* e) {  // 1.13c Fog+0x3857
    e->lock(0xC);
    uint32_t r = e->updatetlocinfo_lk();
    e->unlock(0xC);
    return r;
}

// 1.13c Fog+0x38CB
// __getptd(): preserve LastError; fetch the FLS/TLS slot; if empty, calloc(1,0x8C), store it, init
// _initaddr(+0x14)=initaddr, _holdrand(+0x14 per CRT 7.1: +0x14)=1, _thandle(+4)=-1, _tid(+0)=tid;
// calloc/set failure -> __amsg_exit(0x10). Restore LastError.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t z29_getptd(Z29Env* e) {  // 1.13c Fog+0x38CB
    uint32_t err = e->GetLastError();
    uint32_t p = e->flsGetValue(e->tls_index);
    if (p == 0) {
        p = e->calloc(1, 0x8C);
        if (p == 0 || e->flsSetValue(e->tls_index, p) == 0) {
            e->amsg_exit(0x10);
        } else {
            z29_wr(e, p + 0x54, e->initaddr);  // _initaddr
            z29_wr(e, p + 0x14, 1);            // _holdrand
            z29_wr(e, p + 4, 0xFFFFFFFFu);     // _thandle
            z29_wr(e, p + 0, e->GetCurrentThreadId());  // _tid
        }
    }
    e->SetLastError(err);
    return p;
}

// 1.13c Fog+0x393C
// __freefls@4(ptd): free the owned ptd members (+0x24,+0x2C,+0x34,+0x3C,+0x44,+0x48, +0x54 unless it
// is the static initaddr), drop the lconv ref (+0x60) under lock 0xD, drop the locinfo refs (+0x64)
// under lock 0xC (___freetlocinfo at zero unless a static one), then free ptd.
extern "C" EMSCRIPTEN_KEEPALIVE void z29_freefls(uint32_t ptd, Z29Env* e) {  // 1.13c Fog+0x393C
    if (ptd == 0) return;
    static const uint32_t off[] = {0x24, 0x2C, 0x34, 0x3C, 0x44, 0x48};
    for (uint32_t o : off) { uint32_t p = z29_rd(e, ptd + o); if (p) e->free(p); }
    if (z29_rd(e, ptd + 0x54) != e->initaddr) e->free(z29_rd(e, ptd + 0x54));
    e->lock(0xD);
    uint32_t pl = z29_rd(e, ptd + 0x60);
    if (pl != 0) {
        uint32_t n = z29_rd(e, pl) - 1; z29_wr(e, pl, n);
        if (n == 0 && pl != e->lconv_c) e->free(pl);
    }
    e->unlock(0xD);
    e->lock(0xC);
    uint32_t pi = z29_rd(e, ptd + 0x64);
    if (pi != 0) {
        z29_wr(e, pi, z29_rd(e, pi) - 1);
        static const uint32_t sub[] = {0x2C, 0x34, 0x30, 0x40};  // piVar1[0xb,0xd,0xc,0x10]
        for (uint32_t o : sub) { uint32_t q = z29_rd(e, pi + o); if (q) z29_wr(e, q, z29_rd(e, q) - 1); }
        uint32_t q = z29_rd(e, pi + 0x4C);  // piVar1[0x13]
        z29_wr(e, q + 0xB4, z29_rd(e, q + 0xB4) - 1);
        if (pi != e->locinfo_a && pi != e->locinfo_b && z29_rd(e, pi) == 0) e->freetlocinfo(pi);
    }
    e->unlock(0xC);
    e->free(ptd);
}

// 1.13c Fog+0x3F78
// __flsbuf(ch, FILE*): FILE = {+0 ptr, +4 cnt, +8 base, +0xC flag, +0x10 file, +0x18 bufsiz}.
// Returns ch&0xFF on success, -1 (flag |= 0x20) on error.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t z29_flsbuf(int32_t ch, uint32_t fp, Z29Env* e) {  // 1.13c Fog+0x3F78
    uint32_t flag = z29_rd(e, fp + 0xC), fh = z29_rd(e, fp + 0x10);
    if ((flag & 0x82) == 0 || (flag & 0x40) != 0) { z29_wr(e, fp + 0xC, flag | 0x20); return -1; }
    if (flag & 1) {
        z29_wr(e, fp + 4, 0);
        if ((flag & 0x10) == 0) { z29_wr(e, fp + 0xC, flag | 0x20); return -1; }
        z29_wr(e, fp, z29_rd(e, fp + 8));
        z29_wr(e, fp + 0xC, flag & ~1u);
    }
    flag = z29_rd(e, fp + 0xC);
    z29_wr(e, fp + 4, 0);
    int32_t written = 0, want;
    z29_wr(e, fp + 0xC, (flag & ~0x10u) | 2);
    if ((flag & 0x10C) == 0 &&
        ((fp != e->stdout_addr && fp != e->stderr_addr) || e->isatty(fh) == 0))
        e->getbuf(fp);
    if ((z29_rd(e, fp + 0xC) & 0x108) == 0) {
        want = 1;
        uint8_t c = (uint8_t)ch;
        written = e->write(fh, &c, 1);
    } else {
        uint32_t base = z29_rd(e, fp + 8), ptr = z29_rd(e, fp);
        z29_wr(e, fp, base + 1);
        want = (int32_t)(ptr - base);
        z29_wr(e, fp + 4, z29_rd(e, fp + 0x18) - 1);
        if (want < 1) {
            uint32_t io = (fh == 0xFFFFFFFFu) ? e->ioinfo_dummy
                        : z29_rd(e, e->ioinfo_tab + (fh >> 5) * 4) + (fh & 0x1F) * 0x24;
            if (*e->mem(io + 4) & 0x20) e->lseek(fh, 0, 2);
        } else {
            written = e->write(fh, e->mem(base), (uint32_t)want);
        }
        *e->mem(base) = (uint8_t)ch;
    }
    if (written == want) return ch & 0xFF;
    z29_wr(e, fp + 0xC, z29_rd(e, fp + 0xC) | 0x20);
    return -1;
}
