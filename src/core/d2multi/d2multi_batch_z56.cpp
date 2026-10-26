// D2Multi batch z56: 218D 22A6 22D9 22FD 2B85 2BFC 2CCC 2D8D 2DC8 2DD1 2E01 2E72 (MSVC CRT: printf core, locale, tls).
// ponytail: retail CRT; register args (AL/ECX/ESI/EDI) taken as explicit seam params. Guessed: 22A6/22D9/22FD param roles,
// 2B85 (this = ctype hi-word scratch, dropped), 2E01 (calloc/TLS seams). 32-bit links are uint32_t.
#include <cstdint>
#include <cstring>

struct CrtFile { uint32_t ptr, cnt, base, flag, file, charbuf, bufsiz, tmpfname; };  // MSVC7 FILE, 32-bit

extern "C" {
int32_t d2_multi_flsbuf(int32_t ch, CrtFile* f);                 // 0x218D seam (self, below)
int32_t d2_multi_isatty(uint32_t fh);
void    d2_multi_getbuf(CrtFile* f);
int32_t d2_multi_write(uint32_t fh, const void* buf, uint32_t n);
int32_t d2_multi_lseek(uint32_t fh, int32_t off, int32_t whence);
int32_t d2_multi_crtGetStringTypeA(int32_t, uint32_t src, int32_t cch, uint16_t* out, uint32_t cp, int32_t lcid, int32_t);
void    d2_multi_free(uint32_t p);
void    d2_multi_free_lconv_mon(uint32_t p);
void    d2_multi_free_lconv_num(uint32_t p);
void    d2_multi_free_lc_time(uint32_t p);
void    d2_multi_lock(int32_t n);
void    d2_multi_unlock(int32_t n);
uint32_t d2_multi_getptd(void);
uint32_t d2_multi_calloc(uint32_t n, uint32_t sz);
void    d2_multi_amsg_exit(int32_t);
void    d2_multi_mtdeletelocks(void);
void    d2_multi_TlsAlloc(void);
uint32_t d2_multi_TlsGetValue(uint32_t);
int32_t d2_multi_TlsSetValue(uint32_t, uint32_t);
uint32_t d2_multi_GetLastError(void);
void    d2_multi_SetLastError(uint32_t);
uint32_t d2_multi_GetCurrentThreadId(void);
uint32_t* d2_multi_mem(uint32_t addr);   // 32-bit link -> host pointer
extern uint32_t DAT_6f9e9600, DAT_6f9e9620, DAT_6f9e9490, DAT_6fa0b3a0, DAT_6f9ebd70, DAT_6fa0a364, DAT_6fa0a368,
    DAT_6fa0a360, DAT_6f9ebd6c, PTR_DAT_6f9e90ac, DAT_6f9e9058, DAT_6fa0a12c, DAT_6f9e91e4, DAT_6f9e94c0;
}
static inline uint32_t& W(uint32_t a, uint32_t off) { return *(d2_multi_mem(a) + off / 4); }

// 1.13c D2Multi+0x218D  __flsbuf
int32_t d2_multi_flsbuf(int32_t ch, CrtFile* f) {
    uint32_t fl = f->flag, fh = f->file;
    int32_t wr = 0, want = 0;
    if ((fl & 0x82) == 0 || (fl & 0x40)) { f->flag = fl | 0x20; return -1; }
    if (fl & 1) {
        f->cnt = 0;
        if (!(fl & 0x10)) { f->flag = fl | 0x20; return -1; }
        f->ptr = f->base;
        f->flag = fl & ~1u;
    }
    fl = f->flag; f->cnt = 0;
    f->flag = (fl & ~0x10u) | 2;
    if ((fl & 0x10c) == 0 && ((uintptr_t)f != DAT_6f9e9600 && (uintptr_t)f != DAT_6f9e9620 || !d2_multi_isatty(fh)))
        d2_multi_getbuf(f);
    if ((f->flag & 0x108) == 0) {
        want = 1; char c = (char)ch; wr = d2_multi_write(fh, &c, 1);
    } else {
        uint32_t b = f->base, p = f->ptr;
        f->ptr = b + 1; want = (int32_t)(p - b); f->cnt = f->bufsiz - 1;
        if (want < 1) {
            if (fh == 0xffffffffu ? (reinterpret_cast<uint8_t*>(&DAT_6f9e9490)[4] & 0x20)
                                  : (W(W(DAT_6fa0b3a0, (fh >> 5) * 4) + (fh & 0x1f) * 0x24, 4) & 0x20))
                d2_multi_lseek(fh, 0, 2);
        } else wr = d2_multi_write(fh, d2_multi_mem(b), (uint32_t)want);
        *reinterpret_cast<char*>(d2_multi_mem(f->base)) = (char)ch;
    }
    if (wr == want) return ch & 0xff;
    f->flag |= 0x20;
    return -1;
}

