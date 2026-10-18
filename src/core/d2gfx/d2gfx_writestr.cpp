// 1.13c D2Gfx CRT leaves recovered from the 1.13c decompile (base 0x6FA80000), product side.
// The character helper 0x1D9A is already implemented in src/core/fn/crt_leaves_b19.cpp; this file only
// adds the functions the function list still had UNMAPPED.
#include <cstdint>
#include <cstdlib>

// The 1.13c FILE (32-bit CRT): ptr 0x0, cnt 0x4, base 0x8, flag 0xC, file 0x10, charbuf 0x14,
// bufsiz 0x18, tmpfname 0x1C. Same definition as src/core/fn/crt_leaves_b19.cpp (one class, two TUs).
struct CrtStream32 { uint32_t ptr; int32_t cnt; uint32_t base; uint32_t flag; int32_t file; uint32_t charbuf; int32_t bufsiz; uint32_t tmpfname; };
static_assert(__builtin_offsetof(CrtStream32, base) == 0x8 && __builtin_offsetof(CrtStream32, flag) == 0xC &&
              sizeof(CrtStream32) == 32, "1.13c MSVC 2003 FILE layout");

// 1.13c D2Gfx+0x01D9A (_write_char), defined in src/core/fn/crt_leaves_b19.cpp.
void d2gfx_write_char(CrtStream32* f, uint8_t ch, int32_t* count);

// 1.13c D2Gfx+0x01DF1 (_write_string): MSVC 2003 Release CRT output.c
// `static void __cdecl write_string(FILE *f, const char *s, int len, int *pcount)` -- the decompile
// takes &count in EAX, the text in ECX, FILE in EDI and len on the stack. If the stream is _IOSTRG
// (0x40) with no buffer, retail only adds len to the count; otherwise it emits len characters through
// _write_char (0x1D9A) and stops after the character that leaves the count at -1 (checked after each
// character, not before the first; len < 1 returns without touching the count).
void d2gfx_write_string(int32_t* pcount, const uint8_t* s, CrtStream32* f, int32_t len) {
    if ((f->flag & 0x40) == 0 || f->base != 0) {
        for (; len > 0; --len) {
            d2gfx_write_char(f, *s++, pcount);
            if (*pcount == -1) break;
        }
    } else {
        *pcount += len;
    }
}
