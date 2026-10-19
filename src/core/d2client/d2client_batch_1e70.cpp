// src/core/d2client/d2client_batch_1e70.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows reimplemented from
// the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000; every RVA below is
// retail's, and every global this file owns is a static at its retail RVA comment.
//
// Unrecovered internal callees are host seams (the src/core/d2client/cursor_init_166c0.cpp pattern): the
// function takes a D2ClientBatch1e70Env AFTER its retail arguments, which keep their order.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>
#include "native_list_abi.h"
#include <Windows.h>   // CRITICAL_SECTION no-ops and the deterministic GetTickCount

// Retail module state, all zero-initialised.
static uint8_t  g_csTable[0x200];       // +0x0DB0C8  FUN_6fab47ab's critical-section table, 8-byte stride
static uint8_t  g_pickTable[0x200];     // +0x1175C0..+0x1177C0  16 records of 0x20 (0x5CC10)
static uint32_t g_pickEnabled;          // +0x11C280  DAT_6fbcc280
static uint32_t g_tickGate;             // +0x0DD5D4  DAT_6fb8d5d4
static uint32_t g_tickDeadline;         // +0x0FAD50  DAT_6fbaad50
static uint32_t g_seh8dc;               // +0x0DB8DC  DAT_6fb8b8dc
static uint32_t g_seh8e0;               // +0x0DB8E0  DAT_6fb8b8e0
static uint32_t g_seh8e4;               // +0x0DB8E4  DAT_6fb8b8e4
static uint32_t g_slots10A608[8 * 0x20]; // +0x10A608  slot lists, row stride 0x80 (0xA5B00/0xA5B40)
static uint32_t g_slots109A08[4 * 0x80]; // +0x109A08  slot lists, row stride 0x200 (0xA7340)



// FUN_6fab47ab(idx): LeaveCriticalSection(&DAT_6fb8b0c8[idx * 2]) -- byte stride 8.
static void leave_cs(uint32_t idx) {
    LeaveCriticalSection((LPCRITICAL_SECTION)(g_csTable + (size_t)idx * 8));
}

// 1.13c D2Client+0x01E70: MSVC CRT lock teardown -- DeleteCriticalSection(cs), cs in EAX.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_1e70(LPCRITICAL_SECTION cs) {
    DeleteCriticalSection(cs);
}

// 1.13c D2Client+0x03121: tail-call FUN_6fab47ab(0xE).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_3121(void) {
    leave_cs(0xE);
}

// 1.13c D2Client+0x036F1: __cdecl wrapper for __fsopen(name, mode, _SH_DENYNO = 0x40).
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_36f1(const char* name, const char* mode,
                                                     const D2ClientBatch1e70Env* env) {
    return env->fsopen(name, mode, 0x40);
}

// 1.13c D2Client+0x03F7C: tail-call 0x3516, which calls FUN_6fab47ab(8).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_3f7c(void) {
    leave_cs(8);
}

// 1.13c D2Client+0x091F2: stores the entry EAX, [EBP+8] and EBP into the three SEH globals
// (0x0DB8E0 <- [EBP+8], 0x0DB8DC <- EAX, 0x0DB8E4 <- EBP), in that order.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_91f2(uint32_t eax, uint32_t ebp, uint32_t stack8) {
    g_seh8e0 = stack8;
    g_seh8dc = eax;
    g_seh8e4 = ebp;
}

// 1.13c D2Client+0x091E9 (9 B, __NLG_Notify1): the __fastcall sibling of 0x91F2 above -- stores the
// ECX argument, EAX and EBP into the same three SEH globals, in that order.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_91e9(uint32_t ecx, uint32_t eax, uint32_t ebp) {
    g_seh8e0 = ecx;
    g_seh8dc = eax;
    g_seh8e4 = ebp;
}

// 1.13c D2Client+0x213A0: __fastcall(int arg /*ECX*/, object /*EAX*/). If the object, the argument
// and the object's type word (*obj == 1) are all live, call 0x212F0(obj, arg, 4); else nothing.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_213a0(int32_t* obj, uint32_t arg,
                                                     const D2ClientBatch1e70Env* env) {
    if (obj != nullptr && arg != 0 && *obj == 1) env->dispatch(obj, arg, 4);
}

// 1.13c D2Client+0x5CC10: scan the 16 0x20-byte records of the pick table (base 0x1175C0, object
// pointer at +0x08, live at +0x0C); among the records whose object has +0x04 == key, return the one
// with the smallest object +0x34, or NULL. Ordinal 10044 gates each record. NULL when 0x11C280 == 0.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_5cc10(int32_t key, const D2ClientBatch1e70Env* env) {
    if (g_pickEnabled == 0) return nullptr;
    uint8_t* best = nullptr;
    for (uint32_t off = 8; off < 0x200; off += 0x20) {          // DAT_6fbc75c8 .. DAT_6fbc77c8
        if (env->ord10044() == 0) continue;
        int32_t* obj = *(int32_t**)(g_pickTable + off);
        if (obj == nullptr) continue;
        if (obj[1] != key) continue;                             // *(int *)(obj + 4) == key
        if (best == nullptr ||
            (uint32_t)obj[0x34 / 4] < (uint32_t)(*(int32_t**)(best + 8))[0x34 / 4])
            best = g_pickTable + off - 8;                        // record base = pointer slot - 8
    }
    return best;
}

// 1.13c D2Client+0xA5420: if the object's +0x64 list is non-empty, 0xA8630(pop everything) and clear
// +0x64. Object in EDI, untouched otherwise.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_a5420(void* obj, const D2ClientBatch1e70Env* env) {
    if (*(int32_t*)((uint8_t*)obj + 100) != 0) {
        env->removeAll(obj);
        *(int32_t*)((uint8_t*)obj + 100) = 0;
    }
}

// 1.13c D2Client+0xA5B00: __fastcall(row /*ECX*/, value /*EDX*/): 0xA6EF0(0x10A608 + row*0x80,
// value & 0x7F, value, row).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_a5b00(uint32_t row, uint32_t value,
                                                     const D2ClientBatch1e70Env* env) {
    env->insert((uint8_t*)g_slots10A608 + (size_t)row * 0x80, value & 0x7F, value, row);
}

// 1.13c D2Client+0xA5B40: __fastcall(key /*ECX*/, slot /*EDX*/, EAX = slot again):
// 0xA4E20(key, 0x10A608 + slot*0x80, slot). The found node is discarded (0xA4E20 exits on mismatch).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_a5b40(uint32_t key, uint32_t slot,
                                                     const D2ClientBatch1e70Env* env) {
    env->find(key, (uint8_t*)g_slots10A608 + (size_t)slot * 0x80, slot);
}

// 1.13c D2Client+0xA7340: __fastcall(row /*ECX*/, value /*EDX*/): 0xA6EF0(0x109A08 + row*0x200,
// value & 0x7F, value, row).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_a7340(uint32_t row, uint32_t value,
                                                     const D2ClientBatch1e70Env* env) {
    env->insert((uint8_t*)g_slots109A08 + (size_t)row * 0x200, value & 0x7F, value, row);
}

// 1.13c D2Client+0xBE2E0: while 0x0DD5D4 is set, arm 0x0FAD50 to GetTickCount() + 100.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_be2e0(void) {
    if (g_tickGate != 0) g_tickDeadline = GetTickCount() + 100;
}
