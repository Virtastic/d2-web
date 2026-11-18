// BNClient 0xD280 (ECX = name, EDI = FILETIME*; stack data, len; ret 8): creates the file (GENERIC_WRITE, CREATE_ALWAYS,
// FILE_ATTRIBUTE_NORMAL); -1 is returned as is. Otherwise write(data, len), SetFileTime(h, ft, ft, ft) and the close's
// result is returned (EAX is left from Fog 10109).
#include "core/bnclient/bnclient_file_seams.h"
#include <emscripten.h>

// 1.13c BNClient+0x0D280
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_write_file_time_d280(const char* name, const uint32_t* ft, const uint8_t* data, uint32_t len,
                                                                     const BnFileSeams* s) {
    uint32_t written = 0;
    const uint32_t h = s->open(name, 0x40000000u, 0, 0, 2, 0x80, 0);
    if (h == 0xFFFFFFFFu) return h;
    s->write(h, data, len, &written, 0);
    s->set_time(h, ft, ft, ft);
    return s->close(h);
}
