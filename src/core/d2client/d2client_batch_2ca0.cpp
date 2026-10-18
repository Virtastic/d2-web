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
extern "C" void d2_client_2400(uint32_t base, uint32_t node_arg, uint32_t mode, uint32_t anchor_arg);
extern "C" void d2_client_3121(void);
extern "C" void d2_fn_d2client_30a0(uint32_t* self);

static_assert(sizeof(D2SkillsTxt) == 0x23C, "D2SkillsTxt stride");
static_assert(offsetof(D2SkillsTxt, dwParam) == 0x148, "D2SkillsTxt::dwParam");

// Skills rows are addressed as sgptDataTables->pSkillsTxt + skillId * 0x23C.
static const uint8_t* skill_row(int32_t nSkill) {
    if (nSkill < 0 || nSkill >= sgptDataTables->nSkillsTxtRecordCount) return nullptr;
    return reinterpret_cast<const uint8_t*>(sgptDataTables->pSkillsTxt) +
           static_cast<uintptr_t>(nSkill) * sizeof(D2SkillsTxt);
}

// 1.13c D2Client+0x2CA0 (112 B): deleting destructor thunk. EAX = this, [esp+4] = flags, [esp+8] = the
// real destructor 0x38410, [esp+0xC] = the debug operator delete (0xBE6C). Runs the destructor, then, when
// (flags & 1) and this, frees this through "delete" with type 0xFFFFFFFF and line 0. Both callees are
// host seams: 0x38410 is not yet implemented and 0xBE6C is an import-recorder fold in the port.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_fn_d2client_2ca0(void* self, uint32_t flags, void (*dtor)(void*),
                                               void (*dbgFree)(void*, const char*, uint32_t, uint32_t)) {
    dtor(self);
    if ((flags & 1u) != 0 && self != nullptr) dbgFree(self, "delete", 0xFFFFFFFFu, 0);
    return self;
}

// 1.13c D2Client+0x2CD0 (56 B): EAX = skill id, EDX = level. Invalid id or null row or level <= 0 -> 0;
// otherwise (level - 1) * row->dwParam[1] + row->dwParam[0], 32-bit wrap.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2client_2cd0(int32_t nSkill, int32_t nLevel) {
    const uint8_t* row = skill_row(nSkill);
    if (row == nullptr || nLevel <= 0) return 0;
    const int32_t* param = reinterpret_cast<const int32_t*>(row + 0x148);
    return static_cast<int32_t>(static_cast<uint32_t>(nLevel - 1) *
                                    static_cast<uint32_t>(param[1]) +
                                static_cast<uint32_t>(param[0]));
}

// 1.13c D2Client+0x2D20 (56 B): EAX = skill id, EDX ignored. Invalid id or null row -> 0; otherwise
// row->dwParam[0] (the level-1 value).
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2client_2d20(int32_t nSkill) {
    const uint8_t* row = skill_row(nSkill);
    if (row == nullptr) return 0;
    return *reinterpret_cast<const int32_t*>(row + 0x148);
}

// 1.13c D2Client+0x2E20 (62 B): EAX = base, [esp+4] = the inline Unicode default constructor 0xCEB00.
// Builds six arrays of 300 elements of stride 2 at base+0x118 + i*0x26A with d2_client_12e0. Returns base.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_fn_d2client_2e20(void* base, void (*unicodeDefaultCtor)(uint32_t)) {
    for (int32_t i = 0; i < 6; ++i) {
        d2_client_12e0(300, unicodeDefaultCtor,
                       static_cast<uint32_t>(reinterpret_cast<uintptr_t>(
                           static_cast<uint8_t*>(base) + 0x118 + static_cast<size_t>(i) * 0x26A)),
                       2);
    }
    return base;
}

// 1.13c D2Client+0x2E60: EAX = this, [esp+4] = the inline Unicode default constructor 0xCEB00.
// 300 constructor calls on consecutive 2-byte elements starting at this+7. Returns this.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_fn_d2client_2e60(void* self, void (*unicodeDefaultCtor)(uint32_t)) {
    uint32_t p = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(self) + 7);
    for (int32_t i = 0; i < 300; ++i) {
        unicodeDefaultCtor(p);
        p += 2;
    }
    return self;
}

