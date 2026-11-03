// 1.13c D2Client+0x72970 probe: r[0]=unit blob (ECX) or 0. The port reads native (8-byte) pointers, so the probe builds a native unit/path
// pair around the blob's 32-bit fields: unit+0x54 is the path, path+0x30 the flags block (the blob itself, cleared and flagged in place).
#include <emscripten.h>
#include <cstdint>
#include <cstring>
extern "C" void d2_client_72970(void* unit);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_72970(uint32_t* r) {
    if (!r[0]) { d2_client_72970(nullptr); r[4] = 0; return; }
    uint8_t unit[0x60] = {}; uint8_t path[0x40] = {};
    uint32_t p, f;
    memcpy(&p, (const uint8_t*)(uintptr_t)r[0] + 0x54, 4);
    if (p) {
        memcpy(&f, (const uint8_t*)(uintptr_t)p + 0x30, 4);
        void* fp = (void*)(uintptr_t)f; memcpy(path + 0x30, &fp, sizeof fp);
        void* pp = path; memcpy(unit + 0x54, &pp, sizeof pp);
    }
    d2_client_72970(unit);
    r[4] = 0;
}
