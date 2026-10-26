// D2Multi batch z26: MSVC CRT 2003 statics (stdio write helpers, locale refcounts, tls/ptd).
// ponytail: decompile hides register args (write_char AL/ECX/ESI etc.); assumed from callers. Guessed: 22A6 22D9 22FD (reg args), 2B85 (this/param roles), 2DC8 (empty body).
#include <cstdint>
extern "C" {
// seams: guest 32-bit link -> host pointer, CRT callees, retail globals
void*    z26_p(uint32_t);
void     z26_free(void*);
int      z26_flsbuf_6f9d218d(int, void*);
int      z26_isatty(int);
void     z26_getbuf(void*);
int      z26_write(int, const void*, unsigned);
long     z26_lseek(int, long, int);
int      z26_crtGetStringTypeA(int, uint32_t, const char*, int, uint16_t*, int, int);
void     z26_free_lconv_mon(void*);
void     z26_free_lconv_num(void*);
void     z26_free_lc_time(void*);
void     z26_lock(int);
void     z26_unlock_c(void);
void     z26_unlock_d(void);
void     z26_mtdeletelocks(void);
void*    z26_calloc(unsigned, unsigned);
void     z26_amsg_exit(int);
uint32_t z26_GetLastError(void);
void     z26_SetLastError(uint32_t);
uint32_t z26_GetCurrentThreadId(void);
uint32_t z26_TlsAlloc(void);
void*    (*z26_TlsGetValue)(uint32_t);
int      (*z26_TlsSetValue)(uint32_t, void*);
uint32_t (*z26_TlsFree)(uint32_t);
extern uint8_t z26_DAT_6f9e9600[0x20], z26_DAT_6f9e9620[0x20], z26_DAT_6f9e9490[0x24];
extern uint32_t z26_DAT_6fa0b3a0[];            // ioinfo table
extern uint32_t z26_DAT_6f9ebd70, z26_DAT_6fa0a364, z26_DAT_6fa0a368, z26_DAT_6fa0a360;
extern uint32_t z26_DAT_6f9ebd6c, z26_DAT_6fa0a12c;
extern uint32_t z26_PTR_DAT_6f9e90ac;          // current global locinfo
extern uint8_t  z26_DAT_6f9e9058[], z26_DAT_6f9e94c0[];
extern uint32_t z26_DAT_6f9e91e4;              // tls index

struct z26_FILE { uint32_t ptr; int32_t cnt; uint32_t base; uint32_t flag; int32_t file; int32_t charbuf; int32_t bufsiz; uint32_t tmpfname; };
void z26_freetlocinfo_6f9d2bfc(void*);
void* z26_getptd_6f9d2e01(void);
}

static inline uint32_t& W(void* b, int o) { return *(uint32_t*)((uint8_t*)b + o); }
static inline int32_t&  I(void* b, int o) { return *(int32_t*)((uint8_t*)b + o); }
static inline void* P(void* b, int o) { return z26_p(W(b, o)); }

// 1.13c D2Multi+0x218D  __flsbuf
extern "C" int z26_flsbuf_6f9d218d(int ch, void* fp) {
    z26_FILE* f = (z26_FILE*)fp;
    uint32_t fl = f->flag; int fh = f->file;
    if (((fl & 0x82) == 0) || (fl & 0x40)) { f->flag = fl | 0x20; return -1; }
    if (fl & 1) {
        f->cnt = 0;
        if (!(fl & 0x10)) { f->flag = fl | 0x20; return -1; }
        f->ptr = f->base; f->flag = fl & ~1u;
    }
    fl = f->flag; f->cnt = 0;
    int written = 0, want;
    f->flag = (fl & ~0x10u) | 2;
    if (((fl & 0x10c) == 0) && ((fp != (void*)z26_DAT_6f9e9600 && fp != (void*)z26_DAT_6f9e9620) || z26_isatty(fh) == 0))
        z26_getbuf(fp);
    char c = (char)ch;
    if ((f->flag & 0x108) == 0) { want = 1; written = z26_write(fh, &c, 1); }
    else {
        uint32_t base = f->base, p = f->ptr;
        f->ptr = base + 1; want = (int)(p - base); f->cnt = f->bufsiz - 1;
        if (want < 1) {
            uint8_t* io = (fh == -1) ? z26_DAT_6f9e9490 : (uint8_t*)z26_p(z26_DAT_6fa0b3a0[fh >> 5]) + (fh & 0x1f) * 0x24;
            if (io[4] & 0x20) z26_lseek(fh, 0, 2);
        } else written = z26_write(fh, z26_p(base), want);
        *(char*)z26_p(f->base) = c;
    }
    if (written == want) return ch & 0xff;
    f->flag |= 0x20; return -1;
}

