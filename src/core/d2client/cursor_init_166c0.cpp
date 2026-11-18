#include <cstdint>
#include <cstddef>
#include <emscripten.h>

// The D2Client globals 0x11B824..0x11B867 this function writes, in memory order.
struct Client166c0State {
    int32_t  centerY;      // +0x00 0x11B824 = [0xDBC4C] / 2
    int32_t  centerX;      // +0x04 0x11B828 = [0xDBC48] / 2
    uint32_t arg;          // +0x08 0x11B82C = EAX on entry
    uint32_t pad0[2];      // +0x0C 0x11B830, 0x11B834 untouched
    uint32_t cursor[7];    // +0x14 0x11B838.. one per cursor-name row (stride 0x1C at 0xF8570)
    uint32_t v854;         // +0x30 0x11B854 = 5
    uint32_t v858;         // +0x34 0x11B858 = 0
    uint32_t v85c;         // +0x38 0x11B85C, low word := 0
    uint32_t tick;         // +0x3C 0x11B860 = GetTickCount()
    uint32_t v864;         // +0x40 0x11B864 = 1
};
static_assert(offsetof(Client166c0State, cursor) == 0x14 && offsetof(Client166c0State, v864) == 0x40, "166c0 state layout");

struct Client166c0Env {
    uint32_t (*gfxWindow)();                                                // D2Gfx ordinal 10048 (0xD0B4)
    void (*uiInit)(uint32_t hwnd, uint32_t ecx, uint32_t edx, uint32_t esi); // 0x3CA80, one call per 12-byte row
    uint32_t (*loadCursor)(uint32_t index);                                  // 0x2B420 on "DATA\\GLOBAL\\UI\\CURSOR\\<name[index]>"
    void (*fail)();                                                          // lang string 0x179 + message box (0xBF92/0xBF80); retail then _exit(-1)
};

// 1.13c D2Client+0x166C0: cursor/UI-resource init. tbl = 9 dwords at 0xF8530 (rows esi,ecx,edx); size = [0xDBC48],[0xDBC4C]; v85c is the
// prior value of 0x11B85C (only its low word is cleared). Returns 1; 0 where retail would _exit (not vectored).
extern "C" uint32_t d2_client_166c0(uint32_t eax, Client166c0State* g, const uint32_t* tbl, int32_t w, int32_t h, uint32_t tick, uint32_t* v11c4f0, const Client166c0Env* env) {
    g->arg = eax;
    uint32_t hwnd = env->gfxWindow();
    if (!hwnd) { env->fail(); return 0; }
    for (uint32_t i = 0; i < 3; ++i) env->uiInit(hwnd, tbl[i * 3 + 1], tbl[i * 3 + 2], tbl[i * 3]);
    for (uint32_t i = 0; i < 7; ++i) g->cursor[i] = env->loadCursor(i);
    *v11c4f0 = 1;   // 0x11C4F0
    g->v864 = 1;
    g->v854 = 5;
    g->v858 = 0;
    g->v85c &= 0xFFFF0000u;
    g->tick = tick;
    g->centerX = w / 2;
    g->centerY = h / 2;
    return 1;
}
