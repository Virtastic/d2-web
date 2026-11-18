// 1.13c D2Client+0x061BC
//
// MSVC 2003 (VC7.1) shared scanf engine (`_input`), rva 0x061BC, 3562 bytes,
// base 0x6FAB0000.  Called by _sscanf at 0x038F4 (see d2client_batch_038a3.cpp),
// which builds a read view of the source string in the CRT FILE and calls this
// with (FILE*, format, vararg cursor).  NOT __fsopen despite the function list name.
//
// Callees (retail address -> seam field; 0x7E60 __chkstk, 0xA9F0 _memset and
// 0x77D2 __security_check_cookie are compiler prologue, not calls):
//   0x03FD3 _isdigit   (ctype mask 4;  Ghidra labels it FID_conflict__iscntrl,
//                       the retail body is `and $0x4` -- it is _isdigit)
//   0x0400D _isxdigit  (ctype mask 0x80)
//   0x0404C _isspace   (ctype mask 8)
//   0x061A6 __inc      (getc fast path; d2_client_61a6; its first argument is
//                       dead -- d2_client_61a6 ignores it)
//   0x0A975 _ungetc
//   0x0AB10 _mbtowc
//   0x0DB8F0 conversion slot, _fassign(flag, dest, digits) at runtime.  CRT init
//            (rva 0x0CAC82) patches the slot from __fptrap (0x0BB2B, aborts with
//            __amsg_exit(2)) to __fassign (0x0CB266).  flag = intSize-1: 0 stores
//            a 4-byte float, nonzero an 8-byte double; __fassign calls the float
//            parsers 0x0CC315 / 0x0CC2D0.  Host seam.
//
// Data it reads:
//   +0x0DB900 _pctype pointer -> table at 0x6FB7FAE8 (2 bytes/char, high bit of
//            the second byte = MBCS lead byte).  Shipped table has no lead-byte
//            bit set (C locale), so the zeroed model here is behaviour-identical;
//            it is kept so the global is pinned.
//   +0x0DB910 mbtowc byte count (shipped 1).
//   +0x0DB914 decimal point (shipped 0x2E '.').
//   +0x0DB780 stack cookie (__security_check_cookie).
//
// The 0x20-byte scanset buffer of the retail code is a stack local (SEH scope
// guard at 0x68fe, memset 0xA9F0) and never escapes this function.
//
// State notes from the disassembly: intSize starts at 1 per conversion and is
// 'h' -1 / 'l','L' +1; the numeric store is 64-bit if I64, 4-byte if
// intSize != 0, else 2-byte.  sizeMod starts 0, is forced to +1 for S/C else
// -1, and +1 means wide (%ls, %ws, %S, %lc, %C).  The width check in the
// numeric prefix uses the post-decrement zero, the float/string loops the
// pre-decrement zero -- both are reproduced as shipped.

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <emscripten.h>

// CRT FILE read view; same layout as the one d2_client_38f4 builds.
#pragma pack(push, 1)
struct D2cScanFile {
    const char* ptr;   // 0x00
    int32_t cnt;       // 0x04
    const char* base;  // 0x08
    int32_t flag;      // 0x0C
};
#pragma pack(pop)
static_assert(offsetof(D2cScanFile, ptr) == 0x00, "FILE._ptr");
static_assert(offsetof(D2cScanFile, cnt) == 0x04 + 1 * (sizeof(void*) - 4), "FILE._cnt");
static_assert(offsetof(D2cScanFile, base) == 0x08 + 1 * (sizeof(void*) - 4), "FILE._base");
static_assert(offsetof(D2cScanFile, flag) == 0x0C + 2 * (sizeof(void*) - 4), "FILE._flag");

struct D2ClientBatch061BCEnv {
    int32_t (*isdigit)(int32_t);                         // 0x03FD3
    int32_t (*isspace)(int32_t);                         // 0x0404C
    int32_t (*isxdigit)(int32_t);                        // 0x0400D
    int32_t (*inc)(void* stream);                        // 0x061A6 __inc, EOF = -1
    int32_t (*ungetc_)(int32_t, void* stream);           // 0x0A975
    int32_t (*mbtowc_)(uint16_t*, const char*, int32_t); // 0x0AB10
    void (*fassign)(int32_t, void*, const uint8_t*);     // 0x0DB8F0 -> 0x0CB266
};

