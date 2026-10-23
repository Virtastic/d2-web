// D2Multi MSVC CRT batch b144 (__CRT_INIT@12, DllMain, _output, __mtinit, __mtinitlocknum, __onexit_lk,
// SEH prolog/epilog, _free, _calloc_crt, _crtLCMapStringA). Retail callees and data are weak seams
// (d2multi_*), not linked here. Retail 32-bit addresses stay uint32_t; real pointers are native.
#include <cstddef>
#include <cstdint>
#include <cstring>
#define W __attribute__((weak))

// ---- seams: retail CRT / Win32 callees and globals
extern int d2multi_heap_init() W; extern void d2multi_heap_term() W;
extern int d2multi_mtinit() W; extern void d2multi_mtterm() W; extern int d2multi_mtinitlocks() W;
extern void d2multi_RTC_Initialize() W; extern char* d2multi_GetCommandLineA() W;
extern void* d2multi_crtGetEnvironmentStringsA() W;
extern int d2multi_ioinit() W; extern void d2multi_ioterm() W; extern int d2multi_setargv() W;
extern int d2multi_setenvp() W; extern int d2multi_cinit(int) W; extern void d2multi_cexit() W;
extern void d2multi_freeptd(void*) W; extern void d2multi_initptd(void*, void*) W;
extern uint32_t d2multi_GetCurrentThreadId() W;
extern int d2multi_GetVersionExA(void* osvi) W;
extern void* d2multi_malloc(size_t) W; extern void d2multi_free(void*) W;
extern void* d2multi_memset(void*, int, size_t) W; extern size_t d2multi_msize(void*) W;
extern void* d2multi_realloc(void*, size_t) W;
extern void d2multi_lock(int) W; extern void d2multi_unlock(int) W; extern int* d2multi_errno_ptr() W;
extern void* d2multi_sbh_alloc_block(size_t) W; extern void* d2multi_sbh_find_block(void*) W;
extern void d2multi_sbh_free_block(void*, void*) W;
extern void* d2multi_HeapAlloc(void*, uint32_t, size_t) W; extern int d2multi_HeapFree(void*, uint32_t, void*) W;
extern int d2multi_callnewh(size_t) W;
extern int d2multi_crtInitCritSecAndSpinCount(void*, uint32_t) W;
extern int d2multi_dllmain_body(void* hinst) W;   // retail FUN_6f9d7ee0

extern int DAT_6f9eba84 W, DAT_6f9eba90 W, DAT_6f9eba94 W, DAT_6f9eba88 W, DAT_6f9eba8c W;
extern int DAT_6f9ebac8 W, DAT_6f9ebac0 W, DAT_6f9ebe38 W, DAT_6f9ebc48 W, DAT_6f9ebda0 W, DAT_6f9ebdb0 W;
extern char* DAT_6fa0b4a0 W; extern void* DAT_6f9ebacc W;
extern int (*DAT_6fa0b4a4)(void*, int, void*) W;
extern uint32_t DAT_6f9e91e4 W;
extern uintptr_t (*DAT_6f9ebae8)(void*) W; extern void* (*DAT_6f9ebaec)(uint32_t) W;
extern int (*DAT_6f9ebaf0)(uint32_t, void*) W; extern int (*DAT_6f9ebaf4)(uint32_t) W;
extern void* (*d2multi_GetProcAddress)(const char*) W;   // kernel32 lookup; null if module absent
extern void* d2multi_flsfree_cb W; extern void* d2multi_DAT_6f9e94c0 W;
struct d2multi_lockslot { void* cs; int kind; };
extern d2multi_lockslot DAT_6f9e91f8[] W;  // lock table
extern void** DAT_6fa0b4ac W; extern void** DAT_6fa0b4b0 W;
extern int DAT_6fa0b388 W; extern void* DAT_6fa0b384 W; extern size_t DAT_6fa0a118 W;

