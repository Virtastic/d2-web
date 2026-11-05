// src/core/storm/storm_batch_19040.cpp -- 12 UNMAPPED 1.13c Storm.dll rows (0x19040, 0x19150,
// 0x191F0, 0x192A0, 0x19520, 0x1C580, 0x35CD0, 0x35DD0, 0x36020, 0x36120, 0x36290, 0x36680)
// reimplemented from the decompiles in build-wasm/ghidra-storm/decomp/0x<RVA>.c (alpha lane:
// marker only, no spec, no vector).
//
// These are Storm's DirectDraw surface front end (0x19040/0x19150/0x191F0/0x192A0/0x19520), the
// SCode pool allocator (0x1C580) and the SEVT command-table group (0x35CD0..0x36680). Retail globals
// are file statics below, each tagged with its 1.13c address; the SEVT nodes keep retail's 4-byte
// field layout. The unmapped internal callees and the gdi32/user32 imports are host seams on
// StormBatch19040Env, which every body takes AFTER its retail argument list (the
// src/core/storm/storm_batch_382c0.cpp pattern).
//
// Four Ghidra artifacts are resolved from the retail disassembly and named where they occur:
//   * 0x19150 really takes two stdcall arguments (ret 8); `unaff_retaddr` is its param_1 and the
//     stack-address compare is exactly `(local != g_draw[0]) == (param_1 != 0)`;
//   * 0x192A0's 0x1B-buffer result words are index 9 (status) and index 4 (value);
//   * 0x36020/0x36120/0x36680 pass their bucket-group in EDI / EBX (register seams below);
//   * 0x36290's lookup callee is 0x35C70 (ESI/EDX registers), not 0x25C70.
#include <cstdint>
#include <cstring>
#include <Windows.h>

#include "storm_batch_19040.h"

// SEVT command table (0x36290 et al.): shared with the FN probes through storm_batch_19040.h.
uint32_t g_evtTable;              // 0x6FC45370  hash buckets of group records
uint32_t g_evtSize;               // 0x6FC45374  bucket count (power of two)
uint32_t g_evtCount;              // 0x6FC45378  group-record count
CRITICAL_SECTION g_evtLock;       // 0x6FC458C8
int32_t  g_evtDirty;              // 0x6FC42F58  table changed since the last walk
uint32_t g_breakAnchor;           // 0x6FC458BC  list anchor handed to 0x1520
int32_t  g_evtDepth;              // 0x6FC4536C  dispatch recursion depth
uint32_t g_breakList;             // 0x6FC458C4  AUBREAKCMD list head

namespace {

int32_t  g_lastError;             // 0x6FC45388  last Storm error, mirrored to SetLastError
void*    g_display;               // 0x6FC453E4  display interface (vtable +0x38)
void*    g_paletteIface;          // 0x6FC453E8  palette interface (vtable +0x18/+0x10)
uint32_t g_palette[0x100];        // 0x6FC453F0  current palette entry table
void*    g_hwnd;                  // 0x6FC453D8  window
void*    g_hpal;                  // 0x6FC453DC  GDI palette
int32_t  g_captureDialog;         // 0x6FC453D4  non-zero while the save dialog is up
void*    g_draw[4];               // 0x6FC45804  primary/back/depth/capture interfaces
int32_t  g_rectLock;              // 0x6FC457F0  non-zero while the capture rect is held
uint32_t g_rect[4];               // 0x6FC457F4  capture rect {left,top,right,bottom}
uint32_t g_width;                 // 0x6FC41A20
uint32_t g_height;                // 0x6FC41A24

// SCode pool state (0x1C580).
uint8_t  g_heapReady;             // 0x6FC45168  non-zero once the pool is usable
uint32_t* g_heapTrack;            // 0x6FC45160  heap node to validate after an allocation
uint32_t g_heapLists[0x100];      // 0x6FC43150  per-id heap nodes (next at +0, id at +4)
CRITICAL_SECTION g_heapLocks[0x100]; // 0x6FC43558  0x18-byte lock per id
int32_t  g_heapDirty[0x100];      // 0x6FC44D60  per-id: list has freeable nodes
void     StormSeamVirtualFree(void*, uint32_t, uint32_t) {} // 0x6FC3323C, node is released by the host

// SEVT command table (0x36290 et al.).

// Retail record layouts (4-byte fields, as in 0x36290).
struct EvtNode   { uint32_t key, seq, handler, next; };        // 0x10
struct EvtGroup  { uint32_t buckets, size, used, next; };      // 0x10
struct EvtRecord { uint32_t k1, k2, seq, group, next; };       // 0x14

void StormSetLastError(uint32_t e) { g_lastError = (int32_t)e; }

// (**(code **)(*obj + off))(obj, ...) -- retail calls the object's vtable slot.
template <typename Fn>
Fn vcall(void* obj, uintptr_t off) {
    return (Fn)(*(void***)obj)[off / sizeof(void*)];
}

template <typename T>
T* ptr(uint32_t v) { return (T*)(uintptr_t)v; }
uint32_t val(const void* p) { return (uint32_t)(uintptr_t)p; }

} // namespace

