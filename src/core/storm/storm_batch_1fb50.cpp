// 1.13c Storm+0x1FB50 (1567 B, ordinal 201, caller 0x20B70): SDLG.CPP paint handler (WM_PAINT).
// Reimplemented from build-wasm/ghidra-storm/decomp/0x1FB50.c (alpha lane: no spec, no vector).
//
// Prior art used: the decompile and the sibling Storm rows already recovered in
// src/core/storm/storm_batch_123b0.cpp and src/core/gfx/storm_batch_16260.cpp. Retail base 0x6FBF0000;
// every global keeps its retail RVA. The body is the literal retail flow: the error/NULL early-outs,
// the paint refcount, the own-window vs. screen-surface paint branches, the child-window enumeration,
// the XOR region split, the SMemAlloc scratch surface and its SMemFree, and the final fErase=0 +
// region release. The two Ghidra artifacts kept as written:
//   * the local Ghidra calls local_68 is reused three times (the argument-valid flag, the client-rect
//     left edge, and the surface-blit flag); the code below keeps the same sequence of assignments;
//   * 0x382C0 is __thiscall and its `this` is never assigned -- it is whatever ECX held on entry, so
//     the first seam call passes 0 with that note.
// The unmapped internal callees are seam function pointers in StormDlgPaintEnv, after the retail
// arguments: 0x1CD30, 0x1D2E0, 0x1E830, 0x382C0, 0x1C580 (SMemAlloc) and 0x1B960 (SMemFree). The
// kernel32/user32/gdi32 entry points the body calls are seams there too, because compat/Windows.h
// carries no window manager or GDI.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include <Windows.h>

namespace {

using HWND = void*;
using HDC = void*;
using HRGN = void*;
using LPRGNDATA = void*;
using BOOL = int32_t;

// Win32 PAINTSTRUCT; only fErase is read here. Pointer fields are host-sized in ours.
struct PAINTSTRUCT {
    HDC hdc;              // +0x00
    BOOL fErase;          // +0x04 (retail 32-bit layout)
    RECT rcPaint;
    BOOL fRestore;
    BOOL fIncUpdate;
    uint8_t rgbReserved[32];
};

// ---- retail module state (all zero-initialised unless noted) ------------------------------------
int32_t g_lastError;        // 0x6FC45388  last Storm error, mirrored to SetLastError
int32_t g_paintRefcount;    // 0x6FC453BC  paint nesting counter, incremented on entry
int32_t g_453ac;            // 0x6FC453AC  windowed/fullscreen selector
int32_t g_453b4;            // 0x6FC453B4  screen surface (SMemAlloc'd scratch's blit source)
int32_t g_453b0;            // 0x6FC453B0  screen surface (second blit source)
int32_t g_453e4;            // 0x6FC453E4  nonzero => use the cached screen size
int32_t g_41a20;            // 0x6FC41A20  cached screen width
int32_t g_41a24;            // 0x6FC41A24  cached screen height
int32_t g_41a0c;            // 0x6FC41A0C  screen rect x
int32_t g_41a10;            // 0x6FC41A10  screen rect y
int32_t g_41a14;            // 0x6FC41A14  screen rect width
int32_t g_41a18;            // 0x6FC41A18  screen rect height

// ---- seam surface: host entry points and the unmapped Storm internals --------------------------
struct StormDlgPaintEnv {
    // kernel32 / user32 / gdi32
    BOOL     (*is_window)(HWND);                                     // IsWindow
    void     (*set_last_error)(uint32_t);                            // SetLastError
    HDC      (*begin_paint)(HWND, PAINTSTRUCT*);                     // BeginPaint
    HRGN     (*create_rect_rgn)(int32_t, int32_t, int32_t, int32_t); // CreateRectRgn
    int32_t  (*get_update_rgn)(HWND, HRGN, BOOL);                    // GetUpdateRgn
    uint32_t (*get_class_long_a)(HWND, int32_t);                     // GetClassLongA
    uint32_t (*set_class_long_a)(HWND, int32_t, int32_t);            // SetClassLongA
    uint32_t (*get_window_long_a)(HWND, int32_t);                    // GetWindowLongA
    BOOL     (*get_client_rect)(HWND, RECT*);                        // GetClientRect
    BOOL     (*client_to_screen)(HWND, POINT*);                      // ClientToScreen
    int32_t  (*get_system_metrics)(int32_t);                         // GetSystemMetrics
    BOOL     (*intersect_rect)(RECT*, const RECT*, const RECT*);     // IntersectRect
    int32_t  (*combine_rgn)(HRGN, HRGN, HRGN, int32_t);              // CombineRgn
    HWND     (*get_window)(HWND, uint32_t);                          // GetWindow
    HWND     (*get_parent)(HWND);                                    // GetParent
    HWND     (*get_desktop_window)(void);                            // GetDesktopWindow
    BOOL     (*delete_object)(void*);                                // DeleteObject
    // unmapped Storm internals, retail argument order
    uint8_t* (*sub_cd30)(uint32_t);                                  // 0x1CD30 surface/video getter
    void     (*sub_d2e0)(HWND, HRGN, HWND);                          // 0x1D2E0 invalidate a window region
    int32_t  (*sub_e830)(int32_t, int32_t*, int32_t, int32_t, int32_t*, int32_t,
                         int32_t, int32_t, uint32_t, uint32_t, int32_t*, void*,
                         uint32_t, int32_t, int32_t*);                // 0x1E830 blit/update
    uint32_t (*sub_382c0)(void*, uint32_t, uint32_t, uint32_t, int32_t, int32_t,
                          int32_t, uint32_t, uint32_t);               // 0x382C0 (__thiscall)
    uint32_t* (*sub_c580)(uint32_t, const char*, int32_t, uint32_t); // 0x1C580 SMemAlloc
    int32_t  (*sub_b960)(uint32_t*, const char*, int32_t);           // 0x1B960 SMemFree
};

constexpr const char* kSdlg = "..\\3rdParty\\STORM\\SOURCE\\SDLG.CPP";

uint32_t rd32(const uint8_t* p, uintptr_t off) {
    uint32_t v;
    std::memcpy(&v, p + off, 4);
    return v;
}

}  // namespace

