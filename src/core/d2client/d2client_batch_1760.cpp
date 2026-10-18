// src/core/d2client/d2client_batch_1760.cpp -- UNMAPPED 1.13c D2Client.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000; every global this file
// names is at its retail RVA. Register-argument functions keep retail's order in the parameter list.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>
#include "D2DataTbls.h"
#include "D2BitManip.h"

// The tables pointer D2Client reaches through its import slot +0x0CE474 (slot -> cell -> tables); the
// vendored sgptDataTables is that same object. 0x17B0 (skill_delay_17b0.cpp) and 0x1840
// (record_lookup_tables_b17.cpp) read the same tables.
static_assert(sizeof(D2MonStatsTxt) == 0x1A8, "monstats.txt record stride 0x1A8");
static_assert(offsetof(D2MonStatsTxt, dwMonStatsFlags) == 0x0C, "dwMonStatsFlags at +0x0C");
static_assert(sizeof(D2SkillsTxt) == 0x23C, "skills.txt record stride 0x23C");
static_assert(offsetof(D2SkillsTxt, wSkillDesc) == 0x194, "wSkillDesc at +0x194");
static_assert(sizeof(D2SkillDescTxt) == 0x120, "skilldesc.txt record stride 0x120");

// 1.13c D2Client+0x01760: EAX = monster id, EDX = flag bit. A bit of the monstats.txt record's
// dwMonStatsFlags (+0x0C) through gdwBitMasks (D2Client+0x0CE50); out of range or a zero record gives 0.
// The byte index is the arithmetic shift of the full 32-bit bit number, like retail's `sar edx,3`.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_1760(int32_t nMonsterId, int32_t nFlag) {
    if (nMonsterId < 0 || nMonsterId >= sgptDataTables->nMonStatsTxtRecordCount) return 0;
    const uint8_t* rec = reinterpret_cast<const uint8_t*>(sgptDataTables->pMonStatsTxt) +
                         (size_t)nMonsterId * sizeof(D2MonStatsTxt);
    if (rec == nullptr) return 0;
    uint8_t bits = rec[0xC + (nFlag >> 3)];
    return gdwBitMasks[nFlag & 7] & bits;
}

// 1.13c D2Client+0x017F0: EAX = skill id. The skills.txt row's wSkillDesc (+0x194, signed) indexes the
// skilldesc.txt array (stride 0x120); out of range or a negative/zero record gives NULL.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_fn_17f0(int32_t nSkillId) {
    if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount) return nullptr;
    const uint8_t* rec = reinterpret_cast<const uint8_t*>(sgptDataTables->pSkillsTxt) +
                         (size_t)nSkillId * sizeof(D2SkillsTxt);
    if (rec == nullptr) return nullptr;
    int32_t nDesc = static_cast<int16_t>(*reinterpret_cast<const uint16_t*>(rec + 0x194));
    if (nDesc < 0 || nDesc >= sgptDataTables->nSkillDescTxtRecordCount) return nullptr;
    return const_cast<void*>(static_cast<const void*>(
        reinterpret_cast<const uint8_t*>(sgptDataTables->pSkillDescTxt) +
        (size_t)nDesc * sizeof(D2SkillDescTxt)));
}

// 1.13c D2Client+0x019B0: stdcall, [esp+4] = p. Zero the five dwords at +0x00..+0x10.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_19b0(uint32_t* p) {
    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
    p[4] = 0;
}
