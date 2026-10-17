// 1.13c D2Game+0x4352 (_write_char) / +0x43A9 (_write_string): MSVC 2003 CRT printf output helpers.
// FILE is the 32-bit CRT layout {ptr, cnt, base, flag}; *pCount is printf's running count (-1 = error).
// ponytail: the cnt<0 __flsbuf flush path is not modelled; callers keep the buffer non-empty.
#include <cstdint>
struct CrtFile32 { uint32_t ptr; int32_t cnt; uint32_t base; uint32_t flag; };
extern "C" void d2game_write_char(uint8_t ch, CrtFile32* f, int32_t* pCount) {   // 1.13c D2Game+0x4352
    if (!(f->flag & 0x40) || f->base) {
        f->cnt--;
        *(uint8_t*)(uintptr_t)f->ptr = ch; f->ptr++;   // cnt>=0 only
    }
    *pCount += 1;
}
extern "C" void d2game_write_string(int32_t* pCount, const uint8_t* s, CrtFile32* f, int32_t len) {   // 1.13c D2Game+0x43A9
    if (!(f->flag & 0x40) || f->base) {
        for (; len > 0; --len) { d2game_write_char(*s++, f, pCount); if (*pCount == -1) break; }   // the -1 check follows each char
    } else *pCount += len;
}
