// src/core/gfx/storm_batch_16260.cpp -- 12 1.13c Storm.dll exports reimplemented from the decompiles
// in build-wasm/ghidra-storm/decomp/0x<RVA>.c (Stage A, RE-DONE; no spec, no vector).
//
// Prior art used: the decompiles only -- these are Storm's SDlg dialog layer, its timer/autimer
// lists, the bitmap-record list and the bnupdate patch installer, none of which D2MOO 1.10f
// reimplements (Storm is replaced by StormLib in this project).
//
// All twelve are Win32/dialog code that never runs in ours: the engine has no retail dialog
// resources, no advapi32/CryptoAPI provider and no bnupdate process. Each body keeps retail's
// control flow and its "not available here" early-outs; the missing kernel32/user32/gdi32 entry
// points are one-line seams below (compat/Windows.h carries only CRITICAL_SECTION and GetTickCount).
// Two paths past the deterministic early-out are left as skeletons with a note:
//   0x16260 (the Patch list walk, CreateProcessA) and 0x27560 (advapi32 signature verification).
//
// The r[] probe ABI is the storm one (see storm_13eb0.cpp): args 0..3 in r[0..3], args 4.. in
// r[5..], result in r[4].
#include <cstdint>
#include <cstring>
#include <strings.h>
#include <emscripten.h>
#include <Windows.h>

