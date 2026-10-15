#include <D2Game/GAME/Game.h>

int32_t __stdcall GAME_ReturnArgument(int32_t value)
{
    // 1.13c D2Game+0x29AD0
    return value;
}

// Replay hook for s3206.
#include <emscripten.h>
extern "C" EMSCRIPTEN_KEEPALIVE int d2_game_return_argument(int v) { return GAME_ReturnArgument(v); }

// s3240/s3241 replay hooks: D2Game 0x15D0 and 0xE250.
#include <D2Game/MONSTER/MonsterUnique.h>
#include <D2Game/ITEMS/Items.h>
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_game_monflag(int id, int flag) { return MONSTERUNIQUE_CheckMonStatsFlag(id, flag); }
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_game_uniqdrop(int nullGame, int id)
{
    // pGame stand-in; the capture's pattern at dword k from 0x1B24 (retail offset), here at dwUniqueFlags (wasm64 layout differs)
    static D2GameStrc g;
    uint32_t* p = &g.dwUniqueFlags[0];
    for (uint32_t k = 0; k < 129; ++k) p[k] = 0x9E3779B1u * ((k + 0x1B24 / 4) + 1) ^ ((k + 0x1B24 / 4) << 7);
    return ITEMS_HasUniqueBeenDroppedAlready(nullGame ? nullptr : &g, id);
}

// s3251 replay hook: D2Game 0x348A0 QUESTS_QuestInit on a stand-in game whose pGameSeed is the vector's pre-state.
// Words out (D2VecQuestInit348A0 layout from postSeedLo): postLo, postHi, info[9], nRecords, rec[n] x2 words.
#include <D2Game/QUESTS/Quests.h>
static uint32_t gQuestInitOut[2 + 9 + 1 + 96 * 2];
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_game_questinit_word(int i) { return gQuestInitOut[i]; }
extern "C" EMSCRIPTEN_KEEPALIVE int d2_game_questinit(unsigned lo, unsigned hi, int expansion)
{
    static D2GameStrc g;
    memset(&g, 0, sizeof(g));
    g.bExpansion = expansion;
    memcpy(&g.pGameSeed, (uint32_t[]){ lo, hi }, 8);
    QUESTS_QuestInit(&g);
    auto* c = g.pQuestControl;
    uint32_t* o = gQuestInitOut;
    memcpy(o, &g.pGameSeed, 8);
    o[2] = c->pLastQuest != nullptr; o[3] = c->bExecuting; o[4] = c->bPickedSet; o[5] = c->pQuestFlags != nullptr;
    o[6] = c->pTimer != nullptr; o[7] = c->dwTick;
    memcpy(o + 8, &c->pSeed, 8);
    o[10] = c->unk0x20 | (c->unk0x21[0] << 8) | (c->unk0x21[1] << 16) | (c->unk0x21[2] << 24);
    uint32_t n = 0;
    for (auto* q = c->pLastQuest; q && n < 96; q = q->pPrev, ++n)
        { o[12 + 2 * n] = q->nQuestNo; o[13 + 2 * n] = q->nActNo | (q->bNotIntro << 8) | (q->bActive << 16); }
    o[11] = n;
    return n;
}

// s3260 replay hook: D2Game 0xCACC0 (sub_6FC68110) on a stand-in game; retail stubbed 0xCAB30 (region build), ours runs it on a null pool.
// field 0..2 = pGameSeed lo/hi (+0xD0/+0xD4), dwMonSeed (+0xEC).
#include <D2Game/MONSTER/MonsterRegion.h>
// 1.13c offsets hold only on the 32-bit layout; wasm64 widens pointers, so the probe addresses fields by name.
static_assert(sizeof(void*) != 4 || (offsetof(D2GameStrc, nDifficulty) == 0x6D && offsetof(D2GameStrc, pGameSeed) == 0xD0 && offsetof(D2GameStrc, dwMonSeed) == 0xEC), "CACC0 offsets");
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_cacc0(uint32_t lo, uint32_t hi, uint32_t diff, int field)
{
    static D2GameStrc g;
    memset(&g, 0, sizeof g);
    g.nDifficulty = (uint8_t)diff;
    g.pGameSeed.nLowSeed = lo; g.pGameSeed.nHighSeed = hi;
    sub_6FC68110(&g);
    return field == 0 ? g.pGameSeed.nLowSeed : field == 1 ? g.pGameSeed.nHighSeed : g.dwMonSeed;
}

// s3243 replay hook: D2Game 0x8FBE0 SUNITPROXY_InitializeNpcControl (kind 3261) on a stand-in game whose pGameSeed is the
// vector's pre-state. Words: postLo, postHi, nArraySize, unk10, ctlLo, ctlHi, then per record {nNPC, bytes +0x22..+0x25,
// bytes +0x26..+0x27, bGambleInit} (the D2VecNpcRec8FBE0 layout), 64 records.
#include <D2Game/UNIT/SUnitProxy.h>
#include <D2Game/UNIT/SUnitNpc.h>
#include "../tables/shared.h"
#include "../d2moo/compat/datatbls_bridge.h"   // the capture ran with retail tables: monstats INTERACT rows pick the NPCs
static_assert(offsetof(D2NpcTradeStrc, nAct) == 2 && offsetof(D2NpcTradeStrc, bRefreshInventory) == 7, "npc trade layout");
static uint32_t gNpcCtlOut[6 + 64 * 4];
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_game_npcctl_word(int i) { return gNpcCtlOut[i]; }
extern "C" EMSCRIPTEN_KEEPALIVE int d2_game_npcctl(unsigned lo, unsigned hi)
{
    if (!d2::drlgLoadTables(d2::sharedVfs(), d2::sharedLang())) return -1;
    static D2GameStrc g;
    memset(&g, 0, sizeof(g));
    memcpy(&g.pGameSeed, (uint32_t[]){ lo, hi }, 8);
    memset(gNpcCtlOut, 0, sizeof gNpcCtlOut);
    SUNITPROXY_InitializeNpcControl(&g);
    auto* c = g.pNpcControl;
    uint32_t* o = gNpcCtlOut;
    memcpy(o, &g.pGameSeed, 8);
    o[2] = c->nArraySize; o[3] = c->unk0x10;
    memcpy(o + 4, &c->pSeed, 8);
    for (int i = 0; i < c->nArraySize && i < 64; ++i)
    {
        const D2NpcRecordStrc* r = &c->pFirstRecord[i];
        uint32_t* p = o + 6 + 4 * i;
        p[0] = r->nNPC;
        p[1] = r->npcTrade.nAct | (r->npcTrade.bTrader << 8) | (r->npcTrade.bLevelRefresh << 16) | ((uint32_t)r->npcTrade.bInited << 24);
        // npcctl-8fbe0-b packing: {bForceVendor, b27 = bRefreshInventory, pad, pad}, then dw0C = bGambleInit as the raw dword
        p[2] = r->npcTrade.bForceVendor | (r->npcTrade.bRefreshInventory << 8);
        p[3] = (uint32_t)r->bGambleInit;
    }
    return c->nArraySize;
}
