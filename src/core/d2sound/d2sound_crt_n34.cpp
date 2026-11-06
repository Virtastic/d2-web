// src/core/d2sound/d2sound_crt_n34.cpp -- D2Sound.dll MSVC CRT float-format helpers and small thunks.
// Register-passed args (EAX/EBX/EDI/ESI) become explicit parameters; host seams are function-pointer args.
#include <cstdint>
#include <cstring>

struct D2sStrFlt { int sign; int decpt; int flag; char* mantissa; };  // STRFLT: sign at +0, decpt +4, mantissa +0xC
static char g_d2sDecPoint = '.';  // DAT_6f9c57d4

// 1.13c D2sound+0x5C4C  _tolower: locale-less wasm, ASCII fold (the locale tail __tolower_mt is C-locale)
int d2s_tolower_n34(int c) { return (c >= 'A' && c <= 'Z') ? c + 32 : c; }

// 1.13c D2sound+0x5C6E  ___isctype_mt: ctype table (uint16 at ptd+0x48); wide/DBCS path via host GetStringTypeA seam
unsigned d2s_isctype_mt_n34(const uint16_t* ctype, int c, unsigned mask,
                            int (*getStringType)(const char* src, int len, uint16_t* out)) {
    uint16_t t;
    if ((unsigned)(c + 1) < 0x101) t = ctype[c];
    else {
        char b[3]; int n;
        if (ctype[(c >> 8) & 0xff] & 0x8000) { b[0] = (char)(c >> 8); b[1] = (char)c; b[2] = 0; n = 2; }
        else { b[0] = (char)c; b[1] = 0; n = 1; }
        if (!getStringType(b, n, &t)) return 0;
    }
    return t & mask;
}

// 1.13c D2sound+0x7230  DirectSound buffer start: lock, vtbl+0x24 probe, retry vtbl+0x50 while busy.
// Seams: enter/leave/sleep/fatal host callbacks, stop = sub_6980 (ECX = the object); vt = buffer vtable (slot 0x24/4 and 0x50/4).
// GetStatus writes the status word; bit 2 (DSBSTATUS_BUFFERLOST) runs Restore until it succeeds (sleeping 10 ms on
// DSERR_BUFFERLOST), re-reading the vtable each try, then stops the object through sub_6980.
typedef int32_t (*D2sStatusFn)(void* self, uint32_t* status);
typedef int32_t (*D2sRestoreFn)(void* self);
// The object, the interface and its vtable hold 4-byte guest pointers (a void* element is 8 bytes on wasm64).
static inline uint32_t d2s_rd32_n34(const void* p, uint32_t off) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + off, 4); return v; }
static inline void* d2s_vfn_n34(uint32_t iface, uint32_t off) {
    return (void*)(uintptr_t)d2s_rd32_n34((const void*)(uintptr_t)d2s_rd32_n34((const void*)(uintptr_t)iface, 0), off);
}
int d2s_buffer_start_n34(void** pbuf, void (*enter)(), void (*leave)(), void (*sleepMs)(int), void (*fatal)(), void (*stop)(void** obj)) {
    if (!d2s_rd32_n34(pbuf, 0)) { fatal(); return 0; }
    enter();
    const uint32_t b = d2s_rd32_n34(pbuf, 0); uint32_t status = 0;
    if (((D2sStatusFn)d2s_vfn_n34(b, 0x24))((void*)(uintptr_t)b, &status) < 0) { leave(); return 0; }
    if (status & 2) {
        int32_t r;
        do {
            const uint32_t cur = d2s_rd32_n34(pbuf, 0);
            r = ((D2sRestoreFn)d2s_vfn_n34(cur, 0x50))((void*)(uintptr_t)cur);
            if (r == -0x7787ff6a) { sleepMs(10); r = -1; }
        } while (r < 0);
        stop(pbuf);
    }
    leave();
    return 1;
}

// 1.13c D2sound+0x9C4F  float-conversion hook table install (6 slots at 0x6f9c5b20)
void d2s_fphooks_n34(void** tbl, void* cfltcvt, void* a1, void* fassign, void* forcdecpt, void* a4) {
    tbl[0] = cfltcvt; tbl[1] = a1; tbl[2] = fassign; tbl[3] = forcdecpt; tbl[4] = a4; tbl[5] = cfltcvt;
}