namespace {

using HWND = void*;
using HDC = void*;
using HGDIOBJ = void*;
using HINSTANCE = void*;
using HRSRC = void*;
using HGLOBAL = void*;
using WPARAM = uint32_t;
using LPARAM = intptr_t;
using INT_PTR = intptr_t;
using BOOL = int32_t;
using LPDWORD = uint32_t*;
using LPBYTE = uint8_t*;
using LPCVOID = const void*;

// ---- retail module state (globals keep their retail RVAs) ---------------------------------------
int32_t g_lastError = 0;          // 0x6FC45388
int32_t g_453ac = 0;              // 0x6FC453AC  pending ShowCursor(1) count
int32_t g_453bc = 0;              // 0x6FC453BC  paint refcount
uint32_t g_459fc = 0;             // 0x6FC459FC  timer / AUTIMERREC list head
uint32_t g_45980[2] = {0, 0};     // 0x6FC45980  default node for 0x26CB0
uint32_t g_45a08 = 0;             // 0x6FC45A08  AUBITMAPREC list head
uint32_t g_45a00 = 0;             // 0x6FC45A00  AUBITMAPREC allocator list
uint32_t g_453b8 = 0;             // 0x6FC453B8  dialog class registered flag
uint32_t g_453d8 = 0;             // 0x6FC453D8  owner HWND for the provider dialog
uint32_t g_45390 = 0;             // 0x6FC45390  HINSTANCE resources are loaded from
uint32_t g_374cc[1] = {0};        // 0x6FC374CC  default control-id stream (layout mode 1)
uint32_t g_374e4[1] = {0};        // 0x6FC374E4  default control-id stream (other modes)
CRITICAL_SECTION g_lockDlg;       // 0x6FC45A24
CRITICAL_SECTION g_lockDiag;      // 0x6FC45988

uint32_t rd32(uintptr_t p, uint32_t off) { uint32_t v; std::memcpy(&v, (void*)(p + off), 4); return v; }
void wr32(uintptr_t p, uint32_t off, uint32_t v) { std::memcpy((void*)(p + off), &v, 4); }

// ---- kernel32 / user32 / gdi32 seams ------------------------------------------------------------
// The wasm target has no window manager: handles are opaque 32-bit tokens, no callback fires, and
// the resource table is empty. Each seam below is the "nothing present" branch of the real call.
void StormSetLastError(uint32_t e) { g_lastError = (int32_t)e; }
void StormShowCursor(int) {}
void StormEndPaint() {}
void StormSetPropA(HWND, const char*, HANDLE) {}
HANDLE StormGetPropA(HWND, const char*) { return nullptr; }
void StormRemovePropA(HWND, const char*) {}
void StormEndDialog(HWND, INT_PTR) {}
void StormSetTextColor(HDC, uint32_t) {}
void StormSetBkColor(HDC, uint32_t) {}
void StormSetBkMode(HDC, int) {}
HGDIOBJ StormGetStockObject(int) { return nullptr; }
int StormGetClassNameA(HWND, LPSTR out, int n) { if (n > 0) out[0] = '\0'; return 0; }
void StormPostMessageA(HWND, uint32_t, WPARAM, LPARAM) {}
HWND StormGetDesktopWindow() { return nullptr; }
BOOL StormIsIconic(HWND) { return 0; }
void StormEnableWindow(HWND, BOOL) {}
void StormDestroyWindow(HWND) {}
HWND StormGetDlgItem(HWND, int) { return nullptr; }
BOOL StormGetClientRect(HWND, RECT* r) { std::memset(r, 0, sizeof(*r)); return 1; }
BOOL StormGetWindowRect(HWND, RECT* r) { std::memset(r, 0, sizeof(*r)); return 1; }
BOOL StormScreenToClient(HWND, POINT*) { return 1; }
BOOL StormIsWindow(HWND) { return 1; }
HRSRC StormFindResourceA(HINSTANCE, const char*, const char*) { return nullptr; }
HGLOBAL StormLoadResource(HINSTANCE, HRSRC) { return nullptr; }
void* StormLockResource(HGLOBAL) { return nullptr; }
HMODULE StormGetModuleHandleA(const char*) { return nullptr; }
HMODULE StormLoadLibraryA(const char*) { return nullptr; }

// ---- unmapped internal callees ----------------------------------------------------------------
// 0x2BDD0 reads the "Patch"/"Patches" config key. The config is never populated in ours, so the
// lookup reports "absent" and 0x16260 takes its own early-out.
int32_t StormSub2BDD0(const char*, const char*, int32_t, int32_t**, uint8_t*, uint32_t) { return 0; }
// 0x11F10 fills the three provider-info blocks (header sizes only; defaults are host handles).
int32_t StormSub11F10(uint32_t*, uint32_t*, uint32_t*, uint32_t* a, uint32_t* b, uint32_t* c) {
    std::memset(a, 0, 0x10);
    std::memset(b, 0, 0x50);
    std::memset(c, 0, 0x14);
    a[0] = 0x10;
    b[0] = 0x50;
    c[0] = 0x14;
    return 1;
}
void StormSub169A0() {}                       // 0x169A0 SDlg init
int32_t StormSub1220() { return 1; }          // 1.13c Storm+0x21220: SDlg class registration (Win32 RegisterClassA; no-op, returns true)
void* StormSub1DF90(void*, int*, HWND, void*, LPARAM) { return nullptr; }  // 0x1DF90 dialog create
void StormSub1D600() {}                       // 0x1D600 message pump
uint32_t StormSub16AC0(void*, int32_t) { return 0xFFFFFFFFu; }  // 0x16AC0 find hash entry
void StormSub4D60(void*) {}                   // 0x4D60 list unlink
void StormSub1B960(void*, const char*, int32_t) {}  // 0x1B960 allocator free
void StormSub3E20(void*) {}                   // 0x3E20 allocator destroy
void StormSub3B080(uint32_t* dst, const uint8_t* src, uint32_t n) { std::memcpy(dst, src, n); }

// 0x4C60 allocates an AUBITMAPREC from the 0x6FC45A00 pool and appends it to 0x6FC45A08.
// ponytail: one static record, enough for the deterministic skeleton; a real pool when a vector
// exercises two inserts.
uint32_t* StormSub4C60(uint32_t*, int32_t, int32_t) {
    static uint32_t rec[0x10];
    std::memset(rec, 0, sizeof(rec));
    g_45a08 = (uint32_t)(uintptr_t)rec;
    return rec;
}

// 0x1CB20 sends SDlg_EndDialog/SDlg_EndResult through the window properties, then EndDialog.
void StormSub1CB20(HWND hwnd, HANDLE result) {
    StormSetPropA(hwnd, "SDlg_EndDialog", (HANDLE)(uintptr_t)1);
    StormSetPropA(hwnd, "SDlg_EndResult", result);
    StormEndDialog(hwnd, (INT_PTR)(uintptr_t)result);
}

// 0x1D780 creates (or reuses) the AUBITMAPREC for a control rectangle. Return 0 in ours: the
// bitmap/crypto records it needs are not present.
uint32_t StormSub1D780(uint32_t param_1, uint32_t param_2, const uint8_t* param_3,
                       uint32_t param_4, uint32_t param_5, uint32_t param_6, int32_t* param_7,
                       uint32_t param_8, uint32_t param_9, int32_t param_10) {
    uint32_t uVar5;
    if (param_1 == 0 && (param_3 == nullptr || *param_3 == '\0')) uVar5 = 0;
    else uVar5 = 0xFFFFFFFFu;
    if ((-(uint32_t)(param_5 != 0) & uVar5 & (uint32_t)(((param_5 & 0xFFFF0000u) != 0) - 1)) == 0) {
        g_lastError = 0x57;
        StormSetLastError(0x57);
        return 0;
    }
    if (param_3 == nullptr) param_3 = (const uint8_t*)"";
    if (param_10 != -1) return 0;

    // Drop every record whose two window fields are no longer live windows. The head stays put.
    uint32_t* p = (uint32_t*)(uintptr_t)(((int32_t)g_45a08 < 1) ? 0 : g_45a08);
    while ((intptr_t)p > 0) {
        BOOL ok = (p[2] == 0 || StormIsWindow((HWND)(uintptr_t)p[2])) &&
                  (p[3] == 0 || StormIsWindow((HWND)(uintptr_t)p[3]));
        if (ok) {
            p = (uint32_t*)(uintptr_t)p[1];
        } else {
            uint32_t next = p[1];
            StormSub4D60(p);
            StormSub1B960(p, "__AUBITMAPREC__", -2);
            p = (uint32_t*)(uintptr_t)(((int32_t)next < 1) ? 0 : next);
        }
    }
    // Reuse means delete the matching old record (same handle pair, id, frame and name).
    uint32_t* q = (uint32_t*)(uintptr_t)g_45a08;
    while ((intptr_t)q > 0) {
        bool same = q[2] == param_1 && q[3] == param_2 && q[4] == param_5 && q[5] == param_4 &&
                    strcasecmp((const char*)(q + 0xF), (const char*)param_3) == 0;
        if (same) {
            StormSub4D60(q);
            StormSub1B960(q, "__AUBITMAPREC__", -2);
            break;
        }
        q = (uint32_t*)(uintptr_t)q[1];
        if ((intptr_t)q < 1) break;
    }
    if (param_6 == 0) return 1;
    if ((int32_t)param_8 < 1) return 1;
    if ((int32_t)param_9 < 1) return 1;
    uint32_t* rec = StormSub4C60(&g_45a00, 0, 0);
    rec[3] = param_2;
    rec[6] = param_6;
    rec[2] = param_1;
    rec[5] = param_4;
    rec[4] = param_5;
    rec[0xB] = param_8;
    rec[0xC] = param_9;
    rec[0xD] = 0;
    rec[0xE] = 0;
    if (param_7 != nullptr) {
        rec[7] = (uint32_t)((int32_t)param_7[0] % (int32_t)param_8);
        rec[8] = (uint32_t)((int32_t)param_7[1] % (int32_t)param_9);
        rec[9] = (uint32_t)(param_7[2] - param_7[0]) + rec[7];
        uVar5 = (uint32_t)(param_7[3] - param_7[1]) + rec[8];
        rec[0xA] = uVar5;
        if ((int32_t)rec[9] <= (int32_t)param_8 && (int32_t)uVar5 <= (int32_t)param_9) {
            StormSub3B080(rec + 0xF, param_3, 0x20);
            return 1;
        }
        rec[0xD] = rec[7];
        rec[0xE] = rec[8];
    }
    rec[7] = 0;
    rec[8] = 0;
    rec[9] = param_8;
    rec[0xA] = param_9;
    StormSub3B080(rec + 0xF, param_3, 0x20);
    return 1;
}

// 0x21350 runs the modal SDlg dialog until its message pump sets SDlg_EndDialog.
HANDLE StormSub21350(HINSTANCE hinst, int* tmpl, HWND parent, void* dlgProc, LPARAM lparam) {
    if (g_453b8 == 0 && StormSub1220() == 0) return (HANDLE)(uintptr_t)0xFFFFFFFFu;
    HWND h = (HWND)StormSub1DF90(hinst, tmpl, parent, dlgProc, lparam);
    if (h == nullptr) return (HANDLE)(uintptr_t)0xFFFFFFFFu;
    if (parent != nullptr && parent != StormGetDesktopWindow()) {
        StormSetPropA(h, "SDlg_Modal", (HANDLE)(uintptr_t)1);
        if (!StormIsIconic(parent)) StormEnableWindow(parent, 0);
    }
    HANDLE endDlg = StormGetPropA(h, "SDlg_EndDialog");
    while (endDlg == nullptr) {
        StormSub1D600();
        endDlg = StormGetPropA(h, "SDlg_EndDialog");
    }
    HANDLE result = StormGetPropA(h, "SDlg_EndResult");
    StormRemovePropA(h, "SDlg_EndDialog");
    StormRemovePropA(h, "SDlg_EndResult");
    StormRemovePropA(h, "SDlg_Modal");
    StormDestroyWindow(h);
    if (parent != nullptr && parent != StormGetDesktopWindow()) StormEnableWindow(parent, 1);
    return result;
}

}  // namespace

