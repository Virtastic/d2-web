// 1.13c D2Client+0xAA4E0  panel_press: left-button press on the panel. Stdcall, one stack word (event*), ret 4.
// Gate 0x63710 (nonzero: return its eax, nothing written). Raw x > W/2 or y > H-0x31: outside (AA380, target clear,
// slide, no BBE84). Otherwise the panel-local band (AA290 on px = x - [11B9A0], py = y + [11B9A4]) plays sound 6 on
// a band change, calls AA090 and A9F00 and BBE84; then the box test sets FCDBC = 1 (sound 4) or FCDBD = AA240 (sound 4
// when not -1); raw x > W or y > H-0x31 exits without BBE84; otherwise BBE84 again.
// Returns the eax of the last BBE84; 0 on the paths that exit without it (see the spec, open point).
#include <cstdint>
#include <emscripten.h>
#include "panel_env_aa4e0.h"

extern "C" {
EMSCRIPTEN_KEEPALIVE uint32_t d2c_panel_press_aa4e0(const D2cPanelEnv* e, D2cPanelEvent* ev, uint32_t evRef) {
    const D2cPanelGlobals& g = e->g;
    const int32_t x = ev->x, y = ev->y;
    const uint32_t gate = panelCall(e, 0x63710);
    if (gate) return gate;
    const int32_t w = *g.screenW, h = *g.screenH;

    if (x > w / 2 || y > h - 0x31) {
        panelCall(e, 0xAA380);
        const int32_t prev = *g.target;
        panelClearTarget(e, prev);
        if (panelSlide(e)) {
            const int32_t ebx = *g.cb828;
            if (ebx > w / 2) panelCall(e, 0x161B0, 0, uint32_t(w), 0, uint32_t(ebx - w / 4));
        }
        return 0;
    }

    const int32_t px = x - *g.panelX;
    const int32_t py = y + *g.panelY;
    const int32_t band = int32_t(panelCall(e, 0xAA290, 0, 0, 0, 0, uint32_t(px), uint32_t(py)));
    if (band != -1) {
        if (band != *g.lastBand) panelSound(e, 6);
        panelCall(e, 0xAA090, uint32_t(band));
        panelCall(e, 0xA9F00);
        panelBbe84(e, ev, evRef);
    }

    *g.boxSlot = -1;
    const bool inBox = px >= 0x111 && px <= 0x134 && py >= 0x183 && py <= 0x1A4;
    if (inBox) {
        *g.boxFlag = 1;
        panelSound(e, 4);
    } else {
        const int32_t slot = int32_t(panelCall(e, 0xAA240, 0, 0, 0, uint32_t(px), 0, uint32_t(py)));
        *g.boxSlot = slot;
        if (slot != -1) panelSound(e, 4);
    }

    if (x > w || y > h - 0x31) return 0;
    return panelBbe84(e, ev, evRef);
}
}
