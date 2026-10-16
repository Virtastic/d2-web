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

// Replay hooks for kinds 3446 (D2Game 0x47820), 3447 (0xDFC10), 3448 (0xAFB90); s3249, s3247, s3248. Outputs via d2_game_b27_word.
#include <D2Game/UNIT/SUnit.h>
#include <D2Game/MONSTER/Monster.h>
#include <D2States.h>
#include <D2StatList.h>
static uint32_t gB27Out[8 + 8 * 3];
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_game_b27_word(int i) { return gB27Out[i]; }

static D2GameStrc gB27Game; static D2ActiveRoomStrc gB27Room; static D2MonsterRegionStrc gB27Region;
static void b27Wander(D2GameStrc* g, D2ActiveRoomStrc* r, D2MonsterRegionStrc* m)
{
    ++gB27Out[2]; gB27Out[3] = (r == &gB27Room) | ((g == &gB27Game) << 1) | ((m == &gB27Region) << 2);
}
// out: newLo, newHi, entered, ctx (bit0 room, bit1 game, bit2 region at the 0x47660 seam)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_47820(unsigned lo, unsigned hi, unsigned unk, unsigned level)
{
    memset(&gB27Game, 0, sizeof gB27Game); memset(&gB27Room, 0, sizeof gB27Room); memset(&gB27Region, 0, sizeof gB27Region);
    memset(gB27Out, 0, sizeof gB27Out);
    gB27Game.pMonReg[level] = &gB27Region; gB27Region.unk0x2C4 = unk;
    gB27Room.pSeed.nLowSeed = lo; gB27Room.pSeed.nHighSeed = hi;
    g_d2Wander47660Seam = b27Wander;
    sub_47820(&gB27Game, &gB27Room, (int32_t)level, nullptr);
    g_d2Wander47660Seam = nullptr;
    gB27Out[0] = gB27Room.pSeed.nLowSeed; gB27Out[1] = gB27Room.pSeed.nHighSeed;
}

