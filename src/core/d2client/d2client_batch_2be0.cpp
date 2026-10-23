// src/core/d2client/d2client_batch_2be0.cpp -- 1.13c D2Client.dll+0x2BE0 from
// build-wasm/ghidra-d2client/decomp/0x2BE0.c. Base 0x6FAB0000.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>
#include "D2DataTbls.h"

static_assert(sizeof(D2SkillsTxt) == 0x23C, "D2SkillsTxt stride 0x23C");
static_assert(offsetof(D2SkillsTxt, dwParam) == 0x148, "dwParam at 0x148, so its second dword is 0x14C");

// 1.13c D2Client+0x02BE0 (48 B): EAX = skill id. Signed index in [0, nSkillsTxtRecordCount@sgptDataTables+0xBA0)
// yields pSkillsTxt@+0xB98 + index * 0x23C; the non-null record's dword at +0x14C (dwParam[1]) is returned.
// Out of range or a null record address returns 0. Caller 0x14FD0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2c_skill_param1_2be0(int32_t index) {
    const D2DataTablesStrc* dt = sgptDataTables;
    if (index < 0 || index >= dt->nSkillsTxtRecordCount) return 0;
    const uintptr_t rec = (uintptr_t)dt->pSkillsTxt + (uint32_t)index * 0x23Cu;
    if (rec == 0) return 0;
    return *(const uint32_t*)(rec + 0x14C);
}
