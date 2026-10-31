// src/core/fog/fog_batch_tiny_1018c.cpp -- five 1.13c Fog.dll rows transcribed from the cached Ghidra
// decompiles (build-wasm/ghidra-fog/decomp/0x<RVA>.c), in RVA order. Alpha batch lane (OBJECTIVE v8.3):
// implementation + marker only, no spec, no vector. Retail base 0x6FF50000.
//
// All five are MSVC 2003 (VC7.1) Release CRT leaves the linker pulled into Fog.dll: the type_info
// scalar deleting destructor (+0x2BA3), the _fclose __unlock_file tail (+0x2C54), the per-thread
// error-code exchanger (+0x2D1D), and the two FlsAlloc-fallback leaves (+0x3889 _unlock(12),
// +0x3892 TlsAlloc thunk).
//
// Host model follows fog_batch_2000.cpp: the CRT lock table is kernel state, a no-op under wasm.
// Fog+0x68A0 (__unlock_file), +0x2B5D (~type_info) and +0x38CB (__getptd) are separate, still
// UNMAPPED rows kept as seams carrying only the contract this file depends on.
#include <cstddef>
#include <cstdint>

#include <Windows.h>   // CRITICAL_SECTION, LeaveCriticalSection: host state, a no-op under wasm
#include <emscripten.h>

// Storm #403 SMemFree: the free Fog+0x1000 (the operator-delete thunk 0x2BA3 calls) performs. It is
// already a weak seam in src/core/gfx/fog_batch_1000.cpp; this row reuses it.
extern "C" __attribute__((weak)) int32_t d2_fog_smem_free(void* location, const char* logname,
                                                          int32_t logline, uint32_t flags);

namespace {

// +0x2E210 DAT_6ff7e210: the CRT lock table, one 8-byte CRITICAL_SECTION per index (the decompile
// indexes it as dwords, hence `param_1 * 2`). External linkage: fog_batch_2000.cpp and
// fog_batch_tiny_1018b.cpp share this one table, as retail does.
}  // namespace
uint8_t g_fog_csTable[0x400];
namespace {

// Fog+0x3C3F: _unlock(index) -- LeaveCriticalSection on lock-table entry `index`.
void fog_unlock(int32_t index) {
    LeaveCriticalSection((LPCRITICAL_SECTION)(g_fog_csTable + (size_t)index * 8));
}

// Fog+0x2B5D (__thiscall ~type_info) is a separate, still-UNMAPPED row: store the vftable at +0,
// take lock 0xE, free the name block at +4 through _free (0x1FAD) when non-null, release the lock
// (0x2B9A). The scalar deleting destructor only needs the call; the row owns the body.
void fog_type_info_dtor(void* self) { (void)self; }

// Fog+0x68A0 (__unlock_file) body, inlined at the 0x2C54 tail. A FILE anywhere in the _iob window
// 0x6FF7E340..0x6FF7E5A0 (both ends inclusive) releases CRT lock 0x10 + (stream - base) / 0x20;
// anything else leaves the critical section at stream + sizeof(FILE) = +0x20. wasm64 stream pointers
// never land in that retail window, so the second arm is the one a host build takes.
void fog_unlock_file(uint8_t* stream) {
    const uintptr_t p = (uintptr_t)stream;
    if (p >= (uintptr_t)0x6FF7E340u && p <= (uintptr_t)0x6FF7E5A0u) {
        fog_unlock((int32_t)((p - (uintptr_t)0x6FF7E340u) >> 5) + 0x10);
        return;
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)(stream + 0x20));
}

// _ptiddata's +0x74 dword, the field 0x2D1D exchanges (Ghidra: _NLG_dwCode).
struct FogTiddataPrefix {
    uint8_t  pad[0x74];
    uint32_t nlgCode;   // +0x74
};
static_assert(offsetof(FogTiddataPrefix, nlgCode) == 0x74, "nlgCode at +0x74");

}  // namespace

// 1.13c Fog+0x2BA3
// type_info::`scalar deleting destructor'(flags): run ~type_info(this) (0x2B5D), then, when bit 0
// of the stack flags argument is set, hand the object to the operator-delete thunk (0x1000 ->
// Ordinal_403(this, "delete", -1, 0)). The object pointer is the result either way.
extern "C" EMSCRIPTEN_KEEPALIVE void* fog_sub_2BA3(void* self, uint32_t flags) {
    fog_type_info_dtor(self);
    if ((flags & 1u) != 0) d2_fog_smem_free(self, "delete", -1, 0);
    return self;
}

// 1.13c Fog+0x2C54
// The _fclose tail (0x2C0B's fourth step): forward the FILE* held in ESI to __unlock_file. The
// nine-byte thunk takes no stack argument of its own; ESI is the enclosing frame's _File, so it is
// the explicit parameter here.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_2C54(uint8_t* stream) {
    fog_unlock_file(stream);
}

// 1.13c Fog+0x2D1D
// __getptd()->_NLG_dwCode exchange: read the dword at +0x74, store `code`, return the previous
// value. The two __getptd calls stay in retail's order; caller 0x164A0 installs its debug report
// hook (0x6FF65EE0) through this and drops the old value.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t fog_sub_2D1D(uint32_t code,
                                                      FogTiddataPrefix* (*get_ptd)(void)) {
    FogTiddataPrefix* ptd = get_ptd();
    const uint32_t old = ptd->nlgCode;
    ptd = get_ptd();
    ptd->nlgCode = code;
    return old;
}

// 1.13c Fog+0x3889
// Nine-byte leaf used by ___updatetlocinfo (0x3857): _unlock(12).
extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_3889(void) {
    fog_unlock(0xC);
}

// 1.13c Fog+0x3892
// The FlsAlloc fallback (`call [TlsAlloc]; ret 4`): the callback argument is ignored and the
// imported allocator's index is returned in EAX to __mtinit (0x3AB2), which stores it at
// 0x6FF7E20C; on allocation failure that is TLS_OUT_OF_INDEXES (0xFFFFFFFF).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t fog_sub_3892(void* callback,
                                                      uint32_t (*tls_alloc)(void)) {
    (void)callback;
    return tls_alloc();
}
