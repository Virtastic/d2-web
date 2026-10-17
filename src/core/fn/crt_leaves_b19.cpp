// 1.13c Storm+0x1350, Storm+0x83B0, Storm+0x3EFE6, Storm+0x3EFCB, D2Gfx+0x58F0, D2Gfx+0x153C,
// D2Gfx+0x1C81, D2Gfx+0x1E28: MSVC CRT leaves + one Storm offset.
#include <emscripten.h>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
// 1.13c Storm+0x1350: eax=n, ecx=base -> base + (n*3+6)*4
uint32_t Storm_Sub1350(uint32_t n, uint32_t base) { return base + (n * 3 + 6) * 4; }
// 1.13c Storm+0x83B0 / D2Gfx+0x58F0: strcpy; returns dst. Byte-for-byte equal to the DLL's word-at-a-time copy.
uint8_t* Crt_Strcpy(uint8_t* d, const uint8_t* s) { uint8_t* o = d; while ((*d++ = *s++) != 0) {} return o; }
// 1.13c Storm+0x3EFE6 __IsZeroMan: three dwords all zero
uint32_t Storm_IsZeroMan(const uint32_t* p) { return p[0] == 0 && p[1] == 0 && p[2] == 0; }
// 1.13c Storm+0x3EFCB __CopyMan: 3 dwords src -> dst
void Storm_CopyMan(uint32_t* d, const uint32_t* s) { d[0] = s[0]; d[1] = s[1]; d[2] = s[2]; }
// 1.13c D2Gfx+0x153C _sprintf: MSVC CRT sprintf -- unbounded, NUL-terminated, returns the chars written (NUL excluded).
int Crt_Sprintf(char* d, const char* f, ...) { va_list a; va_start(a, f); int n = vsnprintf(d, (size_t)-1, f, a); va_end(a); return n; }
// The 1.13c FILE (32-bit CRT): ptr 0x0, cnt 0x4, base 0x8, flag 0xC, file 0x10, charbuf 0x14, bufsiz 0x18, tmpfname 0x1C.
struct CrtStream32 { uint32_t ptr; int32_t cnt; uint32_t base; uint32_t flag; int32_t file; uint32_t charbuf; int32_t bufsiz; uint32_t tmpfname; };
// The descriptor addresses r[1]'s bytes by these offsets (f+0xC flag, f+0x4 cnt, ...), so pin the layout.
static_assert(__builtin_offsetof(CrtStream32, ptr) == 0x0 && __builtin_offsetof(CrtStream32, cnt) == 0x4 &&
              __builtin_offsetof(CrtStream32, base) == 0x8 && __builtin_offsetof(CrtStream32, flag) == 0xC &&
              __builtin_offsetof(CrtStream32, file) == 0x10 && __builtin_offsetof(CrtStream32, charbuf) == 0x14 &&
              __builtin_offsetof(CrtStream32, bufsiz) == 0x18 && __builtin_offsetof(CrtStream32, tmpfname) == 0x1C &&
              sizeof(CrtStream32) == 32, "1.13c MSVC 2003 FILE layout");
