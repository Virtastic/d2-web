// Derived from cached 1.13c D2Client decompile (0x25280); no D2MOO prior art. Unrecovered callees are host seams.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct Env25280 {
    int32_t  (*o11044)(uint32_t g, uint32_t z);  // Ordinal_11044(DAT_6fbcc3b8,0)
    uint32_t g_bcc3b8;                           // D2Client+0xBCC3B8 value
    void     (*f1a220)(void);
    void     (*o10595)(void);
    void     (*f13d0)(uint32_t a, uint32_t b);
    void     (*f24920)(void* esi);
    void     (*fatal)(void);                     // Ordinal_10265(0x66)->10024(msg), _exit(-1)
};
// 1.13c D2Client+0x25280
// esi=state, ebx=skip flag. *(*(esi+0x14)) = cfg; cfg+0x139 enables; cfg+0xC4[state+0x10] byte table.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_25280(uint8_t* esi, uint32_t ebx, const Env25280* e) {
    uint8_t *p, *cfg;
    std::memcpy(&p, esi + 0x14, sizeof p); std::memcpy(&cfg, p, sizeof cfg);
    if (!cfg[0x139]) return 0;
    auto mode = [&]() { uint32_t v; std::memcpy(&v, esi + 0x10, 4); return v; };
    int32_t k = e->o11044(e->g_bcc3b8, 0);
    if (k) {
        if (k > 0 && k < 4) {
            if (mode() == 0) {
                uint32_t one = 1; std::memcpy(esi + 0x10, &one, 4);
                if (!ebx) e->f1a220();
                e->o10595();
                e->f13d0(2, cfg[0xC4 + mode()]);
            }
            e->f24920(esi);
            return 1;
        }
        e->fatal();
        return 0;
    }
    if (mode() == 1 || mode() == 2) {
        uint32_t z = 0; std::memcpy(esi + 0x10, &z, 4);
        if (!ebx) e->f1a220();
        e->o10595();
        e->f24920(esi);
        uint32_t f; std::memcpy(&f, esi + 0xC4, 4);
        f = cfg[0xC4 + mode()] ? (f | 2) : (f & ~2u);
        std::memcpy(esi + 0xC4, &f, 4);
        return 1;
    }
    return 0;
}
