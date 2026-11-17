// The client's missile sound decisions the play path asks (D2Client+0xB30F0 create: Missiles.txt TravelSound; the terminal
// D2Client+0xB1420: HitSound, else the ExplosionMissile's TravelSound; the pCltDoFunc 1/9/47 and pCltHitFunc 53 do/hit
// ports in clmissile_b17c0.cpp, blizzard_bbc10.cpp, cltactivefunc_c9be0.cpp). Every rule reads the loaded Missiles.txt and
// Skills.txt records; the caller (play/assets/missile-sounds.mjs) keeps only the frame clock and the seeded rolls.
#include <cstdint>
#include <vector>
#include <emscripten.h>
#include <D2DataTbls.h>
#include <DataTbls/MissilesTbls.h>
#include <DataTbls/SkillsTbls.h>

namespace {
enum : uint8_t { kMonster = 1, kCast = 2, kCltBoulder = 4, kTrail = 8, kBlast = 16, kPick = 32, kMaker = 64, kPierce = 128 };
// A missile only a monster skill fires (Skills.txt srvmissile*/cltmissile*, a class skill's own excluded, and the variants that
// share a name stem: shafire2/3 follow shafire1). Derived from the 1.13c Missiles.txt and Skills.txt; the stem needs the
// Missile names, which the loaded records do not keep.
const uint16_t kMonsterMissiles[] = { 13,14,15,16,17,20,21,22,23,24,25,26,32,76,77,78,79,80,106,120,121,122,129,130,131,133,135,136,142,144,145,170,171,172,176,177,179,203,208,209,210,211,220,243,244,246,247,271,276,277,283,322,323,324,325,326,329,331,336,398,399,404,407,411,416,419,422,425,450,451,466,467,468,469,471,474,488,489,490,491,492,493,494,495,498,502,503,517,519,523,524,526,540,546,550,554,555,589,592,593,594,595,596,613,614,641,643,653,660,665,670,671,672,675,678,679 };
std::vector<uint8_t> g_flags; const D2MissilesTxt* g_built = nullptr; int32_t g_count = 0;

const D2MissilesTxt* txt(int32_t id) { return id >= 0 && id < g_count ? &sgptDataTables->pMissilesTxt[id] : nullptr; }
int32_t rowOf(uint16_t v) { return static_cast<int16_t>(v) > 0 ? v : 0; }                   // a Sounds.txt row, 0 when none
int32_t linkOf(uint16_t v, int32_t n) { return v < n ? v : -1; }                              // a missile id, -1 when none

void build() {
    const int32_t n = static_cast<int32_t>(sgptDataTables->nMissilesTxtRecordCount);
    if (g_built == sgptDataTables->pMissilesTxt && g_count == n && !g_flags.empty()) return;
    g_built = sgptDataTables->pMissilesTxt; g_count = n; g_flags.assign(n, 0);
    for (uint16_t id : kMonsterMissiles) if (id < n) g_flags[id] |= kMonster;
    for (int32_t i = 0; i < n; ++i) {
        const D2MissilesTxt& m = g_built[i];
        if (m.wCltHitFunc == 53) { const int32_t b = linkOf(m.wCltHitSubMissile[0], n); if (b >= 0) g_flags[b] |= kCltBoulder; }
        if (m.wCltDoFunc == 47) { const int32_t b = linkOf(m.wSubMissile[0], n); if (b >= 0) g_flags[b] |= kTrail; }
        if (m.wCltDoFunc == 13 || (i >= 159 && i <= 162)) g_flags[i] |= kTrail;           // the engine flies the blizzard center and shards (blizzard_bbc10.cpp)
        if (m.wCltDoFunc == 6 && linkOf(m.wCltSubMissile[1], n) >= 0) { g_flags[i] |= kMaker; const int32_t p = linkOf(m.wCltSubMissile[0], n); if (p >= 0) g_flags[p] |= kPick; }
        if (m.wCltDoFunc == 1 && m.wSrvDoFunc == 1 && m.nClientCol == 1 && m.nNextHit == 1 && rowOf(m.wHitSound)) g_flags[i] |= kPierce;
    }
    for (int32_t k = 0; k < static_cast<int32_t>(sgptDataTables->nSkillsTxtRecordCount); ++k) {
        const D2SkillsTxt& s = sgptDataTables->pSkillsTxt[k];
        if (s.nCharClass >= 0) { const int32_t c = linkOf(s.wCltMissile, n); if (c >= 0) g_flags[c] |= kCast; }
        if (s.wCltDoFunc == 24) { const int32_t c = linkOf(s.wCltMissileA, n); if (c >= 0) g_flags[c] |= kBlast; }
    }
}
uint8_t flags(int32_t id) { build(); return id >= 0 && id < g_count ? g_flags[id] : 0; }
}

