// src/core/storm/storm_sub_1efc0.cpp -- 1.13c Storm.dll row 0x1EFC0 (ordinal 210, 1631 bytes,
// SDLG.CPP) reimplemented from build-wasm/ghidra-storm/decomp/0x1EFC0.c (alpha lane: marker only,
// no spec, no vector). Row s3763.
//
// The SDLG "restore the window's pixels from the display back buffer" call: validate the window,
// resolve the display-mode record (0x1CD30), clamp the client rect to the mode/caller inset, build
// the window's visible regions (the window itself and its previous siblings, walking up while the
// parent chain is a WS_POPUP-style window), optionally punch out the captured sub-rect, blit, and
// composite the capture sprites back. Retail globals are file statics tagged with their 1.13c
// addresses. The unmapped callees and the user32/gdi32 imports are host seams on StormSub1Efc0Env,
// every body taking it AFTER its retail argument list (the storm_batch_382c0.cpp pattern).
#include <cstdint>

// display-mode record returned by 0x1CD30; only the words this row reads.
struct StormSub1Efc0Mode {
    uint8_t  pad00[0x18];
    int32_t  f18;     // +0x18
    int32_t  left;    // +0x1C
    int32_t  top;     // +0x20
    int32_t  right;   // +0x24
    int32_t  bottom;  // +0x28
    int32_t  f2c;     // +0x2C
    uint32_t f30;     // +0x30
};

struct StormSub1Efc0Rect { int32_t left, top, right, bottom; };

struct StormSub1Efc0Env {
    // ---- unmapped internal callees, retail argument order --------------------------------------
    StormSub1Efc0Mode* (*f_1cd30)(uint32_t mode);                          // 0x1CD30 mode record
    void (*f_1d2e0)(void* hwnd, void* rgn, void* wnd);                     // 0x1D2E0 window region
    void (*f_1e830)(uint32_t dst, int32_t* rect, uint32_t src, int32_t a3, int32_t* a4, int32_t a5,
                    int32_t a6, int32_t a7, int32_t a8, uint32_t rop, int32_t* pt, void* rgn,
                    uint32_t flags, int32_t a13, int32_t* out);            // 0x1E830 blit wrapper
    void*    (*f_1c580)(uint32_t size, const char* file, int32_t line, uint32_t type); // 0x1C580
    void     (*f_b960)(void* p, const char* file, int32_t line);           // 0x0B960 debug free
    uint32_t (*f_382c0)(void* self, uint32_t dst, uint32_t src, int32_t w, int32_t h, int32_t a5,
                        int32_t a6, int32_t a7, uint32_t rop);             // 0x382C0 blit core
    // ---- user32 / gdi32 imports ---------------------------------------------------------------
    int32_t  (*is_window)(void* hwnd);
    uint32_t (*get_window_long_a)(void* hwnd, int32_t index);
    int32_t  (*get_client_rect)(void* hwnd, StormSub1Efc0Rect* rect);
    int32_t  (*client_to_screen)(void* hwnd, int32_t* point);
    int32_t  (*get_system_metrics)(int32_t index);
    int32_t  (*intersect_rect)(StormSub1Efc0Rect* out, const StormSub1Efc0Rect* a,
                               const StormSub1Efc0Rect* b);
    void*    (*create_rect_rgn)(int32_t l, int32_t t, int32_t r, int32_t b);
    int32_t  (*combine_rgn)(void* dst, void* a, void* b, int32_t mode);
    int32_t  (*delete_object)(void* obj);
    void*    (*get_window)(void* hwnd, uint32_t cmd);
    void*    (*get_parent)(void* hwnd);
    void*    (*get_desktop_window)(void);
};

namespace {

int32_t g_error;       // 0x6FC45388  Storm last error, mirrored to SetLastError
void*   g_453e4;       // 0x6FC453E4  display interface; zero until a mode is set
uint32_t g_453ac;      // 0x6FC453AC  lock word, cleared before each blit
uint32_t g_453bc;      // 0x6FC453BC  mode-select flag
uint32_t g_453b4;      // 0x6FC453B4  capture sprite A (width blit)
uint32_t g_453b0;      // 0x6FC453B0  capture sprite B (height blit)
int32_t  g_41a0c;      // 0x6FC41A0C  capture rect left
int32_t  g_41a10;      // 0x6FC41A10  capture rect top
uint32_t g_41a14;      // 0x6FC41A14  capture rect width
int32_t  g_41a18;      // 0x6FC41A18  capture rect height
uint32_t g_41a20;      // 0x6FC41A20  screen width
uint32_t g_41a24;      // 0x6FC41A24  screen height

void StormSetLastError(uint32_t e) { g_error = (int32_t)e; }

} // namespace