// 1.13c Storm+0x1CAF0: drop the queued ShowCursor(1) calls, the paint refcount, then EndPaint.
// The decompile leaves the tail as an unresolved indirect jump; it is EndPaint's epilogue.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_1caf0(uint32_t* r) {
    while (g_453ac != 0) {
        StormShowCursor(1);
        g_453ac = g_453ac - 1;
    }
    g_453bc = g_453bc - 1;
    StormEndPaint();
    r[4] = 0;
}

// 1.13c Storm+0x1CB20: end an SDlg dialog, publishing the result through two window properties.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_1cb20(uint32_t* r) {
    StormSub1CB20((HWND)(uintptr_t)r[0], (HANDLE)(uintptr_t)r[1]);
    r[4] = 0;
}

// 1.13c Storm+0x1D570: fire every due timer in the 0x6FC459FC list; always returns 1.
// Node: +0x04 next, +0x08 hwnd, +0x0C wparam, +0x10 interval, +0x14 callback, +0x18 last tick.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_1d570(uint32_t* r) {
    uint32_t now = GetTickCount();
    uint32_t node = ((int32_t)g_459fc < 1) ? 0 : g_459fc;
    if ((int32_t)node < 1) {
        r[4] = 1;
        return;
    }
    for (;;) {
        uint32_t interval = rd32(node, 0x10);
        uint32_t elapsed = now - rd32(node, 0x18);
        if (interval <= elapsed) {
            if (interval * 2 < elapsed) wr32(node, 0x18, now);
            else wr32(node, 0x18, interval + rd32(node, 0x18));
            uint32_t cb = rd32(node, 0x14);
            if (cb != 0) {
                auto fn = (void (*)(uint32_t, uint32_t, uint32_t, uint32_t))(uintptr_t)cb;
                fn(rd32(node, 0x08), 0x113, rd32(node, 0x0C), now);
                r[4] = 1;
                return;
            }
            StormPostMessageA((HWND)(uintptr_t)rd32(node, 0x08), 0x113, rd32(node, 0x0C), 0);
        }
        node = rd32(node, 0x04);
        if ((int32_t)node < 1) {
            r[4] = 1;
            return;
        }
    }
}

