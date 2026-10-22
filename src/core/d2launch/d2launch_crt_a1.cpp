// src/core/d2launch/d2launch_crt_a1.cpp -- statically linked MSVC CRT leaves of 1.13c D2Launch.dll
// (alpha lane, implementation + marker only). Register arguments are modelled as explicit parameters.
#include <cstdint>
#include <cstddef>

// MSVC 7.1 FILE layout (32-bit).
struct LaunchCrtFile {
    uint8_t* ptr; int32_t cnt; uint8_t* base; int32_t flag; int32_t file; int32_t charbuf; int32_t bufsiz; char* tmpfname;
};
// 1.13c D2Launch+0x01B9E (__flsbuf) and +0x03CA1 (__filbuf): not yet mapped.
extern "C" __attribute__((weak)) int32_t d2launch_flsbuf(int32_t ch, LaunchCrtFile* f);
extern "C" __attribute__((weak)) int32_t d2launch_filbuf(LaunchCrtFile* f);

// 1.13c D2Launch+0x03350 -- _strlen
extern "C" size_t d2launch_strlen(const char* s) {
    size_t n = 0;
    while (s[n]) ++n;
    return n;
}

// 1.13c D2Launch+0x02501 -- __inc (fastcall, FILE* in EDX): getc
extern "C" uint32_t d2launch_inc(LaunchCrtFile* f) {
    if (--f->cnt >= 0) return *f->ptr++;
    return (uint32_t)d2launch_filbuf(f);
}

// 1.13c D2Launch+0x01CB7 -- _write_char (AL = ch, ECX = FILE*, ESI = &charsout)
extern "C" void d2launch_write_char(uint8_t ch, LaunchCrtFile* f, int32_t* charsout) {
    if ((f->flag & 0x40) == 0 || f->base != nullptr) {
        uint32_t r;
        if (--f->cnt < 0) {
            r = (uint32_t)d2launch_flsbuf((int8_t)ch, f);
        } else {
            *f->ptr++ = ch;
            r = ch;
        }
        if (r == 0xffffffffu) { *charsout = -1; return; }
    }
    ++*charsout;
}

// 1.13c D2Launch+0x01CEA -- _write_multi_char (EAX = &charsout; same AL/ECX/ESI context as write_char)
extern "C" void d2launch_write_multi_char(uint8_t ch, LaunchCrtFile* f, int32_t* charsout, int32_t num) {
    do {
        if (num < 1) return;
        --num;
        d2launch_write_char(ch, f, charsout);
    } while (*charsout != -1);
}