// +0x0DB900 -> table at 0x6FB7FAE8.  Zero lead-byte bits, as shipped.
static uint8_t g_pctype[0x100 * 2];
// +0x0DB910, +0x0DB914
static const int32_t g_mbtowcMax = 1;
static const uint8_t g_decimalPoint = 0x2E;

extern "C" EMSCRIPTEN_KEEPALIVE int32_t
d2_client_61bc(void* file, const uint8_t* format, const uint32_t* args,
               const D2ClientBatch061BCEnv* env)
{
    D2cScanFile* stream = (D2cScanFile*)file;
    const uint8_t* fmt = format;
    const uint32_t* argp = args;
    const uint32_t* savedArg = nullptr;

    uint32_t ch = 0;          // -0x190, EOF = 0xFFFFFFFF
    int32_t nread = 0;        // -0x184
    int8_t _char = 0;         // -0x195
    int32_t nmatch = 0;       // -0x1C0

    uint32_t value32 = 0;     // -0x1A8, reset per conversion
    uint64_t value64 = 0;     // -0x1B0 low, -0x1AC high
    uint8_t* dest = nullptr;  // -0x1B8

    int32_t widthVal = 0;     // -0x188
    int32_t widthDigits = 0;  // -0x1BC
    int32_t intSize = 1;      // -0x18B
    int32_t sizeMod = 0;      // -0x189
    int32_t use64 = 0;        // -0x1D0
    int32_t suppress = 0;     // -0x18A
    int32_t stop = 0;         // -0x17D
    int32_t wide = 0;         // -0x196
    int32_t invert = 0;       // -0x1B1
    int32_t negative = 0;     // -0x197
    int32_t nchar = 0;        // -0x1A0
    uint32_t conv = 0;        // -0x1C4, effective (lowercased) conversion char

    const uint8_t* cur = nullptr;
    const uint8_t* q = nullptr;
    uint8_t* fp = nullptr;
    uint8_t* countp = nullptr;
    uint8_t b1 = 0, b2 = 0, lo = 0, hi = 0, setLast = 0;
    uint32_t v = 0, ch2 = 0, c0 = 0;
    int32_t old = 0;
    uint16_t wc = 0;
    uint8_t mb[2] = {0, 0};
    uint8_t scanset[0x20];
    uint8_t numbuf[0x160];

    // ------------------------------------------------------------------
LAB_6fab6224:
    if (*fmt == 0) goto LAB_6fab6f9e;
    c0 = *fmt;
    // ---- whitespace in the format: drain input whitespace, then push the first
    // non-space byte back so the next conversion reads it (the tail of the outer
    // loop, after the `break` that leaves a conversion)
    if (env->isspace((int32_t)c0) != 0) {
        nread--;
        do {
            nread++;
            ch = (uint32_t)env->inc(stream);
        } while (env->isspace((int32_t)ch) != 0);
        if (ch != 0xFFFFFFFFu) env->ungetc_((int32_t)ch, stream);
        do {
            fmt++;
        } while (env->isspace((int32_t)*fmt) != 0);
        goto LAB_6fab6224;
    }
    if (*fmt != 0x25 /* '%' */) goto LAB_6fab6ef3;

    // ---- spec parse (resets at 0x62a1..0x62f1) ----
    value32 = 0;
    setLast = 0;
    nchar = 0;
    widthDigits = 0;
    widthVal = 0;
    invert = 0;
    negative = 0;
    suppress = 0;
    stop = 0;
    wide = 0;
    sizeMod = 0;
    intSize = 1;
    use64 = 0;

    for (;;) {
        cur = fmt + 1;
        b1 = *cur;
        if (env->isdigit((int32_t)b1) != 0) {
            widthDigits++;
            widthVal = (int32_t)(b1 - 0x30) + widthVal * 10;
        } else if (b1 < 0x4F) {
            if (b1 == 0x4E) {                 // 'N' ignored
            } else if (b1 == 0x2A) {          // '*'
                suppress++;
            } else if (b1 == 0x46) {          // 'F' ignored
            } else if (b1 == 0x49) {          // 'I'
                b2 = fmt[2];
                if (b2 == 0x36 && fmt[3] == 0x34) {          // "I64"
                    use64++;
                    value64 = 0;
                    cur = fmt + 3;
                } else if (b2 == 0x33 && fmt[3] == 0x32) {   // "I32"
                    cur = fmt + 3;
                } else if (b2 == 0x64 || b2 == 0x69 || b2 == 0x6F ||
                           b2 == 0x78 || b2 == 0x58) {       // d i o x X
                    // bare 'I' ignored; its follower is reparsed next round
                } else {
                    stop++;
                }
            } else if (b1 == 0x4C) {          // 'L'
                intSize++;
            } else {
                stop++;
            }
        } else {
            if (b1 == 0x68) {                 // 'h'
                intSize--;
                sizeMod--;
            } else {
                if (b1 == 0x6C) intSize++;    // 'l'
                if (b1 == 0x6C || b1 == 0x77) sizeMod++;   // 'l', 'w'
                else stop++;                  // conversion char: the retail goto 0x63b8 skips sizeMod
            }
        }
        fmt = cur;  // sits on the conversion character
        if (stop != 0) break;
    }

    // ---- 0x63f3: fetch the output pointer ----
    if (suppress == 0) {
        savedArg = argp;
        dest = (uint8_t*)(uintptr_t)*argp;
        argp++;
    }

    // ---- 0x6422 ----
    stop = 0;
    if (sizeMod == 0)
        sizeMod = (*fmt == 0x53 || *fmt == 0x43) ? 1 : -1;  // S / C
    conv = *fmt | 0x20;   // 0x6450: the whole tail compares the lowercased char
    c0 = conv;

    // ---- whitespace skip (0x6456..0x648f); %c/%[ read first, %n neither ----
    if (c0 != 0x6E /* 'n' */) {
        if (c0 == 0x63 /* 'c' */ || c0 == 0x7B /* '[' */) {
            nread++;
            ch = (uint32_t)env->inc(stream);
        } else {
            do {
                nread++;
                ch = (uint32_t)env->inc(stream);
            } while (env->isspace((int32_t)ch) != 0);
        }
    }

    // ---- 0x64a8: width already exhausted ----
    if (widthDigits != 0 && widthVal == 0) goto LAB_6fab6f5f;

    // ---- conversion dispatch (0x64bf / 0x6899) ----
    if (c0 == 0x6F) goto LAB_6fab6bc8;                // 'o'
    if (c0 == 0x63) goto LAB_6fab674e;                // 'c'
    if (c0 == 0x64) goto LAB_6fab6bc8;                // 'd'
    if (c0 < 0x64) goto LAB_6fab68c3;                 // literal fallback
    if (c0 <= 0x67) goto LAB_6fab652f;                // e f g
    if (c0 == 0x69) { conv = 0x64; goto LAB_6fab6514; } // 'i'
    if (c0 == 0x6E) goto LAB_6fab64f9;                // 'n'
    if (c0 > 0x6F) {
        if (c0 == 0x70) { intSize = 1; goto LAB_6fab6bc8; }   // 'p'
        if (c0 == 0x73) goto LAB_6fab6762;                    // 's'
        if (c0 == 0x75) goto LAB_6fab6bc8;                    // 'u'
        if (c0 == 0x78) goto LAB_6fab6514;                    // 'x'
        if (c0 == 0x7B) goto LAB_6fab68fe;                    // '['
    }
    goto LAB_6fab68c3;

    // ---- '%c' (0x674e): default width 1 ----
LAB_6fab674e:
    if (widthDigits == 0) {
        widthDigits = 1;
        widthVal++;
    }
    // fall through

LAB_6fab6762:
    if (sizeMod > 0) wide = 1;
    goto LAB_6fab6772;

    // ---- '%[' (0x68fe) ----
LAB_6fab68fe:
    if (sizeMod > 0) wide = 1;
    q = fmt + 1;
    if (*q == 0x5E /* '^' */) {
        q++;
        invert = 0xFF;
    }
    fmt = q;
    memset(scanset, 0, sizeof(scanset));
    if (*q == 0x5D /* ']' */) {
        setLast = 0x5D;
        q++;
        scanset[0x5D >> 3] |= (uint8_t)(1u << (0x5D & 7));
    }
    for (;;) {
        b1 = *q;
        if (b1 == 0x5D) break;
        if (b1 == 0x2D /* '-' */ && setLast != 0) {
            b2 = q[1];
            if (b2 != 0x5D) {
                lo = b2 < setLast ? b2 : setLast;
                hi = b2 < setLast ? setLast : b2;
                for (v = lo; v <= (uint32_t)hi; v++)
                    scanset[v >> 3] |= (uint8_t)(1u << (v & 7));
                q += 2;
                setLast = 0;
                continue;
            }
        }
        setLast = b1;
        scanset[b1 >> 3] |= (uint8_t)(1u << (b1 & 7));
        q++;
    }
    fmt = q;  // sits on ']'
    goto LAB_6fab6772;

    // ---- '%n' (0x64f9) ----
LAB_6fab64f9:
    value32 = (uint32_t)nread;
    if (suppress != 0) goto LAB_6fab6736;
    goto LAB_6fab6ebc;

    // ---- numeric entry (0x6bc8): sign, then the prefix/state is set ----
LAB_6fab6bc8:
    if (ch == 0x2D /* '-' */) {
        negative = 1;
        goto LAB_6fab6be1;
    }
    if (ch != 0x2B /* '+' */) goto LAB_6fab6c0f;
LAB_6fab6be1:
    widthVal--;
    if (widthVal != 0 || widthDigits == 0) {
        nread++;
        ch = (uint32_t)env->inc(stream);
        goto LAB_6fab6c0f;
    }
    stop = 1;
    goto LAB_6fab6c0f;

    // ---- 'i' / 'x' (0x6514): sign and 0 / 0x prefix pick the base ----
LAB_6fab6514:
    if (ch == 0x2D) {
        negative = 1;
        goto LAB_6fab6a61;
    }
    if (ch != 0x2B) goto LAB_6fab6a8f;
LAB_6fab6a61:
    widthVal--;
    if (widthVal != 0 || widthDigits == 0) {
        nread++;
        ch = (uint32_t)env->inc(stream);
        goto LAB_6fab6a8f;
    }
    stop = 1;
LAB_6fab6a8f:
    if (ch != 0x30 /* '0' */) goto LAB_6fab6c0f;
    nread++;
    ch = (uint32_t)env->inc(stream);
    if (ch == 0x78 || ch == 0x58) goto LAB_6fab6b0a;  // 0x / 0X
    nchar = 1;
    if (conv != 0x78) {
        if (widthDigits != 0) {
            widthVal--;
            if (widthVal == 0) stop = 1;
        }
        conv = 0x6F;  // octal
    } else {
        // `%x` on a lone '0': push the follower back and rescan the '0' as a
        // hex digit (retail keeps conv = 'x' and does not spend width here)
        nread--;
        if (ch != 0xFFFFFFFFu) env->ungetc_((int32_t)ch, stream);
        ch = 0x30;
    }
    goto LAB_6fab6c0f;
LAB_6fab6b0a:
    nread++;
    ch = (uint32_t)env->inc(stream);
    if (widthDigits != 0) {
        widthVal -= 2;
        if (widthVal < 1) stop = 1;
    }
    conv = 0x78;
    // fall through

LAB_6fab6c0f:
    if (use64 != 0) {
        // ---- 64-bit digit loop (0x6c1c) ----
        for (;;) {
            if (stop != 0) goto LAB_6fab6d51;
            if (conv == 0x78 || conv == 0x70) goto LAB_6fab6c94;
            if (env->isdigit((int32_t)ch) == 0) goto LAB_6fab6cd1;
            if (conv == 0x6F) {
                if (ch >= 0x38) goto LAB_6fab6cd1;
                value64 <<= 3;
            } else {
                value64 *= 10;
            }
            goto LAB_6fab6cd7;
        LAB_6fab6c94:
            if (env->isxdigit((int32_t)ch) == 0) goto LAB_6fab6cd1;
            value64 <<= 4;
            if (env->isdigit((int32_t)ch) == 0)
                ch = (ch & 0xFFFFFFDF) - 7;
        LAB_6fab6cd7:
            if (stop != 0) {
                nread--;
                if (ch != 0xFFFFFFFFu) env->ungetc_((int32_t)ch, stream);
            } else {
                nchar++;
                value64 += (uint64_t)(ch - 0x30);
                if (widthDigits != 0) {
                    widthVal--;
                    if (widthVal == 0) {
                        stop = 1;
                        continue;
                    }
                }
                nread++;
                ch = (uint32_t)env->inc(stream);
                continue;
            }
            break;
        LAB_6fab6cd1:
            stop = 1;
            goto LAB_6fab6cd7;
        }
        goto LAB_6fab6d51;
    }

    // ---- 32-bit digit loop (0x6d82) ----
    for (;;) {
        if (stop != 0) goto LAB_6fab6e6f;
        if (conv == 0x78 || conv == 0x70) goto LAB_6fab6dca;
        if (env->isdigit((int32_t)ch) == 0) goto LAB_6fab6def;
        if (conv == 0x6F) {
            if (ch >= 0x38) goto LAB_6fab6def;
            value32 <<= 3;
        } else {
            value32 *= 10;
        }
        goto LAB_6fab6df5;
    LAB_6fab6dca:
        if (env->isxdigit((int32_t)ch) == 0) goto LAB_6fab6def;
        value32 <<= 4;
        if (env->isdigit((int32_t)ch) == 0)
            ch = (ch & 0xFFFFFFDF) - 7;
    LAB_6fab6df5:
        if (stop != 0) {
            nread--;
            if (ch != 0xFFFFFFFFu) env->ungetc_((int32_t)ch, stream);
        } else {
            nchar++;
            value32 += ch - 0x30;
            if (widthDigits != 0) {
                widthVal--;
                if (widthVal == 0) {
                    stop = 1;
                    continue;
                }
            }
            nread++;
            ch = (uint32_t)env->inc(stream);
            continue;
        }
        break;
    LAB_6fab6def:
        stop = 1;
        goto LAB_6fab6df5;
    }

LAB_6fab6e6f:
    if (negative != 0) value32 = (uint32_t)(-(int32_t)value32);
    goto LAB_6fab6e7e;

LAB_6fab6d51:
    if (negative != 0) value64 = (uint64_t)(-(int64_t)value64);

LAB_6fab6e7e:
    if (conv == 0x46 /* 'F' */) nchar = 0;  // as shipped; unreachable
    if (nchar == 0) goto LAB_6fab6f9e;
    if (suppress != 0) goto LAB_6fab6736;
    nmatch++;
    goto LAB_6fab6ebc;

    // ---- float: %e %f %g (0x652f) ----
LAB_6fab652f:
    fp = numbuf;
    if (ch == 0x2D) {
        *fp++ = (uint8_t)ch;
        goto LAB_6fab6553;
    }
    if (ch != 0x2B) goto LAB_6fab6576;
LAB_6fab6553:
    widthVal--;
    nread++;
    ch = (uint32_t)env->inc(stream);
LAB_6fab6576:
    if (widthDigits == 0 || widthVal > 0x15D) widthVal = 0x15D;
    while (env->isdigit((int32_t)ch) != 0) {          // mantissa digits
        old = widthVal;
        widthVal--;
        if (old == 0) break;
        nchar++;
        *fp++ = (uint8_t)ch;
        nread++;
        ch = (uint32_t)env->inc(stream);
    }
    if (ch == g_decimalPoint) {
        old = widthVal;
        widthVal--;
        if (old != 0) {
            nread++;
            ch = (uint32_t)env->inc(stream);
            *fp++ = g_decimalPoint;
            while (env->isdigit((int32_t)ch) != 0) {  // fraction digits
                old = widthVal;
                widthVal--;
                if (old == 0) break;
                nchar++;
                *fp++ = (uint8_t)ch;
                nread++;
                ch = (uint32_t)env->inc(stream);
            }
        }
    }
    if (nchar != 0 && (ch == 0x65 || ch == 0x45)) {   // exponent
        old = widthVal;
        widthVal--;
        if (old != 0) {
            *fp++ = 0x65;
            nread++;
            ch = (uint32_t)env->inc(stream);
            if (ch == 0x2D) *fp++ = (uint8_t)ch;
            if (ch == 0x2D || ch == 0x2B) {
                old = widthVal;
                widthVal--;
                if (old != 0) {
                    nread++;
                    ch = (uint32_t)env->inc(stream);
                } else {
                    widthVal = 0;
                }
            }
            while (env->isdigit((int32_t)ch) != 0) {  // exponent digits
                old = widthVal;
                widthVal--;
                if (old == 0) break;
                nchar++;
                *fp++ = (uint8_t)ch;
                nread++;
                ch = (uint32_t)env->inc(stream);
            }
        }
    }
    nread--;
    if (ch != 0xFFFFFFFFu) env->ungetc_((int32_t)ch, stream);
    if (nchar == 0) goto LAB_6fab6f9e;
    if (suppress != 0) goto LAB_6fab6736;
    nmatch++;
    *fp = 0;
    env->fassign(intSize - 1, dest, numbuf);  // flag 0 = float, else double
    goto LAB_6fab6736;

    // ---- shared store (0x6ebc) ----
LAB_6fab6ebc:
    if (use64 != 0) {
        *(uint64_t*)(uintptr_t)dest = value64;
    } else if (intSize != 0) {
        *(uint32_t*)(uintptr_t)dest = value32;
    } else {
        *(uint16_t*)(uintptr_t)dest = (uint16_t)value32;
    }
    goto LAB_6fab6736;

    // ---- string / scanset loop (0x6772) ----
LAB_6fab6772:
    countp = dest;
    nread--;
    if (ch != 0xFFFFFFFFu) env->ungetc_((int32_t)ch, stream);

LAB_6fab6796:
    if (widthDigits != 0) {
        old = widthVal;
        widthVal--;
        if (old == 0) goto LAB_6fab6b73;
    }
    nread++;
    ch = (uint32_t)env->inc(stream);
    if (ch == 0xFFFFFFFFu) goto LAB_6fab6b5a;
    if (conv != 0x63) {
        if (conv == 0x73) {
            if ((ch >= 9 && ch <= 0x0D) || ch == 0x20)   // %s stops at whitespace
                goto LAB_6fab6b5a;
        } else if (conv == 0x7B) {
            if ((((uint32_t)scanset[ch >> 3] ^ (uint32_t)invert) &
                 (1u << (ch & 7))) == 0)
                goto LAB_6fab6b5a;
        } else {
            goto LAB_6fab6b5a;
        }
    }
    if (suppress != 0) {
        countp++;
        goto LAB_6fab6796;
    }
    if (wide == 0) {
        *dest++ = (uint8_t)ch;
        goto LAB_6fab6796;
    }
    mb[0] = (uint8_t)ch;
    if ((g_pctype[((uint32_t)(ch & 0xFF) << 1) + 1] & 0x80) != 0) {
        nread++;
        mb[1] = (uint8_t)env->inc(stream);
    }
    wc = 0;
    env->mbtowc_(&wc, (const char*)mb, g_mbtowcMax);
    *(uint16_t*)(uintptr_t)dest = wc;
    dest += 2;
    goto LAB_6fab6796;

LAB_6fab6b5a:
    nread--;
    if (ch != 0xFFFFFFFFu) env->ungetc_((int32_t)ch, stream);

LAB_6fab6b73:
    if (countp == dest) goto LAB_6fab6f9e;
    if (suppress == 0) {
        nmatch++;
        if (conv != 0x63) {
            if (wide == 0) *dest = 0;
            else *(uint16_t*)(uintptr_t)dest = 0;
        }
    }

    // ---- advance (0x6736 / 0x673c) ----
LAB_6fab6736:  // 0x6736 / 0x673c, fmt is already current here
    _char++;
    fmt++;

LAB_6fab6f7c:
    if (ch != 0xFFFFFFFFu) goto LAB_6fab6224;
    if (*fmt != 0x25 || fmt[1] != 0x6E) goto LAB_6fab6f9e;
    goto LAB_6fab6224;

    // ---- non-'%' literal in the format (0x6ef3) ----
LAB_6fab6ef3:
    nread++;
    ch = (uint32_t)env->inc(stream);
    b1 = *fmt;
    fmt++;
    if (b1 != (uint8_t)ch) goto LAB_6fab6f5f;
    if ((g_pctype[((uint32_t)(ch & 0xFF) << 1) + 1] & 0x80) != 0) {
        nread++;
        ch2 = (uint32_t)env->inc(stream);
        b2 = *fmt;
        fmt++;
        if (b2 != (uint8_t)ch2) {
            if (ch2 != 0xFFFFFFFFu) env->ungetc_((int32_t)ch2, stream);
            goto LAB_6fab6f5f;
        }
        nread--;
    }
    goto LAB_6fab6f7c;

    // ---- literal conversion specifier (0x68c3) ----
LAB_6fab68c3:
    if (*fmt != (uint8_t)ch) goto LAB_6fab6f5f;
    _char--;
    if (suppress == 0) argp = savedArg;  // argument not consumed
    goto LAB_6fab6736;

    // ---- failure (0x6f5f) ----
LAB_6fab6f5f:
    if (ch != 0xFFFFFFFFu) env->ungetc_((int32_t)ch, stream);

LAB_6fab6f9e:
    if (ch == 0xFFFFFFFFu && nmatch == 0 && _char == 0) return -1;
    return nmatch;
}
