// 1.13c D2Client+0x23C00 (FUN_6fad3c00): the walk/run footstep due-check, one call per movement-mode unit per
// tick. Not an exact port: the sound queue (0x88A70), the layer-probability roll (0x89640) and the terrain
// category (0x226A0) stay with the caller.
//   cur = unit+0x44 (frame, 8.8), total = +0x48, step = +0x4C (frame advance per tick), last = +0x84 (tick of the
//   previous footfall, 0 for a new unit), now = D2Client+0x1197F8 (client tick).
//   n/off: player 2/0; monster from its MonSounds record (+0x5C FsCnt, +0x60 FsOff << 8).
// Rule: a footfall is one period total/n of the animation. When n>1, none within 2/3 of a period (in ticks,
// total/(n*step)) of the last. Due when cur is the nearest of cur-step, cur, cur+step to a footfall phase,
// circular distance (0x895D0) mod period; strict on both sides.
#include <cstdint>
#include <cstdlib>
#include "../d2moo/include/DataTbls/MonsterTbls.h"
#include <emscripten.h>
static int32_t circ(int32_t a, int32_t b, int32_t m) {  // 0x895D0, EDI = m
    int32_t ra = (m & (m - 1)) ? a % m : a & (m - 1), rb = (m & (m - 1)) ? b % m : b & (m - 1);
    int32_t d = rb - ra, d1 = std::abs(d), d2 = std::abs(d - m), d3 = std::abs(ra - rb - m);
    const int32_t lo = d1 < d2 ? d1 : d2;
    return lo < d3 ? lo : d3;
}
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_footstep_due(uint32_t cur, uint32_t total, uint32_t step, uint32_t n, uint32_t off, uint32_t now, uint32_t last) {
    if (!step || !n) return 0;
    const uint32_t tick = total / (n * step);
    if (n > 1 && now - last < (tick * 2) / 3) return 0;
    const int32_t m = (int32_t)(total / n), o = (int32_t)(off << 8);
    if (m <= 0) return 0;
    const int32_t d = circ((int32_t)cur, o, m);
    return d < circ((int32_t)(step + cur), o, m) && d < circ((int32_t)(cur - step), o, m);
}
// MonSounds footstep fields for a monster class: [Footstep row, Layer row, FsCnt, FsOff, FsPrb]; 0 if none.
extern "C" EMSCRIPTEN_KEEPALIVE const uint32_t* d2_client_monster_footstep(int32_t classId) {
    static uint32_t out[5];
    const D2MonSoundsTxt* r = DATATBLS_GetMonSoundsTxtRecordFromMonsterId(classId);
    if (!r) return nullptr;
    out[0] = r->dwFootstep; out[1] = r->dwFootstepLayer; out[2] = r->dwFsCnt; out[3] = r->dwFsOff; out[4] = r->dwFsPrb;
    return out;
}
// One monster unit through 0x23C00: the Sounds row to submit, or 0. mode gate (type 1 branch, 0x23C1D..0x23C70):
// walk 2 / run 0xF; class 0xF also mode 1; class 0x6E only mode 8. Row = MonSounds Footstep + 24 when running
// (terrain category 1: 0x226A0 not recovered). A record with FsCnt 0 never sounds (0x23CD2).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_monster_footstep_row(int32_t classId, uint32_t mode, uint32_t cur, uint32_t total, uint32_t step, uint32_t now, uint32_t last) {
    bool gate = mode == 2 || mode == 0xF;
    if (classId == 0xF) gate = gate || mode == 1; else if (classId == 0x6E) gate = mode == 8;
    const D2MonSoundsTxt* r = gate ? DATATBLS_GetMonSoundsTxtRecordFromMonsterId(classId) : nullptr;
    if (!r || !r->dwFsCnt || !(r->dwFootstep || r->dwFootstepLayer) || !d2_client_footstep_due(cur, total, step, r->dwFsCnt, r->dwFsOff, now, last)) return 0;
    if (!r->dwFootstep) return r->dwFootstepLayer;   // a retail recording: a crow has only the layer row (hawk_wing_1, 1155), submitted on its own footfall (layer roll FsPrb 100 not modeled)
    return r->dwFootstep + (mode == 0xF ? 24 : 0);
}
