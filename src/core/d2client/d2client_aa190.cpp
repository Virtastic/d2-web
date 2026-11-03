// 1.13c D2Client+0xAA190 (141 B): release the three handles at DAT_6fbacdc1/c5/c9.
// For each nonzero handle: C18A(h) (1 stack word), then BF86 (ECX = h, EDX = D2Client+0xD6AB0 file string, stack 0x7a, 0), then zero it.
#include <emscripten.h>
#include <cstring>
#include "d2client_a9_env.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_aa190(uint8_t* handles /* 12 bytes at RVA 0xFCDC1 */, const D2cA9Env* e) {
    for (int i = 0; i < 3; ++i) {
        uint32_t h; memcpy(&h, handles + 4 * i, 4);
        if (!h) continue;
        e->call(e->ctx, 0xC18A, 0, 0, 0, 0, h, 0, 0, 0, 0);
        e->call(e->ctx, 0xBF86, 0, h, 0xB2000000u | 0xD6AB0, 0, 0x7a, 0, 0, 0, 0);
        memset(handles + 4 * i, 0, 4);
    }
}
