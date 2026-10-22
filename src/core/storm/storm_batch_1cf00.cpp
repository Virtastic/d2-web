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

uint32_t Storm_Sub15ED0(int32_t i);                           // 0x25ED0 hash component
void Storm_Sub3AE00(char* dst, const char* src, uint32_t n);  // 0x3AE00 append
void Storm_Sub1D570();                                        // 0x1D570 idle step
uint32_t Storm_Sub19200(void*);                               // 0x19200 I/O thread proc
void* Storm_GetDesktopWindow();
void* Storm_GetTopWindow(void*);
void* Storm_GetNextSibling(void*);
uint32_t Storm_WindowPid(void*);
uint32_t Storm_CurrentPid();
void Storm_InvalidateIntersect(void* hwnd, int32_t l, int32_t t, int32_t r, int32_t b);
bool Storm_PeekMessage(void* msg, uint32_t* message, uintptr_t* wParam);
bool Storm_IsDialogMessage(void* dlg, void* msg);
void Storm_TranslateDispatch(void* msg);
void Storm_PostQuit(uintptr_t code);
void Storm_GetCursorPos(int32_t* x, int32_t* y);
void Storm_GetModuleFileName(char* buf, uint32_t n);
void* Storm_CreateEvent();
void* Storm_CreateThread(uint32_t (*fn)(void*));
void Storm_SetThreadPriority(void* h, int32_t p);
}  // namespace

// 1.13c Storm+0x1CF00: walk the window tree; for windows of this process, invalidate the cursor rect.
void Storm_Sub1CF00(void* hwnd, int32_t x, uint32_t y) {
    uint32_t y0 = y;
    while (hwnd == nullptr) {
        uint32_t oldY = (uint32_t)g_cursorY;
        Storm_Sub1CF00(Storm_GetDesktopWindow(), g_cursorX, oldY);
        g_cursorY = (int32_t)y0;
        g_cursorX = x;
        hwnd = Storm_GetDesktopWindow();
    }
    if (Storm_WindowPid(hwnd) == Storm_CurrentPid())
        Storm_InvalidateIntersect(hwnd, x, (int32_t)y0, g_cursorW + x, g_cursorH + (int32_t)y0);
    for (void* c = Storm_GetTopWindow(hwnd); c != nullptr; c = Storm_GetNextSibling(c))
        Storm_Sub1CF00(c, x, y0);
}

// 1.13c Storm+0x1D600: one pump step; dlg is retail's implicit ESI. Returns 1 if a message was taken.
uint32_t Storm_Sub1D600(void* dlg) {
    alignas(8) unsigned char msg[48];
    uint32_t message; uintptr_t wParam;
    if (!Storm_PeekMessage(msg, &message, &wParam)) {
        Storm_Sub1D570();
        int32_t cx, cy;
        Storm_GetCursorPos(&cx, &cy);
        if (g_cursorX >= 0 && g_cursorY >= 0 && (cx != g_cursorX || cy != g_cursorY))
            Storm_Sub1CF00(nullptr, cx, (uint32_t)cy);
        return 0;
    }
    if (message != 0x12) {
        if (dlg != nullptr && Storm_IsDialogMessage(dlg, msg)) return 1;
        Storm_TranslateDispatch(msg);
        return 1;
    }
    Storm_PostQuit(wParam);
    return 1;
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
