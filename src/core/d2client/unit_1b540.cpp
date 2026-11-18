// 1.13c D2Client+0x1B540: ESI = pUnit; for unit modes 1/5 query D2Common (callees injected), maybe call A7E20.
#include <cstdint>
#include <emscripten.h>
struct Client1B540Callees {
    uint32_t (*c3be)(uint32_t), (*c2ec)(uint32_t), (*c1f0)(uint32_t), (*cd30)(uint32_t), (*c4c0)(uint32_t);
    uint32_t (*cfa0)(uint32_t, uint32_t, uint32_t, uint32_t);        // returns AX
    uint32_t (*c676)(uint32_t, uint32_t* out, uint32_t);              // writes *out (the local `a`)
    void (*a7e20)(uint32_t unit, uint32_t a, uint32_t b, uint32_t zero);
};
extern "C" void d2_client_1b540(const uint8_t* u, uint32_t uAddr, const Client1B540Callees& f) {
    auto w = [](const uint8_t* p, uint32_t o) { return *(const uint32_t*)(p + o); };
    if (!u || (w(u, 0x10) != 1 && w(u, 0x10) != 5)) return;
    const uint32_t t = w(u, 0), pd = w(u, 0x2c);
    const bool direct = t == 2 || (t > 3 && t <= 5);   // direct: read pd+0xc/+0x10 (pd assumed set); else guarded D2Common getter
    uint32_t a = direct ? *(const uint32_t*)(uintptr_t)(pd + 0xc) : pd ? f.c3be(pd) : 0;
    const uint32_t b = direct ? *(const uint32_t*)(uintptr_t)(pd + 0x10) : pd ? f.c2ec(pd) : 0;
    const uint32_t r = f.c1f0(uAddr);
    if (!r) return;
    const uint32_t k = f.cd30(uAddr);
    if (!(uint16_t)f.cfa0(r, a, b, k)) return;
    const uint32_t m = f.c4c0(uAddr);
    f.c676(r, &a, m);
    f.a7e20(uAddr, a, b, 0);
}
