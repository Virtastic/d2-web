// D2Gfx batch b67: small CRT-derived helpers. Skipped: SEH/Win32 rows (5CD4, C138, C96A, C986, C9A2, CA00).
#include <cstdint>
#include <cstddef>
#include <initializer_list>

namespace {
struct CrtFile { // MSVC 7.1 FILE layout (x86): ptr, cnt, base, flag
    char* ptr; int32_t cnt; char* base; int32_t flag;
};
}
extern "C" {
void d2gfx_fn_19bc(int32_t code);                          // D2Gfx+0x19BC
uint32_t d2gfx_flsbuf(int32_t ch, void* file);             // __flsbuf
void d2gfx_fn_360c(int32_t code);                          // D2Gfx+0x360C
int32_t d2gfx_raise(int32_t sig);                          // _raise, D2Gfx+0xCA00
void d2gfx_exit(int32_t code);                             // __exit
void d2gfx_report_failure();                               // D2Gfx+0x4262
extern uint32_t d2gfx_security_cookie;                     // DAT_6fa906e0
}

// 1.13c D2Gfx+0x13BD
void d2gfx_sub_13BD() { d2gfx_fn_19bc(8); }

// 1.13c D2Gfx+0x1D9A  (hidden regs: AL=ch, ECX=file, ESI=count)
void d2gfx_write_char(uint8_t ch, CrtFile* file, int32_t* count) {
    if ((file->flag & 0x40) == 0 || file->base != nullptr) {
        uint32_t r;
        if (--file->cnt < 0) {
            r = d2gfx_flsbuf((char)ch, file);
        } else {
            *file->ptr++ = (char)ch;
            r = ch;
        }
        if (r == 0xffffffffu) { *count = -1; return; }
    }
    ++*count;
}

// 1.13c D2Gfx+0x1DCD  (hidden regs: EAX=count, plus write_char's ch/file; ch is passed through)
void d2gfx_write_multi_char(uint8_t ch, CrtFile* file, int32_t* count, int32_t n) {
    while (n >= 1) {
        --n;
        d2gfx_write_char(ch, file, count);
        if (*count == -1) return;
    }
}

// 1.13c D2Gfx+0x1DF1  (hidden regs: EAX=count, EDI=file, plus ch/ptr per char; `s` is the string walked)
void d2gfx_write_string(const char* s, CrtFile* file, int32_t* count, int32_t n) {
    if ((file->flag & 0x40) == 0 || file->base != nullptr) {
        while (n >= 1) {
            --n;
            d2gfx_write_char((uint8_t)*s++, file, count);
            if (*count == -1) return;
        }
    } else {
        *count += n;
    }
}

// 1.13c D2Gfx+0x4150
size_t d2gfx_strlen(const char* s) {
    size_t n = 0;
    while (s[n]) ++n;
    return n;
}

// 1.13c D2Gfx+0x4293  (__fastcall: ECX=cookie)
void d2gfx_security_check_cookie(uint32_t cookie) {
    if (cookie == d2gfx_security_cookie) return;
    d2gfx_report_failure();
}

// 1.13c D2Gfx+0xC9BA
void d2gfx_abort() {
    d2gfx_fn_360c(10);
    d2gfx_raise(0x16);
    d2gfx_exit(3);
}

// 1.13c D2Gfx+0xC9D2  (hidden regs: EDX=table start, ESI=signal; returns entry or end; `tableCount` = DAT_6fa90224)
const uint32_t* d2gfx_siglookup(const uint32_t* table, int32_t sig, uint32_t tableCount) {
    const uint32_t* p = table;
    const uint32_t* end = table + tableCount * 3;
    do {
        if ((int32_t)p[1] == sig) return p;
        p += 3;
    } while (p < end);
    return p;
}
