// D2Client+0x9FB00: env-seam style. Units are opaque handles; fields via unit_dword(u, dwordIndex).
#include <cstdint>
#include <emscripten.h>
struct D2c9FB00Env {
    uint32_t gameState;                       // DAT_6fbbb214 (0 = fatal)
    uint32_t playerId;                        // DAT_6fbbb220
    uint32_t curPanel;                        // DAT_6fbcbbfc (0 = none)
    uint32_t curPanelKind;                    // *(DAT_6fbcbbfc+0x10)
    void (*fatal)();                          // Ordinal_10265 + 10024 + _exit(-1) (does not return)
    void (*fatalBadUnit)();                   // same with string DAT_6fb8359f
    void (*f9F9E0)();
    void (*f9F460)();
    uint32_t (*unit_head)(uint32_t bucket);   // DAT_6fbbae08[bucket]
    uint32_t (*unit_dword)(uint32_t u, uint32_t idx);  // idx 0 = type, 3 = id, 0x39 = next
    int32_t (*f1E2C0)(uint32_t u, int32_t* buf72);     // buf is 0x48 bytes, buf[0]=0, byte 5 = 1
    int32_t (*ord10005)(int32_t* buf72, uint32_t* outW, int32_t* outH); // Ordinal_10005; outH = iStack_18
    void (*f1E930)(uint32_t u, int32_t a, int32_t b);
    uint32_t (*ord11090_w)(uint32_t u);       // Ordinal_11090(u) -> uVar5
    void (*f1B5A0)(int32_t x, int32_t y);
    void (*ord11090_4)(uint32_t u);           // Ordinal_11090(u,4)
};
extern "C" {
// 1.13c D2Client+0x9FB00
EMSCRIPTEN_KEEPALIVE void d2c_9fb00(const D2c9FB00Env* e) {
    if (e->gameState == 0) { e->fatal(); return; }
    if (e->curPanel == 0 || e->curPanelKind == 0x11) { e->f9F9E0(); return; }
    e->f9F460();
    if (e->gameState != 2) return;
    for (uint32_t u = e->unit_head(e->playerId & 0x7f); u; u = e->unit_dword(u, 0x39)) {
        if (e->unit_dword(u, 3) != e->playerId) continue;
        if (e->unit_dword(u, 0) != 4) { e->fatalBadUnit(); return; }
        int32_t buf[0x12] = {0};
        reinterpret_cast<uint8_t*>(buf)[5] = 1;
        if (!e->f1E2C0(u, buf)) return;
        uint32_t w = 0; int32_t h = 0;
        if (!e->ord10005(buf, &w, &h)) return;
        if (h == 0) return;
        e->f1E930(u, 0, 0);
        uint32_t t = e->ord11090_w(u);
        uint32_t y = t < 0x70 ? (0x72 - t) >> 1 : 0;
        e->f1B5A0(int32_t(0xa7 - (w >> 1)), int32_t(y + 0x6a));
        e->ord11090_4(u);
        return;
    }
}
}
