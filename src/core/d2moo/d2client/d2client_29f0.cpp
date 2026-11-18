// src/core/d2client/d2client_29f0.cpp -- 1.13c D2Client.dll+0x029F0 reimplemented from
// build-wasm/ghidra-d2client/decomp/0x29F0.c (alpha lane). Base 0x6FAB0000.
// Element constructor seam of the TSExplicitList<USGAMEDATA> at 0x2960: the list calls it on each new
// 0xC-byte slot (D2ClientBatch2960Env::construct, void(*)(void* elem)), so the retail argument is the
// element pointer and nothing else. Retail's only work is the 0x28A0 field init behind an SEH frame
// (puStack_8 / ExceptionList): the frame is compiler scaffolding, not state, so it is not modelled.
// 0x28A0 is recovered as d2_client_28a0 (int32_t*), defined in the 0x28A0 row; the call is unconditional
// for a non-null element, as retail's `if (param_1 != 0)` guard is the only branch.
#include <cstdint>
#include <emscripten.h>

// D2Client+0x028A0: initialise the 3-word intrusive-list field an element carries.
extern "C" void d2_client_28a0(int32_t* field);

// 1.13c D2Client+0x029F0 (d2client_29f0.cpp:d2_client_29f0)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_29f0(int32_t* elem) {
    if (elem != nullptr) d2_client_28a0(elem);
}
