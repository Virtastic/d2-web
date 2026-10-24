// Four UNMAPPED D2Gfx CRT leaves from the cached 1.13c decompiles at
// build-wasm/ghidra-d2gfx/decomp/0x{39AD,5867,7897,78A0}.c. Stage A batch:
// markers and host-call seams only; no specs, vectors, or harness rows here.
// Prior art: sibling MSVC CRT leaves in d2gfx_batch_b45.cpp and the D2Client
// type_info deleting-destructor pattern in d2client_batch_0312a.cpp.
#include <cstdint>
#include <emscripten.h>

// +0x10430, +0x1042C, +0x10434: saved ECX, EAX, and EBP for NLG unwind.
static uint32_t g_d2gfx_nlg_param;
static uint32_t g_d2gfx_nlg_eax;
static uint32_t g_d2gfx_nlg_ebp;

// 1.13c D2Gfx+0x039AD (__NLG_Notify1): fastcall ECX argument plus the
// interrupted EAX and EBP register values, saved to the module's NLG cells.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2gfx_39ad(uint32_t ecx, uint32_t eax, uint32_t ebp) {
    g_d2gfx_nlg_param = ecx;
    g_d2gfx_nlg_eax = eax;
    g_d2gfx_nlg_ebp = ebp;
}

// 1.13c D2Gfx+0x05867: release CRT lock 13 through _unlock (0x19BC).
extern "C" void d2gfx_sub_5867(void (*unlock)(uint32_t)) {
    unlock(13);
}

// 1.13c D2Gfx+0x07897: release CRT lock 14 through _unlock (0x19BC).
extern "C" void d2gfx_sub_7897(void (*unlock)(uint32_t)) {
    unlock(14);
}

extern "C" void d2gfx_sub_1000(
    void* allocation,
    void (*storm_free)(void*, const char*, uint32_t, uint32_t));

// 1.13c D2Gfx+0x078A0: run ~type_info (0x785A), conditionally pass this to
// operator delete (0x1000) when flags bit 0 is set, and return this either way.
extern "C" void* d2gfx_sub_78a0(
    void* self, uint32_t flags,
    void (*typeinfo_dtor)(void*),
    void (*storm_free)(void*, const char*, uint32_t, uint32_t)) {
    typeinfo_dtor(self);
    if ((flags & 1u) != 0) d2gfx_sub_1000(self, storm_free);
    return self;
}
