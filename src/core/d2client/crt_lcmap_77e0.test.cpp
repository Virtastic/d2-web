// Host test: mkdir inc; echo '#define EMSCRIPTEN_KEEPALIVE' > inc/emscripten.h
//   g++ -Iinc -o /tmp/t src/core/d2client/crt_lcmap_77e0.test.cpp && /tmp/t
#include "crt_lcmap_77e0.cpp"
#include <cassert>
static int w_ok = 1, wcalls;
static int32_t lw(uint32_t, uint32_t f, const uint16_t* s, int32_t n, uint16_t* d, int32_t nd) {
    wcalls++;
    if (!w_ok) return 0;
    if (d) for (int i = 0; i < n && i < nd; i++) d[i] = (f & 0x200) && s[i] >= 'a' && s[i] <= 'z' ? s[i] - 32 : s[i];
    return n;
}
static int32_t mb2w(uint32_t, uint32_t, const char* s, int32_t n, uint16_t* d, int32_t) {
    if (d) for (int i = 0; i < n; i++) d[i] = (uint8_t)s[i];
    return n;
}
static int32_t w2mb(uint32_t, uint32_t, const uint16_t* s, int32_t n, char* d, int32_t nd, const char*, int32_t*) {
    if (d) for (int i = 0; i < n && i < nd; i++) d[i] = (char)s[i];
    return n;
}
static uint32_t gle() { return 0; }
int main() {
    D2cLcmapStringAEnv e = {lw, nullptr, mb2w, w2mb, gle, nullptr, nullptr};
    char out[8] = {};
    // upper-case "ab\0cd" with cch 5: length folds to 2 at the NUL
    assert(d2c_lcmap_string_a(0, 0x200, "ab\0cd", 5, out, 8, 1252, 0, &e) == 2);
    assert(out[0] == 'A' && out[1] == 'B' && out[2] == 0);
    assert(g_d2c_lcmap_api == 1);
    // size query (cch_dst 0) returns the length without writing
    assert(d2c_lcmap_string_a(0, 0x200, "xyz", 3, nullptr, 0, 1252, 0, &e) == 3);
    // detection is cached: no probe call when api already known; failing W maps to 0
    w_ok = 0; wcalls = 0;
    assert(d2c_lcmap_string_a(0, 0x200, "xyz", 3, out, 8, 1252, 0, &e) == 0 && wcalls == 1);
    return 0;
}
