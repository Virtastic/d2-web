// Browser export of the automap layout the client recomputes each frame: the marker viewport and anchor of 1.13c
// D2Client+0x5F9F0 and the centre scroll of D2Client+0x608D0. Both ports take a struct of global pointers and a host callback
// (608D0 calls 5F9F0 through it); this wrapper owns the globals as plain statics and feeds both from the caller's inputs.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef int32_t i32; typedef uint32_t u32;
struct AmF9F0G { i32 *d3474, *d3470, *c240, *c23c, *c238, *c220, *c224, *c228, *c22c, *c230, *c234; i32 c1b0, c284, bc48, bc4c; };
struct AmScrollG {
    i32 *mode, *root, *cel[4], *celH[4], *offX, *offY, *shiftX, *shiftY, *scrX, *scrY,
        *panel, *panelPrev, *panelSave, *oy, *ox, *side, *gW, *gH, *scale, *halfW, *halfH, *posX,
        *posY, *player, *act, *unitTbl, *unitCnt, *lastIdx;
};
extern "C" i32 d2c_5f9f0(const AmF9F0G* g, i32 (*f108d0)(i32));
extern "C" i32 d2c_608d0(D2cHost h, AmScrollG* g, i32 force);
namespace {
struct Globals { i32 d3474, d3470, c240, c284, c1b0, bc48, bc4c, lastIdx, c23c, c238, c220, c224, c228, c22c, c230, c234, offX, offY; };
Globals* gs;
i32 never(i32) { return 0; }   // 0x108D0 only runs when the saved selector/panel differ; both are pinned equal below
i32 run5f9f0() {
    AmF9F0G g = { &gs->d3474, &gs->d3470, &gs->c240, &gs->c23c, &gs->c238, &gs->c220, &gs->c224, &gs->c228, &gs->c22c, &gs->c230, &gs->c234,
                  gs->c1b0, gs->c284, gs->bc48, gs->bc4c };
    return d2c_5f9f0(&g, never);
}
u32 host(D2cCall* c) { return c->rva == 0x5F9F0 ? (u32)run5f9f0() : 0; }
}  // namespace

// out[0..7] = anchorX (0x11C23C), anchorY (0x11C238), viewport left/top/right/bottom (0x11C228,22C,230,234), scrollX, scrollY (0x11C1E8,1EC).
// mode is AutoMapMode 0..2, panel the AutoMapLeft panel word (0x11C240: nonzero = left), selector the word at 0x11C284,
// center 0 leaves the scroll at zero (the option the JS keeps; retail has no such switch in these two functions).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_automap_layout(i32 width, i32 height, i32 mode, i32 panel, i32 selector, i32 center, i32* out) {
    Globals g = {};
    g.c1b0 = mode; g.c240 = panel; g.c284 = selector; g.bc48 = width; g.bc4c = height; g.lastIdx = 1;
    g.d3474 = g.c284 != 2; g.d3470 = g.c240;
    gs = &g;
    run5f9f0();
    AmScrollG s = {};
    s.mode = &g.c1b0; s.offX = &g.offX; s.offY = &g.offY; s.gW = &g.bc48; s.gH = &g.bc4c; s.ox = &g.c23c; s.oy = &g.c238; s.lastIdx = &g.lastIdx;
    if (center) d2c_608d0(host, &s, 1);
    out[0] = g.c23c; out[1] = g.c238; out[2] = g.c228; out[3] = g.c22c; out[4] = g.c230; out[5] = g.c234;
    out[6] = g.offX; out[7] = g.offY;
    gs = nullptr;
}
