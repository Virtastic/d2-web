// D2Client 0x465F0: ESI = a packet; marks the current-unit refresh (caller 0x62E60).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
extern "C" uint32_t* d2_client_46150(uint8_t, uint32_t, const uintptr_t*, uint32_t* (*)(uintptr_t), void (*)());
// 1.13c D2Client+0x465F0 ; pkt = ESI (null: retail exits via string 0xFC6, not modelled), g9731 = the dword at 0x119731,
// g9735 = the word at 0x119735; g9721/g971d/h = the 0x46150 lookup state ([0x119721], [0x11971D], [0x10A808]).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_465f0(const uint8_t* pkt, uint32_t* g9731, uint16_t* g9735,
                                                     uint8_t g9721, uint32_t g971d, const uintptr_t* h, void (*fatal)()) {
    if (!pkt) { fatal(); return; }
    uint16_t w; std::memcpy(&w, pkt + 1, 2);
    if (w != 3) return;
    if (!d2_client_46150(g9721, g971d, h, nullptr, fatal)) return;
    *g9731 = 1; *g9735 = w;
}
