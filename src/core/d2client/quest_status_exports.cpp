// Browser export of the quest log's per-quest record mode: 1.13c D2Client+0x1CE00 run over the player's quest record
// (D2Common's quest-state query on the 96-byte words) and the init-state bytes the quest-log state packet filled (the
// 41 bytes at +0x122496). The mode is the record's dword at +0x266: 0 completing, 1 done, 2 locked, 3 in progress.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "D2QuestRecord.h"

struct QuestLogOps {
    void* c;
    int (*qs)(void*, uint32_t, uint32_t, uint32_t);
    uint32_t (*lang)(void*, uint32_t);
    void (*ncpy)(void*, uint8_t*, uint32_t, uint32_t);
    void (*ctor)(void*, uint8_t*);
    void (*csprintf)(void*, uint8_t*, int32_t);
    void (*touni)(void*, uint8_t*, uint8_t*, uint32_t);
    void (*ucpy)(void*, uint8_t*, uint32_t);
    int (*ulen)(void*, uint32_t);
    void (*ucat)(void*, uint8_t*, uint8_t*);
    void (*usprintf)(void*, uint32_t, uint8_t*, uint32_t, uint32_t);
    void (*fatal)(void*);
};
struct QuestLogGlobals { uint32_t rec1, rec2; int32_t g23e4; uint32_t g23ec; uint32_t g394; const uint8_t* init; uint8_t* state; };
void d2client_sub_1CE00(uint8_t*, const QuestLogGlobals*, const QuestLogOps*, uint8_t*);

namespace {
uint8_t g_live[41];   // 0x1224C0: the state the log last showed per entry
// The text halves of 1CE00 do not matter to the mode; the query is the real one, over the record handed in.
D2BitBufferStrc* g_rec;   // the record being asked about (the retail slot is 32 bits; the build's pointers are not)
int qs(void*, uint32_t, uint32_t quest, uint32_t state) {
    return QUESTRECORD_GetQuestState(g_rec, (int)quest, (int)state);
}
uint32_t lang(void*, uint32_t) { return 0; }
void ncpy(void*, uint8_t*, uint32_t, uint32_t) {}
void ctor(void*, uint8_t*) {}
void csprintf(void*, uint8_t*, int32_t) {}
void touni(void*, uint8_t*, uint8_t*, uint32_t) {}
void ucpy(void*, uint8_t*, uint32_t) {}
int ulen(void*, uint32_t) { return 0; }
void ucat(void*, uint8_t*, uint8_t*) {}
void usprintf(void*, uint32_t, uint8_t*, uint32_t, uint32_t) {}
void fatal(void*) {}
}  // namespace

extern "C" {
// words = the player's 96-byte quest record; init = the 41 state bytes of the 0x52 packet (null = none arrived, zeros).
// Returns the mode of quest id `questId`.
EMSCRIPTEN_KEEPALIVE int32_t d2_client_quest_mode(const uint8_t* words, const uint8_t* init, int32_t questId) {
    uint8_t buf[96], zero[41] = {};
    std::memcpy(buf, words, 96);
    D2BitBufferStrc rec = {};
    rec.pBuffer = buf; rec.nBits = 96 * 8;
    g_rec = &rec;
    QuestLogGlobals g = { 1, 1, 0, 0, 0, init ? init : zero, g_live };
    QuestLogOps o = { nullptr, qs, lang, ncpy, ctor, csprintf, touni, ucpy, ulen, ucat, usprintf, fatal };
    uint8_t S[0x26C] = {}, scratch[32] = {};
    const uint32_t id = (uint32_t)questId;
    std::memcpy(S + 1, &id, 4);
    d2client_sub_1CE00(S, &g, &o, scratch);
    uint32_t mode; std::memcpy(&mode, S + 0x266, 4);
    return (int32_t)mode;
}
EMSCRIPTEN_KEEPALIVE void d2_client_quest_log_reset() { std::memset(g_live, 0, sizeof g_live); }
}
