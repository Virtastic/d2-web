// BNClient 0xCF10 (ECX = name, EAX = reference FILETIME): opens the file for read (Fog 10107: GENERIC_READ, share 3,
// OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN); -1 returns 0. GetFileTime's last-write time minus the reference, as a signed
// 64-bit value; when its magnitude is at most 0x2625A00 (4 s) the result is GetFileSize, else 0. The file is closed either way.
#include "core/bnclient/bnclient_file_seams.h"
#include <emscripten.h>

// 1.13c BNClient+0x0CF10
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_file_time_compare_cf10(const char* name, const uint32_t* ref, const BnFileSeams* s) {
    const uint32_t h = s->open(name, 0x80000000u, 3, 0, 3, 0x8000000u, 0);
    if (h == 0xFFFFFFFFu) return 0;
    uint32_t ft[2] = { 0, 0 };
    s->get_time(h, nullptr, nullptr, ft);
    const int64_t d = (int64_t)(((uint64_t)ft[1] << 32 | ft[0]) - ((uint64_t)ref[1] << 32 | ref[0]));
    const uint64_t mag = d < 0 ? 0 - (uint64_t)d : (uint64_t)d;
    if (mag > 0x2625A00u) { s->close(h); return 0; }
    const uint32_t n = s->size(h, nullptr);
    s->close(h);
    return n;
}