// Replay hook for kind 3450 (D2Game 0x47A20, s3252): the nine callees are scripted by one xorshift32 stream and logged.
// In: d2_game_47a20_set(i, v) i = 0..27 (see the s3252 scenario); out: d2_game_47a20_out(i) = 6 header words, then 48 x 5 log words.
static uint32_t g47In[28], g47Out[6 + 48 * 5], g47Ss;
static D2RoomCoordListStrc g47Nodes[3]; static D2MonStatsTxt g47Rec;
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_47a20_set(int i, unsigned v) { g47In[i] = v; }
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_game_47a20_out(int i) { return g47Out[i]; }
static uint32_t g47Next() { g47Ss ^= g47Ss << 13; g47Ss ^= g47Ss >> 17; g47Ss ^= g47Ss << 5; return g47Ss; }
static uint32_t g47N(const void* p)
{
    if (p == &gB27Game) return 1; if (p == &gB27Room) return 2; if (p == &gB27Region) return 3;
    for (uint32_t i = 0; i < 3; i++) if (p == &g47Nodes[i]) return 4 + i;
    if (p == &g47Rec) return 8;
    return (uint32_t)(uintptr_t)p;
}
static void g47Log(uint32_t id, uint32_t a, uint32_t b, uint32_t c, uint32_t d)
{
    if (g47Out[4] < 48) { uint32_t* o = g47Out + 6 + 5 * g47Out[4]; o[0] = id; o[1] = a; o[2] = b; o[3] = c; o[4] = d; }
    ++g47Out[4];
}
static int32_t s47Gate(D2GameStrc* g, D2ActiveRoomStrc* r) { g47Log(1, g47N(r), g47N(g), 0, 0); return (int32_t)g47In[4]; }
static D2RoomCoordListStrc* s47Coord(D2ActiveRoomStrc* r) { g47Log(2, g47N(r), 0, 0, 0); return g47In[6] ? &g47Nodes[0] : nullptr; }
static int32_t s47Level(D2ActiveRoomStrc* r) { g47Log(3, g47N(r), 0, 0, 0); return (int32_t)g47In[3]; }
static int32_t s47Preset(D2GameStrc* g, D2MonsterRegionStrc* m, D2ActiveRoomStrc* r, D2MonStatsTxt** out, int32_t nParam, int32_t mode)
{
    const uint32_t v = g47Next(); g47Log(4, (uint32_t)mode, (uint32_t)nParam, g47N(m), g47N(r) * 16 + g47N(g));
    *out = (v % 16 == 0) ? nullptr : &g47Rec; return (int32_t)((v >> 8) % 200 + 1);
}
static int32_t s47Boss(D2MonsterRegionStrc* m, D2ActiveRoomStrc* r) { g47Log(5, g47N(m), g47N(r), 0, 0); return (int32_t)(g47Next() % 4); }
static int32_t s47Fall(int32_t id) { g47Log(6, (uint32_t)id, 0, 0, 0); return (int32_t)(g47Next() & 1); }
static int32_t s47Group(D2GameStrc* g, D2ActiveRoomStrc* r, D2RoomCoordListStrc* n, int32_t id, uint8_t mn, uint8_t mx)
{ g47Log(7, (uint32_t)id, mn, mx, g47N(n) * 256 + g47N(r) * 16 + g47N(g)); return (int32_t)(g47Next() & 1); }
static int32_t s47Champ(D2GameStrc* g, D2ActiveRoomStrc* r, D2RoomCoordListStrc* n, int32_t id)
{ g47Log(9, (uint32_t)id, g47N(n), g47N(r), g47N(g)); return (int32_t)(g47Next() & 1); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_47a20_run()
{
    const uint32_t* in = g47In;   // 0 seedLo 1 seedHi 2 monDen 3 level 4 gate 5 script 6 nNodes 7.. node[3] x {bNode,nIndex,x,y,r,b} 25 min 26 max 27 sparse
    memset(&gB27Game, 0, sizeof gB27Game); memset(&gB27Room, 0, sizeof gB27Room); memset(&gB27Region, 0, sizeof gB27Region);
    memset(g47Nodes, 0, sizeof g47Nodes); memset(&g47Rec, 0, sizeof g47Rec); memset(g47Out, 0, sizeof g47Out);
    gB27Game.pMonReg[in[3]] = &gB27Region; gB27Region.dwMonDen = in[2];
    gB27Game.pGameSeed.nLowSeed = in[0]; gB27Game.pGameSeed.nHighSeed = in[1];
    for (uint32_t i = 0; i < in[6] && i < 3; i++) {
        D2RoomCoordListStrc& n = g47Nodes[i]; const uint32_t* p = in + 7 + 6 * i;
        n.bNode = p[0]; n.nIndex = p[1]; n.pBox[1].nPosX = p[2]; n.pBox[1].nPosY = p[3]; n.pBox[1].nWidth = p[4]; n.pBox[1].nHeight = p[5];
        n.pNext = (i + 1 < in[6] && i + 1 < 3) ? &g47Nodes[i + 1] : nullptr;
    }
    g47Rec.nMinGrp = (uint8_t)in[25]; g47Rec.nMaxGrp = (uint8_t)in[26]; g47Rec.nSparsePopulate = (uint8_t)in[27];
    g47Ss = in[5] ? in[5] : 1;
    D2Populate47A20Seam& z = g_d2Pop47A20;
    z.gate = s47Gate; z.coordList = s47Coord; z.levelId = s47Level; z.preset = s47Preset; z.bossType = s47Boss; z.fallenScarab = s47Fall; z.group = s47Group; z.champion = s47Champ;
    D2GAME_PopulateRoom_6FC67190(&gB27Game, &gB27Room);
    z = D2Populate47A20Seam{};
    g47Out[0] = gB27Game.pGameSeed.nLowSeed; g47Out[1] = gB27Game.pGameSeed.nHighSeed; g47Out[2] = (uint32_t)gB27Region.unk0x08; g47Out[3] = gB27Region.dwMonDen;
    g47Out[5] = g47Out[4] < 48 ? g47Out[4] : 48;
}

// Replay hook for kind 3451 (D2Game 0x46CD0, s3253): the four callees (five call sites) return the record's values and are logged.
// In: d2_game_46cd0_set(i, v) i = 0..9 (regNull act pre4 pre0C monDen lvl1 pop cnt lvl2 cleared); out: ret, post4, post0C, nCalls, then 5 x {id, arg0, arg1}.
static uint32_t g46In[10], g46Out[4 + 15], g46Room;
static uint8_t g46Act;
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_46cd0_set(int i, unsigned v) { g46In[i] = v; }
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_game_46cd0_out(int i) { return g46Out[i]; }
static void g46Log(uint32_t id, uint32_t a, uint32_t b)
{
    if (g46Out[3] < 5) { uint32_t* o = g46Out + 4 + 3 * g46Out[3]; o[0] = id; o[1] = a; o[2] = b; }
    ++g46Out[3];
}
static int32_t s46RoomLevel(D2ActiveRoomStrc*) { const bool second = g46Room++ != 0; g46Log(second ? 4 : 1, 2, 0); return (int32_t)g46In[second ? 8 : 5]; }
static int32_t s46Pop(D2ActiveRoomStrc*) { g46Log(2, 2, 0); return (int32_t)g46In[6]; }
static int32_t s46Count(D2DrlgActStrc* a, int32_t level) { g46Log(3, a == (D2DrlgActStrc*)&g46Act ? 4 : (uint32_t)(uintptr_t)a, (uint32_t)level); return (int32_t)g46In[7]; }
static int32_t s46Cleared(D2GameStrc*) { g46Log(5, 1, 0); return (int32_t)g46In[9]; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_46cd0_run()
{
    memset(&gB27Game, 0, sizeof gB27Game); memset(&gB27Room, 0, sizeof gB27Room); memset(&gB27Region, 0, sizeof gB27Region); memset(g46Out, 0, sizeof g46Out); g46Room = 0;
    const uint32_t* in = g46In;
    gB27Game.pMonReg[in[5]] = in[0] ? nullptr : &gB27Region; gB27Game.pAct[in[1]] = (D2DrlgActStrc*)&g46Act;
    gB27Region.nAct = (uint8_t)in[1]; gB27Region.unk0x04 = in[2]; gB27Region.unk0x0C = (int32_t)in[3]; gB27Region.dwMonDen = in[4];
    D2Gate46CD0Seam& z = g_d2Gate46CD0;
    z.roomLevel = s46RoomLevel; z.populatedLevel = s46Pop; z.countRooms = s46Count; z.chaosCleared = s46Cleared;
    g46Out[0] = (uint32_t)D2GAME_Gate46CD0(&gB27Game, &gB27Room);
    z = D2Gate46CD0Seam{};
    g46Out[1] = gB27Region.unk0x04; g46Out[2] = (uint32_t)gB27Region.unk0x0C;
}

// Replay hook for kind 3452 (D2Game 0xE1430, s3255): our SUNIT_SpawnPresetUnitsInRoom with the three callees (presets, level, spawn) scripted by the record.
// In: d2_game_e1430_set(i, v) i = 0..28 (gameNull level seed ctr0 n pre[8][3]); out: ctr1, nSp, nHead, then spawn[16].
struct D2Gate1430Seam { D2PresetUnitStrc* (*presets)(D2ActiveRoomStrc*); int32_t (*level)(D2ActiveRoomStrc*); void (*spawn)(D2GameStrc*, D2ActiveRoomStrc*, D2PresetUnitStrc*); };
extern D2Gate1430Seam g_d2Gate1430;
void __fastcall SUNIT_SpawnPresetUnitsInRoom(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom);
static uint32_t gE1In[29], gE1Out[3 + 16]; static D2PresetUnitStrc gE1Pre[8];
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_e1430_set(int i, unsigned v) { gE1In[i] = v; }
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_game_e1430_out(int i) { return gE1Out[i]; }
static D2PresetUnitStrc* s14Presets(D2ActiveRoomStrc*) { ++gE1Out[2]; return gE1In[4] ? gE1Pre : nullptr; }
static int32_t s14Level(D2ActiveRoomStrc*) { return (int32_t)gE1In[1]; }
static void s14Spawn(D2GameStrc*, D2ActiveRoomStrc*, D2PresetUnitStrc* p) { if (gE1Out[1] < 16) gE1Out[3 + gE1Out[1]] = (uint32_t)(p - gE1Pre); ++gE1Out[1]; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_e1430_run()
{
    memset(&gB27Game, 0, sizeof gB27Game); memset(&gB27Room, 0, sizeof gB27Room); memset(gE1Pre, 0, sizeof gE1Pre); memset(gE1Out, 0, sizeof gE1Out);
    const uint32_t* in = gE1In;
    gB27Game.dwInitSeed = in[2]; gB27Game.dwUberSandsCounter = in[3];
    for (uint32_t i = 0; i < in[4]; i++) { D2PresetUnitStrc& p = gE1Pre[i]; p.nUnitType = (int32_t)in[5 + 3 * i]; p.nIndex = (int32_t)in[6 + 3 * i]; p.bSpawned = (BOOL)in[7 + 3 * i]; p.pNext = i + 1 < in[4] ? &gE1Pre[i + 1] : nullptr; }
    D2Gate1430Seam& z = g_d2Gate1430; z.presets = s14Presets; z.level = s14Level; z.spawn = s14Spawn;
    SUNIT_SpawnPresetUnitsInRoom(in[0] ? nullptr : &gB27Game, &gB27Room);
    z = D2Gate1430Seam{};
    gE1Out[0] = gB27Game.dwUberSandsCounter;
}

static D2UnitStrc gB27Units[8]; static D2StatListExStrc gB27Lists[8]; static uint32_t gB27Words[8][8];
static uint32_t gB27Cnt, gB27Bad, gB27Arg;
static void __fastcall b27Iter(D2GameStrc* g, D2UnitStrc* u, void* arg)
{
    if (gB27Cnt < 8) gB27Out[2 + gB27Cnt] = (uint32_t)(u - gB27Units) + 1;
    ++gB27Cnt; if (g != &gB27Game || (uint32_t)(uintptr_t)arg != gB27Arg) ++gB27Bad;
}
// bucket/state7: 8 packed bytes each (lo, hi words). out: calls, ctxBad, then the 1-based unit index per call
extern "C" EMSCRIPTEN_KEEPALIVE int d2_game_dfc10(unsigned n, unsigned arg, unsigned bLo, unsigned bHi, unsigned sLo, unsigned sHi)
{
    if (!d2::drlgLoadTables(d2::sharedVfs(), d2::sharedLang())) return -1;
    memset(&gB27Game, 0, sizeof gB27Game); memset(gB27Units, 0, sizeof gB27Units); memset(gB27Lists, 0, sizeof gB27Lists); memset(gB27Words, 0, sizeof gB27Words);
    memset(gB27Out, 0, sizeof gB27Out);
    D2UnitStrc* tail[128] = {};
    for (unsigned i = 0; i < n; ++i)
    {
        unsigned bucket = ((i < 4 ? bLo : bHi) >> (8 * (i & 3))) & 0xFF, st7 = ((i < 4 ? sLo : sHi) >> (8 * (i & 3))) & 0xFF;
        D2UnitStrc* u = &gB27Units[i];
        u->dwUnitType = UNIT_PLAYER; u->pStatListEx = &gB27Lists[i];
        gB27Lists[i].dwFlags = STATLIST_EXTENDED; gB27Lists[i].StatFlags = gB27Words[i];
        gB27Words[i][0] = st7 ? 0x80u : 0x01u;
        if (!tail[bucket]) gB27Game.pUnitList[0][bucket] = u; else tail[bucket]->pListNext = u;
        tail[bucket] = u;
    }
    gB27Cnt = gB27Bad = 0; gB27Arg = arg;
    SUNIT_IterateLivingPlayers(&gB27Game, b27Iter, (void*)(uintptr_t)arg);
    gB27Out[0] = gB27Cnt; gB27Out[1] = gB27Bad;
    return 0;
}

static D2UnitStrc gB27Unit; static D2MonsterDataStrc gB27Mon; static uint8_t gB27Inv[16], gB27Hover[16];
static uint32_t b27Norm(void* p)
{
    if (p == &gB27Unit) return 0xF1000000u; if (p == &gB27Game) return 0xF1000001u; if (p == gB27Inv) return 0xF1000002u;
    if (p == gB27Hover) return 0xF1000003u; return (uint32_t)(uintptr_t)p;
}
static uint32_t gB27Calls;
static void b27Free(int32_t site, void* a, void* b)
{
    if (gB27Calls < 8) { uint32_t* o = gB27Out + 1 + 3 * gB27Calls; o[0] = site; o[1] = b27Norm(a); o[2] = b27Norm(b); }
    ++gB27Calls; gB27Out[0] = gB27Calls;
}
// out: nCalls, then {site, a, b} per call (args as logged at the seam; the scenario maps retail's columns onto them)
extern "C" EMSCRIPTEN_KEEPALIVE int d2_game_afb90(unsigned type, int cls, unsigned hasMon, unsigned hasInv, unsigned hasHover)
{
    if (!d2::drlgLoadTables(d2::sharedVfs(), d2::sharedLang())) return -1;
    memset(&gB27Game, 0, sizeof gB27Game); memset(&gB27Unit, 0, sizeof gB27Unit); memset(&gB27Mon, 0, sizeof gB27Mon);
    memset(gB27Out, 0, sizeof gB27Out);
    gB27Game.pMemoryPool = (void*)(uintptr_t)0x600D1C1Cu;
    gB27Unit.dwUnitType = type; gB27Unit.dwClassId = cls;
    gB27Unit.pMonsterData = hasMon ? &gB27Mon : nullptr; gB27Unit.pInventory = hasInv ? (D2InventoryStrc*)gB27Inv : nullptr; gB27Unit.pHoverText = hasHover ? (D2HoverTextStrc*)gB27Hover : nullptr;
    gB27Mon.pAiControl = (D2AiControlStrc*)(uintptr_t)0xAAAA0028u; gB27Mon.pAiParam = (D2AiParamStrc*)(uintptr_t)0xAAAA002Cu; gB27Mon.pMonInteract = (D2MonsterInteractStrc*)(uintptr_t)0xAAAA0030u;
    gB27Calls = 0; g_d2MonsterFreeSeam = b27Free;
    MONSTER_Free(&gB27Game, &gB27Unit);
    g_d2MonsterFreeSeam = nullptr;
    return 0;
}

// 1.13c D2Game+0x4B500 (stdcall client, game, x, y; EAX = room or 0, ESI = player unit): move the player into a room, send the room/position updates,
// re-attach the inventory items to the game and finish the iron-golem hand-over. Every callee is a seam call `call(id, words)` (ids and word layouts:);
// handles are opaque 32-bit codes, the game's unit hash lists are walked through bucket/nodeId/nodeNext.
struct D2G4B500Seam
{
    uint32_t (*call)(uint32_t id, const uint32_t* w, uint32_t* out0, uint32_t* out1);
    uint32_t (*act)(uint32_t nAct);                  // game->pAct[nAct]
    uint32_t (*bucket)(int list, uint32_t b);        // game->pUnitList[list][b]
    uint32_t (*nodeId)(uint32_t node);               // node +0xC
    uint32_t (*nodeNext)(uint32_t node);             // node +0xE4
};
struct D2G4B500Client { uint8_t nAct; uint32_t dwFlags; uint32_t nIronGolemItemGUID; };   // client +0x1AC, +0x3D4, +0x484
struct D2G4B500Unit { uint32_t type, id, inventory; };                                  // unit +0, +0xC, +0x60

static void GAME_4B500(const D2G4B500Seam& s, D2G4B500Client* cl, uint32_t hClient, uint32_t hGame, uint32_t hUnit, const D2G4B500Unit& u, uint32_t room, uint32_t x, uint32_t y)
{
    if (!hGame || !hUnit || !hClient) return;
    auto C = [&](uint32_t id, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0, uint32_t e = 0, uint32_t f = 0, uint32_t g = 0, uint32_t* o0 = nullptr, uint32_t* o1 = nullptr) {
        const uint32_t w[7] = { a, b, c, d, e, f, g }; return s.call(id, w, o0, o1);
    };
    auto find = [&](int list, uint32_t guid) { for (uint32_t n = s.bucket(list, guid & 0x7F); n; n = s.nodeNext(n)) if (s.nodeId(n) == guid) return n; return 0u; };
    const uint32_t nAct = cl->nAct;                  // CLIENTS_GetActNo (0x49DB0)
    const uint32_t pAct = s.act(nAct);
    if (!room) {
        const uint32_t r13 = C(1, pAct), r89 = C(2, hUnit);
        room = C(3, pAct, r13, 0, r89, 0, 0, 0, &x, &y);
        uint32_t c10 = 0, c14 = 0;
        C(4, room, 0, 0, 0, 0, 0, 0, &c10, &c14);
        const uint32_t level = C(5, room) & 0xFF;
        C(6, c10, c14, hClient, level);
    }
    C(7, hClient, room);
    C(8, room, hUnit, y, x, hGame, 0);
    cl->dwFlags &= 0xFFFFFFFEu;
    C(9, 0x15, u.type, u.id, hClient, x, y, 1);
    C(10, hClient, 0x7E, 5);
    for (uint32_t item = C(11, u.inventory); item;) {
        const uint32_t guid = C(12, item), t = C(13, item);
        if (!t) {
            const uint32_t w = C(14, item);
            const uint32_t next = C(15, item);
            C(16, u.inventory, guid, 0);
            if (guid && (w & 0xFFFF)) C(17, hGame, hUnit, w, guid, 0xFFFF, 0, 0);
            item = next;
        } else {
            if (const uint32_t node = find(0, guid)) {
                const uint32_t r85 = C(18, nAct & 0xFF), r89 = C(2, node);
                uint32_t ox = 0, oy = 0;
                const uint32_t room2 = C(3, s.act(nAct), r85, 0, r89, 0, 0, 0, &ox, &oy);
                C(19, room2, ox, oy, hGame, guid);
            }
            item = C(15, item);
        }
    }
    C(20, hUnit, hGame);
    const uint32_t g = cl->nIronGolemItemGUID;
    if (g && g != 0xFFFFFFFFu) {
        if (const uint32_t node = find(3, g)) {
            if (const uint32_t v = C(21, hUnit, 0x5A)) {
                const uint32_t res = C(22, hUnit, v, 1);
                const uint32_t a = C(23, hUnit); C(24, node, a);
                const uint32_t b = C(25, hUnit); C(26, node, b);
                C(27, hUnit, node);
                C(28, hUnit, hGame, 0x5A, res, 1, 0, 0);
            }
        }
        cl->nIronGolemItemGUID = 0xFFFFFFFFu;
    }
}

// Replay hook for kind 3453 (D2Game 0x4B500, s3254): the port above over a fake game whose callees are scripted by the record (the same stand-ins the capture's stubs were).
// In: d2_game_4b500_set(i, v) i = 0..37 (D2Vec4B500 inputs); out: d2_game_4b500_out(i) i = 0..3 flags1 golem1 nCalls over, then log[56][8].
static uint32_t g4In[38], g4Out[4 + 56 * 8], g4RoomK;
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_4b500_set(int i, unsigned v) { g4In[i] = v; }
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_game_4b500_out(int i) { return g4Out[i]; }
struct G4Node { uint32_t id, next; };
static G4Node g4Nodes[8]; static uint32_t g4Bucket[2][128];
static void g4Ins(int list, uint32_t n, uint32_t id) { g4Nodes[n].id = id; g4Nodes[n].next = g4Bucket[list][id & 0x7F]; g4Bucket[list][id & 0x7F] = 0xC0 + n; }
static uint32_t g4Call(uint32_t id, const uint32_t* w, uint32_t* o0, uint32_t* o1)
{
    const uint32_t* in = g4In; uint32_t ret = 0;
    if (g4Out[2] < 56) { uint32_t* q = &g4Out[4 + 8 * g4Out[2]]; q[0] = id; for (int k = 0; k < 7; k++) q[1 + k] = w[k]; ++g4Out[2]; } else ++g4Out[3];
    auto item = [&](uint32_t h, int f) { return in[10 + 5 * (h - 0xB0) + f]; };
    switch (id)
    {
    case 1: ret = in[26]; break;
    case 2: ret = in[27]; break;
    case 3: *o0 = in[28]; *o1 = in[29]; ret = 0xA5 + (g4RoomK++ & 3); break;
    case 4: *o0 = in[30]; *o1 = in[31]; break;
    case 5: ret = in[32] & 0xFF; break;
    case 11: ret = in[9] ? 0xB0 : 0; break;
    case 12: ret = item(w[0], 0); break;
    case 13: ret = item(w[0], 1); break;
    case 14: ret = item(w[0], 2); break;
    case 15: ret = (w[0] - 0xB0 + 1) < in[9] ? w[0] + 1 : 0; break;
    case 18: ret = in[33]; break;
    case 21: ret = in[34]; break;
    case 22: ret = in[35]; break;
    case 23: ret = in[36]; break;
    case 25: ret = in[37]; break;
    }
    return ret;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_4b500_run()
{
    const uint32_t* in = g4In; memset(g4Out, 0, sizeof g4Out); memset(g4Nodes, 0, sizeof g4Nodes); memset(g4Bucket, 0, sizeof g4Bucket); g4RoomK = 0;
    for (uint32_t i = 0; i < in[9]; i++) { if (in[10 + 5 * i + 3]) g4Ins(0, i, in[10 + 5 * i]); if (in[10 + 5 * i + 4]) g4Ins(0, 3 + i, in[10 + 5 * i] + 0x80); }
    if (in[25] == 2) g4Ins(1, 7, in[6] + 0x80);
    if (in[25] >= 1) g4Ins(1, 6, in[6]);
    D2G4B500Seam s;
    s.call = g4Call;
    s.act = [](uint32_t n) -> uint32_t { return 0x90 + n; };
    s.bucket = [](int list, uint32_t b) -> uint32_t { return g4Bucket[list ? 1 : 0][b]; };
    s.nodeId = [](uint32_t n) -> uint32_t { return g4Nodes[n - 0xC0].id; };
    s.nodeNext = [](uint32_t n) -> uint32_t { return g4Nodes[n - 0xC0].next; };
    D2G4B500Client cl{ (uint8_t)in[4], in[5], in[6] };
    D2G4B500Unit u{ in[7], in[8], 0xA9 };
    GAME_4B500(s, (in[0] & 4) ? nullptr : &cl, (in[0] & 4) ? 0 : 0xA2, (in[0] & 1) ? 0 : 0xA1, (in[0] & 2) ? 0 : 0xA3, u, in[1] ? 0xA4 : 0, in[2], in[3]);
    g4Out[0] = cl.dwFlags; g4Out[1] = cl.nIronGolemItemGUID;
}

// 1.13c D2Game+0x4B1D0 (ESI = client, stack = new room) and +0x4B3C0 (EAX = client, EDI = room): move a client's room, diffing the old and new rooms' neighbour lists
// (new-only rooms get 9DC10, old-only rooms get 9D980 plus the 0x174 unit hand-over 788B0), then re-flag the unit. Callees are seam calls `call(id, w)` (ids:);
// handles are opaque 32-bit values and the neighbour lists come through list(room, buf) (A55C: up to 4 rooms).
struct D2G4B1D0Seam { uint32_t (*call)(uint32_t id, const uint32_t* w); uint32_t (*list)(uint32_t room, uint32_t* buf); };
struct D2G4B1D0Client { uint32_t game, room, unit, v16c, v170, flags; };   // client +0x1A8, +0x1B4, +0x174, +0x16C, +0x170, +0x3D4
static void GAME_4B1D0(const D2G4B1D0Seam& s, D2G4B1D0Client* cl, uint32_t hClient, uint32_t nw)
{
    if (!hClient || !cl->game) return;
    const uint32_t od = cl->room;
    if (od == nw) return;
    auto C = [&](uint32_t id, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0) { const uint32_t w[7] = { a, b, c, d, 0, 0, 0 }; return s.call(id, w); };
    C(1, od, nw); C(2, od, nw);
    uint32_t A[4], B[4], nA = 0, nB = 0;
    if (nw) { C(3, nw); nA = s.list(nw, A); }
    if (od) { C(3, od); nB = s.list(od, B); }
    auto has = [](const uint32_t* v, uint32_t n, uint32_t x) { for (uint32_t i = 0; i < n; i++) if (v[i] == x) return true; return false; };
    for (uint32_t i = 0; i < nA; i++) if (!has(B, nB, A[i])) C(4, A[i], hClient, cl->game);
    for (uint32_t j = 0; j < nB; j++) {
        if (has(A, nA, B[j])) continue;
        C(5, B[j], cl->game, hClient);
        if (od != B[j]) continue;
        uint32_t u;
        if (cl->flags & 1) u = cl->unit;
        else if (!cl->unit) u = 0;
        else { u = C(6, cl->game, cl->v170, cl->v16c); if (!u) cl->unit = 0; }
        C(7, cl->game, u, hClient, 0);
    }
    cl->room = nw;
    if (nw && cl->unit) { const uint32_t r = C(8, nw), a = C(9, cl->unit), b = C(10, r); C(11, cl->unit, a | b); }
}
static void GAME_4B3C0(const D2G4B1D0Seam& s, D2G4B1D0Client* cl, uint32_t hClient, uint32_t room)
{
    if (!cl) { const uint32_t w[7] = { 0x78B }, v = s.call(2, w), x[7] = { 0xF9FF9, v }, y[7] = { 0xFFFFFFFFu }; s.call(3, x); s.call(4, y); return; }   // assert(client) -> _exit(-1)
    if (room != cl->room) { const uint32_t w[7] = { hClient, room }; s.call(1, w); }   // 4B1D0 itself is a seam call here (the capture stubbed it)
    cl->room = room;
}

// Replay hooks for kinds 3454 / 3455 (D2Game 0x4B1D0 s3256, 0x4B3C0 s3257): the ports above over fake clients, callees scripted by the record.
// In: d2_game_4b1d0_set(i, v) i = 0..21 (D2Vec4B1D0 inputs); out: d2_game_4b1d0_out(i) = room1 unit1 nCalls over, then log[32][8]. 4b3c0: set i = 0..2 (nullClient edi old); out room1 nCalls over, log[4][8].
static uint32_t g4dIn[22], g4dOut[4 + 256], g4cIn[3], g4cOut[3 + 32];
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_4b1d0_set(int i, unsigned v) { g4dIn[i] = v; }
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_game_4b1d0_out(int i) { return g4dOut[i]; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_4b3c0_set(int i, unsigned v) { g4cIn[i] = v; }
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_game_4b3c0_out(int i) { return g4cOut[i]; }
static uint32_t g4dCall(uint32_t id, const uint32_t* w)
{
    if (g4dOut[2] < 32) { uint32_t* q = &g4dOut[4 + 8 * g4dOut[2]]; q[0] = id; for (int k = 0; k < 7; k++) q[1 + k] = w[k]; ++g4dOut[2]; } else ++g4dOut[3];
    return id == 6 ? g4dIn[18] : id == 8 ? g4dIn[19] : id == 9 ? g4dIn[20] : id == 10 ? g4dIn[21] : 0;
}
static uint32_t g4dList(uint32_t room, uint32_t* buf)
{
    const bool nw = room == g4dIn[2]; const uint32_t n = nw ? g4dIn[3] : g4dIn[4]; for (uint32_t i = 0; i < 4; i++) buf[i] = g4dIn[(nw ? 5 : 9) + i]; return n;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_4b1d0_run()
{
    memset(g4dOut, 0, sizeof g4dOut); const uint32_t* in = g4dIn;
    D2G4B1D0Client cl{ (in[0] & 2) ? 0 : in[17], in[1], in[14], in[15], in[16], in[13] };
    D2G4B1D0Seam s{ g4dCall, g4dList };
    GAME_4B1D0(s, &cl, (in[0] & 1) ? 0 : 0xA2, in[2]);
    g4dOut[0] = cl.room; g4dOut[1] = cl.unit;
}
static uint32_t g4cCall(uint32_t id, const uint32_t* w)
{
    if (g4cOut[1] < 4) { uint32_t* q = &g4cOut[3 + 8 * g4cOut[1]]; q[0] = id; for (int k = 0; k < 7; k++) q[1 + k] = w[k]; ++g4cOut[1]; } else ++g4cOut[2];
    return id == 2 ? 0x5A5A : 0;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_4b3c0_run()
{
    memset(g4cOut, 0, sizeof g4cOut);
    D2G4B1D0Client cl{ 1, g4cIn[2], 0, 0, 0, 0 };
    D2G4B1D0Seam s{ g4cCall, [](uint32_t, uint32_t*) -> uint32_t { return 0; } };
    GAME_4B3C0(s, g4cIn[0] ? nullptr : &cl, 0xA2, g4cIn[1]);
    g4cOut[0] = g4cIn[0] ? g4cIn[2] : cl.room;
}

// 1.13c D2Game+0x34860 (EAX, ECX, EDX = event words, stack = arg, ret 4): builds a 7-word event {eax, 3, -, arg, -, ecx, edx} on its stack and returns QUESTS_EventCallback(event, 1, 0) (0x33CE0).
// The callee is a seam call: id 1, words {eax, 3, arg, ecx, edx, 1, 0}.
struct D2G34860Seam { uint32_t (*call)(uint32_t id, const uint32_t* w); };
static uint32_t GAME_34860(const D2G34860Seam& s, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t arg)
{
    const uint32_t w[7] = { eax, 3, arg, ecx, edx, 1, 0 };
    return s.call(1, w);
}

// Replay hooks for kind 3456 (D2Game 0x34860, s3258): in d2_game_34860_set(i, v) i = 0..4 (eax ecx edx arg rCallee); out d2_game_34860_out(i) = result nCalls over, then log[2][8].
static uint32_t g34In[5], g34Out[3 + 16];
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_34860_set(int i, unsigned v) { g34In[i] = v; }
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_game_34860_out(int i) { return g34Out[i]; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_34860_run()
{
    memset(g34Out, 0, sizeof g34Out);
    D2G34860Seam s{ [](uint32_t id, const uint32_t* w) -> uint32_t {
        if (g34Out[1] < 2) { uint32_t* q = &g34Out[3 + 8 * g34Out[1]]; q[0] = id; for (int k = 0; k < 7; k++) q[1 + k] = w[k]; ++g34Out[1]; } else ++g34Out[2];
        return g34In[4]; } };
    g34Out[0] = GAME_34860(s, g34In[0], g34In[1], g34In[2], g34In[3]);
}

// 1.13c D2Game+0x769A0 (ESI = handler index, EDI = unit, stack a0 a1 a2, ret 0xC): index outside 0..14 -> nothing. Indices 3 4 5 8 12 go straight to the table; all others first need
// 968E(unit, 1) == 0 or FFA90(unit) != 0. Then, when the handler table entry (D2Game+0xFBC14) is non-null: handler(ecx a0, edx unit, a1, a2).
// Seam call ids: 1 968E {unit, 1}, 2 FFA90 {unit}, 3 handler {idx, a0, unit, a1, a2}; has(idx) = the table entry is non-null.
struct D2G769A0Seam { uint32_t (*call)(uint32_t id, const uint32_t* w); bool (*has)(int32_t idx); };
static void GAME_769A0(const D2G769A0Seam& s, int32_t idx, uint32_t unit, uint32_t a0, uint32_t a1, uint32_t a2)
{
    if (idx < 0 || idx >= 15) return;
    if (idx != 3 && idx != 4 && idx != 5 && idx != 8 && idx != 12) {
        const uint32_t w[7] = { unit, 1 }, v[7] = { unit };
        if (s.call(1, w) && !s.call(2, v)) return;
    }
    if (!s.has(idx)) return;
    const uint32_t w[7] = { (uint32_t)idx, a0, unit, a1, a2 };
    s.call(3, w);
}

// Replay hooks for kind 3457 (D2Game 0x769A0, s3259): d2_game_769a0_set(i, v) i = 0..7 (D2Vec769A0 inputs); out(i) = nCalls over, then log[4][8].
static uint32_t g76In[8], g76Out[2 + 32];
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_769a0_set(int i, unsigned v) { g76In[i] = v; }
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_game_769a0_out(int i) { return g76Out[i]; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_769a0_run()
{
    memset(g76Out, 0, sizeof g76Out);
    D2G769A0Seam s{ [](uint32_t id, const uint32_t* w) -> uint32_t {
                        if (g76Out[0] < 4) { uint32_t* q = &g76Out[2 + 8 * g76Out[0]]; q[0] = id; for (int k = 0; k < 7; k++) q[1 + k] = w[k]; ++g76Out[0]; } else ++g76Out[1];
                        return id == 1 ? g76In[5] : id == 2 ? g76In[6] : 0; },
                    [](int32_t idx) { return !(g76In[7] >> idx & 1); } };
    GAME_769A0(s, (int32_t)g76In[0], g76In[1], g76In[2], g76In[3], g76In[4]);
}

// 1.13c D2Game+0x4B400 (EAX = client, stack = room): re-home a client onto `room`. hClient/unit handles are opaque.
// cl->room = client +0x1B4, *cnt = client +0x1B0; flagBit = bit 21 of unit +0xC8.
static void GAME_4B400(const D2G4B1D0Seam& s, D2G4B1D0Client* cl, uint32_t* cnt, uint32_t hClient, uint32_t arg, uint32_t flagBit)
{
    if (!arg || !hClient) return;
    const uint32_t od = cl->room;
    auto C = [&](uint32_t id, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0) { const uint32_t w[7] = { a, b, c, d, 0, 0, 0 }; return s.call(id, w); };
    const uint32_t u = C(1, hClient, 0);
    if (!u) return;
    const uint32_t a = C(2, u);
    C(3, od, hClient, od); C(4, od, hClient, arg); C(5, u, u, 0xCC2F0);
    if (flagBit) C(7, arg, u, 1);
    ++*cnt;
    if (od != a) {
        const uint32_t x = C(8, od), y = C(8, a);
        if (x != y) { C(10, arg, x, y, u); C(11, x, arg, u, y); }
        C(12, hClient, a);
    }
    C(13, hClient, arg);
    if (C(2, u)) C(14, u);
}

// Replay hook for kind 3459 (D2Game 0x4B400 s3264). In: d2_game_4b400_set(i, v) i = 0..9 = nullMask arg oldRoom flag rUnit rA478a rA478b rA556a rA556b cnt0; out(i): cnt1 nCalls over, then log[16][8].
static uint32_t g4eIn[10], g4eOut[3 + 128], g4eN2, g4eN8;
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_4b400_set(int i, unsigned v) { g4eIn[i] = v; }
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_game_4b400_out(int i) { return g4eOut[i]; }
static uint32_t g4eCall(uint32_t id, const uint32_t* w)
{
    if (g4eOut[1] < 16) { uint32_t* q = &g4eOut[3 + 8 * g4eOut[1]]; q[0] = id; for (int k = 0; k < 7; k++) q[1 + k] = w[k]; ++g4eOut[1]; } else ++g4eOut[2];
    return id == 1 ? (g4eIn[4] ? 0xB1 : 0) : id == 2 ? (g4eN2++ ? g4eIn[6] : g4eIn[5]) : id == 8 ? (g4eN8++ ? g4eIn[8] : g4eIn[7]) : 0;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_4b400_run()
{
    memset(g4eOut, 0, sizeof g4eOut); g4eN2 = g4eN8 = 0;
    D2G4B1D0Client cl{ 0, g4eIn[2], 0, 0, 0, 0 }; uint32_t cnt = g4eIn[9];
    D2G4B1D0Seam s{ g4eCall, [](uint32_t, uint32_t*) -> uint32_t { return 0; } };
    GAME_4B400(s, &cl, &cnt, (g4eIn[0] & 1) ? 0 : 0xA2, (g4eIn[0] & 2) ? 0 : g4eIn[1], g4eIn[3]);
    g4eOut[0] = cnt;
}

// 1.13c D2Game+0x4BCC0 (EAX = client, stdcall game a2 a3 a4 a5): add a player to a game (load the unit via 56430, validate the client/game mode flags, set up the unit and its skill hotkeys,
// notify). id 0 (unlogged) fetches the unit 56430 produced. Handles: client 0xA2, game 0xA1, unit 0xB1 (0 = none).
struct D2G4BCC0St { uint32_t cflags, f3d4, c174, uflags, g70, nSk, utype, uid, w70, w74, w78, w7C, slotA[16], slotD[16]; };   // client +0xA/+0x3D4/+0x174, unit +0xC8, game +0x70
static uint32_t GAME_4BCC0(const D2G4B1D0Seam& s, D2G4BCC0St& c, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
{
    auto C = [&](uint32_t id, uint32_t a = 0, uint32_t b = 0, uint32_t cc = 0, uint32_t d = 0, uint32_t e = 0, uint32_t f = 0, uint32_t g = 0) { const uint32_t w[7] = { a, b, cc, d, e, f, g }; return s.call(id, w); };
    const uint32_t G = 0xA1, CL = 0xA2;
    c.f3d4 |= 1;
    const uint32_t err = C(1, a4, a5, G, 0xD, a2, a3, CL);
    const uint32_t u = C(0);
    auto fail = [&](uint32_t v) { c.c174 = 0; return v; };
    if (err) { const uint32_t m = C(2, CL); C(3, 0xF9FF9, 0xFC228, m, err, 0xFC270); return fail(err); }
    if (c.cflags & 0x20) { if (!c.g70) return fail(0x18); } else if (c.g70) return fail(0x17);
    if (c.cflags & 4) { if (c.cflags & 8) return fail(0x15); if (!(C(4, G) & 0x800)) return fail(0x14); } else if (C(4, G) & 0x800) return fail(0x13);
    C(5, u, CL); const uint32_t m = C(6, u); C(7, m, 0x5F, CL); C(8, u, u, 0xCC2F0);
    if (u && (c.uflags & 1)) { C(9, 0, u, CL); c.uflags |= 0x200000; }
    for (uint32_t i = 0; i < 16; i++) { const int16_t sk = (int16_t)c.slotA[i]; if (sk >= 0 && sk < (int32_t)c.nSk) C(10, i, c.slotD[i], CL, (c.slotA[i] >> 16) & 0xFF); }
    if (C(11, u)) {
        C(12, u ? c.utype : 6, u ? c.uid : 0xFFFFFFFFu, CL, 1, c.w74 & 0xFFFF, c.w7C);
        C(12, u ? c.utype : 6, u ? c.uid : 0xFFFFFFFFu, CL, 0, c.w70 & 0xFFFF, c.w78);
    }
    C(8, u, u, 0xCC2F0); C(15, u, u, 1); C(16, u, G);
    if (c.g70) C(17, u, G);
    return 0;
}

// Replay hook for kind 3460 (D2Game 0x4BCC0 s3265). In: d2_game_4bcc0_set(i, v) i = 0..52 (D2Vec4BCC0 inputs a2..slotD); out(i): ret c174 f3d4o uflags1 nCalls over, then log[32][8].
static uint32_t g4fIn[53], g4fOut[6 + 256];
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_4bcc0_set(int i, unsigned v) { g4fIn[i] = v; }
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_game_4bcc0_out(int i) { return g4fOut[i]; }
static uint32_t g4fCall(uint32_t id, const uint32_t* w)
{
    if (id == 0) return g4fIn[8] ? 0xB1 : 0;
    if (g4fOut[4] < 32) { uint32_t* q = &g4fOut[6 + 8 * g4fOut[4]]; q[0] = id; for (int k = 0; k < 7; k++) q[1 + k] = w[k]; ++g4fOut[4]; } else ++g4fOut[5];
    return id == 1 ? g4fIn[7] : id == 2 ? g4fIn[13] : id == 4 ? g4fIn[12] : id == 6 ? g4fIn[14] : id == 11 ? (g4fIn[16] ? 0xB2 : 0) : 0;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_4bcc0_run()
{
    memset(g4fOut, 0, sizeof g4fOut);
    D2G4BCC0St c{ g4fIn[4], g4fIn[5], 0x5A5A, g4fIn[11], g4fIn[6], g4fIn[15], g4fIn[9], g4fIn[10], g4fIn[17], g4fIn[18], g4fIn[19], g4fIn[20] };
    memcpy(c.slotA, &g4fIn[21], sizeof c.slotA); memcpy(c.slotD, &g4fIn[37], sizeof c.slotD);
    D2G4B1D0Seam s{ g4fCall, [](uint32_t, uint32_t*) -> uint32_t { return 0; } };
    g4fOut[0] = GAME_4BCC0(s, c, g4fIn[0], g4fIn[1], g4fIn[2], g4fIn[3]); g4fOut[1] = c.c174; g4fOut[2] = c.f3d4; g4fOut[3] = c.uflags;
}

// 1.13c D2Game+0x8A3E0 (EAX = client, stdcall src, count): append one outgoing packet to the client's queue of 0x208-byte blocks (head +0x1B8, tail +0x1BC, free list +0x1C0).
// New block: the free-list head if any, else the pool allocator (SCmd.cpp line 0xA0, via `alloc`); zeroed, linked behind the tail. A packet over 0x200 is not split (caller contract).
struct D2G8A3E0Blk { uint32_t used; uint8_t data[0x200]; D2G8A3E0Blk* next; };
struct D2G8A3E0Client { D2G8A3E0Blk *head, *tail, *freeList; };
template <class Alloc> static void GAME_8A3E0(D2G8A3E0Client* c, const void* src, uint32_t n, Alloc alloc)
{
    if (!c) return;
    uint32_t used = c->tail ? c->tail->used : 0;
    D2G8A3E0Blk* b = c->tail;
    if (!b || used + n > 0x200) {
        used = 0;
        b = c->freeList;
        if (b) c->freeList = b->next; else b = alloc();
        memset(b, 0, sizeof *b);
        if (c->tail) c->tail->next = b; else c->head = b;
        c->tail = b;
    }
    memcpy(b->data + used, src, n);
    b->used += n;
}

// Replay hook for kind 3463 (D2Game 0x8A3E0 s3266). In: d2_game_8a3e0_set(i, v) i = 0..5 (nullClient count hasTail tailUsed nFree seed); out(i) = head tail free nAlloc aLine aEdx used[4] next[4] hash[4] (D2Vec8A3E0 from head).
static uint32_t g8aIn[6], g8aOut[18];
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8a3e0_set(int i, unsigned v) { g8aIn[i] = v; }
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_game_8a3e0_out(int i) { return g8aOut[i]; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8a3e0_run()
{
    static D2G8A3E0Blk P[4]; static uint32_t src[0x81]; const uint32_t NEXTFILL = 0xAAAAAAAAu;
    for (auto& b : P) { memset(&b, 0xAA, sizeof b); b.next = nullptr; }   // next of an untouched block is the 0xAA fill in the image: tracked below as 'fill'
    bool fill[4] = { true, true, true, true };
    D2G8A3E0Client c{};
    if (g8aIn[2]) { memset(P[0].data, 0xCC, 0x200); P[0].used = g8aIn[3]; P[0].next = nullptr; fill[0] = false; c.head = c.tail = &P[0]; }
    for (int k = 1; k <= 2; k++) { P[k].used = 0xBBBBBB00u + k; for (int i = 0; i < 0x200; i += 4) { uint32_t w = 0xBBBBBB00u + k; memcpy(P[k].data + i, &w, 4); } P[k].next = (k == 1 && g8aIn[4] >= 2) ? &P[2] : nullptr; fill[k] = false; }
    if (g8aIn[4] >= 1) c.freeList = &P[1];
    uint32_t s = g8aIn[5] ? g8aIn[5] : 1; for (auto& w : src) { s ^= s << 13; s ^= s >> 17; s ^= s << 5; w = s; }
    memset(g8aOut, 0, sizeof g8aOut);
    GAME_8A3E0(g8aIn[0] ? nullptr : &c, src, g8aIn[1], [&]() { g8aOut[3]++; g8aOut[4] = 0xA0; g8aOut[5] = 0x208; return &P[3]; });
    if (g8aOut[3]) fill[3] = false;   // the allocated block was zeroed, its next is a real null now
    auto idx = [&](const D2G8A3E0Blk* p) -> uint32_t { for (uint32_t k = 0; k < 4; k++) if (p == &P[k]) return k; return 0xFF; };
    g8aOut[0] = idx(c.head); g8aOut[1] = idx(c.tail); g8aOut[2] = idx(c.freeList);
    for (int k = 0; k < 4; k++) {
        g8aOut[6 + k] = P[k].used; g8aOut[10 + k] = fill[k] ? 0xFE : idx(P[k].next);
        uint32_t w[0x82], h = 0x811C9DC5u; w[0] = P[k].used; memcpy(w + 1, P[k].data, 0x200);
        w[0x81] = fill[k] ? NEXTFILL : P[k].next ? idx(P[k].next) + 1 : 0;
        for (uint32_t x : w) { h ^= x; h *= 0x01000193u; }
        g8aOut[14 + k] = h;
    }
}

// 1.13c D2Game+0x8C380
// 1.13c D2Game+0x4AB70
// 1.13c D2Game+0x8A980
// Small server->client packet builders; each fills a stack buffer and hands it to 0x8A3E0 (`send(client, buf, n)`).
// 8C380 (AL = type, ECX = value, stack client): {type, value LE32}, 5 bytes. 4AB70 (EAX = unit or null, stack client): {0x0B, unit type low byte, unit id LE32}, 6 bytes; null unit sends type 6, id -1.
// 8A980 (EAX = word, CL = byte, EDX = dword, stack client, flag): {0x7B, byte, (word & 0xFFF | 0x8000 if flag) LE16, dword LE32}, 8 bytes.
template <class Send> static void GAME_8C380(uint8_t type, uint32_t value, void* client, Send send)
{
    uint8_t b[5]; b[0] = type; memcpy(b + 1, &value, 4); send(client, b, 5);
}
template <class Send> static void GAME_4AB70(const D2UnitStrc* unit, void* client, Send send)
{
    uint32_t type = unit ? unit->dwUnitType : 6, id = unit ? unit->dwUnitId : 0xFFFFFFFFu;
    uint8_t b[6]; b[0] = 0x0B; b[1] = (uint8_t)type; memcpy(b + 2, &id, 4); send(client, b, 6);
}
template <class Send> static void GAME_8A980(uint32_t word, uint8_t byte, uint32_t dword, void* client, int flag, Send send)
{
    uint16_t w = (uint16_t)((word & 0xFFF) | (flag ? 0x8000 : 0));
    uint8_t b[8]; b[0] = 0x7B; b[1] = byte; memcpy(b + 2, &w, 2); memcpy(b + 4, &dword, 4); send(client, b, 8);
}

// Replay hook for kinds 3464-3466 (s3267-s3269): fn 0/1/2 = 8C380/4AB70/8A980; in a..e as D2VecPkt3; out(i) = nCalls, client (0 null, 1 pinned), count, bytes[0..3] LE, bytes[4..7] LE.
static uint32_t g3pOut[5];
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_pkt3(int fn, unsigned a, unsigned b, unsigned c, unsigned d, unsigned e)
{
    static uint8_t clientBuf[16]; static D2UnitStrc unit;
    memset(g3pOut, 0, sizeof g3pOut);
    void* cl = e ? nullptr : clientBuf;
    auto send = [](void* client, const uint8_t* p, uint32_t n) { g3pOut[0]++; g3pOut[1] = client ? 1 : 0; g3pOut[2] = n; uint8_t t[8] = {}; memcpy(t, p, n < 8 ? n : 8); memcpy(&g3pOut[3], t, 8); };
    if (fn == 0) GAME_8C380((uint8_t)a, b, cl, send);
    else if (fn == 1) { memset(&unit, 0, sizeof unit); unit.dwUnitType = b; unit.dwUnitId = d; GAME_4AB70(a ? nullptr : &unit, cl, send); }
    else GAME_8A980(a, (uint8_t)b, c, cl, d, send);
}
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_game_pkt3_out(int i) { return g3pOut[i]; }

// 1.13c D2Game+0x2EEA0 (EBX = unit, stack game + one unused word, ret 8): skill-use gate. B2D6(unit) != 0 -> 0; skill = B204(unit), 0 -> 0; B2E2(unit) != 0 -> 0; skill >= cnt (signed, cnt = [[0x6FD1829C]] + 0xC5C) -> 0;
// A808(unit) != 0 -> 0. this = game skill array entry: when this.+8 == 0x7FFFFFFF it becomes B18C(game[0xBC + 4 * this.byte0], skill); this.+4++; when the second stack word's +0x210 is non-zero,
// 2EAD0(this, game, unit, word) == 1 -> 0; else 1. Seam call ids: 1 B2D6 {unit}, 2 B204 {unit}, 3 B2E2 {unit}, 4 A808 {unit}, 5 B18C {game array word, skill}, 6 2EAD0 {thisCode, 0xA1, unit, word}.
struct D2G2EEA0Seam { uint32_t (*call)(uint32_t id, const uint32_t* w); };
static uint32_t GAME_2EEA0(const D2G2EEA0Seam& s, uint32_t unit, uint32_t thisB, uint32_t& thisCnt, uint32_t& thisV8, uint32_t gArr, uint32_t g210, uint32_t cnt, uint32_t skillBase)
{
    (void)thisB; (void)skillBase;
    const uint32_t u[7] = { unit };
    if (s.call(1, u)) return 0;
    const uint32_t skill = s.call(2, u);
    if (!skill) return 0;
    if (s.call(3, u)) return 0;
    if ((int32_t)skill >= (int32_t)cnt) return 0;
    if (s.call(4, u)) return 0;
    if (thisV8 == 0x7FFFFFFFu) { const uint32_t w[7] = { gArr, skill }; thisV8 = s.call(5, w); }
    thisCnt++;
    if (g210) { const uint32_t w[7] = { 0xA3 + skill % 3, 0xA1, unit, g210 }; if (s.call(6, w) == 1) return 0; }
    return 1;
}

// Replay hooks for kind 3458 (D2Game 0x2EEA0, s3260): d2_game_2eea0_set(i, v) i = 0..12 (D2Vec2EEA0 inputs); out(i) = result, this+4, this+8, nCalls, over, then log[6][8].
static uint32_t g2eIn[13], g2eOut[5 + 48];
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_2eea0_set(int i, unsigned v) { g2eIn[i] = v; }
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_game_2eea0_out(int i) { return g2eOut[i]; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_2eea0_run()
{
    memset(g2eOut, 0, sizeof g2eOut);
    D2G2EEA0Seam s{ [](uint32_t id, const uint32_t* w) -> uint32_t {
        if (g2eOut[3] < 6) { uint32_t* q = &g2eOut[5 + 8 * g2eOut[3]]; q[0] = id; for (int k = 0; k < 7; k++) q[1 + k] = w[k]; ++g2eOut[3]; } else ++g2eOut[4];
        return id == 1 ? g2eIn[7] : id == 2 ? g2eIn[8] : id == 3 ? g2eIn[9] : id == 4 ? g2eIn[10] : id == 5 ? g2eIn[11] : g2eIn[12]; } };
    uint32_t c = g2eIn[2], v8 = g2eIn[3];
    g2eOut[0] = GAME_2EEA0(s, g2eIn[0], g2eIn[1], c, v8, g2eIn[4], g2eIn[5], g2eIn[6], 0);
    g2eOut[1] = c; g2eOut[2] = v8;
}
