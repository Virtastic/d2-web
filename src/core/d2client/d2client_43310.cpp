// 1.13c D2Client+0x43310 -- the key-binding scan (no arguments). The 114-entry, 10-byte table at +11A2E0 holds {i32 id, u16 vk, i32 mode}.
// Binding (id 0x24, mode 1) first, then (0x24, mode 0): its vk (0xFFFF = unbound) goes to GetAsyncKeyState (user32 IAT, via the callback);
// a set bit 15 sets [11C3F0] = 1, no match at all clears it. Mode 1 down returns at once; mode 1 up / unbound falls to the mode 0 scan.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
namespace {
int find43310(const uint8_t* tbl, uint32_t id, uint32_t mode) {
    for (int i = 0; i < 114; ++i) { uint32_t a, b; std::memcpy(&a, tbl + i * 10, 4); std::memcpy(&b, tbl + i * 10 + 6, 4); if (a == id && b == mode) return i; }
    return -1;
}
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_43310(const uint8_t* tbl, uint32_t* g11c3f0, uint32_t (*getAsyncKeyState)(uint32_t)) {
    int i = find43310(tbl, 0x24, 1);
    if (i >= 0) {
        uint16_t vk; std::memcpy(&vk, tbl + i * 10 + 4, 2);
        if (vk != 0xFFFF && (getAsyncKeyState(vk) & 0x8000)) { *g11c3f0 = 1; return; }
    }
    i = find43310(tbl, 0x24, 0);
    if (i < 0) { *g11c3f0 = 0; return; }
    uint16_t vk; std::memcpy(&vk, tbl + i * 10 + 4, 2);
    if (vk == 0xFFFF) { *g11c3f0 = 0; return; }
    *g11c3f0 = (getAsyncKeyState(vk) & 0x8000) ? 1 : 0;
}