// 1.13c Storm+0x1EFC0: restore a window's pixels from the back buffer. Rejects a zero mode or a null
// window with ERROR_INVALID_PARAMETER; a non-window returns 0; a transparent extended-style window
// in mode 1 is a success no-op. The caller's inset rect (left/top keep their non-negative halves,
// right/bottom their non-positive halves), then the mode/flag clamp, then the visible-region walk
// and the blit. The capture sub-rect compositing is the second half of the body.
extern "C"
uint32_t Storm_Sub1EFC0(void* hwnd, uint32_t mode, void* clip, int32_t param_4, int32_t param_5,
                        uint32_t* inset, uint32_t flags, const StormSub1Efc0Env* env) {
    int32_t local_50 = -(int32_t)(hwnd != nullptr);
    if (mode == 0 || hwnd == nullptr) {
        g_error = 0x57;
        StormSetLastError(0x57);
        return 0;
    }
    if (!env->is_window(hwnd)) return 0;
    // GWL_EXSTYLE, WS_EX_TRANSPARENT: a layered pass-through window is already screen-true.
    if (mode == 1 && (env->get_window_long_a(hwnd, -0x14) & 0x20) != 0) return 1;

    StormSub1Efc0Mode* disp = env->f_1cd30(mode);
    if (disp == nullptr) return 0;

    StormSub1Efc0Rect client;
    env->get_client_rect(hwnd, &client);
    env->client_to_screen(hwnd, &client.left);
    env->client_to_screen(hwnd, &client.right);
    if (inset != nullptr) {
        // Retail masks: left/top add only a non-negative inset, right/bottom only a non-positive one.
        client.left  += (int32_t)(inset[0] & (uint32_t)(((int32_t)inset[0] < 0) - 1));
        client.top   += (int32_t)(inset[1] & (uint32_t)(((int32_t)inset[1] < 0) - 1));
        client.right += (int32_t)(inset[2] & (uint32_t)((0 < (int32_t)inset[2]) - 1));
        client.bottom += (int32_t)(inset[3] & (uint32_t)((0 < (int32_t)inset[3]) - 1));
    }
    if ((flags & 1) == 0) {
        int32_t r = (disp->right - disp->left) + client.left;
        if (r <= client.right) client.right = r;
        bool setBottom = true;
        int32_t b = 0;
        if ((flags & 2) == 0) {
            b = (disp->bottom - disp->top) + client.top;
            if (client.bottom < b) setBottom = false;
        } else {
            int32_t c = (((disp->top - disp->bottom) - client.top) + client.bottom) / 2;
            if (c < 1) {
                setBottom = false;
            } else {
                client.top += c;
                b = (disp->bottom - disp->top) + client.top;
            }
        }
        if (setBottom) client.bottom = b;
    }

    int32_t local_38 = disp->f2c;   // blit source descriptor, opaque here
    int32_t local_34 = (int32_t)disp->f30;
    (void)local_34;                 // loaded by retail, never read on this path
    StormSub1Efc0Rect bounds;
    bounds.left = (int32_t)g_41a20;
    if (g_453e4 == nullptr) bounds.left = env->get_system_metrics(0);
    if (g_453e4 == nullptr) bounds.top = env->get_system_metrics(1);
    else bounds.top = (int32_t)g_41a24;

    uint32_t sel = 3;
    if (g_453ac == 0) {
        sel = 0x1003;
        if (g_453bc == 0) sel = 0x3003;
    }
    int32_t* lock = (g_453ac != 0) ? nullptr : (int32_t*)&g_453ac;
    int32_t iVar6 = disp->f2c;
    int32_t iVar1 = disp->f18;
    if (lock != nullptr) *lock = 0;

    if (local_50 != 0) {
        if ((sel & 2) != 0) {
            StormSub1Efc0Rect vis;   // local_10, then local_20 later
            vis.left = client.left;
            int32_t visRight = client.right;
            if ((int32_t)bounds.left < client.right - client.left) visRight = (int32_t)bounds.left + client.left;
            int32_t visBottom = client.bottom;
            if ((int32_t)bounds.top < client.bottom - client.top) visBottom = disp->bottom + (int32_t)bounds.top;
            if ((sel & 1) == 0) {
                // Dead in retail: every sel above has bit 0 set (Ghidra flags the block unreachable).
                int32_t w = disp->right - disp->left;
                if (w < visRight - client.left) visRight = w + client.left;
                int32_t h = disp->bottom - disp->top;
                if (h < visBottom - client.top) visBottom = h + client.top;
            }
            vis.top = client.top;
            StormSub1Efc0Rect winrect;   // local_20
            env->get_client_rect(hwnd, &winrect);
            env->client_to_screen(hwnd, &winrect.left);
            env->client_to_screen(hwnd, &winrect.right);
            local_50 = winrect.left;     // reused as the capture point
            int32_t local_4c = winrect.top;
            bounds.left = vis.left;
            bounds.top = vis.top;
            bounds.right = visRight;
            bounds.bottom = visBottom;
            env->intersect_rect(&winrect, &winrect, &bounds);
            if (winrect.left < winrect.right && winrect.top < winrect.bottom) {
                env->get_client_rect(hwnd, &vis);
                void* rgn = env->create_rect_rgn(vis.left, vis.top, vis.right, vis.bottom);
                if (rgn != nullptr) {
                    if (clip != nullptr) env->combine_rgn(rgn, rgn, clip, 1);   // RGN_AND
                    env->f_1d2e0(hwnd, rgn, hwnd);
                    void* w = hwnd;
                    uint32_t style = 0;
                    do {
                        // GW_HWNDPREV (retail passes 3), the sibling chain.
                        for (void* sib = env->get_window(w, 3); sib != nullptr; sib = env->get_window(sib, 3)) {
                            env->f_1d2e0(hwnd, rgn, sib);
                        }
                        style = env->get_window_long_a(w, -0x10);   // GWL_STYLE
                    } while ((style & 0x40000000) != 0 && (w = env->get_parent(w)) != nullptr &&
                             w != env->get_desktop_window());

                    void* diff = nullptr;   // hrgnSrc2
                    bool capture = !(g_453b4 == 0 || g_453b0 == 0 || g_41a0c < 0 || g_41a10 < 0);
                    void* dst = rgn;
                    if (capture) {
                        diff = env->create_rect_rgn(g_41a0c - local_50, g_41a10 - local_4c,
                                                    ((int32_t)g_41a14 - local_50) + g_41a0c,
                                                    g_41a18 + (g_41a10 - local_4c));
                        dst = env->create_rect_rgn(0, 0, 1, 1);
                        env->combine_rgn(dst, rgn, diff, 4);    // RGN_DIFF
                        env->combine_rgn(diff, rgn, diff, 1);   // RGN_AND
                        env->delete_object(rgn);
                    }
                    env->f_1e830(0, &winrect.left, 0, iVar1, &local_38, iVar6, param_4, param_5, 0,
                                 0xcc0020, &local_50, dst, sel, 0, lock);
                    env->delete_object(dst);
                    if (capture) {
                        void* buf = env->f_1c580(g_41a14 * (uint32_t)g_41a18,
                                                 "..\\3rdParty\\STORM\\SOURCE\\SDLG.CPP", 0x3fb, 0);
                        bounds.left = local_50 - g_41a0c;
                        vis.bottom = g_41a18;
                        bounds.top = local_4c - g_41a10;
                        vis.right = g_41a14;
                        vis.left = 0;
                        vis.top = 0;
                        env->f_1e830((uint32_t)(uintptr_t)buf, &vis.left, g_41a14, iVar1, &local_38,
                                     iVar6, param_4, param_5, 0, 0xcc0020, &bounds.left, diff,
                                     sel & 1, 0, nullptr);
                        // 0x382C0 (mapped elsewhere) with retail's first word, a Ghidra-unassigned
                        // register; the blit core does not read it on this path.
                        env->f_382c0(nullptr, (uint32_t)(uintptr_t)buf, g_453b4, (int32_t)g_41a14,
                                     g_41a18, (int32_t)g_41a14, (int32_t)g_41a14, 0, 0x8800c6);
                        env->f_382c0((void*)(uintptr_t)g_41a18, (uint32_t)(uintptr_t)buf, g_453b0,
                                     (int32_t)g_41a14, g_41a18, (int32_t)g_41a14, (int32_t)g_41a14,
                                     0, 0xee0086);
                        winrect.left = g_41a0c;
                        winrect.top = g_41a10;
                        env->f_1e830(0, &winrect.left, 0, (int32_t)(uintptr_t)buf,
                                     (int32_t*)&g_41a14, (int32_t)g_41a14, 0, 0, 0, 0xcc0020,
                                     &local_50, diff, sel & 0xfffffffeu, 0, lock);
                        env->delete_object(diff);
                        env->f_b960(buf, "..\\3rdParty\\STORM\\SOURCE\\SDLG.CPP", 0x43c);
                    }
                }
            }
        }
        return 1;
    }
    g_error = 0x57;
    StormSetLastError(0x57);
    return 1;
}
