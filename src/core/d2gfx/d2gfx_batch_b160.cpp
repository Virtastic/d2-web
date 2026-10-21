// Prior art used: D2MOO's D2Gfx DllMain.cpp and D2Gfx.h cover the 1.10f module entry
// and graphics interface, but contain no implementations of these VC7.1 CRT helpers.
// Their rules below follow the cached 1.13c D2Gfx decompiles. The compatibility
// IsBadReadPtr shim remains the host seam for pointer validation.
#include <Windows.h>

#include <cstdint>
#include <cstring>

static uintptr_t g_d2gfx_window_handle; // DAT_6fa91264, module HWND at D2Gfx+0x11264

// 1.13c D2Gfx+0x0B160 (25 B, caller entry 0x01807): DllMain's process attach
// clears DAT_6fa91264; all reasons return TRUE. The module argument is unused.
extern "C" uint32_t d2gfx_sub_b160(void* module, int32_t reason) {
    (void)module;
    if (reason == 1) g_d2gfx_window_handle = 0;
    return 1;
}

extern "C" uintptr_t d2gfx_window_handle() {
    return g_d2gfx_window_handle;
}

// probe seam (checked against retail)
extern "C" void d2gfx_set_window_handle(uintptr_t v) { g_d2gfx_window_handle = v; }

// 1.13c D2Gfx+0x0BBEE (7 B, callers 0x0C2FC/0x0C0D4): the VC7.1
// _CallMemberFunction1 tail thunk installs pthis in ECX and jumps to the
// member target. In the host ABI, carry that this pointer as an explicit arg.
extern "C" void d2gfx_call_member_function1(void* pthis, void (*member)(void*)) {
    member(pthis);
}

struct D2GfxPurecallHost {
    int32_t* (*purecall_count)(); // resolves __getptd()->_purecall
};

// 1.13c D2Gfx+0x0C0B9 (27 B, caller 0x0C006): get the thread's pure-call
// counter, and when positive fetch it again and decrement it by one.
extern "C" void d2gfx_sub_c0b9(const D2GfxPurecallHost* host) {
    if (*host->purecall_count() > 0) --*host->purecall_count();
}

struct D2GfxPmd {
    int32_t member_displacement;
    int32_t vbtable_displacement;
    int32_t vbase_displacement;
};

struct D2GfxPmdHost {
    void* (*resolve_retail_address)(uint32_t address);
};

// 1.13c D2Gfx+0x0C119 (31 B, caller 0x0C2FC): implement the VS2003 PMD
// adjustment. pdisp < 0 is the non-virtual case; otherwise the vbtable entry
// is read at *(object + pdisp) + vdisp. The embedded vbtable slot is a retail
// uint32 address, not a wasm64 pointer; resolve it before reading the int32 entry.
// D2MOO D2Gfx has no CRT AdjustPointer body, so the 1.13c decompile settles this rule.
extern "C" void* d2gfx_adjust_pointer(void* object, const D2GfxPmd* pmd,
                                      const D2GfxPmdHost* host) {
    auto* bytes = static_cast<uint8_t*>(object);
    uint8_t* adjusted = bytes + pmd->member_displacement;
    if (pmd->vbtable_displacement >= 0) {
        uint32_t vbtable_address = 0;
        std::memcpy(&vbtable_address, bytes + pmd->vbtable_displacement, sizeof(vbtable_address));
        auto* vbtable = static_cast<const uint8_t*>(host->resolve_retail_address(vbtable_address));
        int32_t vbase = 0;
        std::memcpy(&vbase, vbtable + pmd->vbase_displacement, sizeof(vbase));
        adjusted += pmd->vbtable_displacement + vbase;
    }
    return adjusted;
}

// 1.13c D2Gfx+0x0C96A (28 B, callers 0x0C2FC/0x0C59D): return the inverse
// of IsBadReadPtr for the requested byte span.
extern "C" uint32_t d2gfx_validate_read(const void* pointer, uint32_t size) {
    return IsBadReadPtr(pointer, size) == 0;
}
