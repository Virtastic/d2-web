// 1.13c D2Client+0x42AD0: reset eight client globals. st[] order: 46c8 46c4 c3dc c3e0 c3f8 c3fc c2f8 c2f4 (module RVAs 0xF46C8 0xF46C4 0x11C3DC 0x11C3E0 0x11C3F8 0x11C3FC 0x11C2F8 0x11C2F4).
#include <emscripten.h>
#include <cstdint>
extern "C" EMSCRIPTEN_KEEPALIVE void D2Client_Reset42AD0(uint32_t* st) {
    st[0] = 0x10; st[1] = 0x10;
    for (int i = 2; i < 8; ++i) st[i] = 0;
}
