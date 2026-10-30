// D2Client Hurricane (Druid skill 250, state 0x90) client handlers, wired into the session tick.
//   1.13c D2Client+0xC9BE0  cltactivefunc 90 (per tick)   fastcall(unit, skillId) + stack arg (skill level)
//   1.13c D2Client+0x72C20  setfunc 7 for state 0x90 (once, when the state turns on)
//   1.13c D2Client+0xC97D0  one missile spawn (its client-hero-seed draws are kept; the missile itself is not simulated)
// Thunks (import slots read from D2Client.dll): 0xC1F0 = D2Common #10331 UNITS_GetRoom, 0xC208 = #10057 DUNGEON_IsRoomInTown,
// 0xC316 = #10786 SKILLS_EvaluateSkillFormula. The sound-engine submit 0x88A70 is queued for the platform layer
// (d2client_ambient::submit); the town branch's 0x225D0/0x88C20 only touch the sound engine and are not simulated.
// Not simulated: the missile objects (0x630F0 builds client missiles; this engine has no client-missile hash).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "../d2moo/include/D2DataTbls.h"
#include "../d2moo/include/D2Dungeon.h"
#include "../d2moo/include/D2Skills.h"
#include "../d2moo/include/D2States.h"
#include "../d2moo/include/Units/Units.h"

namespace d2client_ambient { void submit(int32_t row); void submitHeld(int32_t row, int32_t life); void releaseHeld(); }

namespace {
constexpr int32_t kSkill = 250, kState = 144;
bool On = false;
const uint8_t* skillRow() {
    if (kSkill >= sgptDataTables->nSkillsTxtRecordCount) return nullptr;
    return reinterpret_cast<const uint8_t*>(sgptDataTables->pSkillsTxt) + uint32_t(kSkill) * 0x23c;
}
int16_t s16(const uint8_t* r, int o) { int16_t v; std::memcpy(&v, r + o, 2); return v; }
uint32_t u32(const uint8_t* r, int o) { uint32_t v; std::memcpy(&v, r + o, 4); return v; }
uint32_t step(uint32_t* s) {   // 0x6AC690C5 LCG on the client hero seed {lo, hi}; returns the new low
    uint64_t v = uint64_t(s[0]) * 0x6ac690c5ull + s[1];
    s[0] = uint32_t(v); s[1] = uint32_t(v >> 32);
    return s[0];
}
// 1.13c D2Client+0xC97D0 (seed draws and the 0xC316 count only; missile 0x630F0 is the sound submit)
void spawn(D2UnitStrc* u, uint32_t* seed, const uint8_t* row, int32_t level, int32_t sound) {
    step(seed);                                   // offset table index (& 7)
    int32_t idx = s16(row, 0xea);
    if (idx < 0) return;
    if (s16(row, 0xec) >= 0) {
        if (s16(row, 0xee) < 0) { if (!(step(seed) & 1)) idx = s16(row, 0xec); }
        else { uint32_t m = step(seed) % 3; if (m == 0) idx = s16(row, 0xec); else if (m == 1) idx = s16(row, 0xee); }
    }
    if (sound > 0) d2client_ambient::submitHeld(sound, 1 << 30);   // missile create submits its Sounds row; its mixer record stays taken until the state ends (the pool then runs dry: 200 records)
    int32_t n = SKILLS_EvaluateSkillFormula(u, u32(row, 0x118), kSkill, level);
    if (n > 0) { if (n & (n - 1)) { uint64_t v = uint64_t(seed[0]) * 0x6ac690c5ull + seed[1]; seed[0] = uint32_t(v); seed[1] = uint32_t(v >> 32); } else step(seed); }
}
}

namespace d2client_hurricane {
void reset() { On = false; }
// One client tick for the hero. active = the Hurricane state is on; seed = the client hero seed.
void tick(D2UnitStrc* u, uint32_t* seed) {
    const bool now = u && STATES_CheckState(u, kState);
    const bool was = On; On = now;
    if (was && !now) d2client_ambient::releaseHeld();   // the mixer records the missile sounds held come free once the state ends
    const uint8_t* row = skillRow();
    if (!now || !row) return;
    const int32_t sound = s16(row, 0x10e);
    const bool town = DUNGEON_IsRoomInTown(UNITS_GetRoom(u)) != 0;
    D2SkillStrc* sk = SKILLS_GetSkillById(u, kSkill, -1);
    const int32_t level = sk ? SKILLS_GetSkillLevel(u, sk, 1) : 0;
    if (!was && sound > 0 && !town) d2client_ambient::submit(sound);   // 0x72C20: 0x88A70(unit,0,0,0)
    if (sound > 0 && !town) d2client_ambient::submit(sound);           // 0xC9BE0 step 3
    if (town) return;                                                  // retail returns 0 here
    int32_t count = SKILLS_EvaluateSkillFormula(u, u32(row, 0x114), kSkill, level);
    for (; count > 0; --count) for (int k = 0; k < 3; ++k) spawn(u, seed, row, level, sound);
}
}
