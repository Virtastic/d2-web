// 1.13c D2Net+0x65D0 (ordinal 10006): packet-length resolver over the 0x71-entry size table at
// D2Net+0xABD8. Table values come from the retail vector.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace {
const int32_t kSize[0x71] = {
    0,5,9,5,9,5,9,9,5,9,9,1,5,9,9,5,9,9,1,9,-1,-1,13,5,17,5,9,9,3,9,9,17,13,9,5,9,5,9,13,9,
    9,9,9,0,0,1,3,9,9,9,17,17,5,17,9,5,13,5,3,3,9,5,5,3,1,1,1,1,17,9,13,13,1,9,0,9,5,3,0,7,
    9,9,5,1,1,0,0,0,3,17,0,0,0,7,6,5,1,3,5,5,0,0,-1,46,37,1,1,1,-1,13,1,0,1};
}

extern "C" EMSCRIPTEN_KEEPALIVE int d2_realm_net_65d0(const uint8_t* p, uint32_t len, int32_t* out) {
    if (len == 0) return 0;
    const uint8_t op = p[0];
    if (op == 0xff) { *out = 0x10; return 1; }
    if (op >= 0x71) return 0;
    int32_t e = *out = kSize[op];
    if (e == 0) return 0;
    if (e > 0) return e;
    switch (op) {
    case 0x14: case 0x15: {   // two NUL-terminated strings, then a signed byte count
        if (len <= 2) return 0;
        const uint8_t* s = p + 3;
        int a = (int)strlen((const char*)s);
        if (a + 4 > (int)len) return 0;
        s += a + 1;
        int t = a + 5 + (int)strlen((const char*)s);
        if (t > (int)len) return 0;
        int r = t + 1 + (int8_t)s[strlen((const char*)s) + 1];
        if (r > (int)len) return 0;
        return *out = r;
    }
    case 0x66: {
        if (len <= 2) return 0;
        uint32_t v = p[1] | (p[2] << 8);
        if (v > 0x1fd) v = 0;
        return *out = (int)v + 3;
    }
    case 0x6c:
        if (len <= 5) return 0;
        return *out = p[1] + 7;
    default:
        *out = 0;
        return 0;
    }
}