// 1.13c Storm+0x1FB50 (sub_1FB50): the WM_PAINT handler. Returns the BeginPaint HDC, or NULL when
// paint is NULL, the window is NULL, or IsWindow fails. Sets last error 0x57 on the NULL cases.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_fn_storm_1fb50(HWND param_1, PAINTSTRUCT* param_2,
                                             StormDlgPaintEnv* env) {
    HWND hWnd = param_1;
    const uint32_t argOk = (param_1 != nullptr) ? 0xFFFFFFFFu : 0u;   // Ghidra's local_68
    if (param_2 == nullptr || param_1 == nullptr) {
        g_lastError = 0x57;
        env->set_last_error(0x57);
        return nullptr;
    }
    if (env->is_window(param_1) == 0) return nullptr;
    g_paintRefcount += 1;

    uint32_t exStyle = env->get_window_long_a(param_1, -0x14);
    uint8_t* surface;
    if ((exStyle & 0x20) != 0 || (surface = env->sub_cd30(1), surface == nullptr)) {
        return env->begin_paint(param_1, param_2);
    }

    HRGN updateRgn = env->create_rect_rgn(0, 0, 1, 1);
    env->get_update_rgn(param_1, updateRgn, 0);
    uint32_t oldClassLong = env->get_class_long_a(param_1, -10);
    env->set_class_long_a(param_1, -10, 0);
    HDC hdc = env->begin_paint(param_1, param_2);
    env->set_class_long_a(param_1, -10, (int32_t)oldClassLong);

    if (param_2->fErase != 0) {
        if (argOk == 0) {
            g_lastError = 0x57;
            env->set_last_error(0x57);
        } else if (env->is_window(param_1) != 0 &&
                   (env->get_window_long_a(param_1, -0x14) & 0x20) == 0 &&
                   (surface = env->sub_cd30(1)) != nullptr) {
            RECT rcSelf{};                                   // local_20 + local_14
            env->get_client_rect(param_1, &rcSelf);
            env->client_to_screen(param_1, (POINT*)&rcSelf);
            env->client_to_screen(param_1, (POINT*)&rcSelf.right);
            int32_t surf2c = *(int32_t*)(surface + 0x2C);    // local_38
            int32_t surf30 = *(int32_t*)(surface + 0x30);    // local_34
            int32_t screenW = (g_453e4 == 0) ? env->get_system_metrics(0) : g_41a20;  // local_40
            int32_t screenH = (g_453e4 == 0) ? env->get_system_metrics(1) : g_41a24;  // local_3c
            uint32_t mode = 3;                               // Ghidra's reused param_1
            if (g_453ac == 0) {
                mode = 0x1003;
                if (g_paintRefcount == 0) mode = 0x3003;
            }
            int32_t surf2c2 = *(int32_t*)(surface + 0x2C);   // local_54
            int32_t* clipX = (int32_t*)(surface + 0x1C);     // local_4c
            // local_48: ((g_453ac != 0) - 1) & g_453ac -- zero on both paths, kept as written.
            int32_t* clipPtr = (int32_t*)(uintptr_t)
                ((((g_453ac != 0) ? 1u : 0u) - 1u) & (uint32_t)g_453ac);
            int32_t surf18 = *(int32_t*)(surface + 0x18);    // local_50
            if (clipPtr != nullptr) *clipPtr = 0;
            if (argOk == 0) {
                g_lastError = 0x57;
                env->set_last_error(0x57);
            } else if ((mode & 2) != 0) {
                int32_t clipTop = rcSelf.top;                // local_64
                int32_t clipLeft = rcSelf.left;              // local_68 (the flag is dead from here)
                int32_t clipRight = rcSelf.right;            // uVar2
                if (screenW < rcSelf.right - rcSelf.left) clipRight = screenW + rcSelf.left;
                int32_t clipBottom = rcSelf.bottom;          // pvVar8 (local_14)
                if (screenH < rcSelf.bottom - rcSelf.top) {
                    clipBottom = *(int32_t*)(surface + 0x28) + screenH;
                }
                if (clipX != nullptr && (mode & 1) == 0) {
                    int32_t w = *(int32_t*)(surface + 0x24) - *clipX;
                    if (w < clipRight - rcSelf.left) clipRight = w + rcSelf.left;
                    int32_t h = *(int32_t*)(surface + 0x28) - *(int32_t*)(surface + 0x20);
                    if (h < clipBottom - rcSelf.top) clipBottom = h + rcSelf.top;
                }
                RECT rcWin{};                                // local_30 + local_24
                env->get_client_rect(hWnd, &rcWin);
                env->client_to_screen(hWnd, (POINT*)&rcWin);
                env->client_to_screen(hWnd, (POINT*)&rcWin.right);
                // local_40 + local_3c form the point handed to 0x1E830 as param_11; keep them as
                // one pair because the seam may write through that pointer before the next use.
                int32_t dstOrigin[2] = {rcWin.left, rcWin.top};
                RECT rcClip{};                               // local_10
                rcClip.left = clipLeft;
                rcClip.top = clipTop;
                rcClip.right = clipRight;
                rcClip.bottom = clipBottom;
                env->intersect_rect(&rcWin, &rcWin, &rcClip);
                if (rcWin.left < rcWin.right && rcWin.top < rcWin.bottom) {
                    RECT rcClient{};                         // local_10 reused
                    env->get_client_rect(hWnd, &rcClient);
                    HRGN paintRgn = env->create_rect_rgn(rcClient.left, rcClient.top,
                                                         rcClient.right, rcClient.bottom);
                    if (paintRgn != nullptr) {
                        if (updateRgn != nullptr) env->combine_rgn(paintRgn, paintRgn, updateRgn, 1);
                        env->sub_d2e0(hWnd, paintRgn, hWnd);
                        HWND walk = hWnd;
                        for (;;) {
                            for (HWND child = env->get_window(walk, 3); child != nullptr;
                                 child = env->get_window(child, 3)) {
                                env->sub_d2e0(hWnd, paintRgn, child);
                            }
                            uint32_t walkStyle = env->get_window_long_a(walk, -0x10);
                            if ((walkStyle & 0x40000000u) == 0) break;
                            walk = env->get_parent(walk);
                            if (walk == nullptr || walk == env->get_desktop_window()) break;
                        }
                        void* srcRgn = nullptr;              // hrgnSrc2
                        int32_t blitSurface;                 // local_68, reused as the flag
                        if (g_453b4 == 0 || g_453b0 == 0 || g_41a0c < 0) {
                            blitSurface = 0;
                        } else {
                            blitSurface = 1;
                            if (g_41a10 < 0) blitSurface = 0;
                        }
                        void* blitRgn = paintRgn;            // hrgnDst_00
                        int32_t surfPt[2] = {surf2c, surf30}; // &local_38 pair
                        if (blitSurface != 0) {
                            srcRgn = env->create_rect_rgn(g_41a0c - dstOrigin[0],
                                                          g_41a10 - dstOrigin[1],
                                                          (g_41a14 - dstOrigin[0]) + g_41a0c,
                                                          g_41a18 + (g_41a10 - dstOrigin[1]));
                            blitRgn = env->create_rect_rgn(0, 0, 1, 1);
                            env->combine_rgn(blitRgn, paintRgn, srcRgn, 4);
                            env->combine_rgn(srcRgn, paintRgn, srcRgn, 1);
                            env->delete_object(paintRgn);
                        }
                        env->sub_e830(0, (int32_t*)&rcWin, 0, surf18, surfPt, surf2c2,
                                      0, 0, 0, 0xcc0020u, dstOrigin, blitRgn,
                                      mode, 0, clipPtr);
                        env->delete_object(blitRgn);
                        if (blitSurface != 0) {
                            uint32_t* scratch = env->sub_c580((uint32_t)(g_41a14 * g_41a18),
                                                              kSdlg, 0x3FB, 0);
                            int32_t destPt[2] = {dstOrigin[0] - g_41a0c,
                                                 dstOrigin[1] - g_41a10};
                            RECT srcRect;                // local_10
                            srcRect.left = 0;
                            srcRect.top = 0;
                            srcRect.right = g_41a14;
                            srcRect.bottom = g_41a18;
                            env->sub_e830((int32_t)(uintptr_t)scratch, (int32_t*)&srcRect,
                                          g_41a14, surf18, surfPt, surf2c2, 0, 0, 0,
                                          0xcc0020u, destPt, srcRgn, mode & 1u, 0, nullptr);
                            // 0x382C0's `this` is unassigned in the decompile (ECX on entry).
                            env->sub_382c0(nullptr, (uint32_t)(uintptr_t)scratch,
                                           (uint32_t)g_453b4, (uint32_t)g_41a14,
                                           g_41a18, g_41a14, g_41a14, 0, 0x8800c6u);
                            env->sub_382c0((void*)(uintptr_t)(uint32_t)g_41a18,
                                           (uint32_t)(uintptr_t)scratch, (uint32_t)g_453b0,
                                           (uint32_t)g_41a14, g_41a18, g_41a14, g_41a14, 0,
                                           0xee0086u);
                            rcWin.left = g_41a0c;
                            rcWin.top = g_41a10;
                            env->sub_e830(0, (int32_t*)&rcWin, 0,
                                          (int32_t)(uintptr_t)scratch,
                                          (int32_t*)&g_41a14, g_41a14, 0, 0, 0, 0xcc0020u,
                                          dstOrigin, srcRgn, mode & 0xFFFFFFFEu,
                                          0, clipPtr);
                            env->delete_object(srcRgn);
                            env->sub_b960(scratch, kSdlg, 0x43C);
                        }
                    }
                }
            }
        }
    }
    param_2->fErase = 0;
    env->delete_object(updateRgn);
    return hdc;
}
