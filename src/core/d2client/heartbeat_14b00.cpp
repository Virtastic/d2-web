// 1.13c D2Client+0x14B00: periodic (5 s) 0x6D packet [6d][tick:4][chk:4][g830>>1:4] via D2Net #10024 (13, 0, buf).
// chk = u32 from two WORDs read in a module named by 3 obfuscated globals (x 0x470e31c1; 0 if name empty,
// 0xCCCCCCCC if module/read missing), times -0x49bf21bf. Host supplies the module probe and the send.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct Client14B00State { uint32_t d848, d84c, d850, d830, lastTick, c4e0, c4e8; };  // 6fbc9848/4c/50, 6fbc9830, 6fbcc4f4, 6fbcc4e0, 6fbcc4e8
static_assert(sizeof(Client14B00State) == 28, "state");
// probe(name, off, &w0, &w20): 0 if module absent or either read fails, else 1.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_14b00(Client14B00State* g, uint32_t now,
    int (*probe)(const char*, uint32_t, uint16_t*, uint16_t*), void (*send)(uint32_t, uint32_t, const uint8_t*)) {
    uint32_t nm[4] = { g->d848 * 0x470e31c1u, g->d84c * 0x470e31c1u, 0, 0 };
    uint32_t off = g->d850 * 0x470e31c1u;
    uint32_t chk = 0;
    if ((uint8_t)nm[0] != 0) {
        chk = 0xCCCCCCCCu;
        uint16_t a, b;
        if (probe((const char*)nm, off, &a, &b)) chk = ((uint32_t)b << 16) | a;
    }
    if (now - g->lastTick < 5000) return;
    g->lastTick = now;
    uint8_t p[13]; p[0] = 0x6d;
    uint32_t v[3] = { now, chk * 0xB640DE41u, g->d830 >> 1 };   // -0x49bf21bf mod 2^32
    memcpy(p + 1, v, 12);
    send(13, 0, p);
    g->c4e0 += 13; g->c4e8 += 1;
}