// 1.13c D2Multi+0x22A6  write_char  (AL=ch, ECX=FILE*, ESI=&charsout)
extern "C" void z26_write_char_6f9d22a6(uint8_t ch, void* fp, int* charsout) {
    z26_FILE* f = (z26_FILE*)fp;
    if (!(f->flag & 0x40) || f->base != 0) {
        int r;
        if (--f->cnt < 0) r = z26_flsbuf_6f9d218d((char)ch, fp);
        else { *(uint8_t*)z26_p(f->ptr) = ch; f->ptr++; r = ch; }
        if (r == -1) { *charsout = -1; return; }
    }
    ++*charsout;
}

// 1.13c D2Multi+0x22D9  write_multi_char  (EAX=&charsout, param_2=count; ch/FILE in regs)
extern "C" void z26_write_multi_char_6f9d22d9(uint8_t ch, int n, void* fp, int* charsout) {
    do { if (n < 1) return; --n; z26_write_char_6f9d22a6(ch, fp, charsout); } while (*charsout != -1);
}

// 1.13c D2Multi+0x22FD  write_string  (EAX=&charsout, EDI=FILE*, ESI=ptr to chars)
extern "C" void z26_write_string_6f9d22fd(int n, const uint8_t* s, void* fp, int* charsout) {
    z26_FILE* f = (z26_FILE*)fp;
    if (!(f->flag & 0x40) || f->base != 0) {
        do { if (n < 1) return; --n; z26_write_char_6f9d22a6(*s++, fp, charsout); } while (*charsout != -1);
    } else *charsout += n;
}

// 1.13c D2Multi+0x2B85  ___isctype_mt  (this=char, param_1=locinfo, param_2=c, param_3=mask)
extern "C" uint32_t z26_isctype_mt_6f9d2b85(uint32_t thisv, void* li, int c, uint32_t mask) {
    uint16_t t;
    uint8_t* ctype = (uint8_t*)z26_p(W(li, 0x48));
    if ((uint32_t)(c + 1) < 0x101) t = *(uint16_t*)(ctype + c * 2);
    else {
        char buf[4] = {0}; int len;
        if ((ctype[1 + ((c >> 8) & 0xff) * 2] & 0x80) == 0) { buf[0] = (char)c; len = 1; }
        else { buf[0] = (char)(c >> 8); buf[1] = (char)c; len = 2; }
        // buffer passed by guest address in retail; seam takes host buffer via cast
        if (!z26_crtGetStringTypeA(1, (uint32_t)(uintptr_t)buf, (const char*)(uintptr_t)len, 0, &t, (int)W(li, 4) /*codepage*/, (int)W(li, 0x14))) return 0;
        (void)thisv;
    }
    return t & mask;
}

static void dec(uint32_t link) { if (link) --*(int32_t*)z26_p(link); }
static void dec_locinfo_refs(void* li) {
    dec(W(li, 0x2c)); dec(W(li, 0x34)); dec(W(li, 0x30)); dec(W(li, 0x40));
    --I(z26_p(W(li, 0x4c)), 0xb4);
}

// 1.13c D2Multi+0x2BFC  ___freetlocinfo
extern "C" void z26_freetlocinfo_6f9d2bfc(void* li) {
    if (W(li, 0x3c) != z26_DAT_6f9ebd70 && W(li, 0x3c) != 0 && *(int32_t*)P(li, 0x2c) == 0) {
        int32_t* p = (int32_t*)P(li, 0x34);
        if (p && *p == 0 && W(li, 0x34) != z26_DAT_6fa0a364) { z26_free(p); z26_free_lconv_mon(P(li, 0x3c)); }
        p = (int32_t*)P(li, 0x30);
        if (p && *p == 0 && W(li, 0x30) != z26_DAT_6fa0a368) { z26_free(p); z26_free_lconv_num(P(li, 0x3c)); }
        z26_free(P(li, 0x2c)); z26_free(P(li, 0x3c));
    }
    int32_t* p = (int32_t*)P(li, 0x40);
    if (W(li, 0x40) != z26_DAT_6fa0a360 && p && *p == 0) { z26_free(p); z26_free((uint8_t*)z26_p(W(li, 0x44)) - 0xfe); }
    uint32_t t = W(li, 0x50);
    if (t != z26_DAT_6f9ebd6c && t != 0 && W(z26_p(t), 0xb4) == 0) { z26_free_lc_time(z26_p(t)); z26_free(P(li, 0x50)); }
    z26_free(li);
}