// 1.13c D2Multi+0x3454
void d2multi_SEH_prolog() {}   // compiler-generated frame/ExceptionList link; no C++ equivalent
// 1.13c D2Multi+0x348F
void d2multi_SEH_epilog() {}

// 1.13c D2Multi+0x3B90
void d2multi_unlock4() { d2multi_unlock(4); }

// 1.13c D2Multi+0x3A59
void d2multi_free_b144(void* mem) {
    if (!mem) return;
    if (DAT_6fa0b388 == 3) {
        d2multi_lock(4);
        void* b = d2multi_sbh_find_block(mem);
        if (b) d2multi_sbh_free_block(b, mem);
        d2multi_unlock(4);
        if (b) return;
    }
    d2multi_HeapFree(DAT_6fa0b384, 0, mem);
}

// 1.13c D2Multi+0x3ACA
void* d2multi_calloc_crt(uint32_t n, uint32_t sz) {
    if (n != 0 && sz > 0xffffffe0u / n) return nullptr;
    size_t size = (size_t)n * sz, bytes = size ? size : 1;
    for (;;) {
        void* p = nullptr;
        if (bytes < 0xffffffe1u) {
            if (DAT_6fa0b388 == 3) {
                bytes = (bytes + 0xf) & 0xfffffff0u;
                if (size <= DAT_6fa0a118) {
                    d2multi_lock(4);
                    p = d2multi_sbh_alloc_block(size);
                    d2multi_unlock4();
                    if (p) { d2multi_memset(p, 0, size); return p; }
                }
            }
            p = d2multi_HeapAlloc(DAT_6fa0b384, 8, bytes);
        }
        if (p) return p;
        if (DAT_6f9ebe38 == 0) return nullptr;
        if (d2multi_callnewh(bytes) == 0) return nullptr;
    }
}

// 1.13c D2Multi+0x3206
int d2multi_mtinitlocknum(int n) {
    auto* slot = &DAT_6f9e91f8[n];
    if (slot->cs) return 1;
    void* m = d2multi_malloc(0x18);
    if (!m) { *d2multi_errno_ptr() = 12; return 0; }
    d2multi_lock(10);
    if (!slot->cs) {
        if (!d2multi_crtInitCritSecAndSpinCount(m, 4000)) {
            d2multi_free_b144(m);
            *d2multi_errno_ptr() = 12;
            d2multi_unlock(10);   // __local_unwind2 runs the finally block
            return 0;
        }
        slot->cs = m;
    } else d2multi_free_b144(m);
    d2multi_unlock(10);
    return 1;
}

// 1.13c D2Multi+0x32D7  (EDI = function pointer to append, passed explicitly)
void d2multi_onexit_lk(void* fn) {
    size_t sz = d2multi_msize(DAT_6fa0b4b0);
    if (sz < (size_t)((char*)DAT_6fa0b4ac + 4 - (char*)DAT_6fa0b4b0)) {
        size_t grow = sz < 0x800 ? sz : 0x800;
        void** p = (void**)d2multi_realloc(DAT_6fa0b4b0, grow + sz);
        if (!p) { p = (void**)d2multi_realloc(DAT_6fa0b4b0, sz + 0x10); if (!p) return; }
        DAT_6fa0b4ac = p + (DAT_6fa0b4ac - DAT_6fa0b4b0);
        DAT_6fa0b4b0 = p;
    }
    *DAT_6fa0b4ac++ = fn;
}

