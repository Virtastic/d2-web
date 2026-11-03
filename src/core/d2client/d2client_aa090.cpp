// 1.13c D2Client+0xAA090 (197 B): EAX = level. lvl >= DAT 0x6FB8E6D0 (signed compare) is 0. Cases 1..4 walk C58C(DAT 0x6FBC973B, quest, 0)
// with quests 0x1C (case 4 only), 0x17 (3,4), 0xF (2..4), 7 (1..4); a clear flag sets the level to 3, 2, 1, 0 respectively and the later checks
// run on. The result is stored to DAT 0x6FBACDD6.
#include <emscripten.h>
#include "d2client_a9_env.h"
struct D2cAA090G { int32_t max; uint32_t rec; uint32_t out; };   // RVA 0xDE6D0, 0x11973B, 0xFCDD6
static uint32_t q(const D2cA9Env* e, uint32_t rec, uint32_t id) { return e->call(e->ctx, 0xC58C, 0, 0, 0, 0, rec, id, 0, 0, 0); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_aa090(int32_t lvl, D2cAA090G* g, const D2cA9Env* e) {
    int32_t s = lvl;
    if (!(s < g->max)) s = 0;
    switch (s) {
    case 4: if (!q(e, g->rec, 0x1C)) s = 3; [[fallthrough]];
    case 3: if (!q(e, g->rec, 0x17)) s = 2; [[fallthrough]];
    case 2: if (!q(e, g->rec, 0xF)) s = 1; [[fallthrough]];
    case 1: if (!q(e, g->rec, 7)) s = 0; [[fallthrough]];
    default: break;
    }
    g->out = (uint32_t)s;
}