// 1.13c D2Multi+0x22A6  write_char (AL=ch, ECX=FILE, ESI=&count)
void d2_multi_write_char(uint8_t ch, CrtFile* f, int32_t* count) {
    if (!(f->flag & 0x40) || f->base) {
        int32_t r;
        if ((int32_t)--f->cnt < 0) r = d2_multi_flsbuf((int8_t)ch, f);
        else { *reinterpret_cast<uint8_t*>(d2_multi_mem(f->ptr)) = ch; f->ptr++; r = ch; }
        if (r == -1) { *count = -1; return; }
    }
    ++*count;
}

// 1.13c D2Multi+0x22D9  write_multi_char (EAX=&count; ch/FILE carried in regs)
void d2_multi_write_multi_char(uint8_t ch, int32_t n, CrtFile* f, int32_t* count) {
    while (n-- > 0) { d2_multi_write_char(ch, f, count); if (*count == -1) return; }
}

// 1.13c D2Multi+0x22FD  write_string (EAX=&count, EDI=FILE; chars from ESI cursor)
void d2_multi_write_string(int32_t n, CrtFile* f, int32_t* count, const uint8_t* s) {
    if (!(f->flag & 0x40) || f->base) {
        while (n-- > 0) { d2_multi_write_char(*s++, f, count); if (*count == -1) return; }
    } else *count += n;
}

// 1.13c D2Multi+0x2B85  ___isctype_mt (param_1 = threadlocinfo link)
uint32_t d2_multi_isctype_mt(uint32_t pl, int32_t c, uint32_t mask) {
    uint16_t t;
    if ((uint32_t)(c + 1) < 0x101) {
        t = *reinterpret_cast<uint16_t*>(d2_multi_mem(W(pl, 0x48) + (uint32_t)c * 2));
    } else {
        uint8_t buf[2]; int32_t len;
        uint8_t lead = *reinterpret_cast<uint8_t*>(d2_multi_mem(W(pl, 0x48) + 1 + ((c >> 8) & 0xff) * 2));
        if (lead & 0x80) { buf[0] = (uint8_t)(c >> 8); buf[1] = (uint8_t)c; len = 2; } else { buf[0] = (uint8_t)c; buf[1] = 0; len = 1; }
        if (!d2_multi_crtGetStringTypeA(1, (uint32_t)(uintptr_t)buf, len, &t, W(pl, 4), (int32_t)W(pl, 0x14), 1)) return 0;
    }
    return t & mask;
}

// 1.13c D2Multi+0x2BFC  ___freetlocinfo
void d2_multi_freetlocinfo(uint32_t p) {
    uint32_t v;
    if (W(p, 0x3c) != DAT_6f9ebd70 && W(p, 0x3c) != 0 && W(W(p, 0x2c), 0) == 0) {
        v = W(p, 0x34);
        if (v && W(v, 0) == 0 && v != DAT_6fa0a364) { d2_multi_free(v); d2_multi_free_lconv_mon(W(p, 0x3c)); }
        v = W(p, 0x30);
        if (v && W(v, 0) == 0 && v != DAT_6fa0a368) { d2_multi_free(v); d2_multi_free_lconv_num(W(p, 0x3c)); }
        d2_multi_free(W(p, 0x2c));
        d2_multi_free(W(p, 0x3c));
    }
    v = W(p, 0x40);
    if (v != DAT_6fa0a360 && v && W(v, 0) == 0) { d2_multi_free(v); d2_multi_free(W(p, 0x44) - 0xfe); }
    v = W(p, 0x50);
    if (v != DAT_6f9ebd6c && v && W(v, 0x2d * 4) == 0) { d2_multi_free_lc_time(v); d2_multi_free(W(p, 0x50)); }
    d2_multi_free(p);
}

