// D2Client NPC Talk-box speech selection: the Introduction and Gossip rows.
// 1.13c D2Client+0x49130 (Introduction row), +0x491E0 (Gossip row), +0x48590 (speak the picked line), +0x46910 (per-NPC gossip roll),
// +0x46E00 (first-use roll of every NPC), +0x45C00 (NPC class to record).// The record table is D2Client's data at +0xF37A8 (46 records, stride 0x16), each pointing at an array of 0xF-byte message entries.
#include <cstdint>
#include "npc_speech.h"

namespace d2client_npcspeech {

#include "npc_speech_table.inc"

static_assert(sizeof(kRecs) / sizeof(kRecs[0]) == kRecordCount, "retail has 46 NPC speech records");

int recordIndex(int32_t npcClass) {   // 1.13c D2Client+0x45C00: first record of this class, else record 0
    for (int i = 0; i < kRecordCount; ++i)
        if (kRecs[i].cls == npcClass) return i;
    return 0;
}
const Rec& record(int index) { return kRecs[index]; }

// 1.13c D2Client+0x46910: roll one record's gossip index from the hero seed. Up to 10 draws; an index below 2 (the introductions) is
// rejected; an entry whose class field is neither the hero's class nor 7 is rejected; a flagged entry also needs its quest state to equal
// its wanted value. Ten rejections leave index 2. (Retail's extra test for the record at +0xF2C08 can only set index 2, which is
// that entry's own index, so it cannot change a result.)
static void rollRecord(State& st, int index, const Env& env) {
    const Rec& r = kRecs[index];
    for (int left = 9;; --left) {
        const int32_t idx = r.count < 1 ? 0 : int32_t(env.rand(env.ctx, uint32_t(r.count)));
        st.idx[index] = idx;
        if (idx >= 2) {
            const Msg& m = r.msgs[idx];
            if (m.cls == env.playerClass || m.cls == 7) {
                if (!m.flag) return;
                if (env.quest(env.ctx, m.quest) == m.want) return;
            }
        }
        if (left == 0) { st.idx[index] = 2; return; }
    }
}

void reset(State& st) {   // 1.13c D2Client+0x46A20 clears the first-use latch when a game starts
    st.rolled = false;
    for (int i = 0; i < kRecordCount; ++i) st.idx[i] = 0;
}

// 1.13c D2Client+0x46E00, run by the first Introduction/Gossip click of a game: every record rolls in table order.
static void ensureRolled(State& st, const Env& env) {
    if (st.rolled) return;
    for (int i = 0; i < kRecordCount; ++i) rollRecord(st, i, env);
    st.rolled = true;
}

static int32_t speakId(const Rec& r, int32_t idx) {   // 0x48590: entries[idx % count].id
    if (r.count < 1) return -1;
    return r.msgs[idx % r.count].id;
}

bool hasIntroduction(int32_t npcClass) { return !kRecs[recordIndex(npcClass)].noIntro; }   // 0x49D80: the row exists when record+0x14 is 0

int32_t introductionId(int32_t npcClass, int32_t playerClass) {   // 0x49130
    const Rec& r = kRecs[recordIndex(npcClass)];
    if (r.noIntro) return -1;
    int32_t idx = 0;
    if (npcClass == 0x201) { if (playerClass == 4) idx = 15; }          // Malah: Barbarian
    else if (npcClass == 0x202) { if (playerClass == 2) idx = 11; }     // Nihlathak: Necromancer
    else if (npcClass == 0x203) { if (playerClass == 3) idx = 10; }     // Qual-Kehk: Paladin
    else if (r.count > 1 && playerClass == r.msgs[1].cls) idx = 1;      // class variant of the first line
    return speakId(r, idx);
}

int32_t gossipId(State& st, int32_t npcClass, const Env& env) {   // 0x491E0 then 0x48590 with the rolled index
    ensureRolled(st, env);
    const int index = recordIndex(npcClass);
    return speakId(kRecs[index], st.idx[index]);
}

}  // namespace d2client_npcspeech

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
extern "C" uint32_t d2_session_client_rand(uint32_t n);
extern "C" int d2_session_quest_flag(uint32_t stateFlag, uint32_t bit);

namespace {
d2client_npcspeech::State g_state;
uint32_t sessRand(void*, uint32_t n) { return d2_session_client_rand(n); }
int sessQuest(void*, int32_t q) { return d2_session_quest_flag(uint32_t(q), 0) > 0 ? 1 : 0; }
}

extern "C" {
// The new game's first-use latch (D2Client+0x46A20 clears it): call when a game starts.
EMSCRIPTEN_KEEPALIVE void d2_client_npc_speech_reset() { d2client_npcspeech::reset(g_state); }
// Introduction row: the string id the row speaks, -1 when the NPC has no such row.
EMSCRIPTEN_KEEPALIVE int32_t d2_client_npc_intro_id(int32_t npcClass, int32_t playerClass) {
    return d2client_npcspeech::introductionId(npcClass, playerClass);
}
// Gossip row: the string id of this NPC's rolled line (the first call of a game rolls every NPC from the client hero seed).
EMSCRIPTEN_KEEPALIVE int32_t d2_client_npc_gossip_id(int32_t npcClass, int32_t playerClass) {
    d2client_npcspeech::Env env{sessRand, sessQuest, nullptr, playerClass};
    return d2client_npcspeech::gossipId(g_state, npcClass, env);
}
}
#endif
