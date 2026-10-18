// src/core/d2client/d2client_batch_4c8f.cpp -- 1.13c D2Client.dll row 0x04C8F: _output, the MSVC 2003
// Release narrow-format engine behind the DLL's sprintf family (caller 0x3146 _sprintf). It parses a
// narrow format string and writes through the FILE* it is handed, accumulating the emitted character
// count (which goes negative, -1, on a write error); the first NUL byte or a negative count ends it.
// The parse is the retail state machine: flags -> width -> precision -> size -> conversion, with
// %c/%C, %s/%S (narrow unless 'l'/'w'), %d/%i/%u/%o/%x/%X/%p, %Z (a length-prefixed counted string),
// %n, and %e/%E/%f/%g/%G through the +0x0DB8E8 float hooks. Unrecovered CRT callees are host seams,
// after the retail arguments: 0x4C01 write_char, 0x4C34 write_multi_char, 0x4C58 write_string,
// 0xA024 _wctomb, 0x7D5F _malloc, 0x4891 _free, 0x6FF0 _strlen, 0xBB2B __fptrap, and the three float
// hook slots +0x0DB8E8 / +0x0DB8F4 / +0x0DB8EC.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

// The 32-bit (MSVC 2003) FILE object, VS2003 layout (d2client_batch_0312a.cpp).
struct CrtFile {
    uint32_t _ptr;      // 0x00
    int32_t  _cnt;      // 0x04
    uint32_t _base;     // 0x08
    int32_t  _flag;     // 0x0C
    int32_t  _file;     // 0x10
    int32_t  _charbuf;  // 0x14
    int32_t  _bufsiz;   // 0x18
    uint32_t _tmpfname; // 0x1C
};
static_assert(offsetof(CrtFile, _ptr) == 0x00);
static_assert(offsetof(CrtFile, _cnt) == 0x04);
static_assert(offsetof(CrtFile, _base) == 0x08);
static_assert(offsetof(CrtFile, _flag) == 0x0C);

// +0x0DB900 _pctype: two bytes per byte; the high bit of the odd byte marks an MBCS lead byte. The
// loader fills it from the locale; zero here means every byte stands alone (the ASCII case).
static uint8_t g_pctype[0x100 * 2];

// Host seams for the callees this row does not implement (address = retail RVA). The last three are
// the function-pointer slots at +0x0DB8E8/+0x0DB8F4/+0x0DB8EC, __fptrap until the FP engine is in.
struct D2ClientBatch4C8FEnv {
    void     (*write_char)(CrtFile*, uint8_t, int32_t*);                               // 0x04C01
    void     (*write_multi_char)(CrtFile*, uint8_t, int32_t, int32_t*);                // 0x04C34
    void     (*write_string)(CrtFile*, const char*, int32_t, int32_t*);                // 0x04C58
    int32_t  (*wctomb)(char*, uint32_t);                                               // 0x0A024
    void*    (*malloc_)(uint32_t);                                                     // 0x07D5F
    void     (*free_)(void*);                                                          // 0x04891
    int32_t  (*strlen_)(const char*);                                                  // 0x06FF0
    void     (*float_to_str)(const uint32_t*, char*, int32_t, int32_t, int32_t);       // +0x0DB8E8
    void     (*force_decimal)(char*);                                                  // +0x0DB8F4
    void     (*crop_zeros)(char*);                                                     // +0x0DB8EC
};

