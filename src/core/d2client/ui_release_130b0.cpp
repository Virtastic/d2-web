#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x130B0: UI pointer-release dispatch and clear.
// Decision logic only; the callees run through cb(op, arg): 0 = 0x112E0(param), 1 = 0x12EB0(), 2 = 0x88A70(0,0,0,0),
// 3 = 0x11180(eax live-in), 4 = handler[k] (0x1A-byte stride table at D2Client+0xF8B9A).
// st = the 10 dwords D2Client+0x11C50C..0x11C528 (press record, 8 dwords; [1] = flag A, [7] = flag B), then
// 0x11C538 (table index), 0x11C53C (k). tbl = the id table (10-byte stride, first dword = id).
typedef uint32_t (*Cb130B0)(uint32_t op, uint32_t arg);
static_assert(sizeof(uint32_t) == 4, "dword");
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_130b0(uint32_t eax, uint32_t param, uint32_t* st, const uint8_t* tbl, Cb130B0 cb) {
    const uint32_t k = st[9];
    if (st[1] && cb(0, param) && *(const uint32_t*)(tbl + st[8] * 10) != 0x39) { cb(1, 0); cb(2, 0); }
    if (st[7] && k != 3 && cb(3, eax) == k) { cb(4, k); cb(2, 0); }
    for (int i = 0; i < 8; ++i) st[i] = 0;
}
