// src/core/gfx/fog_sub_1e360.cpp -- UNMAPPED 1.13c Fog.dll row reimplemented from the cached decompile
// build-wasm/ghidra-fog/decomp/0x1E360.c (alpha lane: no spec, no vector). Retail base 0x6FF50000.
//
// Fog+0x1E360 is the D2QSSocket wing's event wait. The object is FogQSSocket (layout in
// src/core/fog/fog_batch_1e3d0.cpp): +0x114 holds the handle CreateEventA returned. The 27-byte body
// tests the object, replaces the first stack argument with the handle read from +0x114 and tail-jumps
// through the WaitForSingleObject IAT slot at 0x6FF751A8. Ghidra typed the row `void` because of that
// indirect jump; the disassembly shows EAX is the tail call's result, and the null-object path leaves
// EAX = 0. __stdcall, two stack arguments.
//
// The kernel32 import is a host seam: wasm has no event objects, so with no seam installed the wait is
// a no-op returning WAIT_OBJECT_0 (0).
#include <cstdint>
#include <cstring>

#include <emscripten.h>

namespace {

struct FogSub1E360Api {
    uint32_t (*wait_object)(uint32_t handle, uint32_t milliseconds);  // WaitForSingleObject
};
FogSub1E360Api g_api{};

}  // namespace

// Install the WaitForSingleObject seam; nullptr restores the no-op.
extern "C" void fog_sub_1E360_seams(uint32_t (*wait_object)(uint32_t, uint32_t)) {
    g_api.wait_object = wait_object;
}

// 1.13c Fog+0x1E360 (ordinal 10076, 27 B, __stdcall; header records no D2 callers)
// Null object: return 0 without touching memory. Otherwise wait on *(HANDLE*)(object + 0x114) for
// `milliseconds` and return the wait result.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t fog_sub_1E360(uint32_t object, uint32_t milliseconds) {
    if (object == 0) return 0;
    uint32_t handle = 0;
    std::memcpy(&handle, reinterpret_cast<const void*>(static_cast<uintptr_t>(object + 0x114)),
                sizeof handle);
    if (g_api.wait_object == nullptr) return 0;
    return g_api.wait_object(handle, milliseconds);
}