// 1.13c Storm+0x1D460: remove every AUTIMERREC matching (param_1, param_2); 1 if any matched.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_1d460(uint32_t* r) {
    uint32_t found = 0;
    uint32_t node = ((int32_t)g_459fc < 1) ? 0 : g_459fc;
    while ((int32_t)node > 0) {
        if (rd32(node, 0x08) == r[0] && rd32(node, 0x0C) == r[1]) {
            uint32_t next = rd32(node, 0x04);
            found = 1;
            StormSub4D60((void*)(uintptr_t)node);
            StormSub1B960((void*)(uintptr_t)node, "__AUTIMERREC__", -2);
            node = ((int32_t)next < 1) ? 0 : next;
        } else {
            node = rd32(node, 0x04);
        }
    }
    r[4] = found;
}

// 1.13c Storm+0x1CB60: the SDlgStatic colour/dialog window procedure.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_1cb60(uint32_t* r) {
    HWND hwnd = (HWND)(uintptr_t)r[0];
    uint32_t msg = r[1];
    HDC hdc = (HDC)(uintptr_t)r[2];
    HGDIOBJ obj = nullptr;
    if (msg < 0x135) {
        if (0x132 < msg) goto stock_white;
        if (msg == 0x10) {
            StormSub1CB20(hwnd, (HANDLE)nullptr);
            r[4] = 0;
            return;
        }
        if (msg == 0x110) {
            r[4] = 1;
            return;
        }
    } else if (msg == 0x138) {
        char cls[256];
        cls[0] = '\0';
        StormGetClassNameA((HWND)(uintptr_t)r[3], cls, 0x100);
        if (strcasecmp(cls, "SDlgStatic") == 0) {
            StormSetTextColor(hdc, 0xFFFFFF);
            StormSetBkMode(hdc, 1);
            obj = StormGetStockObject(5);
            r[4] = (uint32_t)(uintptr_t)obj;
            return;
        }
        goto stock_white;
    }
    r[4] = 0;
    return;
