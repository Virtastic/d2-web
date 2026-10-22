// src/core/d2win/d2win_batch_85a0.cpp -- four 1.13c D2Win rows from the cached Ghidra decompiles.
// Alpha batch lane: implementation + marker only. Rows 0x085A0 0x08A50 0x0A820 0x0B340 0x0B3A0
// 0x0F5E0 0x0F6B0 0x0F7E0 have no cached decompile and are skipped. Callees are seams.
#include <cstdint>
#include <emscripten.h>

struct D2WinBatch85A0Env {
    int32_t (*maxLineWidth)(const uint16_t*);               // 0x11FB0
    int32_t (*textBlockHeight)(const uint16_t*);            // 0x11E80
    void (*win2Unicode)(uint16_t* dst, const char* src, int32_t n);  // Fog/Unicode::win2Unicode
    void (*sub_14DF0)(int32_t* a, const uint16_t* text);    // 0x14DF0
    uint32_t (*getWindow)(void);                            // D2Gfx #10048
    void (*getDimensions)(int32_t*, int32_t*);              // #10080
    void (*getWindowRect)(uint32_t hwnd, int32_t* rect);    // user32 GetWindowRect
    void (*d2gfx10104)(int32_t);
    int32_t (*d2gfx10060)(uint32_t, int32_t);
    void (*fatal)(const char*);                             // #10029, then _exit(-1)
    void (*d2gfx10061)(void*);
    void (*d2gfx10068)(int32_t, int32_t, int32_t, int32_t);
};

extern "C" {
uint32_t d2win_g_6f9014b8, d2win_g_6f9a9cec;              // 0x17070
uint32_t d2win_g_6f90149c, d2win_g_6f9014d4, d2win_g_6f9014d8;
int32_t d2win_g_6f9014cc[4];                              // window rect
uint32_t d2win_g_6f9a9e28, d2win_g_6f8ffd44;
uint8_t d2win_g_6f922338[1];
}

// 1.13c D2Win+0x12700 -- out1 = widest line, out2 = block height (of the implicit ECX text).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_12700(const uint16_t* text, int32_t* out1, int32_t* out2, const D2WinBatch85A0Env* env) {
    *out1 = env->maxLineWidth(text);
    *out2 = env->textBlockHeight(text);
}

// 1.13c D2Win+0x14E70 -- widen the ANSI string (up to 0x100 units) and pass it to 0x14DF0.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_14E70(int32_t* a, const char* s, const D2WinBatch85A0Env* env) {
    uint16_t buf[512] = {};
    env->win2Unicode(buf, s, 0x100);
    env->sub_14DF0(a, buf);
}

// 1.13c D2Win+0x17070 -- store two module words, report 1.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2win_sub_17070(uint32_t a, uint32_t b) {
    d2win_g_6f9014b8 = a;
    d2win_g_6f9a9cec = b;
    return 1;
}

// 1.13c D2Win+0x17B60 -- first call saves window size and rect, then resize to mode; fatal on failure.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2win_sub_17B60(uint32_t mode, const D2WinBatch85A0Env* env) {
    if (d2win_g_6f90149c == 0 && d2win_g_6f9014d4 == 0) {
        uint32_t hwnd = env->getWindow();
        int32_t w, h;
        env->getDimensions(&w, &h);
        d2win_g_6f9014d4 = (uint32_t)w;
        d2win_g_6f9014d8 = (uint32_t)h;
        env->getWindowRect(hwnd, d2win_g_6f9014cc);
    }
    d2win_g_6f90149c = mode;
    env->d2gfx10104(1);
    if (env->d2gfx10060(mode, 0) == 0) env->fatal("Failed to resize window... this is fatal!");
    d2win_g_6f9a9e28 = 0;
    d2win_g_6f8ffd44 = 0;
    env->d2gfx10061(d2win_g_6f922338);
    env->d2gfx10068(0, 0x800000, 0x7d000, 0);
    return 1;
}