// 1.13c D2Multi+0x2FE8
int d2multi_mtinit_b144() {
    if (!d2multi_mtinitlocks()) { d2multi_mtterm(); return 0; }
    // kernel32 Fls* lookup; fall back to Tls* when FlsGetValue is absent
    if (d2multi_GetProcAddress) {
        DAT_6f9ebae8 = (uintptr_t(*)(void*))d2multi_GetProcAddress("FlsAlloc");
        DAT_6f9ebaec = (void*(*)(uint32_t))d2multi_GetProcAddress("FlsGetValue");
        DAT_6f9ebaf0 = (int(*)(uint32_t, void*))d2multi_GetProcAddress("FlsSetValue");
        DAT_6f9ebaf4 = (int(*)(uint32_t))d2multi_GetProcAddress("FlsFree");
        if (!DAT_6f9ebaec) {
            DAT_6f9ebaec = (void*(*)(uint32_t))d2multi_GetProcAddress("TlsGetValue");
            DAT_6f9ebaf0 = (int(*)(uint32_t, void*))d2multi_GetProcAddress("TlsSetValue");
            DAT_6f9ebae8 = (uintptr_t(*)(void*))d2multi_GetProcAddress("TlsAlloc");   // FUN_6f9d2dc8 shim
            DAT_6f9ebaf4 = (int(*)(uint32_t))d2multi_GetProcAddress("TlsFree");
        }
    }
    DAT_6f9e91e4 = (uint32_t)DAT_6f9ebae8(d2multi_flsfree_cb);
    if (DAT_6f9e91e4 != 0xffffffffu) {
        uint32_t* ptd = (uint32_t*)d2multi_calloc_crt(1, 0x8c);   // retail _ptiddata, 32-bit slots
        if (ptd && DAT_6f9ebaf0(DAT_6f9e91e4, ptd)) {
            ptd[0x15] = (uint32_t)(uintptr_t)d2multi_DAT_6f9e94c0;
            ptd[5] = 1;
            ptd[1] = 0xffffffffu;
            ptd[0] = d2multi_GetCurrentThreadId();
            return 1;
        }
    }
    d2multi_mtterm();
    return 0;
}

// 1.13c D2Multi+0x1EF5
int d2multi_CRT_INIT_12(void* hinst, int reason) {
    if (reason == 1) {
        uint32_t osvi[37] = {0x94};   // OSVERSIONINFOA: size, major, minor, build, platform, szCSD...
        if (!d2multi_GetVersionExA(osvi)) return 0;
        DAT_6f9eba84 = osvi[4]; DAT_6f9eba90 = osvi[1]; DAT_6f9eba94 = osvi[2];
        DAT_6f9eba88 = osvi[3] & 0x7fff;
        if (osvi[4] != 2) DAT_6f9eba88 |= 0x8000;
        DAT_6f9eba8c = osvi[1] * 0x100 + osvi[2];
        if (d2multi_heap_init()) {
            if (d2multi_mtinit_b144()) {
                d2multi_RTC_Initialize();
                DAT_6fa0b4a0 = d2multi_GetCommandLineA();
                DAT_6f9ebacc = d2multi_crtGetEnvironmentStringsA();
                if (d2multi_ioinit() >= 0) {
                    if (d2multi_setargv() >= 0 && d2multi_setenvp() >= 0 && d2multi_cinit(0) == 0) {
                        DAT_6f9ebac8++;
                        return 1;
                    }
                    d2multi_ioterm();
                }
                d2multi_mtterm();
            }
            d2multi_heap_term();
        }
    } else if (reason == 0) {
        if (DAT_6f9ebac8 > 0) {
            DAT_6f9ebac8--;
            if (DAT_6f9ebac0 == 0) d2multi_cexit();
            d2multi_ioterm(); d2multi_mtterm(); d2multi_heap_term();
            return 1;
        }
    } else {
        if (reason != 2) { if (reason == 3) d2multi_freeptd(nullptr); return 1; }
        void* ptd = d2multi_calloc_crt(1, 0x8c);
        if (ptd) {
            uint32_t loc = DAT_6f9e91e4;
            if (DAT_6f9ebaf0(loc, ptd)) {   // retail passes no args here (decompiler); seam keeps (key, ptd)
                d2multi_initptd(ptd, (void*)(uintptr_t)loc);
                ((uint32_t*)ptd)[0] = d2multi_GetCurrentThreadId();
                ((uint32_t*)ptd)[1] = 0xffffffffu;
                return 1;
            }
            d2multi_free_b144(ptd);
        }
    }
    return 0;
}

