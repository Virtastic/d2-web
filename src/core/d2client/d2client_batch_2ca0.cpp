// src/core/d2client/d2client_batch_2ca0.cpp -- 1.13c D2Client.dll batch from
// build-wasm/ghidra-d2client/decomp/0x{2CA0,2CD0,2D20,2E20,2E60,2E90,2EC0,2F00,2F70,2FF0,30A0,30E4}.c.
// Base 0x6FAB0000. Register arguments and SEH framing were resolved against the retail disassembly.
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <emscripten.h>

#include "D2DataTbls.h"

// Already implemented in this DLL; declarations only.
extern "C" void d2_client_12e0(int32_t count, void (*fn)(uint32_t), uint32_t obj, int32_t stride);
extern "C" void d2_client_3121(void);

static_assert(sizeof(D2SkillsTxt) == 0x23C, "D2SkillsTxt stride");
static_assert(offsetof(D2SkillsTxt, dwParam) == 0x148, "D2SkillsTxt::dwParam");

// Skills rows are addressed as sgptDataTables->pSkillsTxt + skillId * 0x23C.
static const uint8_t* skill_row(int32_t nSkill) {
    if (nSkill < 0 || nSkill >= sgptDataTables->nSkillsTxtRecordCount) return nullptr;
    return reinterpret_cast<const uint8_t*>(sgptDataTables->pSkillsTxt) +
           static_cast<uintptr_t>(nSkill) * sizeof(D2SkillsTxt);
}

// 1.13c D2Client+0x02CA0 (40 B): deleting destructor thunk. EAX = this, [esp+4] = flags, [esp+8] = the
// real destructor 0x38410, [esp+0xC] = the debug operator delete (0xBE6C). Runs the destructor, then, when
// (flags & 1) and this, frees this through "delete" with type 0xFFFFFFFF and line 0. Both callees are
// host seams: 0x38410 is not yet implemented and 0xBE6C is an import-recorder fold in the port.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2client_deleting_dtor_thunk(void* self, uint32_t flags, void (*dtor)(void*),
                                               void (*dbgFree)(void*, const char*, uint32_t, uint32_t)) {
    dtor(self);
    if ((flags & 1u) != 0 && self != nullptr) dbgFree(self, "delete", 0xFFFFFFFFu, 0);
    return self;
}

// 1.13c D2Client+0x2CD0 (56 B): EAX = skill id, EDX = level. Invalid id or null row or level <= 0 -> 0;
// otherwise (level - 1) * row->dwParam[1] + row->dwParam[0], 32-bit wrap.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2client_skill_param_by_level(int32_t nSkill, int32_t nLevel) {
    const uint8_t* row = skill_row(nSkill);
    if (row == nullptr || nLevel <= 0) return 0;
    const int32_t* param = reinterpret_cast<const int32_t*>(row + 0x148);
    return static_cast<int32_t>(static_cast<uint32_t>(nLevel - 1) *
                                    static_cast<uint32_t>(param[1]) +
                                static_cast<uint32_t>(param[0]));
}

// 1.13c D2Client+0x02D20 (48 B): EAX = skill id, EDX ignored. Invalid id or null row -> 0; otherwise
// row->dwParam[0] (the level-1 value).
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_skill_param0(int32_t nSkill) {
    const uint8_t* row = skill_row(nSkill);
    if (row == nullptr) return 0;
    return *reinterpret_cast<const int32_t*>(row + 0x148);
}

// 1.13c D2Client+0x2E20 (57 B): EAX = base, [esp+4] = the inline Unicode default constructor 0xCEB00.
// Builds six arrays of 300 elements of stride 2 at base+0x118 + i*0x26A with d2_client_12e0. Returns base.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_2e20(void* base, void (*unicodeDefaultCtor)(uint32_t)) {
    for (int32_t i = 0; i < 6; ++i) {
        d2_client_12e0(300, unicodeDefaultCtor,
                       static_cast<uint32_t>(reinterpret_cast<uintptr_t>(
                           static_cast<uint8_t*>(base) + 0x118 + static_cast<size_t>(i) * 0x26A)),
                       2);
    }
    return base;
}

// 1.13c D2Client+0x02E60 EAX = this, [esp+4] = the inline Unicode default constructor 0xCEB00.
// 300 constructor calls on consecutive 2-byte elements starting at this+7. Returns this.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2client_init_300_unicode_2e60(void* self, void (*unicodeDefaultCtor)(uint32_t)) {
    uint32_t p = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(self) + 7);
    for (int32_t i = 0; i < 300; ++i) {
        unicodeDefaultCtor(p);
        p += 2;
    }
    return self;
}

// 1.13c D2Client+0x2E90 (48 B): EAX = skill id, EDX ignored. Invalid id or null row -> 0; otherwise
// row->dwParam[5].
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t skillDataParam5(int32_t nSkill) {
    const uint8_t* row = skill_row(nSkill);
    if (row == nullptr) return 0;
    return reinterpret_cast<const int32_t*>(row + 0x148)[5];
}

// 1.13c D2Client+0x030E4: EDI = this. Type_info deleting destructor: publish the module vftable, free the
// mangled name at this+4, then run the critical-section leave 0x3121. __lock(0xE) is dropped (the engine
// is single-threaded here, as in the BNClient type_info destructor).
extern "C" const void* g_d2client_typeinfo_vftable[1];
const void* g_d2client_typeinfo_vftable[1];
extern "C"
void d2_client_30e4(void* self) {   // retail object: two 32-bit fields (vftable, name)
    if (self == nullptr) return;
    uint32_t* o = static_cast<uint32_t*>(self);
    o[0] = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_d2client_typeinfo_vftable));
    if (o[1] != 0) std::free(reinterpret_cast<void*>(static_cast<uintptr_t>(o[1])));
}
