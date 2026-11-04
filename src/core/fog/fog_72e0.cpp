// Prior art used: MSVC VC7.1 CRT __fflush_lk (decompile cache, 46 bytes); FILE layout +0 _ptr, +4 _cnt, +8 _base, +0xC _flag, +0x10 _file.
#include <cstdint>

struct Fog72E0File {
    uint32_t ptr;
    uint32_t cnt;
    uint32_t base;
    uint32_t flag;
    int32_t file;
};

// 1.13c Fog+0x072E0: __fflush_lk(FILE*). The flush (Fog+0x7283) failing returns -1. Otherwise, when _flag has
// 0x4000 (commit mode), commit _file (Fog+0xA4D8) and return -(commit != 0); else return 0.
int32_t fog_72e0_fflush_lk(const Fog72E0File* f, int32_t (*flush)(const Fog72E0File*), int32_t (*commit)(int32_t fd))
{
    if (flush(f) != 0)
        return -1;
    if ((f->flag & 0x4000) != 0)
        return -(commit(f->file) != 0);
    return 0;
}
