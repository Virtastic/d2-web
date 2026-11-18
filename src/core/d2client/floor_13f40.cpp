#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x13F40 (register ABI: EAX=cell struct, ESI=x, EDI=y).
// act = [0x6FBCC3B8]; act==0 is the Fog-error + _exit(-1) path (returns 0, no call). Otherwise the stdcall thunk 0xD018 (D2Common #10833)
// is called with act; r is its result. r==0 skips the 0x13CE0 call (returns 2). Else out = the six 13CE0 args {r, 1, y', x', edi, esi}, returns 1.
// eax3c/eax40 = [EAX+0x3C]/[EAX+0x40]. D2Common #11087 (subtile->client) is inlined.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_13f40(uint32_t act, int32_t eax3c, int32_t eax40, int32_t esi, int32_t edi, uint32_t r, uint32_t* out) {
    if (!act) return 0;
    if (!r) return 2;
    const int32_t sx = esi + 5;
    out[0] = r;
    out[1] = 1;
    out[2] = (uint32_t)(16 * (edi - sx) + eax3c);
    out[3] = (uint32_t)(8 * (edi + sx) + 0x28 + eax40);
    out[4] = (uint32_t)edi;
    out[5] = (uint32_t)esi;
    return 1;
}
