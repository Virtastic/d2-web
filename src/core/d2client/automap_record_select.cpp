// Browser export of the automap cell choice: the row walk of 1.13c D2Common+0x6FAC0 (DATATBLS_GetAutomapCellId, s3131)
// over the loaded AutoMap table, without its seeded Cel roll. The client picks one of the row's Cels itself (the order
// retail rolls gpAutomapSeed in is not the client's), so the export names the row and lets the caller read its fields.
#include <cstdint>
#include <emscripten.h>
#include "../include/D2DataTbls.h"
#include "../include/DataTbls/LevelsTbls.h"

// The first row, in table order, whose level type, tile type, style and sequence match (Style/StartSequence 0xFF is
// "any", the bounds compare as unsigned bytes); -1 when the level type has no rows or no row matches.
extern "C" EMSCRIPTEN_KEEPALIVE int d2_client_automap_record(int levelType, int tileType, int style, int sequence) {
    if (!sgptDataTables || !sgptDataTables->pAutoMap || levelType <= 0 || levelType >= 36) return -1;
    const int start = sgptDataTables->pAutoMapRand[levelType].nStart, end = sgptDataTables->pAutoMapRand[levelType].nEnd;
    if (start == -1) return -1;
    for (int i = start; i < end; ++i) {
        const D2AutomapShortStrc& r = sgptDataTables->pAutoMap[i];
        if (r.dwAutomapLevelType != uint32_t(levelType) || r.dwAutomapTileType != uint32_t(tileType)) continue;
        if (uint8_t(r.nStyle) != 0xFF && uint32_t(uint8_t(r.nStyle)) != uint32_t(style)) continue;
        if (uint8_t(r.nStartSequence) != 0xFF && (int(uint8_t(r.nStartSequence)) > sequence || sequence > int(uint8_t(r.nEndSequence)))) continue;
        return i;
    }
    return -1;
}
// A field of row i: 0 Cel count, 1-4 Cel1..Cel4, 5 Style, 6 StartSequence, 7 EndSequence (signed, as the .txt reads).
extern "C" EMSCRIPTEN_KEEPALIVE int d2_client_automap_field(int i, int field) {
    if (!sgptDataTables || !sgptDataTables->pAutoMap || i < 0 || i >= sgptDataTables->nAutoMap) return -1;
    const D2AutomapShortStrc& r = sgptDataTables->pAutoMap[i];
    switch (field) {
    case 0: return r.nCels;
    case 1: case 2: case 3: case 4: return r.nCel[field - 1];
    case 5: return r.nStyle;
    case 6: return r.nStartSequence;
    case 7: return r.nEndSequence;
    }
    return -1;
}
