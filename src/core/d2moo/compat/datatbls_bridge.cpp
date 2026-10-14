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
