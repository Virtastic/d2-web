// src/core/d2game/d2game_crt_filbuf.cpp -- 1.13c D2Game.dll row 0x0500C __filbuf, from the cached
// decompile build-wasm/ghidra-d2game/decomp/0x500C.c. Alpha lane (OBJECTIVE v8.3): implementation +
// marker only, no spec, no vector.
//
// Same VS2003 Release CRT leaf as D2Client 0x080C5 (src/core/d2client/d2client_batch_03f82.cpp): the
// refill behind getc. Host model follows src/core/d2game/d2game_batch_81b6.cpp (32-bit VS2003 FILE) and
// d2game_crt_write.cpp (the _pioinfo handle table, records of 0x24 bytes, ioinfo.osfile at record+4).
// Callees 0x081B6 __getbuf and 0x052CA __read are seams after the retail argument; 0x081B6 is mapped
// in-tree but is still taken through the seam so this leaf matches the host's CRT instance.
#include <cstdint>
#include <emscripten.h>

// MSVC 2003 32-bit FILE; only the fields __filbuf touches.
struct D2GameCrtFilbufFile {
    uint32_t _ptr;      // 0x00
    int32_t  _cnt;      // 0x04
    uint32_t _base;     // 0x08
    int32_t  _flag;     // 0x0C
    int32_t  _file;     // 0x10
    int32_t  _charbuf;  // 0x14
    int32_t  _bufsiz;   // 0x18
    uint32_t _tmpfname; // 0x1C
};

struct D2GameCrtFilbufSeams {
    int32_t (*read)(int32_t, void*, uint32_t);   // 0x052CA __read
    void    (*getbuf)(void*);                    // 0x081B6 __getbuf
    uint8_t** pioinfo;                           // 0x6FD3A220
    uint8_t*  consoleRecord;                     // 0x6FD1F730, the fd -1 record
};

// 1.13c D2Game+0x0500C
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_500c(D2GameCrtFilbufFile* f, const D2GameCrtFilbufSeams* s) {
    const uint32_t flag = (uint32_t)f->_flag;
    if ((flag & 0x83) == 0 || (flag & 0x40) != 0) return -1;
    if ((flag & 2) != 0) {
        f->_flag = (int32_t)(flag | 0x20);
        return -1;
    }
    f->_flag = (int32_t)(flag | 1);
    if ((flag & 0x10C) == 0) s->getbuf(f);
    else f->_ptr = f->_base;
    const int32_t n = s->read(f->_file, (void*)(uintptr_t)f->_base, (uint32_t)f->_bufsiz);
    f->_cnt = n;
    if (n != 0 && n != -1) {
        if ((f->_flag & 0x82) == 0) {
            const uint32_t fd = (uint32_t)f->_file;
            const uint8_t* rec = fd == 0xffffffffu
                                     ? s->consoleRecord
                                     : s->pioinfo[fd >> 5] + (fd & 0x1f) * 0x24;
            if ((rec[4] & 0x82) == 0x82) f->_flag |= 0x2000;
        }
        if (f->_bufsiz == 0x200 && (f->_flag & 8) != 0 && (f->_flag & 0x400) == 0)
            f->_bufsiz = 0x1000;
        f->_cnt = n - 1;
        const uint8_t byte = *(const uint8_t*)(uintptr_t)f->_ptr;
        f->_ptr += 1;
        return (int32_t)byte;
    }
    f->_flag |= n != 0 ? 0x20 : 0x10;
    f->_cnt = 0;
    return -1;
}
