// src/core/d2win/d2win_batch_b56a.cpp -- 1.13c D2Win.dll alpha batch (no spec). Retail globals/imports arrive via seams.
// Skipped (register args/SEH/cookie): 0xB8B0, 0xCA00, 0x693C, 0x3A6C.
#include <cstdint>
#include <initializer_list>
#include <emscripten.h>

// 1.13c D2Win+0x6c85
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_fptrap(void (*amsg_exit)(int32_t)) { amsg_exit(2); }

// 1.13c D2Win+0x2dbc
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_2dbc(void (*sub_2396)(int32_t)) { sub_2396(4); }

// 1.13c D2Win+0x41f0 (&ptd->_terrno; ptd = __getptd())
extern "C" EMSCRIPTEN_KEEPALIVE int32_t* d2win_sub_41f0(int32_t* ptd_terrno) { return ptd_terrno; }

// 1.13c D2Win+0x41f9 (&ptd->_tdoserrno)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t* d2win_sub_41f9(uint32_t* ptd_tdoserrno) { return ptd_tdoserrno; }

// 1.13c D2Win+0x5C8D
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sbh_find_block(uint32_t list, uint32_t count, uint32_t block,
                                                              const uint32_t* (*base_at)(uint32_t hdr)) {
    for (uint32_t h = list; h < list + count * 0x14; h += 0x14)
        if (block - *base_at(h + 0xC) < 0x100000) return h;
    return 0;
}

// 1.13c D2Win+0x7920 (buf = module path; truncate after last '\\')
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_7920(char* buf, uint32_t (*getModuleFileName)(char*, uint32_t)) {
    getModuleFileName(buf, 0x104);
    buf[0x103] = 0;
    char* p = buf;
    while (*p) {
        char* q = p + 1;
        while (*q && *q != '\\') q++;
        if (!*q) break;
        p = q;
    }
    *p = 0;
}

// 1.13c D2Win+0xBB00 (copy up to 3 chars, space-pad to 4)
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_bb00(char* out, const char* in) {
    int i = 0;
    for (; i < 3 && in[i]; i++) out[i] = in[i];
    for (; i < 4; i++) out[i] = ' ';
}

struct D2WinB56Lseek {
    int32_t (*getOsHandle)(uint32_t fh);                  // __get_osfhandle
    uint32_t (*setFilePointer)(int32_t h, int32_t off, uint32_t whence); // 0xFFFFFFFF on error
    uint32_t (*getLastError)();
    void (*dosmaperr)(uint32_t);
    int32_t* terrno;
    uint8_t* flagByte;                                    // ioinfo[fh].osfile
};

// 1.13c D2Win+0x394D
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_lseek_lk(uint32_t fh, int32_t off, uint32_t whence, const D2WinB56Lseek* e) {
    int32_t h = e->getOsHandle(fh);
    if (h == -1) { *e->terrno = 9; return 0xFFFFFFFF; }
    uint32_t r = e->setFilePointer(h, off, whence);
    uint32_t err = (r == 0xFFFFFFFF) ? e->getLastError() : 0;
    if (!err) { *e->flagByte &= 0xFD; return r; }
    e->dosmaperr(err);
    return 0xFFFFFFFF;
}
