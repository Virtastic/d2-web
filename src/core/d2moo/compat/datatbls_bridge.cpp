// Compat: sgptDataTables is built by the vendored D2MOO loaders (DataTbls/*.cpp) over the
// shipped .bin tables read through our MPQ VFS -- the same load path as retail 1.13c's
// DATATBLS_LoadAllTxts (#10943) once bCompileTxt is off. The in-memory result was verified
// table by table against the captured sgptDataTables; the
// hand-ported fixups in src/core/tables/fixups.cpp remain the test runner's independent check.
#include <D2Common.h>
#include <D2DataTbls.h>
#include <D2Skills.h>
#include <D2Waypoints.h>
#include "../../../assets/mpqvfs.h"
#include "datatbls_bridge.h"

namespace d2 { MpqVfs* g_drlgVfs = nullptr; }

namespace d2 {

bool drlgLoadTables(MpqVfs& vfs, const LangTables&) {
    static bool loaded = false;
    if (loaded) return true;
    loaded = true;
    g_drlgVfs = &vfs;
    sgptDataTables->bCompileTxt = FALSE;   // sgptDataTables is D2MOO's own global (DataTbls.cpp)
    // (hArchive, a2, a3) = (NULL, 0, 0): no eager DS1 preloading, like the retail capture tool's InitDataTables.
    DATATBLS_LoadAllTxts(nullptr, 0, 0);
    return sgptDataTables->pLevelsTxt && sgptDataTables->pLvlPrestTxt && sgptDataTables->pItemDataTables.pItemsTxt;
}

} // namespace d2

// --- s02 cross-check: the same 16 tables as D2MOO's loaders left them --------------------------
// "<ptr>:<count>" for a table name, or "" when unknown. The harness compares these bytes against
// the captured sgptDataTables too, so the vendored loaders are held to the same standard as the
// hand-ported fixups.
#include <emscripten.h>
#include <string>
#include <vector>
extern "C" EMSCRIPTEN_KEEPALIVE const char* d2_datatbl(const char* name) {
    static std::string out; std::string n(name);
    const void* p = nullptr; int c = 0;
    D2DataTablesStrc* t = sgptDataTables;
    static std::vector<D2LvlSubTxtRetail32> lvlSubRetail;
    if (n == "lvlsub" && t->pLvlSubTxt) {
        lvlSubRetail.assign(t->nLvlSubTxtRecordCount, {});
        for (int i = 0; i < t->nLvlSubTxtRecordCount; ++i) {
            const D2LvlSubTxt& src = t->pLvlSubTxt[i]; D2LvlSubTxtRetail32& dst = lvlSubRetail[i];
            dst.dwType = src.dwType; memcpy(dst.szFile, src.szFile, sizeof(dst.szFile));
            dst.dwCheckAll = src.dwCheckAll; dst.dwBordType = src.dwBordType;
            dst.dwDt1Mask = src.dwDt1Mask; dst.dwGridSize = src.dwGridSize;
            memcpy(dst.nProb, src.nProb, sizeof(dst.nProb)); memcpy(dst.nTrials, src.nTrials, sizeof(dst.nTrials));
            memcpy(dst.nMax, src.nMax, sizeof(dst.nMax)); dst.dwExpansion = src.dwExpansion;
        }
        p = lvlSubRetail.data(); c = static_cast<int>(lvlSubRetail.size());
    }
#define T(nm, ptr, cnt) if (n == nm) { p = t->ptr; c = t->cnt; }
    T("missiles", pMissilesTxt, nMissilesTxtRecordCount) T("skills", pSkillsTxt, nSkillsTxtRecordCount)
    T("monstats", pMonStatsTxt, nMonStatsTxtRecordCount) T("charstats", pCharStatsTxt, nCharStatsTxtRecordCount)
    T("itemstatcost", pItemStatCostTxt, nItemStatCostTxtRecordCount) T("levels", pLevelsTxt, nLevelsTxtRecordCount)
    T("lvlprest", pLvlPrestTxt, nLvlPrestTxtRecordCount) T("difficultylevels", pDifficultyLevelsTxt, nDifficultyLevelsTxtRecordCount)
    T("shrines", pShrinesTxt, nShrinesTxtRecordCount) T("objects", pObjectsTxt, nObjectsTxtRecordCount)
    T("objgroup", pObjGroupTxt, nObjGroupTxtRecordCount) T("lvltypes", pLvlTypesTxt, nLvlTypesTxtRecordCount)
    T("lvlwarp", pLvlWarpTxt, nLvlWarpTxtRecordCount) T("lvlmaze", pLvlMazeTxt, nLvlMazeTxtRecordCount)
    T("cubemain", pCubeMainTxt, nCubeMainTxtRecordCount)
    // sets/setitems have no captured vector, so their gate is the shipped .bin itself (the .bin IS
    // the in-memory record array before fixups): s02-tables compares these bytes against it.
    T("sets", pSetsTxt, nSetsTxtRecordCount) T("setitems", pSetItemsTxt, nSetItemsTxtRecordCount)
#undef T
    out = p ? std::to_string((uintptr_t)p) + ":" + std::to_string(c) : "";
    return out.c_str();
}