stock_white:
    StormSetTextColor(hdc, 0xFFFFFF);
    StormSetBkColor(hdc, 0);
    StormSetBkMode(hdc, 2);
    obj = StormGetStockObject(4);
    r[4] = (uint32_t)(uintptr_t)obj;
}

// 1.13c Storm+0x21350: show a modal SDlg dialog and return its SDlg_EndResult.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_21350(uint32_t* r) {
    HANDLE result = StormSub21350((HINSTANCE)(uintptr_t)r[0], (int*)(uintptr_t)r[1],
                                  (HWND)(uintptr_t)r[2], (void*)(uintptr_t)r[3],
                                  (LPARAM)(uintptr_t)r[5]);
    r[4] = (uint32_t)(uintptr_t)result;
}

// 1.13c Storm+0x17810: provider-select dialog; 1 when a dialog ran, 0 on every unmapped path.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_17810(uint32_t* r) {
    EnterCriticalSection(&g_lockDlg);
    uint32_t p6 = r[6];
    if (p6 != 0) wr32((uintptr_t)p6, 0, 0);
    uint32_t local_b0[4];
    uint32_t local_a0[5];
    uint8_t local_8c[60];
    uint32_t local_50[20];
    if (StormSub11F10((uint32_t*)(uintptr_t)r[2], (uint32_t*)(uintptr_t)r[3],
                      (uint32_t*)(uintptr_t)r[5], local_b0, local_50, local_a0) == 0) {
        g_lastError = 0x57;
        StormSetLastError(0x57);
        LeaveCriticalSection(&g_lockDlg);
        r[4] = 0;
        return;
    }
    StormSub169A0();
    uint32_t local_c4 = r[0];
    HANDLE dialog = (HANDLE)(uintptr_t)0xFFFFFFFFu;
    HRSRC info = StormFindResourceA((HINSTANCE)(uintptr_t)g_45390, "SELECTPROVIDER_DIALOG",
                                    (const char*)5);
    if (info != nullptr) {
        HGLOBAL data = StormLoadResource((HINSTANCE)(uintptr_t)g_45390, info);
        if (data != nullptr) {
            int* tmpl = (int*)StormLockResource(data);
            if (tmpl != nullptr) {
                HMODULE mod = StormGetModuleHandleA(nullptr);
                dialog = StormSub21350((HINSTANCE)mod, tmpl, (HWND)(uintptr_t)g_453d8, nullptr,
                                       (LPARAM)(uintptr_t)&local_c4);
            }
        }
    }
    if (p6 != 0) wr32((uintptr_t)p6, 0, (uint32_t)(uintptr_t)dialog);
    LeaveCriticalSection(&g_lockDlg);
    r[4] = (dialog != nullptr && dialog != (HANDLE)(uintptr_t)0xFFFFFFFFu) ? 1u : 0u;
}