// 1.13c D2Multi+0x2076  (DllMain / entry)
int d2multi_entry(void* hinst, int reason, void* reserved) {
    int r = 1;
    if (reason == 0 && DAT_6f9ebac8 == 0) return 0;
    if (reason == 1 || reason == 2) {
        if (DAT_6fa0b4a4) r = DAT_6fa0b4a4(hinst, reason, reserved);
        if (r == 0 || d2multi_CRT_INIT_12(hinst, reason) == 0) return 0;
    }
    r = d2multi_dllmain_body(hinst);
    if (reason == 1 && r == 0) d2multi_CRT_INIT_12(hinst, 0);
    if (reason == 0 || reason == 3) {
        if (d2multi_CRT_INIT_12(hinst, reason) == 0) r = 0;
        if (r != 0 && DAT_6fa0b4a4) r = DAT_6fa0b4a4(hinst, reason, reserved);
    }
    return r;
}

// ---- _LCMapStringA (0x35FE)
extern int d2multi_LCMapStringW(uint32_t, uint32_t, const uint16_t*, int, uint16_t*, int) W;
extern int d2multi_LCMapStringA(uint32_t, uint32_t, const char*, int, char*, int) W;
extern int d2multi_MultiByteToWideChar(uint32_t, uint32_t, const char*, int, uint16_t*, int) W;
extern int d2multi_WideCharToMultiByte(uint32_t, uint32_t, const uint16_t*, int, char*, int, const char*, int*) W;
extern uint32_t d2multi_GetLastError() W;
extern uint32_t d2multi_getlocaleinfo_cp(uint32_t lcid) W;                       // retail 0x6530
extern char* d2multi_convertcp(uint32_t from, uint32_t to, const char* s, int* n, char* dst, int dn) W;  // 0x6577

// 1.13c D2Multi+0x35FE
uint32_t d2multi_crtLCMapStringA(uint32_t lcid, uint32_t flags, const char* src, int cch,
                                 char* dst, int cchDst, uint32_t cp, int bError) {
    if (DAT_6f9ebc48 == 0) {
        uint16_t z[1] = {0};
        if (d2multi_LCMapStringW(0, 0x100, z, 1, nullptr, 0) == 0) {
            if (d2multi_GetLastError() == 0x78) DAT_6f9ebc48 = 2;
        } else DAT_6f9ebc48 = 1;
    }
    if (cch > 0) { int n = 0; while (n < cch && src[n]) n++; cch = n; }   // bounded strlen
    if (DAT_6f9ebc48 == 2 || DAT_6f9ebc48 == 0) {
        char* tmp = nullptr; bool tmpOwn = false;
        char* conv = nullptr; char* cvOut = nullptr; uint32_t ret = 0;
        if (!lcid) lcid = (uint32_t)DAT_6f9ebda0;
        if (!cp) cp = (uint32_t)DAT_6f9ebdb0;
        uint32_t lcp = d2multi_getlocaleinfo_cp(lcid);
        if (lcp == 0xffffffffu) return 0;
        if (lcp == cp) {
            ret = (uint32_t)d2multi_LCMapStringA(lcid, flags, src, cch, dst, cchDst);
        } else {
            conv = d2multi_convertcp(cp, lcp, src, &cch, nullptr, 0);
            if (!conv) return 0;
            int n = d2multi_LCMapStringA(lcid, flags, conv, cch, nullptr, 0);
            if (n != 0) {
                tmp = (char*)d2multi_malloc(n);
                if (tmp) {
                    d2multi_memset(tmp, 0, n); tmpOwn = true;
                    n = d2multi_LCMapStringA(lcid, flags, conv, cch, tmp, n);
                    if (n != 0) {
                        cvOut = d2multi_convertcp(lcp, cp, tmp, &n, dst, cchDst);
                        ret = cvOut ? (uint32_t)n : 0;
                    }
                }
            }
            if (tmpOwn) d2multi_free_b144(tmp);
        }
        if (conv) d2multi_free_b144(conv);
        if (cvOut && cvOut != dst) d2multi_free_b144(cvOut);
        return ret;
    }
    // wide path (DAT == 1)
    uint32_t ret = 0;
    if (!cp) cp = (uint32_t)DAT_6f9ebdb0;
    int nw = d2multi_MultiByteToWideChar(cp, bError ? 9 : 1, src, cch, nullptr, 0);
    if (!nw) return 0;
    uint16_t* wbuf = (uint16_t*)d2multi_malloc((size_t)nw * 2);
    if (!wbuf) return 0;
    uint16_t* wout = nullptr;
    if (d2multi_MultiByteToWideChar(cp, 1, src, cch, wbuf, nw)) {
        ret = (uint32_t)d2multi_LCMapStringW(lcid, flags, wbuf, nw, nullptr, 0);
        if (ret) {
            if (flags & 0x400) {
                if (cchDst != 0 && (int)ret <= cchDst)
                    d2multi_LCMapStringW(lcid, flags, wbuf, nw, (uint16_t*)dst, cchDst);
            } else {
                wout = (uint16_t*)d2multi_malloc((size_t)ret * 2);
                if (wout && d2multi_LCMapStringW(lcid, flags, wbuf, nw, wout, (int)ret))
                    ret = (uint32_t)d2multi_WideCharToMultiByte(cp, 0, wout, (int)ret,
                                                               cchDst ? dst : nullptr, cchDst, nullptr, nullptr);
            }
        }
    }
    if (wout) d2multi_free_b144(wout);
    d2multi_free_b144(wbuf);
    return ret;
}

