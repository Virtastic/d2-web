// 1.13c D2Win+0x4202 (MSVC CRT __dosmaperr): store the Win32 error in _tdoserrno and map it to errno.
#include <cstdint>
#include <emscripten.h>

// table = 0x2d pairs {doserr, errno} (retail 0x6F8FF700); tdoserrno/terrno = the thread data fields.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_dosmaperr_4202(uint32_t err, const uint32_t* table, uint32_t* tdoserrno, int32_t* terrno) {
    *tdoserrno = err;
    for (uint32_t i = 0; i < 0x2d; i++)
        if (err == table[i * 2]) { *terrno = (int32_t)table[i * 2 + 1]; return; }
    if (err > 0x12 && err < 0x25) *terrno = 0xd;          // EACCES
    else if (err > 0xbb && err < 0xcb) *terrno = 8;       // ENOEXEC
    else *terrno = 0x16;                                  // EINVAL
}
