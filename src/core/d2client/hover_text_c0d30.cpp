// D2Client+0xC0D30: hover-text dispatcher (caller 0xC39E0); env-seam style. Position + per-type routing;
// the text builders (0xC0450, 0xC0390, 0xC0850, 0xC07C0, 0xBFEC0, case 1/2/5 string assembly) are injected.
#include <cstdint>
#include <emscripten.h>
struct D2cC0D30Env {
    int32_t mode;               // DAT D2Client+0x11C414 (3 = hidden)
    int32_t kindSwitch;         // Ordinal_10013() != 0 -> anchor from unit/screen pair
    int32_t anchorX, anchorY;   // Ordinal_11080/11168 pair (kindSwitch) or Ordinal_10651/11142 derived pair
    int32_t mouseX, mouseY;     // DAT 0x11995C/0x119960 + 0x11C418 (non-kindSwitch branch inputs)
    int32_t boxW;               // DAT D2Client+0x1BC48 (shifted by mode 1/2)
    int32_t (*bubbleLimit)();   // Ordinal_10522 (type 5 only; >100 suppresses)
    int32_t (*anchorOk)();      // Ordinal_10012 (kindSwitch branch; 0 returns)
    int32_t (*case1Special)(const int32_t* p); // 0xBF... /0xAB1230 path for ids 0x1A1/0x1A2; 0 = clear globals
    void (*draw[6])(const int32_t* p, int32_t x, int32_t y); // per type 0..5 (text build + Ordinal_10085/10070)
    int32_t* outX; int32_t* outY; // DAT 0x1BC54 / 0x1BC58
    int32_t DAT_baadb4;         // type 4 gate
    int32_t type5Count;
};
extern "C" {
// 1.13c D2Client+0xC0D30
EMSCRIPTEN_KEEPALIVE void d2c_c0d30(const D2cC0D30Env* e, const int32_t* p) {
    if (e->mode == 3 || !p) return;
    int32_t t = p[0];
    if (t == 5 && e->bubbleLimit() > 100) return;
    int32_t x, y;
    if (!e->kindSwitch) { x = e->mouseX; y = e->mouseY; }
    else {
        if (!e->anchorOk()) return;
        x = e->anchorX; y = e->anchorY + 0x10;
        int32_t q = (e->boxW + ((e->boxW >> 31) & 3)) >> 2;
        if (e->mode == 1) x -= q; else if (e->mode == 2) x += q;
    }
    *e->outX = x; *e->outY = y;
    if (t == 1 && (p[1] == 0x1a1 || p[1] == 0x1a2) && !e->case1Special(p)) return;
    if (t == 4 && e->DAT_baadb4) return;
    if (t >= 0 && t <= 5) e->draw[t](p, x, y);
}
}