// 1.13c Storm+0x1DA10: lay out the controls named by an id stream using 0x1D780's rectangles.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_1da10(uint32_t* r) {
    HWND hwnd = (HWND)(uintptr_t)r[0];
    int32_t* ids = (int32_t*)(uintptr_t)r[1];
    uint32_t* rects = (uint32_t*)(uintptr_t)r[2];
    uint32_t param4 = r[3];
    uint32_t* exts = (uint32_t*)(uintptr_t)r[5];
    int32_t mode = (int32_t)r[6];
    int32_t param7 = (int32_t)r[7];
    uint32_t ok = (hwnd != nullptr && ids != nullptr && param4 != 0 && exts != nullptr) ? 1u : 0u;
    if ((uint32_t)(-(int32_t)ok) & (uint32_t)(((int32_t)exts[0] < 1) - 1) &
        (uint32_t)(((int32_t)exts[1] < 1) - 1)) {
        ok = 1;
    } else {
        ok = 0;
    }
    if (ok == 0) {
        g_lastError = 0x57;
        StormSetLastError(0x57);
        r[4] = 0;
        return;
    }
    if (rects == nullptr) rects = (mode != 1) ? g_374e4 : g_374cc;
    int32_t result = 1;
    int32_t* idp = ids;
    while (*idp != 0) {
        POINT origin = {0, 0};
        int32_t right = 0;
        int32_t bottom = 0;
        HWND ctl = StormGetDlgItem(hwnd, *idp);
        if (ctl == nullptr) {
            result = 0;
        } else {
            RECT client;
            RECT window;
            StormGetClientRect(ctl, &client);
            StormGetWindowRect(ctl, &window);
            uint32_t* rp = rects;
            uint32_t id = *rp;
            int32_t x = origin.x;
            int32_t y = origin.y;
            origin.x = window.left;
            origin.y = window.top;
            while (id != 0) {
                if (mode == 2) {
                    StormScreenToClient(hwnd, &origin);
                    x = origin.x;
                    y = origin.y;
                }
                origin.y = y;
                origin.x = x;
                right = (client.right - client.left) + origin.x;
                bottom = (client.bottom - client.top) + origin.y;
                if (StormSub1D780((uint32_t)(uintptr_t)ctl, 0, nullptr, 0xFFFFFFFFu, *rp, param4,
                                  &origin.x, exts[0], exts[1], param7) == 0) {
                    result = 0;
                }
                if (mode == 1 && (origin.y = bottom, (int32_t)exts[1] <= bottom)) origin.y = 0;
                id = *++rp;
                x = origin.x;
                y = origin.y;
                origin.x = window.left;
                origin.y = window.top;
            }
        }
        idp++;
    }
    r[4] = (uint32_t)result;
}