// 1.13c D2Multi+0x2CCC  ___updatetlocinfo_lk
extern "C" uint32_t z26_updatetlocinfo_lk_6f9d2ccc(void) {
    void* ptd = z26_getptd_6f9d2e01();
    uint32_t old = W(ptd, 0x6c);                       // _tfpecode slot holds ptlocinfo
    if (old != z26_PTR_DAT_6f9e90ac) {
        void* o = old ? z26_p(old) : nullptr;
        if (o) { --I(o, 0); dec_locinfo_refs(o); }
        W(ptd, 0x6c) = z26_PTR_DAT_6f9e90ac;
        void* n = z26_p(z26_PTR_DAT_6f9e90ac);
        ++I(n, 0);
        if (W(n, 0x2c)) ++*(int32_t*)P(n, 0x2c);
        if (W(n, 0x34)) ++*(int32_t*)P(n, 0x34);
        if (W(n, 0x30)) ++*(int32_t*)P(n, 0x30);
        if (W(n, 0x40)) ++*(int32_t*)P(n, 0x40);
        ++I(P(n, 0x4c), 0xb4);
        if (o && *(int32_t*)o == 0 && o != (void*)z26_DAT_6f9e9058) z26_freetlocinfo_6f9d2bfc(o);
    }
    return W(ptd, 0x6c);
}

// 1.13c D2Multi+0x2D8D  ___updatetlocinfo
extern "C" uint32_t z26_updatetlocinfo_6f9d2d8d(void) {
    z26_lock(0xc);
    uint32_t r = z26_updatetlocinfo_lk_6f9d2ccc();
    z26_unlock_c();                                    // FUN_6f9d2dbf
    return r;
}

// 1.13c D2Multi+0x2DC8  FUN_6f9d2dc8 (decompile: TlsAlloc call only; result unused here)
extern "C" void z26_FUN_6f9d2dc8(void) { z26_TlsAlloc(); }

// 1.13c D2Multi+0x2DD1  __mtterm
extern "C" void z26_mtterm_6f9d2dd1(void) {
    if (z26_DAT_6f9e91e4 != 0xffffffffu) { z26_TlsFree(z26_DAT_6f9e91e4); z26_DAT_6f9e91e4 = 0xffffffffu; }
    z26_mtdeletelocks();
}

// 1.13c D2Multi+0x2E01  __getptd
extern "C" void* z26_getptd_6f9d2e01(void) {
    uint32_t err = z26_GetLastError();
    void* p = z26_TlsGetValue(z26_DAT_6f9e91e4);
    if (!p) {
        p = z26_calloc(1, 0x8c);
        if (p && z26_TlsSetValue(z26_DAT_6f9e91e4, p)) {
            W(p, 0x14) = (uint32_t)(uintptr_t)z26_DAT_6f9e94c0;   // _initaddr
            W(p, 0x18) = 1;                                       // _holdrand
            W(p, 0x04) = z26_GetCurrentThreadId();                // _tid
            W(p, 0x08) = 0xffffffffu;                             // _thandle
        } else z26_amsg_exit(0x10);
    }
    z26_SetLastError(err);
    return p;
}

// 1.13c D2Multi+0x2E72  __freefls@4
extern "C" void z26_freefls_6f9d2e72(void* d) {
    if (!d) return;
    static const int fr[] = {0x24, 0x2c, 0x34, 0x3c, 0x44, 0x48};
    for (int o : fr) if (W(d, o)) z26_free(P(d, o));
    if (W(d, 0x54) != (uint32_t)(uintptr_t)z26_DAT_6f9e94c0) z26_free(P(d, 0x54));
    z26_lock(0xd);
    int32_t* p = (int32_t*)P(d, 0x60);
    if (p && --*p == 0 && W(d, 0x60) != z26_DAT_6fa0a12c) z26_free(p);
    z26_unlock_d();                                    // FUN_6f9d2fa4
    z26_lock(0xc);
    void* li = P(d, 100);
    if (li) {
        --I(li, 0); dec_locinfo_refs(li);
        if (W(d, 100) != z26_PTR_DAT_6f9e90ac && li != (void*)z26_DAT_6f9e9058 && I(li, 0) == 0) z26_freetlocinfo_6f9d2bfc(li);
    }
    z26_unlock_c();                                    // FUN_6f9d2fb0
    z26_free(d);
}