// 1.13c D2Client+0x2E90 (46 B): EAX = skill id, EDX ignored. Invalid id or null row -> 0; otherwise
// row->dwParam[5].
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2client_2e90(int32_t nSkill) {
    const uint8_t* row = skill_row(nSkill);
    if (row == nullptr) return 0;
    return reinterpret_cast<const int32_t*>(row + 0x148)[5];
}

// Unlinks the list node stored at *link: node = link[0], encoded predecessor = link[1]; the predecessor
// is absolute (~enc) when enc is negative and a back-offset otherwise. Writes the successor into the
// predecessor and the encoding into node+4, then clears the pair. Shared by 0x2EC0/0x2FF0/0x30A0.
static void orphan_relink(uint32_t* link) {
    uint32_t node = link[0];
    if (node == 0) return;
    uint32_t enc = link[1];
    uint32_t target = (static_cast<int32_t>(enc) < 0)
                          ? ~enc
                          : static_cast<uint32_t>(reinterpret_cast<uintptr_t>(link)) +
                                (enc - *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(node) + 4));
    *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(target)) = node;
    *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(node) + 4) = enc;
    link[0] = 0;
    link[1] = 0;
}

// 1.13c D2Client+0x2EC0 (69 B): EDI = list object. Drains the owned list with 0x30A0, then unlinks the
// secondary pair at this+4 (same body as 0x2580).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_2ec0(void* self) {
    d2_fn_d2client_30a0(reinterpret_cast<uint32_t*>(self));
    orphan_relink(reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(self) + 4));
}

// 1.13c D2Client+0x2F00 (69 B): EDI = this. Initialises the embedded list head: [0] = 0, [1] = head at
// this+4, [2] = ~head. Retail stores the same head twice because the SEH bookkeeping store interleaves;
// the final state is the three values below.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_2f00(uint32_t* self) {
    uint32_t head = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(self) + 4);
    self[0] = 0;
    self[1] = head;
    self[2] = ~head;
}

// 1.13c D2Client+0x2F70: EAX = list base, [esp+4] = size, [esp+8] = line, ESI = mode,
// [esp+0xC] = debug operator new (0xBE66). Allocates size+0xC for "__AUTMESSAGESOURCE__" with type
// 0xFFFFFFFE and line|8, zeroes the node links, links it with 0x2400 when mode != 0, returns it.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_fn_d2client_2f70(
    uint32_t listBase, int32_t size, uint32_t line, uint32_t mode,
    void* (*dbgAlloc)(uint32_t, const char*, uint32_t, uint32_t)) {
    uint32_t* node = static_cast<uint32_t*>(dbgAlloc(
        static_cast<uint32_t>(size) + 0xCu, "__AUTMESSAGESOURCE__", 0xFFFFFFFEu, line | 8u));
    if (node == nullptr) return nullptr;
    node[0] = 0;
    node[1] = 0;
    if (mode != 0) d2_client_2400(listBase, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(node)), mode, 0);
    return node;
}

// 1.13c D2Client+0x2FF0 (68 B): EAX = pair. Single-node unlink of the pair at [0] (node) and [1]
// (encoded predecessor): see orphan_relink.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_2ff0(uint32_t* self) { orphan_relink(self); }

// 1.13c D2Client+0x30A0 (41 B): EDI = this. Drains an owned list: while (int)self[2] > 0, take the pair at
// self[0] + self[2], unlink its node and reload self[2] (retail reloads it on every pass, so a pair whose
// node is null does not advance). The zero-node path is retail's own loop shape.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_30a0(uint32_t* self) {
    while (static_cast<int32_t>(self[2]) > 0) {
        orphan_relink(reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(self[0] + self[2])));
    }
}

// 1.13c D2Client+0x30E4: EDI = this. Type_info deleting destructor: publish the module vftable, free the
// mangled name at this+4, then run the critical-section leave 0x3121. __lock(0xE) is dropped (the engine
// is single-threaded here, as in the BNClient type_info destructor).
static const void* g_d2client_typeinfo_vftable[1];
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_30e4(void* self) {
    if (self == nullptr) return;
    *reinterpret_cast<const void**>(self) = g_d2client_typeinfo_vftable;
    void* name = *reinterpret_cast<void**>(static_cast<uint8_t*>(self) + 4);
    if (name != nullptr) std::free(name);
    d2_client_3121();
}