// 1.13c Storm+0x1D780: create/reuse the AUBITMAPREC for (hwnd, id) and copy a 32-byte name.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_1d780(uint32_t* r) {
    r[4] = StormSub1D780(r[0], r[1], (const uint8_t*)(uintptr_t)r[2], r[3], r[5], r[6],
                         (int32_t*)(uintptr_t)r[7], r[8], r[9], (int32_t)r[10]);
}

// 1.13c Storm+0x26CB0: unlink a node and free it once its +0x140 refcount reaches zero.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_26cb0(uint32_t* r) {
    uint32_t self = r[0];
    EnterCriticalSection(&g_lockDiag);
    uintptr_t p = (self != 0) ? (uintptr_t)self : (uintptr_t)g_45980;
    uint32_t head = rd32(p, 0);
    if (head != 0) {
        uint32_t tag = rd32(p, 4);
        uintptr_t target;
        if ((int32_t)tag < 0) target = (uintptr_t)(uint32_t)~tag;
        else target = p + (uintptr_t)(uint32_t)(tag - rd32((uintptr_t)head, 4));
        wr32(target, 0, head);
        wr32((uintptr_t)head, 4, tag);
        wr32(p, 0, 0);
        wr32(p, 4, 0);
    }
    if (self != 0) {
        uintptr_t ref = (uintptr_t)self + 0x140;   // param_1 + 0x50 dwords
        uint32_t n = rd32(ref, 0) - 1;
        wr32(ref, 0, n);
        if (n == 0) {
            StormSub3E20((void*)(uintptr_t)self);
            StormSub1B960((void*)(uintptr_t)self, "delete", -1);
        }
    }
    LeaveCriticalSection(&g_lockDiag);
    r[4] = 1;
}

// 1.13c Storm+0x16260: run bnupdate from the "Patch"/"Patches" config entry.
// Skeleton: the config lookup is absent in ours, so retail's own iVar4==0 branch is the outcome.
// The Patch-list walk (file map, per-line delete/extract/execute, CreateProcessA) is not modelled.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_16260(uint32_t* r) {
    if (r[0] == 0) {
        g_lastError = 0x57;
        StormSetLastError(0x57);
        r[4] = 0;
        return;
    }
    EnterCriticalSection(&g_lockDlg);
    char patches[0x400];
    patches[0] = '\0';
    int32_t* handle = nullptr;
    if (StormSub2BDD0("Patch", "Patches", 2, &handle, (uint8_t*)patches, 0x3FF) == 0) {
        wr32((uintptr_t)r[0], 0, 0);
        LeaveCriticalSection(&g_lockDlg);
        r[4] = 0;
        return;
    }
    LeaveCriticalSection(&g_lockDlg);
    r[4] = 0;
}

// 1.13c Storm+0x27560: verify the archive's signature hash. Skeleton: advapi32/CryptoAPI is not
// present on the wasm target, so the LoadLibraryA branch is skipped and the error is retail's 0x4DC.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_27560(uint32_t* r) {
    uint32_t p1 = r[0];
    uint32_t p2 = r[1];
    if (p2 != 0) wr32((uintptr_t)p2, 0, 0);
    uint32_t entry = StormSub16AC0((void*)(uintptr_t)p1, 0);
    if (entry == 0xFFFFFFFFu) {
        if (p2 != 0) wr32((uintptr_t)p2, 0, 1);
        g_lastError = 0x4DC;
        StormSetLastError(0x4DC);
        r[4] = 0;
        return;
    }
    int32_t verified = 0;
    HMODULE advapi = StormLoadLibraryA("advapi32.dll");
    if (advapi != nullptr) {
        // Retail imports CryptAcquireContextA .. CryptVerifySignatureA, maps +0x10C of the archive
        // and verifies the BLIZZARDKEY resource. Our LoadLibraryA seam has no advapi32, so this
        // branch is unreachable; left as a named seam rather than a fake crypto implementation.
    }
    if (verified == 0) {
        g_lastError = 0x4DC;
        StormSetLastError(0x4DC);
    }
    r[4] = (uint32_t)verified;
}
