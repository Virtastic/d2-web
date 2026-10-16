#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x12DD0: UI mode toggle. Decision logic only; the six callees and
// the 0x11d-dword copy are performed by the caller from the returned event list.
// Events (in order): 1..8 = call 0x11410, 0x3A3A0, 0x3CC60(+ord10048,&6FBA51BC), 0x429C0, 0x3CCC0(+same),
// 0x42880, 0x11630, copy 0x11d dwords 6FBCA2E0->6FBC9E68; 0x100|g<<4|v = write v to global g
// (g: 0 434, 1 flag 6FBD349C, 2 2F8, 3 2F4, 4 534, 5 538). Returns the event count; out holds up to 16.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_12dd0(uint32_t open, uint32_t gate, uint32_t flag, uint32_t* out) {
    uint32_t n = 0;
    auto E = [&](uint32_t e) { out[n++] = e; };
    E(1);
    if (open) {
        if (!gate) {
            E(8); E(2); E(0x100 | 0 << 4 | 0);
            if (flag == 1) { E(0x100 | 1 << 4 | 0); E(3); }
            E(4); E(0x100 | 2 << 4 | 1); E(0x100 | 3 << 4 | 0);
        }
        E(0x100 | 4 << 4 | 0); E(0x100 | 5 << 4 | 0);
    } else if (!gate) {
        E(0x100 | 0 << 4 | 1);
        if (flag == 0) { E(0x100 | 1 << 4 | 1); E(5); }
        E(6); E(0x100 | 2 << 4 | 0); E(0x100 | 3 << 4 | 0);
    }
    E(7);
    return n;
}