// ---- _output (0x2334): printf core. Output helpers are the retail write_char/write_multi_char/write_string.
struct d2multi_out_env {
    void* ctx; int* count;                              // stream and running count (local_228)
    void (*write_char)(d2multi_out_env*, int ch);
    void (*write_multi_char)(d2multi_out_env*, int ch, int n);
    void (*write_string)(d2multi_out_env*, const char* p, int n);
    void* (*addr)(uint32_t);                            // retail 32-bit address -> host pointer
    int (*wctomb)(char*, uint16_t);
    void (*cfltcvt)(const void* dbl8, char* buf, int fmt, int prec, int caps);   // 0x9860
    void (*cropzeros)(char*); void (*forcdecpt)(char*); // 0x9864, 0x986c
    const uint8_t* mbctype;                             // PTR_DAT_6f9e9488
    const char* nullStr; const uint16_t* nullWStr;      // "(null)" / L"(null)"
    uint32_t (*udivrem)(uint64_t* val, uint32_t base);  // __aulldvrm; returns remainder, val updated
};
extern const char DAT_6f9e63f8[] W; extern const char DAT_6f9e6418[] W;

// 1.13c D2Multi+0x2334
int d2multi_output(d2multi_out_env* e, const uint8_t* fmt, const uint32_t* args) {
    char buf[0x200 + 0x20]; char* const bufEnd = buf + sizeof(buf) - 4;  // last digit slot; wctomb scratch beyond
    uint8_t c = *fmt; int state = 0, written = 0, wide = 0, err = 0;
    int width = 0, prec = -1, hexAdj = 0, prefixLen = 0, capsFlt = 0;
    uint32_t flags = 0; size_t len = 0; const char* str = nullptr; char prefix[2] = {0, 0};
    char* heap = nullptr;
    e->count = &written;
    for (;;) {
        const uint8_t* next = fmt + 1;
        if (c == 0 || written < 0) return written;
        uint32_t cls = ((int8_t)c < ' ' || (int8_t)c > 'x') ? 0 : (DAT_6f9e63f8[(int8_t)c] & 0xf);
        state = (int)(int8_t)DAT_6f9e6418[cls * 8 + state] >> 4;
        const uint32_t* nargs = args;
        switch (state) {
        case 0: normal:
            wide = 0;
            if (e->mbctype[(uint32_t)c * 2 + 1] & 0x80) { e->write_char(e, c); next = fmt + 2; e->write_char(e, fmt[1]); }
            else e->write_char(e, c);
            break;
        case 1: prec = -1; capsFlt = 0; err = 0; width = 0; prefixLen = 0; flags = 0; wide = 0; break;
        case 2:
            if (c == ' ') flags |= 2; else if (c == '#') flags |= 0x80; else if (c == '+') flags |= 1;
            else if (c == '-') flags |= 4; else if (c == '0') flags |= 8;
            break;
        case 3:
            if (c == '*') { width = (int)*args; nargs = args + 1; if (width < 0) { flags |= 4; width = -width; } }
            else width = (int8_t)c - '0' + width * 10;
            break;
        case 4: prec = 0; break;
        case 5:
            if (c == '*') { prec = (int)*args; nargs = args + 1; if (prec < 0) prec = -1; }
            else prec = (int8_t)c - '0' + prec * 10;
            break;
        case 6:
            if (c == 'I') {
                uint8_t b = *next;
                if (b == '6' && fmt[2] == '4') { next = fmt + 3; flags |= 0x8000; }
                else if (b == '3' && fmt[2] == '2') { next = fmt + 3; flags &= ~0x8000u; }
                else if (b != 'd' && b != 'i' && b != 'o' && b != 'u' && b != 'x' && b != 'X') { state = 0; goto normal; }
            } else if (c == 'h') flags |= 0x20; else if (c == 'l') flags |= 0x10; else if (c == 'w') flags |= 0x800;
            break;
        case 7: {
            // conversion character: select string/number, then emit
            int base = 0, hexOff = 0; bool number = false, emit = true;
            switch (c) {
            case 'C': if (!(flags & 0x830)) flags |= 0x800; /* fallthrough */
            case 'c':
                nargs = args + 1;
                if (!(flags & 0x810)) { buf[0] = (char)*args; len = 1; }
                else { int r = e->wctomb(buf, (uint16_t)*args); len = (size_t)r; if (r < 0) err = 1; }
                str = buf; break;
            case 'Z': {
                nargs = args + 1;
                const uint8_t* as = (const uint8_t*)e->addr(*args);   // { u16 len; u16 max; u32 buf }
                const uint32_t bufAddr = as ? *(const uint32_t*)(as + 4) : 0;
                if (!as || !bufAddr) { str = e->nullStr; len = strlen(str); }
                else {
                    str = (const char*)e->addr(bufAddr); len = *(const uint16_t*)as;
                    if (flags & 0x800) { len /= 2; wide = 1; } else wide = 0;
                }
                break; }
            case 'S': if (!(flags & 0x830)) flags |= 0x800; /* fallthrough */
            case 's': {
                int lim = prec == -1 ? 0x7fffffff : prec;
                nargs = args + 1;
                const void* p = *args ? e->addr(*args) : nullptr;
                if (!(flags & 0x810)) {
                    str = p ? (const char*)p : e->nullStr;
                    size_t n = 0; while (lim && str[n]) { lim--; n++; } len = n;
                } else {
                    const uint16_t* w = p ? (const uint16_t*)p : e->nullWStr;
                    str = (const char*)w; wide = 1;
                    size_t n = 0; while (lim && w[n]) { lim--; n++; } len = n;
                }
                break; }
            case 'E': case 'G': capsFlt = 1; c += 0x20; /* fallthrough */
            case 'e': case 'f': case 'g': {
                flags |= 0x40;
                char* out = buf;
                if (prec < 0) prec = 6;
                else if (prec == 0) { if (c == 'g') prec = 1; }
                else {
                    if (prec > 0x200) prec = 0x200;
                    if (prec > 0xa3) { heap = (char*)d2multi_malloc(prec + 0x15d); if (heap) out = heap; else prec = 0xa3; }
                }
                uint32_t dbl[2] = {args[0], args[1]}; nargs = args + 2;
                e->cfltcvt(dbl, out, (int8_t)c, prec, capsFlt);
                uint32_t alt = flags & 0x80;
                if (alt && prec == 0) e->forcdecpt(out);
                if (c == 'g' && !alt) e->cropzeros(out);
                str = out;
                if (*out == '-') { flags |= 0x100; str = out + 1; }
                len = strlen(str);
                break; }
            case 'd': case 'i': flags |= 0x40; base = 10; number = true; break;
            case 'u': base = 10; number = true; break;
            case 'o': base = 8; if (flags & 0x80) flags |= 0x200; number = true; break;
            case 'p': prec = 8; /* fallthrough */
            case 'X': base = 16; hexOff = 7; number = true; break;
            case 'x': base = 16; hexOff = 0x27; number = true; break;
            case 'n':
                nargs = args + 1;
                if (flags & 0x20) *(int16_t*)e->addr(*args) = (int16_t)written; else *(int32_t*)e->addr(*args) = written;
                err = 1; emit = false; break;
            default: break;   // unknown conversion: retail falls to the emit path with the previous operands
            }
            if (number) {
                uint64_t v;
                if (flags & 0x8000) { v = ((uint64_t)args[1] << 32) | args[0]; nargs = args + 2; }
                else {
                    nargs = args + 1;
                    if (flags & 0x20) v = (flags & 0x40) ? (uint64_t)(int64_t)(int16_t)args[0] : (uint16_t)args[0];
                    else v = (flags & 0x40) ? (uint64_t)(int64_t)(int32_t)args[0] : (uint64_t)args[0];
                }
                if ((flags & 0x40) && (int64_t)v < 0) { v = -v; flags |= 0x100; }
                if (!(flags & 0x8000) && !(flags & 0x40)) v &= 0xffffffffu;
                if (prec < 0) prec = 1; else { flags &= ~8u; if (prec > 0x200) prec = 0x200; }
                if (v == 0) prefixLen = 0;
                if (base == 16 && (flags & 0x80) && v) { prefix[1] = (char)(hexOff + 'Q'); prefix[0] = '0'; prefixLen = 2; }
                char* p = bufEnd;
                while (prec-- > 0 || v) {
                    uint32_t d = e->udivrem(&v, (uint32_t)base) + '0';
                    if (d > '9') d += hexOff;
                    *p-- = (char)d;
                }
                len = (size_t)(bufEnd - p); str = p + 1;
                if ((flags & 0x200) && (*str != '0' || len == 0)) { *p = '0'; len++; str = p; }
            }
            if (emit && !err) {
                if (flags & 0x40) {
                    if (flags & 0x100) prefix[0] = '-';
                    else if (flags & 1) prefix[0] = '+';
                    else if (flags & 2) prefix[0] = ' ';
                    else goto pad;
                    prefixLen = 1;
                }
            pad:;
                int padn = width - prefixLen - (int)len;
                if (!(flags & 0xc)) e->write_multi_char(e, ' ', padn);
                e->write_string(e, prefix, prefixLen);
                if ((flags & 8) && !(flags & 4)) e->write_multi_char(e, '0', padn);
                if (!wide || (int)len < 1) e->write_string(e, str, (int)len);
                else {
                    const uint16_t* w = (const uint16_t*)str; size_t left = len;
                    char* mb = bufEnd + 1;
                    do { left--; int n = e->wctomb(mb, *w++); if (n < 1) break; e->write_string(e, mb, n); } while (left);
                }
                if (flags & 4) e->write_multi_char(e, ' ', padn);
            }
            if (heap) { d2multi_free(heap); heap = nullptr; }
            break; }
        default: break;
        }
        fmt = next; c = *fmt; args = nargs;
    }
}
