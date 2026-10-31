// 1.13c D2Client+0xAA740  panel_release: left-button release on the panel. Stdcall, one stack word (event*), ret 4.
// Gate 0x63710 (nonzero: return its eax, nothing written). Raw x > W or y > H-0x31: FCDBC = 0, FCDBD = -1, return
// without BBE84. Otherwise px = x - [11B9A0], py = y + [11B9A4].
//   FCDBC != 0 (box press): AA310(px, py); if 0 -> done. Else target clear, slide, AA380, done.
//   FCDBC == 0: FCDBD == -1 -> done. Else AA240(px, py): mismatch -> done. Match: BBE84, packet 0x14910 (al 0x49),
//   FCDD5 = 1, AA380, FCDBC = 0, FCDBD = -1, target clear, slide, sound 0x8B7, BBE84.
//   done: FCDBC = 0, FCDBD = -1, BBE84.
// Returns the eax of the last BBE84; 0 on the outside path (no BBE84).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "panel_env_aa4e0.h"

namespace {
uint32_t panelDone(const D2cPanelEnv* e, D2cPanelEvent* ev, uint32_t evRef) {
    *e->g.boxFlag = 0;
    *e->g.boxSlot = -1;
    return panelBbe84(e, ev, evRef);
}
}  // namespace

extern "C" {
EMSCRIPTEN_KEEPALIVE uint32_t d2c_panel_release_aa740(const D2cPanelEnv* e, D2cPanelEvent* ev, uint32_t evRef) {
    const D2cPanelGlobals& g = e->g;
    const int32_t x = ev->x, y = ev->y;
    const uint32_t gate = panelCall(e, 0x63710);
    if (gate) return gate;
    const int32_t w = *g.screenW, h = *g.screenH;

    if (x > w || y > h - 0x31) {
        *g.boxFlag = 0;
        *g.boxSlot = -1;
        return 0;
    }

    const int32_t px = x - *g.panelX;
    const int32_t py = y + *g.panelY;

    if (*g.boxFlag != 0) {
        const uint8_t hit = uint8_t(panelCall(e, 0xAA310, uint32_t(px), uint32_t(py)));
        if (hit == 0) return panelDone(e, ev, evRef);
        const int32_t prev = *g.target;
        panelClearTarget(e, prev);
        panelSlide(e);
        panelCall(e, 0xAA380);
        return panelDone(e, ev, evRef);
    }

    if (*g.boxSlot == -1) return panelDone(e, ev, evRef);
    const int32_t slot = int32_t(panelCall(e, 0xAA240, 0, 0, 0, uint32_t(px), 0, uint32_t(py)));
    if (*g.boxSlot != slot) return panelDone(e, ev, evRef);

    panelBbe84(e, ev, evRef);
    uint32_t tabv = 0;
    std::memcpy(&tabv, g.slotTab + 5 * size_t(*g.boxSlot), 4);
    panelCall(e, 0x14910, 0x49, uint32_t(*g.fcdcd), tabv);
    *g.soundFlag = 1;
    panelCall(e, 0xAA380);
    const int32_t prev = *g.target;
    *g.boxFlag = 0;
    *g.boxSlot = -1;
    panelClearTarget(e, prev);
    panelSlide(e);
    panelSound(e, 0x8B7);
    return panelBbe84(e, ev, evRef);
}
}
