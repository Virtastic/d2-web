// 1.13c Storm+0x0E3C7 -- MSVC CRT flush-and-commit of one FILE (fflush body). First port.
// flush(f) != 0 returns -1; else when f->_flag has _IOCOMMIT (0x4000) the descriptor f->_file is committed and a non-zero commit result returns -1; else 0.
// The retail callees (__flush at Storm+0xE36A, the commit body at Storm+0xEA01) are seams.
#include <cstdint>
struct stormCrtFile { uint32_t ptr, cnt, base, flag, file; };
extern "C" int32_t storm_flushcommit_e3c7(stormCrtFile* f, int32_t (*flush)(stormCrtFile*), uint32_t (*commit)(uint32_t)) {
    if (flush(f) != 0) return -1;
    if ((f->flag & 0x4000) != 0) return -(int32_t)(commit(f->file) != 0);
    return 0;
}
