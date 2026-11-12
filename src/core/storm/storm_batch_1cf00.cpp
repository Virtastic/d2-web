// src/core/storm/storm_batch_1cf00.cpp -- five 1.13c Storm.dll functions transcribed from the cached
// decompiles: 0x1CF00 (invalidate cursor rects), 0x1D600 (message pump step), 0x26860 (module dir ->
// base path), 0x26AC0 (hash-table probe), 0x29560 (start async I/O thread). Batch lane: marker only.
// 0x03E20..0x04C60 have no cached decompile; 0x29DF0 reads ESI as an implicit argument (not faithful).
// Win32 and unmapped callees are seams with retail argument order (compat/Windows.h has no window API).
#include <cstdint>
#include <cstring>

namespace {
int32_t g_cursorX = -1;   // 0x6FC41A0C
int32_t g_cursorY = -1;   // 0x6FC41A10
int32_t g_cursorW;        // 0x6FC41A14
int32_t g_cursorH;        // 0x6FC41A18
char g_basePath[0x104];   // 0x6FC43000
const char g_sep[] = "\\";// 0x6FC370D4
void* g_ioThread;         // 0x6FC43110
void* g_ioEvent;          // 0x6FC4312C

uint32_t Storm_Sub15ED0(int32_t) { return 0; }                // 0x25ED0 hash component seam (not wired; 0x26AC0 is not proved here)
// 0x3AE00 bounded strcat (non-null arguments, n > 0, the only case 0x26860 produces).
void Storm_Sub3AE00(char* dst, const char* src, uint32_t n) {
    dst[n - 1] = '\0';
    uint32_t l = (uint32_t)strlen(dst);
    uint32_t room = n - l;
    uint32_t i = 0;
    for (; i + 1 < room && src[i]; ++i) dst[l + i] = src[i];
    dst[l + i] = '\0';
}
void Storm_Sub1D570() {}                                        // 0x1D570 idle step
uint32_t Storm_Sub19200(void*) { return 0; }                               // 0x19200 I/O thread proc
}  // namespace
// user32 / kernel32 imports by retail IAT slot (the FN probe in src/core/fn/storm_walker.cpp records them); no hook answers 0.
extern "C" uint32_t (*g_storm_imp_hook)(uint32_t slot, uint32_t w0, uint32_t w1, uint32_t w2);
namespace {
inline uint32_t Imp(uint32_t slot, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0) {
    return g_storm_imp_hook ? g_storm_imp_hook(slot, a, b, c) : 0;
}
inline uint32_t P(const volatile void* p) { return (uint32_t)(uintptr_t)p; }
void (*g_modFile)(char* buf, uint32_t n);
uint32_t Storm_GetModuleFileName(char* buf, uint32_t n) {
    if (!g_modFile) return 0;
    g_modFile(buf, n);
    return (uint32_t)strlen(buf);
}
void* Storm_CreateEvent() { return (void*)(uintptr_t)1; }   // no host event: a non-null token, as a created handle is
void* Storm_CreateThread(uint32_t (*)(void*)) { return (void*)(uintptr_t)1; }   // no host thread: a non-null token, so the start-once test holds
void Storm_SetThreadPriority(void*, int32_t) {}
}  // namespace

// 1.13c Storm+0x1CF00: walk the window tree; for windows of this process, invalidate the cursor rect.
void Storm_Sub1CF00(void* hwnd, int32_t x, uint32_t y) {
    uint32_t y0 = y;
    while (hwnd == nullptr) {
        uint32_t oldY = (uint32_t)g_cursorY;
        Storm_Sub1CF00((void*)(uintptr_t)Imp(0x43320), g_cursorX, oldY);   // GetDesktopWindow
        g_cursorY = (int32_t)y0;
        g_cursorX = x;
        hwnd = (void*)(uintptr_t)Imp(0x43320);
    }
    const uint32_t h = P(hwnd);
    volatile uint32_t pid = 0;
    Imp(0x43350, h, P(&pid));                                             // GetWindowThreadProcessId
    if (pid == Imp(0x430f4)) {                                            // GetCurrentProcessId
        volatile int32_t src[4] = {x, (int32_t)y0, g_cursorW + x, g_cursorH + (int32_t)y0};
        volatile int32_t wr[4] = {0, 0, 0, 0};
        volatile int32_t dst[4] = {0, 0, 0, 0};
        Imp(0x432b0, h, P(wr));                                           // GetWindowRect
        if (Imp(0x43304, P(dst), P(src), P(wr)) != 0) {                   // IntersectRect
            Imp(0x432b4, h, P(dst));                                      // ScreenToClient (left, top)
            Imp(0x432b4, h, P(dst + 2));                                  // ScreenToClient (right, bottom)
            Imp(0x43308, h, P(dst), 0);                                   // InvalidateRect(h, &dst, FALSE)
        }
    }
    for (uint32_t c = Imp(0x432a0, h); c != 0; c = Imp(0x43358, c, 2))   // GetTopWindow / GetWindow(GW_HWNDNEXT)
        Storm_Sub1CF00((void*)(uintptr_t)c, x, y0);
}