// s3000: drive the mapped 1.13c entry against the same loaded SkillsTxt rows. The null-table
// variant changes only the retail guard input and restores the pointer before returning.
extern "C" EMSCRIPTEN_KEEPALIVE int d2_skillclass_check(int skillId, int nullSkillsTable, int* playerClass) {
    D2SkillsTxt* saved = sgptDataTables->pSkillsTxt;
    if (nullSkillsTable) sgptDataTables->pSkillsTxt = nullptr;
    const int result = SKILLS_IsPlayerClassSkill(skillId, playerClass);
    sgptDataTables->pSkillsTxt = saved;
    return result;
}

// s3029: index of DATATBLS_GetLevelsTxtRecord(id) in the loaded Levels.txt array, or -1 for NULL.
extern "C" EMSCRIPTEN_KEEPALIVE int d2_levels_txt_record(int id) {
    const D2LevelsTxt* r = DATATBLS_GetLevelsTxtRecord(id);
    return r ? (int)(r - sgptDataTables->pLevelsTxt) : -1;
}

// s3049: WAYPOINTS_ValidateAndCopyWaypointData on a static 2x8 word block (wasm64 pointers are BigInt in JS,
// so words are addressed by index): k 0-7 source, 8-15 destination. Arg k<0 runs the call.
extern "C" EMSCRIPTEN_KEEPALIVE int d2_waypoint_copy(int k, int v) {
    static uint16_t b[16];
    if (k < 0) { WAYPOINTS_ValidateAndCopyWaypointData((D2WaypointDataStrc*)b, (D2WaypointDataStrc*)(b + 8)); return 0; }
    if (v >= 0) b[k] = (uint16_t)v;
    return b[k];
}