// 1.13c D2sound+0x9C87  __fpmath: install hooks, P5 FDIV test (no FDIV bug in wasm = 0), precision reset is a no-op
int d2s_fpmath_n34(int param_1, void** tbl, void* cfltcvt, void* a1, void* fassign, void* forcdecpt, void* a4) {
    d2s_fphooks_n34(tbl, cfltcvt, a1, fassign, forcdecpt, a4);
    (void)param_1;
    return 0;
}

// 1.13c D2sound+0xA4F6  __fdivp_sti_st: ST1 /= ST0 and pop; here the quotient of the two operands
double d2s_fdivp_n34(double st1, double st0) { return st1 / st0; }

// 1.13c D2sound+0xAD41  __forcdecpt: insert decimal point before the exponent 'e' by shifting tail right
void d2s_forcdecpt_n34(char* buf) {
    while (d2s_tolower_n34((unsigned char)*buf) != 'e') buf++;  // original also skips non-digits then stops at 'e'
    char c = *buf; *buf = g_d2sDecPoint;
    do { buf++; char n = *buf; *buf = c; c = n; } while (*buf);
}

// 1.13c D2sound+0xADE2  __fassign: parse number into float (flag==0) or double (flag!=0) via host strtod seam
void d2s_fassign_n34(int flag, char* dst, const char* number, double (*parse)(const char*)) {
    double d = parse(number);
    if (flag) memcpy(dst, &d, 8); else { float f = (float)d; memcpy(dst, &f, 4); }
}

// 1.13c D2sound+0xAE20  __shift: memmove(p+n, p, strlen(p)+1) when n != 0
void d2s_shift_n34(char* p, int n) { if (n) memmove(p + n, p, strlen(p) + 1); }

// 1.13c D2sound+0xAE3D  __cftoe2: fixed-form digits to d.ddde+XXX (EAX = f, EBX = buf; returns buf).
// shiftFirst opens one byte for the decimal point after the sign (only when ndec > 0).
void d2s_set_decpoint_n34(char c) { g_d2sDecPoint = c; }
char* d2s_cftoe2_n34(D2sStrFlt* f, char* buf, int ndec, int caps, char shiftFirst) {
    char* const out = buf;
    if (shiftFirst) d2s_shift_n34(buf + (f->sign == '-'), ndec > 0);
    if (f->sign == '-') *buf++ = '-';
    char* p = buf;
    if (ndec > 0) { p = buf + 1; buf[0] = p[0]; *p = g_d2sDecPoint; }
    p += ndec + (shiftFirst == 0);
    memcpy(p, "e+000", 6);
    if (caps) *p = 'E';
    if (*f->mantissa != '0') {
        int e = f->decpt - 1;
        if (e < 0) { e = -e; p[1] = '-'; }
        if (e > 99) { p[2] += (char)(e / 100); e %= 100; }
        if (e > 9) { p[3] += (char)(e / 10); e %= 10; }
        p[4] += (char)e;
    }
    return out;
}

// 1.13c D2sound+0xAEEB  _cftoe: convert via host seams (double->STRFLT, STRFLT->digits), then __cftoe2
int d2s_cftoe_n34(D2sStrFlt* f, char* buf, int ndec, int caps) {
    d2s_cftoe2_n34(f, buf + (ndec > 0) + (f->sign == '-'), ndec, caps, 0);
    return (int)(intptr_t)buf;
}

// 1.13c D2sound+0xAF5B  __cftof2: fixed-form digits to ddd.ddd
char* d2s_cftof2_n34(D2sStrFlt* f, char* buf, int ndec, char shiftFirst) {
    int dp = f->decpt;
    if (shiftFirst && (unsigned)(dp - 1) == (unsigned)ndec) {
        buf[(f->sign == '-') + (dp - 1)] = '0'; buf[(f->sign == '-') + (dp - 1) + 1] = 0;
    }
    char* p = buf;
    if (f->sign == '-') *p++ = '-';
    if (f->decpt < 1) { d2s_shift_n34(p, 1); *p++ = '0'; } else p += f->decpt;
    if (ndec > 0) {
        d2s_shift_n34(p, 1); *p = g_d2sDecPoint;
        if (f->decpt < 0) {
            int n = -f->decpt;
            if (shiftFirst || n <= ndec) ndec = n;
            d2s_shift_n34(p + 1, ndec);
            memset(p + 1, '0', ndec);
        }
    }
    return buf;
}