static void dec(uint32_t p) { if (p) --W(p, 0); }
static void inc(uint32_t p) { if (p) ++W(p, 0); }

// 1.13c D2Multi+0x2CCC  ___updatetlocinfo_lk
uint32_t d2_multi_updatetlocinfo_lk(void) {
    uint32_t ptd = d2_multi_getptd();
    uint32_t old = W(ptd, 0x6c);        // _ptd->ptlocinfo (decompiler: _tfpecode)
    uint32_t cur = PTR_DAT_6f9e90ac;
    if (old != cur) {
        if (old) {
            --W(old, 0); dec(W(old, 0x2c)); dec(W(old, 0x34)); dec(W(old, 0x30)); dec(W(old, 0x40));
            --W(W(old, 0x4c), 0xb4);
        }
        W(ptd, 0x6c) = cur;
        ++W(cur, 0); inc(W(cur, 0x2c)); inc(W(cur, 0x34)); inc(W(cur, 0x30)); inc(W(cur, 0x40));
        ++W(W(cur, 0x4c), 0xb4);
        if (old && W(old, 0) == 0 && old != DAT_6f9e9058) d2_multi_freetlocinfo(old);
    }
    return W(ptd, 0x6c);
}

// 1.13c D2Multi+0x2D8D  ___updatetlocinfo
uint32_t d2_multi_updatetlocinfo(void) {
    d2_multi_lock(0xc);
    uint32_t r = d2_multi_updatetlocinfo_lk();
    d2_multi_unlock(0xc);
    return r;
}

// 1.13c D2Multi+0x2DC8
void d2_multi_tlsalloc_thunk(void) { d2_multi_TlsAlloc(); }

// 1.13c D2Multi+0x2DD1  __mtterm (tls free fn pointer is seam)
void d2_multi_mtterm(void (*tlsFree)(uint32_t)) {
    if (DAT_6f9e91e4 != 0xffffffffu) { tlsFree(DAT_6f9e91e4); DAT_6f9e91e4 = 0xffffffffu; }
    d2_multi_mtdeletelocks();
}

// 1.13c D2Multi+0x2E01  __getptd
uint32_t d2_multi_getptd(void) {
    uint32_t err = d2_multi_GetLastError();
    uint32_t p = d2_multi_TlsGetValue(DAT_6f9e91e4);
    if (!p) {
        p = d2_multi_calloc(1, 0x8c);
        if (p && d2_multi_TlsSetValue(DAT_6f9e91e4, p)) {
            W(p, 0x54) = DAT_6f9e94c0;   // _initaddr
            W(p, 0x14) = 1;              // _holdrand
            W(p, 0x04) = 0xffffffffu;    // _thandle
            W(p, 0x00) = d2_multi_GetCurrentThreadId();  // _tid
        } else d2_multi_amsg_exit(0x10);
    }
    d2_multi_SetLastError(err);
    return p;
}

// 1.13c D2Multi+0x2E72  __freefls@4 (unlock/lock seams for FUN_2FA4/2FB0)
void d2_multi_freefls(uint32_t p, void (*unlock0xd)(void), void (*unlock0xc)(void)) {
    if (!p) return;
    static const uint8_t offs[] = {0x24, 0x2c, 0x34, 0x3c, 0x44, 0x48};
    for (uint8_t o : offs) if (W(p, o)) d2_multi_free(W(p, o));
    if (W(p, 0x54) != DAT_6f9e94c0) d2_multi_free(W(p, 0x54));
    d2_multi_lock(0xd);
    uint32_t v = W(p, 0x60);
    if (v && --W(v, 0) == 0 && v != DAT_6fa0a12c) d2_multi_free(v);
    unlock0xd();
    d2_multi_lock(0xc);
    v = W(p, 100);
    if (v) {
        --W(v, 0); dec(W(v, 0x2c)); dec(W(v, 0x34)); dec(W(v, 0x30)); dec(W(v, 0x40));
        --W(W(v, 0x4c), 0xb4);
        if (v != PTR_DAT_6f9e90ac && v != DAT_6f9e9058 && W(v, 0) == 0) d2_multi_freetlocinfo(v);
    }
    unlock0xc();
    d2_multi_free(p);
}