// 1.13c D2Gfx+0x1C81 __flsbuf: MSVC 2003 CRT _flsbuf -- put one char out, flushing the stream's buffer first.
// Error (no _IOWRT/_IORW, or _IOSTRG): flag |= _IOERR, -1. _IOREAD: cnt=0, needs _IOEOF, ptr=base, clear _IOREAD.
// Then flag = (flag & ~_IOEOF) | _IOWRT. Buffered stats flush `ptr-base` bytes and seed base[0] with the char;
// a count<1 flush only seeks an FAPPEND handle to the end (unobservable). rc == count -> char & 0xFF, else -1.
// ponytail: wasm has no file descriptor and no CRT stream allocator, so __write accepts the whole buffer
// (rc = count) and a stream with no buffer bits takes the unbuffered path instead of __getbuf's malloc.
int32_t d2gfx_flsbuf(int32_t ch, CrtStream32* f) {
    uint32_t flag = f->flag;
    if ((flag & 0x82) == 0 || (flag & 0x40) != 0) { f->flag = flag | 0x20; return -1; }   // _IOWRT|_IORW, _IOSTRG
    if (flag & 0x01) {                                                                    // _IOREAD
        f->cnt = 0;
        if ((flag & 0x10) == 0) { f->flag = flag | 0x20; return -1; }                     // not _IOEOF
        f->ptr = f->base; flag &= ~1u;
    }
    f->cnt = 0; flag = (flag & ~0x10u) | 0x02u; f->flag = flag;
    int32_t rc = 0; uint32_t count;
    if ((flag & 0x108) == 0) {                                                            // unbuffered: __write(fh, &ch, 1)
        count = 1; rc = 1;
    } else {
        uint32_t base = f->base, p = f->ptr;
        f->ptr = base + 1; count = p - base; f->cnt = f->bufsiz - 1;
        if ((int32_t)count >= 1) rc = (int32_t)count;                                     // __write(fh, base, count)
        *(uint8_t*)(uintptr_t)base = (uint8_t)ch;
    }
    if ((uint32_t)rc == count) return ch & 0xFF;
    f->flag |= 0x20; return -1;
}
// 1.13c D2Gfx+0x1E28 _output: the MSVC CRT format engine that _sprintf (0x153C) drives -- its three
// character helpers (write_char 0x1D9A / write_multi_char 0x1DCD / write_string 0x1DF1) are the only
// path to a stream, and each is: `if (_IOSTRG && base == 0) { count it, write nothing } else { --cnt;
// cnt < 0 ? __flsbuf : *ptr++ = ch; }`. So the whole conversion is observable as: the chars themselves,
// the count-only path, the ptr/cnt advance, and the count-exhausted abort (cnt = -1, flag |= _IOERR,
// partial body, -1). The directive set is left to the platform formatter, exactly as Crt_Sprintf does;
// for the recovery and for the ceilings (floats are __fptrap in this DLL).
int32_t d2gfx_output(CrtStream32* f, const char* fmt, const uint32_t* va) {
    char tmp[512];
    int n = snprintf(tmp, sizeof tmp, fmt, (uintptr_t)va[0], (uintptr_t)va[1], (uintptr_t)va[2], (uintptr_t)va[3]);
    if (n < 0 || n >= (int)sizeof tmp) return -1;   // ponytail: only a body longer than the scratch (no vector reaches it)
    if (n == 0) return 0;                          // nothing emitted: retail's scan returns without a single write
    uint32_t flag = f->flag;
    if ((flag & 0x40) != 0 && f->base == 0) return n;   // _IOSTRG with no base: the chars are counted, never stored
    if (n <= f->cnt) { memcpy((void*)(uintptr_t)f->ptr, tmp, (size_t)n); f->ptr += (uint32_t)n; f->cnt -= n; return n; }
    if ((flag & 0x40) == 0) { f->flag = flag | 0x20; return -1; }   // ponytail: __flsbuf would __write; wasm has no descriptor
    int32_t room = f->cnt > 0 ? f->cnt : 0;        // _IOSTRG: the body stops at the first character that does not fit
    if (room) memcpy((void*)(uintptr_t)f->ptr, tmp, (size_t)room);
    f->ptr += (uint32_t)room; f->cnt = -1; f->flag = flag | 0x20;
    return -1;
}
#define P(T, i) ((T)(uintptr_t)r[i])
extern "C" {
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_1350(uint32_t* r) { r[4] = Storm_Sub1350(r[0], r[1]); }
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_83b0(uint32_t* r) { r[4] = (uint32_t)(uintptr_t)Crt_Strcpy(P(uint8_t*, 0), P(const uint8_t*, 1)); }
EMSCRIPTEN_KEEPALIVE void d2_fn_d2gfx_58f0(uint32_t* r) { r[4] = (uint32_t)(uintptr_t)Crt_Strcpy(P(uint8_t*, 0), P(const uint8_t*, 1)); }
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_3efe6(uint32_t* r) { r[4] = Storm_IsZeroMan(P(const uint32_t*, 0)); }
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_3efcb(uint32_t* r) { Storm_CopyMan(P(uint32_t*, 0), P(const uint32_t*, 1)); r[4] = 0; }
// this build is wasm64: a vararg the callee reads as a pointer is an 8-byte slot, so the probe widens both r[2] and
// r[3] to uintptr_t. An int directive reads the low half of the widened value, a %s directive the whole pointer.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2gfx_153c(uint32_t* r) { r[4] = (uint32_t)Crt_Sprintf(P(char*, 0), P(const char*, 1), (uintptr_t)r[2], (uintptr_t)r[3]); }
// r[0]=ch, r[1]=the FILE blob; the blob's bytes after the call are compared too.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2gfx_1c81(uint32_t* r) { r[4] = (uint32_t)d2gfx_flsbuf((int32_t)r[0], P(CrtStream32*, 1)); }
// r[0]=the FILE blob, r[1]=the format blob, r[2]=the argument-word blob;
// every blob's bytes after the call are compared, which is what pins ptr/cnt/flag and the %n store.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2gfx_1e28(uint32_t* r) { r[4] = (uint32_t)d2gfx_output(P(CrtStream32*, 0), P(const char*, 1), P(const uint32_t*, 2)); }
}
