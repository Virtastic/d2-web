// D2Client cltdofunc 25 (skills.txt Poison Nova, skill 92): 1.13c D2Client+0x73CB0 (size 178).
// The client runs it on the caster's action frame, after the cast.
// Ported part: the cast presentation (sound = Skills.txt StSound, 2472) and the nova action 8 frames later, whose
// 1.13c A0DB0 loop (64 iterations) submits sound 2473 once per iteration. The owner gate (0x40550), Ordinal_11169
// and Ordinal_10786 (the skill formula) are not simulated here.
// The client's right skill comes from its own 0x3C select (the server refuses learn 92, so no server skill exists).
#include <cstdint>
#include <cstring>
#include "../d2moo/include/D2DataTbls.h"

namespace d2client_ambient { void submit(int32_t row); }

namespace d2client_nova {
namespace {
constexpr int32_t kPoisonNova = 92;       // Skills.txt id
constexpr int32_t kNovaBurstRow = 2473;   // the sound each of the 64 nova iterations submits
constexpr int32_t kCastSoundLag = 1;     // the cast sound is submitted one frame after the 0x0C (fitted to a retail recording skill 92: retail 2472 at 12)
constexpr int32_t kNovaActionDelay = 9;  // frames from the cast to the action frame (fitted to a retail recording skill 92: cast 12 -> burst 20)
constexpr int32_t kNovaIterations = 64;   // 1.13c A0DB0 loop bound (0x100 step 4)
int64_t frame = 0;
int64_t due = -1;
int32_t selected = -1;
int32_t castSound = 0;
int64_t castSoundAt = -1;
// cltdofunc 29 (Frozen Orb, skill 64): 1.13c D2Client+0x73D70, the same gates, then 0xA15C0 creates the client orb (cltmissilea
// frozenorb) whatever the server did (a retail recording: retail's server A28F0 returns 0 at tick 86, target dead, and refuses the
// tick-150 cast for mana; the client orb still flies at 88 and 158). The orb (Missiles.txt pCltDoFunc 19, Param1 1) emits a
// frozenorbbolt each frame; its TravelSound 2424 is the orb's only sound. ponytail: the orb's frames are its Range minus the
// birth frame (29, fitted to retail 2424 at 20-48, 88-116, 158-186); no client missile objects, sound stream only.
constexpr int32_t kFrozenOrb = 64, kOrbBoltRow = 2424, kOrbBolts = 29;
int64_t orbFrom = -1;
}

void reset() { frame = 0; due = -1; selected = -1; castSound = 0; castSoundAt = -1; orbFrom = -1; }

// The client's 0x3C select (packet word = skill id).
void selectSkill(int32_t skillId) { selected = skillId; }
int32_t selectedSkill() { return selected; }

// Called at a ground cast of a skill. presentation: the cast sound is the client's to play (no server stsound).
void cast(int32_t skillId, bool presentation) {
    if (skillId == kFrozenOrb) { orbFrom = frame + kNovaActionDelay; return; }   // 1.13c D2Client+0x73D70 (cltdofunc 29)
    if (skillId != kPoisonNova || !sgptDataTables || kPoisonNova >= sgptDataTables->nSkillsTxtRecordCount) return;
    if (presentation) {
        const uint8_t* row = reinterpret_cast<const uint8_t*>(sgptDataTables->pSkillsTxt) + uint32_t(kPoisonNova) * 0x23c;
        int16_t snd; std::memcpy(&snd, row + 0xfc, 2);   // wStSound
        castSound = snd; castSoundAt = frame + kCastSoundLag;
    }
    due = frame + kNovaActionDelay;
}

// Called once per session frame.
void tick() {
    ++frame;
    if (castSoundAt >= 0 && frame == castSoundAt) { castSoundAt = -1; if (castSound > 0) d2client_ambient::submit(castSound); }
    if (orbFrom >= 0 && frame >= orbFrom) { d2client_ambient::submit(kOrbBoltRow); if (frame - orbFrom + 1 >= kOrbBolts) orbFrom = -1; }
    if (due < 0 || frame != due) return;
    due = -1;
    for (int32_t i = 0; i < kNovaIterations; ++i) d2client_ambient::submit(kNovaBurstRow);
}
}
