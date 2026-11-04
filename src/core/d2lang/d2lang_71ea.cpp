// Prior art used: MSVC VC7.1 CRT _flush; FILE layout +0 _ptr, +4 _cnt, +8 _base, +0xC _flag, +0x10 _file.
#include <cstdint>

struct D2Lang71EAFile {
    uint32_t ptr;
    uint32_t cnt;
    uint32_t base;
    uint32_t flag;
    int32_t file;
};

// 1.13c D2Lang+0x071EA: flush of a write-mode stream. When (flag & 3) == 2 and (flag & 0x108) != 0 and
// ptr > base, the bytes [base, ptr) are written through write (retail __write, Fog-side seam). A full write
// clears _IOWRT's low bit 0x2 when the flag's sign bit is set; a short write sets 0x20 and returns -1.
// Then cnt = 0 and ptr = base. Returns 0 otherwise.
int32_t d2lang_71ea(D2Lang71EAFile* s, uint32_t (*write)(int32_t fd, uint32_t buf, uint32_t n))
{
    int32_t ret = 0;
    if ((s->flag & 3) == 2 && (s->flag & 0x108) != 0) {
        uint32_t n = s->ptr - s->base;
        if ((int32_t)n > 0) {
            if (write(s->file, s->base, n) == n) {
                if ((int8_t)(uint8_t)(s->flag & 0xFF) < 0)
                    s->flag &= ~0x2u;
            } else {
                s->flag |= 0x20;
                ret = -1;
            }
        }
    }
    s->cnt = 0;
    s->ptr = s->base;
    return ret;
}