// 1.13c Storm+0x1D600: one pump step; dlg is retail's implicit ESI. Returns 1 if a message was taken.
uint32_t Storm_Sub1D600(void* dlg) {
    volatile uint32_t msg[7] = {0, 0, 0, 0, 0, 0, 0};   // MSG: hwnd, message, wParam, lParam, time, pt
    if (Imp(0x43380, P(msg), 0, 0) == 0) {               // PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)
        Storm_Sub1D570();
        volatile int32_t pt[2] = {0, 0};
        Imp(0x43324, P(pt));                             // GetCursorPos
        if (g_cursorX >= 0 && g_cursorY >= 0 && (pt[0] != g_cursorX || pt[1] != g_cursorY))
            Storm_Sub1CF00(nullptr, pt[0], (uint32_t)pt[1]);
        return 0;
    }
    if (msg[1] != 0x12) {                                // WM_QUIT
        if (dlg != nullptr && Imp(0x432f0, P(dlg), P(msg)) != 0) return 1;   // IsDialogMessageA
        Imp(0x4337c, P(msg));                            // TranslateMessage
        Imp(0x4338c, P(msg));                            // DispatchMessageA
        return 1;
    }
    Imp(0x432a8, msg[2]);                                // PostQuitMessage(wParam)
    return 1;
}

// FN probe entry (src/core/fn/storm_walker.cpp): cur = the pinned cursor words (0x51A0C x, y, w, h) copied in and out;
// pump 0 runs 0x1CF00(hwnd, x, y), pump 1 runs 0x1D600(dlg) and stores its result.
extern "C" void storm_1cf00_fnrun(int32_t* cur, void* hwnd, int32_t x, uint32_t y, int pump, void* dlg, uint32_t* ret) {
    g_cursorX = cur[0]; g_cursorY = cur[1]; g_cursorW = cur[2]; g_cursorH = cur[3];
    if (pump) *ret = Storm_Sub1D600(dlg);
    else Storm_Sub1CF00(hwnd, x, y);
    cur[0] = g_cursorX; cur[1] = g_cursorY; cur[2] = g_cursorW; cur[3] = g_cursorH;
}

// 1.13c Storm+0x26860: base path = directory of the host exe (cut at the last '\'), then the
// separator appended. The retail 0x57 branches need a null pointer into the buffer; unreachable.
void Storm_Sub26860() {
    Storm_GetModuleFileName(g_basePath, 0x104);
    char* last = nullptr;
    for (char* p = g_basePath; *p; ++p)
        if (*p == '\\') last = p;
    if (last) *last = '\0';
    Storm_Sub3AE00(g_basePath, g_sep, 0x104);
}

// 1.13c Storm+0x26AC0: probe the 16-byte-entry hash table for (a,b,locale,platform). ECX = table
// object, BL (implicit) = platform; exact match returns at once, else best wildcard slot or -1.
uint32_t Storm_Sub26AC0(void* self, int16_t locale, uint8_t platform) {
    uint32_t h0 = Storm_Sub15ED0(0), h1 = Storm_Sub15ED0(1), h2 = Storm_Sub15ED0(2);
    uint8_t* base = *(uint8_t**)((uint8_t*)self + 0x138);
    uint32_t mask = *(uint32_t*)(*(uint8_t**)((uint8_t*)self + 0x130) + 0x18) - 1;
    uint32_t start = h0 & mask, i = start, best = 0xffffffffu;
    for (;;) {
        uint32_t* e = (uint32_t*)(base + i * 0x10);
        if (e[3] == 0xffffffffu) return best;
        if (e[0] == h1 && e[1] == h2 && e[3] != 0xfffffffeu) {
            int16_t l = (int16_t)e[2];
            uint8_t pl = ((uint8_t*)e)[10];
            if (l == locale && pl == platform) return i;
            if ((l == 0 || l == locale) && (pl == 0 || pl == platform)) best = i;
        }
        i = (i + 1) & mask;
        if (i == start) return best;
    }
}

// 1.13c Storm+0x29560: create the I/O event and low-priority worker thread once.
void Storm_Sub29560() {
    if (g_ioThread == nullptr) {
        g_ioEvent = Storm_CreateEvent();
        g_ioThread = Storm_CreateThread(Storm_Sub19200);
        Storm_SetThreadPriority(g_ioThread, 1);
    }
}

// Capture-probe accessor: the I/O thread and wake event words (module RVAs 0x53110 and 0x5312C) copied in and out around the call.
extern "C" void storm_29560_fnrun(uint32_t* g) {
    g_ioThread = (void*)(uintptr_t)g[0];
    g_ioEvent = (void*)(uintptr_t)g[7];
    Storm_Sub29560();
    g[0] = (uint32_t)(uintptr_t)g_ioThread;
    g[7] = (uint32_t)(uintptr_t)g_ioEvent;
}

// Capture-probe accessor: the base-path buffer (module RVA 0x53000, 0x104 bytes) copied in and out around the call;
// modFile stands in for the GetModuleFileNameA import.
extern "C" void storm_26860_fnrun(char* bp, void (*modFile)(char*, uint32_t)) {
    memcpy(g_basePath, bp, sizeof g_basePath);
    g_modFile = modFile;
    Storm_Sub26860();
    g_modFile = nullptr;
    memcpy(bp, g_basePath, sizeof g_basePath);
}
// The base-path buffer itself (module RVA 0x53000): the probe logs the module address where the call passes it.
extern "C" const char* storm_26860_basepath() { return g_basePath; }
