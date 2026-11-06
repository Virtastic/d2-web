// 1.13c D2Win+0x06B79 -- MSVC CRT flush-and-commit of one FILE (fflush body). First port.
// flush(f) != 0 returns -1; else when f->_flag has _IOCOMMIT (0x4000) the descriptor f->_file is committed and a non-zero commit result returns -1; else 0.
// The retail callees (__flush at D2Win+0x6B1C, the commit body at D2Win+0x7268) are seams.
#include <cstdint>
struct d2winCrtFile { uint32_t ptr, cnt, base, flag, file; };
extern "C" int32_t d2win_flushcommit_6b79(d2winCrtFile* f, int32_t (*flush)(d2winCrtFile*), uint32_t (*commit)(uint32_t)) {
    if (flush(f) != 0) return -1;
    if ((f->flag & 0x4000) != 0) return -(int32_t)(commit(f->file) != 0);
    return 0;
}