extern "C" {
// A class's Missiles.txt field the sound rules read. -1 for a class that does not exist.
EMSCRIPTEN_KEEPALIVE int32_t d2_client_missile_field(int32_t id, int32_t f) {
    build(); const D2MissilesTxt* m = txt(id); if (!m) return -1;
    const uint8_t fl = flags(id);
    switch (f) {
    case 0: return rowOf(m->wTravelSound);
    case 1: return rowOf(m->wHitSound);
    case 2: return rowOf(m->wProgSound);
    case 3: { const D2MissilesTxt* x = txt(linkOf(m->wExplosionMissile, g_count)); return x ? rowOf(x->wTravelSound) : 0; }   // the ExplosionMissile's TravelSound
    case 4: return m->wCltDoFunc;
    case 5: return m->wCltHitFunc;
    case 6: return m->wRange;
    case 7: return static_cast<int32_t>(m->dwCltParam[0]);
    case 8: case 9: case 10: return static_cast<int32_t>(m->dwCltHitPar[f - 8]);
    case 11: case 12: case 13: return linkOf(m->wCltSubMissile[f - 11], g_count);
    case 14: case 15: case 16: return linkOf(m->wCltHitSubMissile[f - 14], g_count);
    case 17: return fl & kPierce ? 1 : 0;
    case 18: return fl & kPick ? 1 : 0;
    case 19: return fl & kMaker ? 1 : 0;
    case 20: return fl & kBlast ? 1 : 0;
    case 21: return id == 577 ? 578 : -1;                              // the armageddon control's client rock (Missiles.txt armageddonrock)
    case 22: return fl & kCltBoulder ? 1 : 0;                           // the client emerge's boulder (pCltHitFunc 53 CltHitSubMissile1)
    case 23: return linkOf(m->wExplosionMissile, g_count);
    default: return -1;
    }
}
// A server missile of class id appears: out[0] = what the client does, out[1] = the TravelSound row, out[2] = 1 for the shrine fireball.
//   0 nothing   1 a blast run (cltdofunc 24)   2 the volcano pair   3 sounds at once (a monster's missile)   4 sounds a tick later   5 a seeded maker's pick
EMSCRIPTEN_KEEPALIVE void d2_client_missile_birth(int32_t id, int32_t* out) {
    build(); const D2MissilesTxt* m = txt(id); const uint8_t fl = flags(id);
    out[0] = 0; out[1] = 0; out[2] = id == 62;
    if (!m) return;
    out[1] = rowOf(m->wTravelSound);
    if (fl & kBlast) { out[0] = 1; return; }
    if (id == 479) { out[0] = 2; return; }
    if (!out[1] || id == 261 || id == 483 || id == 114 || id == 68 || id == 69 || id == 0 || (fl & (kCltBoulder | kTrail))) return;
    out[0] = (fl & kPick) ? 5 : (fl & kMonster) ? 3 : 4;
}
// A missile made and gone inside one engine step (Thunder Storm bolt): the row its creation sounds, 0 for none.
EMSCRIPTEN_KEEPALIVE int32_t d2_client_missile_born_row(int32_t id) {
    build(); const D2MissilesTxt* m = txt(id);
    return m && m->wSrvDoFunc == 3 && !(flags(id) & kTrail) ? rowOf(m->wTravelSound) : 0;
}
// A server missile of class id is gone after span ticks: out[0] = the impact row (HitSound, else the explosion's TravelSound; 0 none),
// out[1] = 1 when it lands 3 ticks later (a monster's), out[2] = the shrine fireball's explosion travel row.
EMSCRIPTEN_KEEPALIVE void d2_client_missile_removal(int32_t id, int32_t span, int32_t ownFireball, int32_t* out) {
    build(); const D2MissilesTxt* m = txt(id); const uint8_t fl = flags(id);
    out[0] = 0; out[1] = 0; out[2] = 0;
    if (!m) return;
    int32_t r = rowOf(m->wHitSound); if (!r) r = d2_client_missile_field(id, 3);
    if (!r || (id == 62 && !ownFireball) || (m->nClientCol == 1 && span >= m->wRange)) return;
    if (m->wCltDoFunc == 1 && !(fl & (kCast | kMonster))) return;
    out[0] = r; out[1] = (fl & kMonster) ? 1 : 0;
    if (id == 62 && ownFireball && rowOf(m->wHitSound)) out[2] = d2_client_missile_field(id, 3);
}
}
