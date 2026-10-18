#include <cstdint>
#include <cstddef>
#include <emscripten.h>

// Alpha-lane batch: the smallest UNMAPPED D2Client rows. Each function is decompile-derived; callees
// that are not recovered yet are passed in as callbacks or written to an out-list, so this file links
// without them. Globals are RVAs into the D2Client image (base 0x6FAB0000 in the 1.13c Ghidra project).

// 1.13c D2Client+0x678E0 (7 B): trampoline/vtable constructor, *ecx = pointer at RVA 0xD5048
// (PTR_LAB_6fb85048). The decompile is a single store; the pointed-to table is not read here.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_678e0(uint32_t* obj) { *obj = 0xD5048u; }

// 1.13c D2Client+0x72BA0 (10 B): tail-call thunk to D2Client+0x20A60, no arguments. w[0] := callee RVA.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_72ba0(uint32_t* w) { w[0] = 0x20A60u; }

// 1.13c D2Client+0x20FA0 (12 B): tail-call thunk to D2Client+0x20A60, no arguments. w[0] := callee RVA.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_20fa0(uint32_t* w) { w[0] = 0x20A60u; }

// 1.13c D2Client+0xCD148 (13 B): compare checked ECX against the caller's frame local [ebp-0x1c]; if they
// differ call D2Client+0x47AB with param in ECX. Returns 1 and fills w = {callee rva, ECX} when it calls.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_cd148(int32_t param, int32_t frame_m1c, uint32_t* w) {
    if (frame_m1c == param) return 0;
    w[0] = 0x47ABu;
    w[1] = (uint32_t)param;
    return 1;
}

// 1.13c D2Client+0x1E1D2 (16 B): switch case body, tail-call to D2Client+0xA53A0 with no arguments.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_1e1d2(uint32_t* w) { w[0] = 0xA53A0u; }

// 1.13c D2Client+0xCB5D2 (18 B): CRT __setdefaultprecision: __controlfp(0x10000, 0x30000), which forwards
// to __control87(0x10000, 0x30000 & 0xFFF7FFFF = 0x30000). There is no x87 control word in wasm; the call
// is recorded with its two arguments (w = {callee rva, new value, mask}).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_cb5d2(uint32_t* w) {
    w[0] = 0xCC655u;
    w[1] = 0x10000u;
    w[2] = 0x30000u;
}

// 1.13c D2Client+0x8A1A0 (19 B): CRT heap-release leaf. The block pointer arrives in EAX; when non-null it
// calls VirtualFree(p, 0, MEM_RELEASE=0x8000). With no block it returns without calling.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8a1a0(uint32_t block, void (*virtual_free)(uint32_t, uint32_t, uint32_t)) {
    if (block) virtual_free(block, 0, 0x8000);
}

// 1.13c D2Client+0x8A2B0 (19 B): identical CRT heap-release leaf (VirtualFree(p, 0, 0x8000) when EAX != 0).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8a2b0(uint32_t block, void (*virtual_free)(uint32_t, uint32_t, uint32_t)) {
    if (block) virtual_free(block, 0, 0x8000);
}

// 1.13c D2Client+0x8A3D0 (19 B): identical CRT heap-release leaf (VirtualFree(p, 0, 0x8000) when EAX != 0).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8a3d0(uint32_t block, void (*virtual_free)(uint32_t, uint32_t, uint32_t)) {
    if (block) virtual_free(block, 0, 0x8000);
}

// 1.13c D2Client+0xA5480 (19 B): if global [0x11BBFC] (DAT_6fbcbbfc) is non-null, return D2Common ordinal
// 10331 (D2Client thunk 0xC1F0) applied to it; else return 0.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_a5480(const uint32_t* g11bbfc, uint32_t (*ord_10331)(uint32_t)) {
    return *g11bbfc ? ord_10331(*g11bbfc) : 0u;
}

// 1.13c D2Client+0xA5B00 (20 B): fastcall(idx=ECX, value=EDX); forwards to D2Client+0xA6EF0 with
// (entry pointer = 0x10A608 + idx*0x80, value&0x7F, value, idx). tbl is the global array at RVA 0x10A608.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_a5b00(uint8_t* tbl, int32_t idx, uint32_t value,
    void (*callee)(uint8_t*, uint32_t, uint32_t, int32_t)) {
    callee(tbl + idx * 0x80, value & 0x7f, value, idx);
}

// 1.13c D2Client+0x1C720 (20 B): clear the global mode word at RVA 0x1223D8 (DAT_6fbd23d8) if it is 1.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_1c720(uint32_t* g1223d8) {
    if (*g1223d8 == 1) *g1223d8 = 0;
}

// 1.13c D2Client+0x108C0 (27 B, export ordinal 10003): fetch the D2Gfx
// window handle (D2Client thunk 0xD0B4 -> D2Gfx ordinal 10048); if non-null post USER32
// SendMessageA(hwnd, WM_ACTIVATEAPP=0x1C, FALSE, 0); then call D2Client thunk 0xD1D4 (D2Gfx ordinal 10003).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_108c0(
    uint32_t (*win_handle)(void),
    void (*send_message)(uint32_t hwnd, uint32_t msg, uint32_t wparam, uint32_t lparam),
    void (*ord_10003)(void)) {
    uint32_t hwnd = win_handle();
    if (hwnd) send_message(hwnd, 0x1c, 0, 0);
    ord_10003();
}