// 1.13c D2Client+0x04C8F (_output). Flag word bits, as the decompile sets them: 0x1 '+', 0x2 ' ',
// 0x4 '-', 0x8 '0', 0x10 'l', 0x20 'h', 0x40 numeric/signed, 0x80 '#', 0x100 negative, 0x200 octal
// leading zero, 0x800 'w' wide, 0x8000 I64. The vararg cursor walks 32-bit words; a double is two.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_4c8f(CrtFile* stream, const uint8_t* format,
                                            const uint32_t* args,
                                            const D2ClientBatch4C8FEnv* env) {
    int32_t count = 0;                 // local_228
    while (*format != 0 && count >= 0) {
        uint8_t ch = *format;
        if (ch != '%') {
            // Literal byte. An MBCS lead byte pulls its trail byte in the same pass.
            env->write_char(stream, ch, &count);
            if ((g_pctype[ch * 2 + 1] & 0x80) != 0) {
                env->write_char(stream, format[1], &count);
                format += 2;
            } else {
                format += 1;
            }
            continue;
        }
        format++;                      // state 1: after '%'
        uint32_t flags = 0;            // local_214
        int32_t width = 0;             // local_23c
        int32_t prec = -1;             // local_21c
        uint32_t radix = 0;            // local_224
        int32_t digit_bias = 0;        // local_24c
        int32_t pfxlen = 0;            // local_230
        char pfx[2] = {0, 0};          // local_22c / local_22b
        bool suppress = false;         // local_248

        // Flags (state 2).
        for (;;) {
            ch = *format;
            if (ch == ' ') flags |= 0x2;
            else if (ch == '#') flags |= 0x80;
            else if (ch == '+') flags |= 0x1;
            else if (ch == '-') flags |= 0x4;
            else if (ch == '0') flags |= 0x8;
            else break;
            format++;
        }
        // Width (state 3).
        ch = *format;
        if (ch == '*') {
            width = (int32_t)args[0];
            args++;
            if (width < 0) { flags |= 0x4; width = -width; }
            format++;
        } else {
            while ((ch = *format) >= '0' && ch <= '9') {
                width = width * 10 + (int32_t)(ch - '0');
                format++;
            }
        }
        // Precision (states 4 and 5).
        if (*format == '.') {
            format++;
            prec = 0;
            if (*format == '*') {
                prec = (int32_t)args[0];
                args++;
                if (prec < 0) prec = -1;
                format++;
            } else {
                while ((ch = *format) >= '0' && ch <= '9') {
                    prec = prec * 10 + (int32_t)(ch - '0');
                    format++;
                }
            }
        }
        // Size (state 6): h/l/w, and I64/I32 (a bare I before a conversion is a no-op prefix).
        bool literal_I = false;
        for (;;) {
            ch = *format;
            if (ch == 'I') {
                if (format[1] == '6' && format[2] == '4') { format += 3; flags |= 0x8000u; }
                else if (format[1] == '3' && format[2] == '2') { format += 3; flags &= ~0x8000u; }
                else if (format[1] == 'd' || format[1] == 'i' || format[1] == 'o' ||
                         format[1] == 'u' || format[1] == 'x' || format[1] == 'X') {
                    format++;
                } else {
                    // Not a size prefix: the state machine falls back to the literal path for 'I'.
                    env->write_char(stream, 'I', &count);
                    format++;
                    literal_I = true;
                    break;
                }
            } else if (ch == 'h') { flags |= 0x20; format++; }
            else if (ch == 'l') { flags |= 0x10; format++; }
            else if (ch == 'w') { flags |= 0x800; format++; }
            else break;
        }
        if (literal_I) continue;
        ch = *format;
        if (ch == 0) break;            // a trailing '%' emits nothing

        // LAB_6fab52f9: sign/prefix, left pad, body, right pad. `wsrc` means `s` points at UTF-16.
        auto emit = [&](const char* s, int32_t n, bool wsrc) {
            if (suppress) return;
            int32_t plen = pfxlen;
            if ((flags & 0x40) != 0) {
                if ((flags & 0x100) != 0) { pfx[0] = '-'; plen = 1; }
                else if ((flags & 0x1) != 0) { pfx[0] = '+'; plen = 1; }
                else if ((flags & 0x2) != 0) { pfx[0] = ' '; plen = 1; }
            }
            int32_t pad = (width - plen) - n;
            if ((flags & 0xC) == 0) env->write_multi_char(stream, ' ', pad, &count);
            if (plen != 0) env->write_string(stream, pfx, plen, &count);
            if ((flags & 0x8) != 0 && (flags & 0x4) == 0)
                env->write_multi_char(stream, '0', pad, &count);
            if (!wsrc || n < 1) {
                env->write_string(stream, s, n, &count);
            } else {
                char mb[8];
                const uint16_t* ws = (const uint16_t*)s;
                for (int32_t i = 0; i < n; i++) {
                    int32_t m = env->wctomb(mb, ws[i]);
                    if (m < 1) break;
                    env->write_string(stream, mb, m, &count);
                }
            }
            if ((flags & 0x4) != 0) env->write_multi_char(stream, ' ', pad, &count);
        };

        switch (ch) {
        case 'C':
        case 'c': {
            if (ch == 'C' && (flags & 0x830) == 0) flags |= 0x800;
            uint32_t v = args[0];
            args++;
            char mb[8];
            int32_t n;
            if ((flags & 0x810) == 0) {
                mb[0] = (char)v;
                n = 1;
            } else {
                n = env->wctomb(mb, (uint16_t)v);   // wchar_t is 16 bits in the 1.13c CRT
                if (n < 0) suppress = true;
            }
            emit(mb, n, false);
            break;
        }
        case 's':
        case 'S': {
            if (ch == 'S' && (flags & 0x830) == 0) flags |= 0x800;
            uint32_t v = args[0];
            args++;
            int32_t limit = (prec == -1) ? 0x7FFFFFFF : prec;
            if ((flags & 0x810) == 0) {
                const uint8_t* p = (const uint8_t*)(uintptr_t)v;
                if (p == nullptr) p = (const uint8_t*)"(null)";
                const uint8_t* q = p;
                while (limit != 0 && (limit--, *q != 0)) q++;
                emit((const char*)p, (int32_t)(q - p), false);
            } else {
                const uint16_t* w = (const uint16_t*)(uintptr_t)v;
                if (w == nullptr) w = (const uint16_t*)(const void*)u"(null)";
                const uint16_t* q = w;
                while (limit != 0 && (limit--, *q != 0)) q++;
                emit((const char*)w, (int32_t)(q - w), true);
            }
            break;
        }
        case 'n': {
            uint32_t addr = args[0];
            args++;
            if ((flags & 0x20) == 0) *(int32_t*)(uintptr_t)addr = count;
            else *(uint16_t*)(uintptr_t)addr = (uint16_t)count;
            suppress = true;
            break;
        }
        case 'Z': {
            // %Z: a counted string, { int16 length; int16 pad; const void* chars; } (8 bytes).
            const uint8_t* z = (const uint8_t*)(uintptr_t)args[0];
            args++;
            uint32_t sp = 0;
            int32_t zlen = -1;
            if (z != nullptr) {
                zlen = (int16_t)((uint16_t)z[0] | ((uint16_t)z[1] << 8));
                sp = (uint32_t)z[4] | ((uint32_t)z[5] << 8) |
                     ((uint32_t)z[6] << 16) | ((uint32_t)z[7] << 24);
            }
            if (z == nullptr || sp == 0) {
                emit("(null)", env->strlen_("(null)"), false);
            } else if ((flags & 0x800) == 0) {
                emit((const char*)(uintptr_t)sp, zlen, false);
            } else {
                emit((const char*)(uintptr_t)sp, zlen / 2, true);
            }
            break;
        }
        case 'e': case 'E': case 'f': case 'g': case 'G': {
            // LAB_6fab4f79. E/G are the upper-case forms: lowercase the conversion, set the scale bit.
            int32_t scale = 0;
            if (ch == 'E' || ch == 'G') { scale = 1; ch = (uint8_t)(ch + 0x20); }
            flags |= 0x40;
            char localbuf[512];        // retail's local_210[22] heads a 512-byte scratch region
            char* buf = localbuf;
            char* heap = nullptr;
            if (prec < 0) {
                prec = 6;
            } else if (prec == 0) {
                if (ch == 'g') prec = 1;
            } else {
                if (prec > 0x200) prec = 0x200;
                if (prec > 0xA3) {
                    heap = (char*)env->malloc_((uint32_t)prec + 0x15D);
                    if (heap == nullptr) prec = 0xA3;
                    else buf = heap;
                }
            }
            uint32_t dbl[2] = {args[0], args[1]};
            args += 2;
            env->float_to_str(dbl, buf, (int32_t)ch, prec, scale);
            if ((flags & 0x80) != 0 && prec == 0) env->force_decimal(buf);
            if (ch == 'g' && (flags & 0x80) == 0) env->crop_zeros(buf);
            const char* s = buf;
            if (*s == '-') { flags |= 0x100; s++; }
            emit(s, env->strlen_(s), false);
            if (heap != nullptr) env->free_(heap);
            break;
        }
        case 'd': case 'i':
        case 'u': case 'o':
        case 'x': case 'X': case 'p': {
            // LAB_6fab4ff0. Radix and prefix per conversion, then the 64-bit divide loop.
            if (ch == 'd' || ch == 'i') { flags |= 0x40; radix = 10; }
            else if (ch == 'u') radix = 10;
            else if (ch == 'o') { radix = 8; if ((flags & 0x80) != 0) flags |= 0x200; }
            else {
                if (ch == 'x') digit_bias = 0x27;
                else digit_bias = 0x7;             // 'X' and 'p'
                radix = 0x10;
                if (ch == 'p') prec = 8;
                if ((flags & 0x80) != 0) {
                    pfx[0] = '0';
                    pfx[1] = (char)(digit_bias + 'Q');   // 'x' for 0x27, 'X' for 0x7
                    pfxlen = 2;
                }
            }
            uint32_t lo, hi;
            if ((flags & 0x8000) == 0) {
                uint32_t v = args[0];
                args++;
                if ((flags & 0x20) == 0) {               // full word
                    lo = v;
                    hi = ((flags & 0x40) != 0) ? (uint32_t)((int32_t)v >> 31) : 0u;
                } else if ((flags & 0x40) == 0) {        // unsigned short
                    lo = v & 0xFFFFu;
                    hi = 0;
                } else {                                 // signed short, sign-extended
                    lo = (uint32_t)(int32_t)(int16_t)(uint16_t)v;
                    hi = (uint32_t)(((int32_t)(int16_t)(uint16_t)v) >> 31);
                }
            } else {
                lo = args[0];
                hi = args[1];
                args += 2;
            }
            if ((flags & 0x40) != 0 && (int32_t)hi < 0) { // negative: 64-bit two's-complement negate
                uint32_t nlo = 0u - lo;
                hi = (uint32_t)(0u - hi) - (lo != 0 ? 1u : 0u);
                lo = nlo;
                flags |= 0x100;
            }
            if ((flags & 0x8000) == 0) hi = 0;
            if (prec < 0) {
                prec = 1;
            } else {
                flags &= ~0x8u;                          // an explicit precision defeats '0'
                if (prec > 0x200) prec = 0x200;
            }
            if (lo == 0 && hi == 0) pfxlen = 0;
            char digits[544];
            char* end = digits + 543;
            char* p = end;
            for (;;) {
                int32_t next = prec - 1;
                if (prec < 1 && lo == 0 && hi == 0) break;
                prec = next;
                uint64_t cur = ((uint64_t)hi << 32) | lo;
                lo = (uint32_t)(cur / radix);
                hi = (uint32_t)((cur / radix) >> 32);
                int32_t d = (int32_t)(cur % radix) + '0';
                if (d > 0x39) d += digit_bias;
                *p = (char)d;
                p--;
            }
            int32_t n = (int32_t)(end - p);
            const char* s = p + 1;
            if ((flags & 0x200) != 0 && (*s != '0' || n == 0)) {   // %#o wants a leading zero
                *p = '0';
                n = (int32_t)(end - p) + 1;
                s = p;
            }
            emit(s, n, false);
            break;
        }
        default:
            // Not a conversion character: the state machine's literal path writes the byte itself.
            env->write_char(stream, ch, &count);
            break;
        }
        format++;
    }
    return count;
}