// s3002: D2Common ordinal 10003 (RVA 0x2D0B0, a SetBit wrapper) on a static 64-byte field: _byte(i, v) sets
// byte i when v>=0 and returns it; _run(mul, add) makes the call.
namespace d2common { void D2Common_10003(std::uint8_t**, std::int32_t, std::int32_t); }
static unsigned char g_c10003[64];
extern "C" EMSCRIPTEN_KEEPALIVE int d2_common_10003_byte(int i, int v) { if (v >= 0) g_c10003[i] = (unsigned char)v; return g_c10003[i]; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_common_10003_run(int mul, int add) { unsigned char* p = g_c10003; d2common::D2Common_10003(&p, mul, add); }

// s3005: byte k of DATATBLS_GetRunesTxtRecord(id) (valid ids only: the getter asserts otherwise), and the record count.
extern "C" EMSCRIPTEN_KEEPALIVE int d2_runes_txt_byte(int id, int k) { return ((const uint8_t*)DATATBLS_GetRunesTxtRecord(id))[k]; }
extern "C" EMSCRIPTEN_KEEPALIVE int d2_runes_txt_count() { return sgptDataTables->pRuneDataTables.nRunesTxtRecordCount; }

// s3009: INVENTORY_GetBackPackItemByType (1.13c D2Common+0x1FE10) over a synthetic inventory with a chain of n type-4 units in grid 2
// (INVGRID_INVENTORY). Mirrors the retail capture tool's D2CAPTURE_BACKPACK_TYPE; returns the chain index or -1.
#include <D2Inventory.h>
#include <D2Items.h>
extern "C" EMSCRIPTEN_KEEPALIVE int d2_backpack_probe(int ownerType, int sigOk, int itemType, int cont, int n, int badIdx, int c0, int c1, int c2, int c3, int c4, int c5) {
    const int cls[6] = { c0, c1, c2, c3, c4, c5 };
    auto mk = [](unsigned type, int classId) { auto* u = (D2UnitStrc*)calloc(1, sizeof(D2UnitStrc)); u->dwUnitType = type; u->dwClassId = classId; u->pItemData = (D2ItemDataStrc*)calloc(1, sizeof(D2ItemDataStrc)); return u; };
    D2UnitStrc* owner = mk(ownerType ? UNIT_MONSTER : UNIT_PLAYER, ownerType ? 0 : 1);
    auto* inv = (D2InventoryStrc*)calloc(1, sizeof(D2InventoryStrc));
    inv->dwSignature = sigOk ? 0x01020304 : 0; inv->pOwner = owner; inv->nGridCount = 3;
    inv->pGrids = (D2InventoryGridStrc*)calloc(3, sizeof(D2InventoryGridStrc));
    D2InventoryGridStrc& g = inv->pGrids[2];
    D2InventoryGridInfoStrc info = {};
    if (ownerType) { info.nGridX = 10; info.nGridY = 4; }
    else DATATBLS_GetInventoryGridInfo(UNITS_GetInventoryRecordId(owner, 0, 1), 0, &info);
    g.nGridWidth = info.nGridX; g.nGridHeight = info.nGridY; g.ppItems = (D2UnitStrc**)calloc(info.nGridX * info.nGridY + 1, sizeof(void*));
    D2UnitStrc* it[6] = {};
    for (int k = 0; k < n; ++k) it[k] = mk(k == badIdx ? UNIT_MONSTER : UNIT_ITEM, cls[k]);
    for (int k = 0; k + 1 < n; ++k) it[k]->pItemData->pExtraData.pNextGridItem = it[k + 1];
    g.pItem = n ? it[0] : nullptr;
    D2UnitStrc* foreign = mk(UNIT_ITEM, 5);
    D2UnitStrc* c = cont == 99 ? foreign : (cont >= 0 ? it[cont] : nullptr);
    D2UnitStrc* r = INVENTORY_GetBackPackItemByType(inv, itemType, c);
    int res = -1; for (int k = 0; k < n; ++k) if (r == it[k]) res = k;
    return res;
}
// s3049: DATATBLS_GetMonItemPercentTxtRecord(id) (1.13c D2Common+0x53390). k=0 isNull, 1 offset from table base, 2..5 the 4 percent bytes (in-range only).
extern "C" EMSCRIPTEN_KEEPALIVE int d2_monitem_count() { return sgptDataTables->pMonItemPercentDataTables.nMonItemPercentTxtRecordCount; }
extern "C" EMSCRIPTEN_KEEPALIVE int d2_monitem_probe(int id, int k) {
    const uint8_t* p = (const uint8_t*)DATATBLS_GetMonItemPercentTxtRecord(id);
    if (k == 0) return !p;
    if (!p) return 0;
    if (k == 1) return (int)(p - (const uint8_t*)sgptDataTables->pMonItemPercentDataTables.pMonItemPercentTxt);
    return (id >= 0 && id < d2_monitem_count()) ? p[k - 2] : 0;
}

// s3054: ITEMS_GetEarName on a synthetic unit (1.13c D2Common+0x23700). flags bit0 pItemData set, bit1 null unit; k=0 isNull, 1 offset.
extern "C" EMSCRIPTEN_KEEPALIVE int d2_earname_probe(unsigned type, unsigned flags, int k) {
    static D2ItemDataStrc data; D2UnitStrc u = {}; u.dwUnitType = type; u.pItemData = (flags & 1) ? &data : nullptr;
    const char* p = ITEMS_GetEarName((flags & 2) ? nullptr : &u);
    return k == 0 ? !p : (p ? (int)(p - (const char*)&data) : 0);
}

// s3052: ITEMS_SetEarLevel on a synthetic unit (1.13c D2Common+0x23690). Returns byte at pItemData+0x48 (0xAA pre-filled).
extern "C" EMSCRIPTEN_KEEPALIVE int d2_earlevel_probe(unsigned type, unsigned flags, unsigned level) {
    static D2ItemDataStrc data; D2UnitStrc u = {}; memset(&data, 0xAA, sizeof data); u.dwUnitType = type; u.pItemData = (flags & 1) ? &data : nullptr;
    ITEMS_SetEarLevel((flags & 2) ? nullptr : &u, (uint8_t)level);
    return ((uint8_t*)&data)[0x48];
}

// s3053: ITEMS_GetEarLevel on a synthetic unit (1.13c D2Common+0x236B0). flags bit0 pItemData set, bit1 null unit; data nEarLvl = 0x37.
extern "C" EMSCRIPTEN_KEEPALIVE int d2_earlevel_get_probe(unsigned type, unsigned flags) {
    static D2ItemDataStrc data; data.nEarLvl = 0x37; D2UnitStrc u = {}; u.dwUnitType = type; u.pItemData = (flags & 1) ? &data : nullptr;
    return ITEMS_GetEarLevel((flags & 2) ? nullptr : &u);
}

// s3121: ITEMS_CheckIfStackable (1.13c D2Common+0x22E20, ordinal 10048) on a synthetic unit. type bit31 = null unit.
extern "C" EMSCRIPTEN_KEEPALIVE int d2_stackable_22e20_probe(unsigned type, unsigned classId) {
    D2UnitStrc u = {}; u.dwUnitType = type & 0x7FFFFFFF; u.dwClassId = classId;
    return ITEMS_CheckIfStackable((type & 0x80000000u) ? nullptr : &u) & 0xFF;
}

// s3122: UNITFINDS_FindAllMatchingUnitsInNeighboredRooms (1.13c D2Common+0x12720) over the record's synthetic room graph (D2VecUnitFinds12720,
// 4-byte fields, same order); fills result/nIndex/maxAfter/hash in place. Callback test: (dwClassId & cbMask) != 0.
#include <Drlg/D2DrlgDrlg.h>
#include <Drlg/D2DrlgDrlgAnim.h>
#include <Units/UnitFinds.h>
static int32_t __fastcall unitfinds12720_cb(D2UnitStrc* u, D2UnitFindArgStrc* a) { return (u->dwClassId & *(uint32_t*)a) != 0; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_unitfinds_12720_probe(uint32_t* r) {
    D2ActiveRoomStrc rooms[4] = {}; D2DrlgRoomStrc dr[4] = {}; D2DrlgLevelStrc lv[4] = {};
    static D2UnitStrc units[4][64];
    D2ActiveRoomStrc* list[4] = { &rooms[0], &rooms[1], &rooms[2], &rooms[3] };
    const int32_t* room = (const int32_t*)(r + 6); const uint32_t nAdj = r[22], town = r[23], *nU = r + 24, nullRoom = r[28];
    for (int i = 0; i < 4; ++i) {
        lv[i].nLevelId = (town >> i & 1) ? 1 : 5; dr[i].pLevel = &lv[i]; rooms[i].pDrlgRoom = &dr[i];
        rooms[i].tCoords.nSubtileX = room[i * 4]; rooms[i].tCoords.nSubtileY = room[i * 4 + 1];
        rooms[i].tCoords.nSubtileWidth = room[i * 4 + 2]; rooms[i].tCoords.nSubtileHeight = room[i * 4 + 3];
        D2UnitStrc* prev = nullptr;
        for (uint32_t k = nU[i]; k > 0; --k) { D2UnitStrc* u = &units[i][k - 1]; *u = {}; u->dwClassId = i * 100 + k; u->pRoomNext = prev; prev = u; }
        rooms[i].pUnitFirst = prev;
    }
    rooms[0].ppRoomList = list; rooms[0].nNumRooms = (int32_t)nAdj + 1;
    uint32_t arg = r[5];
    D2UnitFindDataStrc d = {};
    d.pUnitsArray = (D2UnitStrc**)D2_ALLOC_POOL(nullptr, r[4] * sizeof(void*));
    d.pRoom = nullRoom ? nullptr : &rooms[0]; d.pfnUnitTest = unitfinds12720_cb; d.pUnitFindArg = (D2UnitFindArgStrc*)&arg;
    d.nFlags = r[3]; d.nMaxArrayEntries = r[4]; d.nX = (int32_t)r[0]; d.nY = (int32_t)r[1]; d.nSize = (int32_t)r[2];
    r[29] = (uint32_t)UNITFINDS_FindAllMatchingUnitsInNeighboredRooms(&d);
    r[30] = d.nIndex; r[31] = d.nMaxArrayEntries;
    uint32_t h = 2166136261u; for (int i = 0; i < d.nIndex; ++i) h = (h ^ d.pUnitsArray[i]->dwClassId) * 16777619u;
    r[32] = h; free(d.pUnitsArray);
}

// s3124: D2Common 0x7F5E0 = UNITS_GetDistanceToCoordinates over a synthetic unit; r = {type, nullPath, px, py, x, y, result} (D2VecDist7f5e0).
#include <Units/Units.h>
extern "C" EMSCRIPTEN_KEEPALIVE void d2_dist_7f5e0_probe(uint32_t* r) {
    D2UnitStrc u = {}; D2StaticPathStrc sp = {}; D2DynamicPathStrc dp = {};
    u.dwUnitType = r[0];
    if (r[0] == 2 || r[0] == 4 || r[0] == 5) { sp.tGameCoords.nX = (int32_t)r[2]; sp.tGameCoords.nY = (int32_t)r[3]; u.pStaticPath = &sp; }
    else if (!r[1]) { dp.tGameCoords.wPosX = (uint16_t)r[2]; dp.tGameCoords.wPosY = (uint16_t)r[3]; u.pDynamicPath = &dp; }
    r[6] = UNITS_GetDistanceToCoordinates(&u, (int32_t)r[4], (int32_t)r[5]);
}

// s3125: D2Common 0x4DA00 iso transform; r = {a, b, x, y} (D2VecIso4da00).
namespace d2common { void Iso4DA00(std::int32_t, std::int32_t, std::int32_t*, std::int32_t*); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_iso_4da00_probe(uint32_t* r) { d2common::Iso4DA00((int32_t)r[0], (int32_t)r[1], (int32_t*)&r[2], (int32_t*)&r[3]); }

// s3131: D2Common 0x6FAC0 = DATATBLS_GetAutomapCellId over the loaded AutoMap.txt; r = D2VecAutomapCell6fac0 (seed in/out, result).
#include <DataTbls/LevelsTbls.h>
extern "C" EMSCRIPTEN_KEEPALIVE void d2_automap_6fac0_probe(uint32_t* r) {
    gpAutomapSeed->nLowSeed = r[4]; gpAutomapSeed->nHighSeed = r[5];
    r[6] = (uint32_t)DATATBLS_GetAutomapCellId((uint32_t)r[0], (uint32_t)r[1], (int32_t)r[2], (int32_t)r[3]);
    r[7] = gpAutomapSeed->nLowSeed; r[8] = gpAutomapSeed->nHighSeed;
}

// s3132: D2Common 0x665C0 = UNITROOM_RefreshUnit over synthetic unit/path/room/act; r = D2VecRefresh665c0 (9 u32: inputs 0-4, outputs 5-8).
#include <Units/UnitRoom.h>
#include <Drlg/D2DrlgDrlg.h>
#include <Drlg/D2DrlgDrlgAnim.h>
extern "C" EMSCRIPTEN_KEEPALIVE void d2_refresh_665c0_probe(uint32_t* r) {
    D2UnitStrc u = {}, other = {}; D2StaticPathStrc sp = {}; D2DynamicPathStrc dp = {}; D2ActiveRoomStrc room = {}; D2DrlgActStrc act = {};
    const uint32_t type = r[0], mode = r[1]; act.bHasPendingUnitListUpdates = r[4];
    const bool st = type == 2 || type == 4 || type == 5;
    u.dwUnitType = type; u.dwFlags = r[2];
    if (mode) { if (st) u.pStaticPath = &sp; else u.pDynamicPath = &dp; }
    if (mode >= 2) {
        if (st) sp.pRoom = &room; else dp.pRoom = &room;
        room.pAct = &act;
        room.dwFlags = mode == 2 ? 0 : mode == 3 ? 4 : 5;
        if (r[3]) room.pUnitUpdate = &other;
    }
    UNITROOM_RefreshUnit(&u);
    r[5] = u.dwFlags; r[8] = act.bHasPendingUnitListUpdates;
    D2UnitStrc* nx = u.pChangeNextUnit; D2UnitStrc* hd = room.pUnitUpdate;
    r[6] = !nx ? 0 : nx == &other ? 1 : nx == &u ? 2 : 3;
    r[7] = !hd ? 0 : hd == &u ? 1 : hd == &other ? 2 : 3;
}

// s3133: D2Common 0x2D0F0 = QUESTRECORD_GetQuestState over a 16-byte buffer; r = D2VecQuestState2d0f0 (7 u32: buf 0-3, quest, state, result).
#include <D2QuestRecord.h>
extern "C" EMSCRIPTEN_KEEPALIVE void d2_queststate_2d0f0_probe(uint32_t* r) {
    D2BitBufferStrc rec = {}; rec.pBuffer = (uint8_t*)r;
    r[6] = QUESTRECORD_GetQuestState(&rec, (int)r[4], (int)r[5]) ? 1 : 0;
}

// s3067: D2Common 0x437C0 = COMPOSIT_IsArmorComponent; r = D2VecArmorComp437c0 (2 u32: component, result).
#include <D2Composit.h>
extern "C" EMSCRIPTEN_KEEPALIVE void d2_armorcomp_437c0_probe(uint32_t* r) { r[1] = (uint32_t)COMPOSIT_IsArmorComponent((int)r[0]); }

// s3130: D2Common 0x313F0 = UNITS_IsInMovingMode; r = D2VecMoving313f0 (6 u32: type, flags, classId, mode, seqMode, result).
#include <Units/Units.h>
extern "C" EMSCRIPTEN_KEEPALIVE void d2_moving_313f0_probe(uint32_t* r) {
    D2UnitStrc u = {}; u.dwUnitType = r[0]; u.dwClassId = r[2]; u.dwAnimMode = r[3]; u.dwSeqMode = r[4];
    if (r[1] & 1) u.pAnimSeq = (decltype(u.pAnimSeq))&u;
    r[5] = (uint32_t)UNITS_IsInMovingMode((r[1] & 2) ? nullptr : &u);
}

// s3134: D2Common 0x1930 MONSTERS_GetClassIdFromMonsterChain(id, depth) over the loaded monstats; r = D2VecChain1930 (id, depth, count, result).
#include <D2Monsters.h>
extern "C" EMSCRIPTEN_KEEPALIVE void d2_chain_1930_probe(int32_t* r) {
    r[2] = sgptDataTables->nMonStatsTxtRecordCount;
    r[3] = MONSTERS_GetClassIdFromMonsterChain(r[0], r[1]);
}

// s3135: D2Common 0x12D40 = DRLGANIM_UpdateFrameInAdjacentRooms; r = D2VecAnimAdj12d40 (30 u32: nullMask, n1, chain1[3], frame1[3], n2, chain2[3], init2[9], out2[9]); builds the room graph in our own struct layout.
#include <Drlg/D2DrlgDrlg.h>
#include <Drlg/D2DrlgDrlgAnim.h>
extern "C" EMSCRIPTEN_KEEPALIVE void d2_animadj_12d40_probe(uint32_t* r) {
    static D2DrlgRoomStrc rm1, rm2, n1r[3], n2r[3]; static D2DrlgRoomStrc* arr1[3]; static D2DrlgRoomStrc* arr2[3];
    static D2DrlgTileGridStrc g1[3], g2[3]; static D2DrlgAnimTileGridStrc a1[3], a2[3][3];
    memset(&rm1, 0, sizeof rm1); memset(&rm2, 0, sizeof rm2); memset(n1r, 0, sizeof n1r); memset(n2r, 0, sizeof n2r);
    memset(g1, 0, sizeof g1); memset(g2, 0, sizeof g2); memset(a1, 0, sizeof a1); memset(a2, 0, sizeof a2);
    const uint32_t nm = r[0], n1 = r[1], n2 = r[8]; const int32_t* c1 = (int32_t*)&r[2]; const int32_t* c2 = (int32_t*)&r[9];
    for (uint32_t i = 0; i < 3; ++i) {
        arr1[i] = &n1r[i]; arr2[i] = &n2r[i];
        if (i < n1 && c1[i] >= 0) { n1r[i].pTileGrid = &g1[i]; if (c1[i] > 0) { g1[i].pAnimTiles = &a1[i]; a1[i].nCurrentFrame = (int32_t)r[5 + i]; } }
        if (i < n2 && c2[i] >= 0) {
            n2r[i].pTileGrid = &g2[i];
            for (int j = 0; j < c2[i]; ++j) { if (j == 0) g2[i].pAnimTiles = &a2[i][0]; a2[i][j].nCurrentFrame = (int32_t)r[12 + 3 * i + j]; if (j + 1 < c2[i]) a2[i][j].pNext = &a2[i][j + 1]; }
        }
    }
    rm1.ppRoomsNear = arr1; rm1.nRoomsNear = (int32_t)n1; rm2.ppRoomsNear = arr2; rm2.nRoomsNear = (int32_t)n2;
    DRLGANIM_UpdateFrameInAdjacentRooms((nm & 1) ? nullptr : &rm1, (nm & 2) ? nullptr : &rm2);
    for (uint32_t i = 0; i < 3; ++i) for (int j = 0; j < 3; ++j) r[21 + 3 * i + j] = (j < c2[i]) ? (uint32_t)a2[i][j].nCurrentFrame : 0;
}

// s3233: D2Game 0x1620 = SUNITDMG_CheckMonType; r = D2VecGameCheckMonType1620 (type1, type2 in; result out).
#include <D2Game/UNIT/SUnitDmg.h>
#include "../../tables/shared.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2_checkmontype_1620_probe(int32_t* r) {
    if (!d2::drlgLoadTables(d2::sharedVfs(), d2::sharedLang())) { r[2] = 0x7EADBEEF; return; }   // montype nest comes from the vendored loaders
    r[2] = SUNITDMG_CheckMonType(r[0], r[1]);
}

// s3234: D2Game 0x1540 = SUNITDMG_GetMonStatsTxtRecordFromUnit; r = D2VecGameGetMonStats1540 (type, hasMon, hasUnit, idx in; result nId or -1 out).
#include <D2Game/UNIT/SUnitDmg.h>
extern "C" EMSCRIPTEN_KEEPALIVE void d2_getmonstats_1540_probe(int32_t* r) {
    static D2MonStatsTxt stats[8]; static D2MonsterDataStrc md; static D2UnitStrc u;
    for (int i = 0; i < 8; i++) stats[i].nId = (int16_t)i;
    md = {}; u = {}; md.pMonstatsTxt = &stats[r[3] & 7];
    u.dwUnitType = r[0]; u.pMonsterData = r[1] ? &md : nullptr;
    D2MonStatsTxt* p = SUNITDMG_GetMonStatsTxtRecordFromUnit(r[2] ? &u : nullptr);
    r[4] = p ? p->nId : -1;
}

// s3137: D2Common 0x181F0/0x18220/0x18240 hover-message accessors; r = D2VecHoverChat (in[64] at +0, out[64] at +64); mode 0 lang, 1 copy, 2 timeout.
#include <D2Chat.h>
// s3142: D2Common 0x2FD10 replay; r = {classId, out1, out2} (12-byte D2VecRunWalk2FD10).
#include <Units/Units.h>
static_assert(offsetof(D2CharStatsTxt, nWalkSpeed) == 0x40 && offsetof(D2CharStatsTxt, nRunSpeed) == 0x41, "charstats walk/run offsets");
extern "C" EMSCRIPTEN_KEEPALIVE void d2_runwalk_probe(uint32_t* r) {
    // charstats is not loaded in this boot: install a 7-record table carrying the captured bytes (walk 6, run 9).
    static D2CharStatsTxt rec[7];
    for (auto& e : rec) { e.nWalkSpeed = 6; e.nRunSpeed = 9; }
    D2CharStatsTxt* savedT = sgptDataTables->pCharStatsTxt; int savedN = sgptDataTables->nCharStatsTxtRecordCount;
    sgptDataTables->pCharStatsTxt = rec; sgptDataTables->nCharStatsTxtRecordCount = 7;
    int w = 0x7A7A7A7A, n = 0x7A7A7A7A;
    UNITS_GetRunAndWalkSpeedForPlayer(0, (int)r[0], &w, &n);
    r[1] = (uint32_t)w; r[2] = (uint32_t)n;
    sgptDataTables->pCharStatsTxt = savedT; sgptDataTables->nCharStatsTxtRecordCount = savedN;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_hoverchat_probe(uint8_t* r, int mode) {
    D2HoverTextStrc* h = (D2HoverTextStrc*)r;   // the 64-byte image; msg capped at 48 bytes by the corpus
    if (mode == 0) r[64] = CHAT_GetLangIdFromHoverMsg(h);
    else if (mode == 1) CHAT_CopyHoverMsgToBuffer(h, (char*)r + 64);
    else { uint32_t t = CHAT_GetTimeoutFromHoverMsg(h); memcpy(r + 64, &t, 4); }
}

// s3140: D2Common 0x1A990 MONSTERS_IsSandLeaper(unit, flag) with the monstats count pinned; r = D2VecSandLeaper1A990 (present, type, classId, count, flag, result).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_sandleaper_1a990_probe(int32_t* r) {
    D2UnitStrc u = {};
    u.dwUnitType = r[1]; u.dwClassId = r[2];
    const int32_t real = sgptDataTables->nMonStatsTxtRecordCount;
    sgptDataTables->nMonStatsTxtRecordCount = r[3];
    r[5] = MONSTERS_IsSandLeaper(r[0] ? &u : nullptr, r[4]);
    sgptDataTables->nMonStatsTxtRecordCount = real;
}

// s3141: D2Common 0x9C20 = DRLGPRESET_GetLevelPrestIdFromRoomEx; r = {in nLevelPrest, out result}.
#include <Drlg/D2DrlgPreset.h>
extern "C" EMSCRIPTEN_KEEPALIVE void d2_prestid_9c20_probe(uint32_t* r) {
    D2DrlgRoomStrc room = {}; D2DrlgPresetRoomStrc maze = {};
    maze.nLevelPrest = (int32_t)r[0]; room.pMaze = &maze;
    r[1] = (uint32_t)DRLGPRESET_GetLevelPrestIdFromRoomEx(&room);
}

// s3263: D2Game 0xB510 = OBJRGN_GetTrapMonsterId; r = D2VecObjRgnB510 (level, cachedIn, nMons, mons[8] in; ret, cachedOut out). Synthetic game/object/room/level chain.
#include <D2Game/OBJECTS/ObjRgn.h>
#include <D2Game/OBJECTS/ObjMode.h>
#include <D2Game/MONSTER/MonsterRegion.h>
#include <D2Game/GAME/Game.h>
extern "C" EMSCRIPTEN_KEEPALIVE void d2_objrgn_b510_probe(int32_t* r) {
    if (!d2::drlgLoadTables(d2::sharedVfs(), d2::sharedLang())) { r[11] = 0x7EADBEEF; return; }
    static D2GameStrc game; static D2ObjectControlStrc ctl; static D2ObjectRegionStrc oreg; static D2MonsterRegionStrc mreg;
    static D2UnitStrc obj; static D2DynamicPathStrc path; static D2ActiveRoomStrc room; static D2DrlgRoomStrc drlg; static D2DrlgLevelStrc lvl; static D2ObjOperateFnStrc op;
    game = {}; ctl = {}; oreg = {}; mreg = {}; obj = {}; path = {}; room = {}; drlg = {}; lvl = {}; op = {};
    const uint32_t L = (uint32_t)r[0];
    game.pObjectControl = &ctl; ctl.pObjectRegion[L] = &oreg; game.pMonReg[L] = &mreg;
    oreg.nTrapMonsterId = r[1];
    mreg.nMonCount = (uint8_t)r[2]; for (int i = 0; i < r[2] && i < 8; i++) mreg.pMonData[i].nMonHcIdx = (uint16_t)r[3 + i];
    obj.dwUnitType = UNIT_OBJECT; obj.pStaticPath = (D2StaticPathStrc*)&path; ((D2StaticPathStrc*)&path)->pRoom = &room;
    room.pDrlgRoom = &drlg; drlg.pLevel = &lvl; lvl.nLevelId = (int)L;
    op.pGame = &game; op.pObject = &obj;
    r[11] = OBJRGN_GetTrapMonsterId(&op); r[12] = oreg.nTrapMonsterId;
}