// 1.13c Storm+0x19040 (stdcall, ret 0x10): flip/clear a surface. Arguments (idx, rect, count, rects). Indices 0..3 only; while the capture rect
// is held a primary flip is routed to the depth interface (index 3). A zero result from the flip releases the rect and blits the primary back
// from the capture surface: `count` rects of 16 bytes from `rects` when count != 0, else the stored rect g_rect. (The decompile names the loop
// count and base after param_1 / param_2; the disassembly reads them from the third and fourth stack words.)
extern "C"
uint32_t Storm_Sub19040(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4,
                        const StormBatch19040Env* env) {
    (void)env;
    if ((int32_t)param_1 < 0 || param_1 > 3) {
        StormSetLastError(0x57);
        return 0;
    }
    if (g_display == nullptr || g_draw[param_1] == nullptr) return 0;

    uint32_t iVar2 = param_1;
    if (param_1 == 0 && g_rectLock != 0) iVar2 = 3;
    uint32_t iVar1 = vcall<uint32_t (*)(void*, uint32_t)>(g_draw[iVar2], 0x80)
                         (g_draw[iVar2], param_2);
    if (iVar1 == 0) {
        if (iVar2 == 3 && g_rectLock != 0) {
            g_rectLock = 0;
            if (g_draw[0] != nullptr) {
                using Blt = void (*)(void*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
                if (param_3 != 0) {
                    do {
                        vcall<Blt>(g_draw[0], 0x14)(g_draw[0], param_4, val(g_draw[3]), param_4, 0x1000000, 0);
                        param_4 += 0x10;
                        param_3--;
                    } while (param_3 != 0);
                    return 1;
                }
                vcall<Blt>(g_draw[0], 0x14)(g_draw[0], val(g_rect), val(g_draw[3]), val(g_rect), 0x1000000, 0);
            }
        }
        return 1;
    }
    if (iVar2 == 3) {
        g_rectLock = 0;
        return 0;
    }
    return 0;
}

// 1.13c Storm+0x19150: begin the update pump. The display interface is asked to flush into a
// caller-word, and the pump runs only when that word's relation to the primary surface matches
// param_1: param_1 != 0 wants "changed", param_1 == 0 wants "unchanged". param_2 != 0 also blits the primary into the back surface.
extern "C"
uint32_t Storm_Sub19150(uint32_t param_1, uint32_t param_2, const StormBatch19040Env* env) {
    if (g_captureDialog != 0 || g_display == nullptr || g_draw[0] == nullptr ||
        g_draw[1] == nullptr) {
        return 0;
    }
    uint32_t local_4 = 0;
    vcall<void (*)(void*, uint32_t*)>(g_display, 0x38)(g_display, &local_4);
    uint32_t changed = (local_4 != val(g_draw[0])) ? 1u : 0u;
    if (changed == (param_1 != 0 ? 1u : 0u)) {
        if (param_2 != 0) {   // retail tests the second stack word here (esi was popped), not param_1
            vcall<void (*)(void*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)>(g_draw[1], 0x14)
                (g_draw[1], 0, val(g_draw[0]), 0, 0, 0);
        }
        return env->f_18f00();
    }
    return 1;
}

// 1.13c Storm+0x191F0: upload the palette table to the palette interface and, when the window is
// actually visible, realize it on its DC. Returns 1 only when the interface accepted the palette.
extern "C"
uint32_t Storm_Sub191F0(const StormBatch19040Env* env) {
    if (g_display == nullptr || g_paletteIface == nullptr) return 0;
    uint32_t iVar1 = vcall<uint32_t (*)(void*, uint32_t, uint32_t, uint32_t, void*)>
                         (g_paletteIface, 0x18)(g_paletteIface, 0, 0, 0x100, g_palette);
    if (g_hpal != nullptr) {
        env->set_palette_entries(g_hpal, 0, 0x100, g_palette);
        if (g_hpal != nullptr && g_hwnd != nullptr) {
            if (env->f_18fe0(g_hwnd) != 0) {
                void* hdc = env->get_dc(g_hwnd);
                env->select_palette(hdc, g_hpal, 0);
                env->realize_palette(hdc);
                env->release_dc(g_hwnd, hdc);
            }
        }
    }
    return (iVar1 == 0) ? 1 : 0;
}

extern "C"
uint32_t Storm_Sub192A0(uint32_t param_1, uint32_t* param_2, uint32_t* param_3, uint32_t* param_4,
                        uint32_t param_5, const StormBatch19040Env* env);

// 1.13c Storm+0x192A0: blit a source rectangle onto a surface via the 0x64 interface call, retrying
// while the device reports "still drawing". Status lives in buffer word 9 and the value in word 4.
// When the primary surface is missing and the capture rect is free, the rect is recorded and the
// call recurses once for index 3.
extern "C"
uint32_t Storm_Sub192A0(uint32_t param_1, uint32_t* param_2, uint32_t* param_3, uint32_t* param_4,
                        uint32_t param_5, const StormBatch19040Env* env) {
    if (param_3 != nullptr) *param_3 = 0;
    if (param_4 != nullptr) *param_4 = 0;

    if ((int32_t)param_1 < 0 || param_1 > 3 || param_3 == nullptr) {
        StormSetLastError(0x57);
        return 0;
    }
    if (g_display == nullptr || g_draw[param_1] == nullptr) return 0;

    uint32_t local_6c[0x1b];
    std::memset(local_6c, 0, sizeof(local_6c));
    local_6c[0] = 0x6c;

    uint32_t iVar1;
    do {
        void* surf = g_draw[param_1];
        iVar1 = vcall<uint32_t (*)(void*, uint32_t*, uint32_t*, uint32_t, uint32_t)>(surf, 0x64)
                    (surf, param_2, local_6c, 1, 0);
        if (iVar1 == 0x887601C2u) {
            if (g_draw[0] == nullptr) goto lab_93bc;
            if (vcall<uint32_t (*)(void*)>(g_draw[0], 0x6c)(g_draw[0]) != 0) return 0;
            env->invalidate_rect(g_hwnd, nullptr, 0);
        } else if (iVar1 != 0 && iVar1 != 0x8876021Cu) {
            goto lab_93bc;
        }
    } while (local_6c[9] == 0);

    *param_3 = local_6c[9];
    if (param_4 != nullptr) *param_4 = local_6c[4];
    return 1;

lab_93bc:
    if (local_6c[9] != 0) {
        *param_3 = local_6c[9];
        if (param_4 != nullptr) *param_4 = local_6c[4];
        return 1;
    }
    if (param_1 == 0 && g_draw[3] != nullptr && g_rectLock == 0) {
        if (param_2 == nullptr) {
            g_rect[0] = 0;
            g_rect[1] = 0;
            g_rect[2] = g_width;
            g_rect[3] = g_height;
        } else {
            g_rect[0] = param_2[0];
            g_rect[1] = param_2[1];
            g_rect[2] = param_2[2];
            g_rect[3] = param_2[3];
        }
        g_rectLock = Storm_Sub192A0(3, param_2, param_3, param_4, param_5, env);
        return g_rectLock;
    }
    return 0;
}

// 1.13c Storm+0x19520: set a range of palette entries. A real-colour mode (param_4 != 0) skips the
// 0..0x100 range fixups that move a leading zero entry to index 1 and drop the last one at 0x100.
// The palette is re-uploaded, and the window is only realized when it is visible and not iconic.
extern "C"
uint32_t Storm_Sub19520(uint32_t param_1, uint32_t param_2, uint32_t* param_3, uint32_t param_4,
                        const StormBatch19040Env* env) {
    if (param_1 + param_2 > 0x100u || param_3 == nullptr) {
        StormSetLastError(0x57);
        return 0;
    }
    if (g_display == nullptr || g_paletteIface == nullptr) return 0;

    if (param_1 == 0 && param_4 == 0) {
        param_2--;
        param_1 = 1;
        param_3++;
    }
    if (param_1 + param_2 == 0x100 && param_4 == 0) param_2--;
    uint32_t* dst = &g_palette[param_1];
    for (; param_2 != 0; param_2--) *dst++ = *param_3++;

    env->f_18f90();
    if (g_hwnd != nullptr) {
        if (env->is_window_visible(g_hwnd) != 0) {
            if (env->is_iconic(g_hwnd) == 0) goto lab_95e6;
        }
        return 1;
    }
lab_95e6:
    return Storm_Sub191F0(env);
}

// 1.13c Storm+0x1A600: sweep the SCode pool. Clears the tracked heap, then for every dirty id unlinks
// and releases each node whose use count (+0x14) is zero and whose size (+4) is below 0x80000000.
// Words are retail's 32-bit ones: dirty / lists are the 0x100-entry tables, each list slot a 32-bit node address.
struct StormSweepEnv {
    void (*untrack)();                  // DAT_6FC45160 := 0
    uint32_t* dirty;                    // 0x6FC44D60
    uint32_t* lists;                    // 0x6FC43150
    void (*enter)(int id);              // EnterCriticalSection(lock id)
    void (*leave)(int id);
    void (*release)(void* node);        // VirtualFree(node, 0, MEM_RELEASE)
};
extern "C"
void Storm_Sub1A600Env(const StormSweepEnv& e) {
    e.untrack();
    for (int i = 0; i < 0x100; i++) {
        if (e.dirty[i] == 0) continue;
        e.enter(i);
        e.dirty[i] = 0;
        uint32_t* link = &e.lists[i];
        while (*link != 0) {
            uint32_t* n = ptr<uint32_t>(*link);
            if (n[5] == 0 && n[1] < 0x80000000u) {
                *link = n[0];
                e.release(n);
            } else {
                link = n;
            }
        }
        e.leave(i);
    }
}

extern "C"
void Storm_Sub1A600() {
    static uint32_t lists[0x100];
    for (int i = 0; i < 0x100; i++) lists[i] = g_heapLists[i];
    static uint32_t dirty[0x100];
    for (int i = 0; i < 0x100; i++) dirty[i] = (uint32_t)g_heapDirty[i];
    StormSweepEnv e{ [] { g_heapTrack = nullptr; }, dirty, lists,
                     [](int i) { EnterCriticalSection(&g_heapLocks[i]); },
                     [](int i) { LeaveCriticalSection(&g_heapLocks[i]); },
                     [](void* n) { StormSeamVirtualFree(n, 0, 0x8000); } };
    Storm_Sub1A600Env(e);
    for (int i = 0; i < 0x100; i++) { g_heapLists[i] = lists[i]; g_heapDirty[i] = (int32_t)dirty[i]; }
}

// 1.13c Storm+0x1C580: SCode pool allocate. The pool id selects a 256-entry heap list guarded by its
// own lock. The allocator is fatal when the pool is not up; the request is retried through 0x1BDD0,
// and 0x1A600 validates the tracked heap after the lock is dropped.
extern "C"
uint32_t* Storm_Sub1C580Env(uint32_t param_1, const char* param_2, int32_t param_3, uint32_t param_4,
                         const StormBatch19040Env* env) {
    (void)param_2;
    if (g_heapReady == 0) {
        env->f_2f590(0x8510007Du, "SMemAlloc()", -1, nullptr, 0, 1);
        env->exit_process(1);
    }
    uint32_t uVar1 = env->f_1ad80();
    uint32_t uVar4 = uVar1 & 0xff;
    EnterCriticalSection(&g_heapLocks[uVar4]);
    uint32_t* puVar2 = ptr<uint32_t>(g_heapLists[uVar4]);
    for (; puVar2 != nullptr; puVar2 = ptr<uint32_t>(*puVar2)) {
        if (puVar2[1] == uVar1) break;
    }
    if (puVar2 == nullptr) {
        puVar2 = env->f_1bdd0(param_3, uVar1, (int32_t)uVar4, 0x1000, 0x1000, 0x10000);
    }
    uint32_t* puVar3 = env->f_1c390(param_4, param_1);
    if (uVar4 != 0xffffffff) LeaveCriticalSection(&g_heapLocks[uVar4]);
    if (g_heapTrack != nullptr && g_heapTrack != puVar2) env->f_1a600();
    return puVar3;
}

// SEVT allocations go through the 0x1C580 seam when the caller supplies one (the FN probes log the call), else the pool allocator.
static uint32_t* evtAlloc(uint32_t size, const char* file, int32_t line, uint32_t flags, const StormBatch19040Env* env) {
    if (env->f_1c580) return env->f_1c580(size, file, line, flags);
    return Storm_Sub1C580Env(size, file, line, flags, env);
}

// 1.13c Storm+0x35CD0: remove every bucket node of command param_3 (optionally only those whose
// handler is param_4) from the group record's bucket array, freeing each node with 0x1050.
extern "C" void Storm_Sub1050(void* p);   // 0x1050 SMemFree(p, "delete", -1)
extern "C"
uint32_t Storm_Sub35CD0_Real(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4) {
    uint32_t local_4 = 0;
    EnterCriticalSection(&g_evtLock);
    if (g_evtTable != 0 && g_evtSize != 0) {
        uint32_t* puVar1 = ptr<uint32_t>(ptr<uint32_t>(g_evtTable)[(param_1 ^ param_2) & (g_evtSize - 1)]);
        while (puVar1 != nullptr && (*puVar1 != param_1 || puVar1[1] != param_2)) {
            puVar1 = ptr<uint32_t>(puVar1[4]);
        }
        if (puVar1 != nullptr) {
            EvtGroup* puVar3 = ptr<EvtGroup>(puVar1[3]);
            if (puVar3->buckets != 0 && puVar3->size != 0) {
                uint32_t* puVar2 = &ptr<uint32_t>(puVar3->buckets)[(puVar3->size - 1) & param_3];
                for (uint32_t uVar3 = *puVar2; uVar3 != 0;) {
                    uint32_t* puVar4 = ptr<uint32_t>(uVar3);
                    if (puVar4[0] == param_3 && (param_4 == 0 || puVar4[2] == param_4)) {
                        *puVar2 = puVar4[3];
                        Storm_Sub1050(puVar4);
                        g_evtDirty = 1;
                        local_4 = 1;
                        puVar3 = ptr<EvtGroup>(puVar1[3]);
                        puVar3->used--;
                    } else {
                        puVar2 = &puVar4[3];
                    }
                    uVar3 = *puVar2;
                }
            }
        }
    }
    LeaveCriticalSection(&g_evtLock);
    return local_4;
}
// Env wrapper kept for existing callers; the body is the real extern "C" above.
extern "C"
uint32_t Storm_Sub35CD0(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4,
                        const StormBatch19040Env*) {
    return Storm_Sub35CD0_Real(param_1, param_2, param_3, param_4);
}

// 1.13c Storm+0x35DD0: dispatch command param_3 for the group keyed by (param_1, param_2). Nodes of
// the same command are ordered by their insertion sequence and every duplicate handler is run in
// turn; AUBREAKCMD records with the command id are drained from the break list on the way out.
extern "C"
uint32_t Storm_Sub35DD0(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4,
                        const StormBatch19040Env* env) {
    InterlockedIncrement((volatile long*)&g_evtDepth);
    uint32_t local_8 = 0;
    uint32_t local_4 = 0xffffffff;
    uint32_t* puVar4 = nullptr;

lab_5e00:
    EnterCriticalSection(&g_evtLock);
    {
        uint32_t head = (g_breakList >= 1) ? g_breakList : 0;
        for (uint32_t* piVar3 = ptr<uint32_t>(head); (int32_t)(uintptr_t)piVar3 > 0;
             piVar3 = ptr<uint32_t>(piVar3[1])) {
            if (piVar3[2] == param_4) {
                env->f_4d60(piVar3);
                env->f_1b960(piVar3, "__AUBREAKCMD__", -2);
                LeaveCriticalSection(&g_evtLock);
                goto lab_5f51;
            }
        }
    }
    {
        bool haveCandidate = false;
        if (puVar4 == nullptr || g_evtDirty != 0) {
            puVar4 = nullptr;
            if (g_evtTable != 0 && g_evtSize != 0) {
                uint32_t* puVar1 = ptr<uint32_t>(
                    ptr<uint32_t>(g_evtTable)[(param_1 ^ param_2) & (g_evtSize - 1)]);
                for (; puVar1 != nullptr; puVar1 = ptr<uint32_t>(puVar1[4])) {
                    if (*puVar1 == param_1 && puVar1[1] == param_2) {
                        EvtGroup* puVar2 = ptr<EvtGroup>(puVar1[3]);
                        if (puVar2->buckets != 0 && puVar2->size != 0) {
                            puVar4 = ptr<uint32_t>(
                                ptr<uint32_t>(puVar2->buckets)[(puVar2->size - 1) & param_3]);
                            haveCandidate = true;
                        }
                        break;
                    }
                }
            }
        }
        if (haveCandidate) {
            for (; puVar4 != nullptr; puVar4 = ptr<uint32_t>(puVar4[3])) {
                if (*puVar4 == param_3 && puVar4[1] < local_4) break;
            }
            if (g_evtDepth == 1) g_evtDirty = 0;
        }
    }
    {
        void (*pcVar5)(uint32_t) = nullptr;
        if (puVar4 != nullptr) {
            local_4 = puVar4[1];
            pcVar5 = (void (*)(uint32_t))(uintptr_t)puVar4[2];
            do {
                puVar4 = ptr<uint32_t>(puVar4[3]);
                if (puVar4 == nullptr) break;
            } while (*puVar4 != param_3);
        }
        LeaveCriticalSection(&g_evtLock);
        if (pcVar5 != nullptr) {
            local_8 = 1;
            if (env->call_handler) env->call_handler((uint32_t)(uintptr_t)pcVar5, param_4);
            else pcVar5(param_4);
        }
        if (puVar4 != nullptr) goto lab_5e00;
    }

lab_5f51:
    InterlockedDecrement((volatile long*)&g_evtDepth);
    if ((int32_t)g_breakList > 0) {
        EnterCriticalSection(&g_evtLock);
        uint32_t uVar2 = g_breakList;
        while ((int32_t)(uintptr_t)uVar2 > 0) {
            if (*(int32_t*)(uintptr_t)(uVar2 + 8) == (int32_t)param_4) {
                uVar2 = env->f_1520((void*)(uintptr_t)uVar2);
            } else {
                uVar2 = *(uint32_t*)(uintptr_t)(uVar2 + 4);
            }
        }
        LeaveCriticalSection(&g_evtLock);
    }
    return local_8;
}

// 1.13c Storm+0x36020: drop the group record for (param_1, param_2) and free every bucket-group in
// its chain through 0x35FC0, then unlink and free the record itself.
extern "C"
uint32_t Storm_Sub36020(uint32_t param_1, uint32_t param_2, const StormBatch19040Env* env) {
    uint32_t uVar4 = 0;
    EnterCriticalSection(&g_evtLock);
    if (g_evtTable != 0 && g_evtSize != 0) {
        uint32_t* puVar3 = ptr<uint32_t>(
            ptr<uint32_t>(g_evtTable)[(g_evtSize - 1) & (param_1 ^ param_2)]);
        while (puVar3 != nullptr && (*puVar3 != param_1 || puVar3[1] != param_2)) {
            puVar3 = ptr<uint32_t>(puVar3[4]);
        }
        if (puVar3 != nullptr) {
            uint32_t uVar2 = puVar3[3];
            while (uVar2 != 0) {
                puVar3[3] = *(uint32_t*)(uintptr_t)(uVar2 + 0xc);
                env->f_35fc0((void*)(uintptr_t)uVar2);
                uVar2 = puVar3[3];
            }
            uint32_t uVar8 = (g_evtSize - 1) & (param_1 ^ param_2);
            uint32_t* puVar5 = &ptr<uint32_t>(g_evtTable)[uVar8];
            uVar2 = *puVar5;
            while (uVar2 != 0) {
                uint32_t* puVar1 = ptr<uint32_t>(*puVar5);
                if (puVar1 == puVar3) {
                    *puVar5 = puVar1[4];
                    env->f_1050(puVar1);
                    g_evtCount--;
                } else {
                    puVar5 = &puVar1[4];
                }
                uVar2 = *puVar5;
            }
            uVar4 = 1;
            g_evtDirty = 1;
        }
    }
    LeaveCriticalSection(&g_evtLock);
    return uVar4;
}

// 1.13c Storm+0x36120: retire one bucket-group of the record for (param_1, param_2). The head group
// is freed when a successor exists; the last group takes the whole record out through 0x36020.
extern "C"
uint32_t Storm_Sub36120(uint32_t param_1, uint32_t param_2, const StormBatch19040Env* env) {
    uint32_t local_4 = 0;
    EnterCriticalSection(&g_evtLock);
    if (g_evtTable != 0 && g_evtSize != 0) {
        uint32_t* puVar1 = ptr<uint32_t>(
            ptr<uint32_t>(g_evtTable)[(param_1 ^ param_2) & (g_evtSize - 1)]);
        for (; puVar1 != nullptr; puVar1 = ptr<uint32_t>(puVar1[4])) {
            if (*puVar1 == param_1 && puVar1[1] == param_2) {
                uint32_t uVar2 = *(uint32_t*)(uintptr_t)(puVar1[3] + 0xc);
                if (uVar2 == 0) {
                    local_4 = Storm_Sub36020(param_1, param_2, env);
                } else {
                    env->f_35fc0((void*)(uintptr_t)puVar1[3]);
                    puVar1[3] = uVar2;
                }
                break;
            }
        }
    }
    g_evtDirty = 1;
    LeaveCriticalSection(&g_evtLock);
    return local_4;
}

// 1.13c Storm+0x36290: register command param_3 with handler param_5 under key (param_1, param_2).
// The record table doubles when it is half full; each record's bucket array doubles when half full,
// rehashing the bucket-node chains into fresh arrays. The record's word 2 is a per-record insertion
// counter, so each node carries the sequence number 1, 2, 3, ... as its ordering key.
extern "C"
uint32_t Storm_Sub36290(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4,
                        uint32_t param_5, const StormBatch19040Env* env) {
    static const char* const kSevt = "..\\3rdParty\\STORM\\SOURCE\\SEVT.CPP";
    // 1.13c: -(param_5 != 0) & ((param_4 != 0) - 1) == 0 -- fails when param_4 is nonzero or the handler is null.
    if (param_4 != 0 || param_5 == 0) {
        StormSetLastError(0x57);
        return 0;
    }
    EnterCriticalSection(&g_evtLock);
    EvtRecord* puVar3 = (EvtRecord*)env->f_35c70(param_1, param_2);
    if (puVar3 == nullptr) {
        uint32_t uVar4 = g_evtSize;
        uint32_t* table = ptr<uint32_t>(g_evtTable);
        if (g_evtSize >> 1 <= g_evtCount) {
            uint32_t target = g_evtCount * 2 + 2;
            uint32_t uVar8 = 1;
            uint32_t newSize = 1;
            if (target != 0) {
                do {
                    uVar8 *= 2;
                    newSize = uVar8;
                } while (uVar8 < target || uVar8 - target == 0);
            }
            uint32_t* puVar5 = evtAlloc(newSize * 4, kSevt, 0x143, 8, env);
            uVar4 = newSize;
            table = puVar5;
            if (g_evtTable != 0) {
                uint32_t* oldTable = ptr<uint32_t>(g_evtTable);
                if (g_evtSize != 0) {
                    for (uint32_t i = 0; i < g_evtSize; i++) {
                        if (oldTable[i] != 0) {
                            uint32_t* puVar6 = ptr<uint32_t>(oldTable[i]);
                            do {
                                uint32_t* puVar7 = ptr<uint32_t>(puVar6[4]);
                                uint32_t idx = (puVar6[1] ^ puVar6[0]) & (newSize - 1);
                                puVar6[4] = puVar5[idx];
                                puVar5[idx] = val(puVar6);
                                puVar6 = puVar7;
                            } while (puVar6 != nullptr);
                        }
                    }
                }
                env->f_1b960(oldTable, kSevt, 0x154);
            }
        }
        g_evtSize = uVar4;
        g_evtTable = val(table);
        uint32_t idx = (param_1 ^ param_2) & (g_evtSize - 1);
        puVar3 = (EvtRecord*)evtAlloc(0x14, kSevt, 0x15d, 8, env);
        std::memset(puVar3, 0, 0x14);
        puVar3->k1 = param_1;
        puVar3->k2 = param_2;
        EvtGroup* puVar5g = (EvtGroup*)evtAlloc(0x10, kSevt, 0x160, 8, env);
        std::memset(puVar5g, 0, 0x10);
        puVar3->group = val(puVar5g);
        table = ptr<uint32_t>(g_evtTable);
        puVar3->next = table[idx];
        table[idx] = val(puVar3);
        g_evtCount++;
    }

    EvtGroup* puVar3g = ptr<EvtGroup>(puVar3->group);
    if (puVar3g->size >> 1 <= puVar3g->used) {
        uint32_t uVar4 = puVar3g->size;
        uint32_t uVar8 = 1;
        uint32_t newSize = 1;
        if ((uVar4 & 0x7fffffff) != 0x7fffffff) {
            uint32_t target = (uint32_t)(int32_t)puVar3g->size * 2 + 2;
            do {
                uVar8 *= 2;
                newSize = uVar8;
            } while (uVar8 < target || uVar8 - target == 0);
        }
        uint32_t* puVar5 = evtAlloc(newSize * 4, kSevt, 0x16e, 8, env);
        uint32_t* puVar6 = evtAlloc(newSize * 4, kSevt, 0x16f, 8, env);
        for (uint32_t i = 0; i < newSize; i++) puVar6[i] = val(&puVar5[i]);
        if (puVar3g->buckets != 0 && puVar3g->size != 0) {
            for (uint32_t i = 0; i < puVar3g->size; i++) {
                uint32_t* puVar7 = ptr<uint32_t>(ptr<uint32_t>(puVar3g->buckets)[i]);
                while (puVar7 != nullptr) {
                    uint32_t* puVar2 = ptr<uint32_t>(puVar7[3]);
                    uint32_t idx = puVar7[0] & (newSize - 1);
                    puVar7[3] = 0;
                    *ptr<uint32_t>(puVar6[idx]) = val(puVar7);
                    puVar6[idx] = val(&puVar7[3]);
                    puVar7 = puVar2;
                }
            }
        }
        env->f_1b960(puVar6, kSevt, 0x182);
        if (puVar3g->buckets != 0) env->f_1b960(ptr<uint32_t>(puVar3g->buckets), kSevt, 0x184);
        puVar3g->buckets = val(puVar5);
        puVar3g->size = newSize;
    }

    uint32_t uVar8 = (puVar3g->size - 1) & param_3;
    uint32_t* puVar5 = evtAlloc(0x10, kSevt, 0x18d, 8, env);
    puVar5[0] = param_3;
    uint32_t uVar4 = puVar3->seq;
    puVar3->seq = uVar4 + 1;
    puVar5[1] = uVar4 + 1;
    puVar5[2] = param_5;
    puVar5[3] = ptr<uint32_t>(puVar3g->buckets)[uVar8];
    ptr<uint32_t>(puVar3g->buckets)[uVar8] = val(puVar5);
    puVar3g->used++;
    g_evtDirty = 1;
    LeaveCriticalSection(&g_evtLock);
    return 1;
}

// 1.13c Storm+0x36680: clone the bucket-group of the record for (param_1, param_2) and push the
// clone onto the group chain (the newest group becomes the head).
extern "C"
uint32_t Storm_Sub36680(uint32_t param_1, uint32_t param_2, const StormBatch19040Env* env) {
    uint32_t uVar2 = 0;
    EnterCriticalSection(&g_evtLock);
    if (g_evtTable != 0 && g_evtSize != 0) {
        uint32_t* puVar3 = ptr<uint32_t>(
            ptr<uint32_t>(g_evtTable)[(param_1 ^ param_2) & (g_evtSize - 1)]);
        while (puVar3 != nullptr && (*puVar3 != param_1 || puVar3[1] != param_2)) {
            puVar3 = ptr<uint32_t>(puVar3[4]);
        }
        if (puVar3 != nullptr) {
            uint32_t* puVar1 = evtAlloc(0x10, "..\\3rdParty\\STORM\\SOURCE\\SEVT.CPP",
                                              0x11c, 0, env);
            env->f_365e0(puVar1, (void*)(uintptr_t)puVar3[3]);
            uVar2 = 1;
            puVar1[3] = puVar3[3];
            puVar3[3] = val(puVar1);
            g_evtDirty = 1;
        }
    }
    LeaveCriticalSection(&g_evtLock);
    return uVar2;
}

// FN seam for the capture probes of 0x19040 / 0x19150 / 0x19520 (src/core/fn/storm_19x.h): copy the pinned module words in, run the body with the
// four callee seams the probe supplies, copy the words the body may write back. `which`: 0 = 0x19040(a, b, c, d), 1 = 0x19150(a, b), 2 = 0x19520(a, b, c, d).
// addr[0] / addr[1] return the addresses of the rect and palette words so the probe can log them as the module globals retail passes.
#include "storm_19pins.h"
extern "C" uint32_t Storm_19_FnRun(uint32_t which, uint32_t a, uint32_t b, uint32_t c, uint32_t d, Storm19Pins* p,
                                   uint32_t (*f18f00)(void), void (*f18f90)(void), int32_t (*vis)(void*), int32_t (*ico)(void*),
                                   const void** addr) {
    g_lastError = p->err; g_display = p->display; g_paletteIface = p->pal; g_captureDialog = p->capDlg; g_hwnd = p->hwnd; g_hpal = p->hpal;
    g_rectLock = p->rectLock; memcpy(g_rect, p->rect, sizeof g_rect); memcpy(g_draw, p->draw, sizeof g_draw); memcpy(g_palette, p->palette, sizeof g_palette);
    StormBatch19040Env env{};
    env.f_18f00 = f18f00; env.f_18f90 = f18f90; env.is_window_visible = vis; env.is_iconic = ico;
    addr[0] = g_rect; addr[1] = g_palette;
    uint32_t r = which == 0 ? Storm_Sub19040(a, b, c, d, &env) : which == 1 ? Storm_Sub19150(a, b, &env) : Storm_Sub19520(a, b, (uint32_t*)(uintptr_t)c, d, &env);
    p->err = g_lastError; p->rectLock = g_rectLock; memcpy(p->palette, g_palette, sizeof g_palette);
    return r;
}
