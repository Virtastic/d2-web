// src/core/d2win/d2win_batch_w2.cpp -- nine 1.13c D2Win.dll rows transcribed from the cached Ghidra
// decompiles (build-wasm/ghidra-d2win/decomp/). Alpha batch lane: implementation + marker only.
// Assert failures in retail call Fog #10265(code) then Fog #10024("", msg) and _exit(-1); the seam
// below receives the code, then the process exits.
// Not done (no faithful port from the decompile alone): 0x0E3C0 (decompile reads unrecovered
// registers), 0x0A470 (Storm/Fog ordinals unidentified), 0x11C30 (colour-code text draw, stack
// layout unrecovered).
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

struct D2WinW2Env {
    void (*fatal)(uint32_t code);          // Fog #10265 + #10024 report; may be null
    void (*d2gfx10044)(void);              // D2Gfx #10044 (0x17C40)
    void (*d2gfx10061)(void* table);       // D2Gfx #10061 (0x0EDA0)
    uint32_t (*getTickCount)(void);        // GetTickCount (0x0EDA0)
};
[[noreturn]] static void w2_fatal(const D2WinW2Env* e, uint32_t code) {
    if (e && e->fatal) e->fatal(code);
    _exit(-1);
}
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

// Retail module state.
uint32_t d2win_w2_g_6f90149c;      // 0x6F90149C
uint32_t d2win_w2_g_6f9a9e28;      // 0x6F9A9E28
uint32_t d2win_w2_g_6f9a9e2c;      // 0x6F9A9E2C
uint32_t d2win_w2_g_6f921630;      // 0x6F921630
uint32_t d2win_w2_g_6f8ffd44;      // 0x6F8FFD44
extern uint32_t d2win_g_6f901488, d2win_g_6f90148c;   // mouse x / y, d2win_batch_10030.cpp

// 1.13c D2Win+0x13640 (ordinal 10146): link two type-1 controls, a->+0x278 = b, b->+0x27C = a.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_13640(uint8_t* a, uint8_t* b, const D2WinW2Env* env) {
    if (!a) w2_fatal(env, 0x48a);
    if (rd32(a, 0) != 1) w2_fatal(env, 0x48c);
    if (!b) w2_fatal(env, 0x48e);
    if (rd32(b, 0) != 1) w2_fatal(env, 0x490);
    wr32(a, 0x9e * 4, (uint32_t)(uintptr_t)b);
    wr32(b, 0x9f * 4, (uint32_t)(uintptr_t)a);
}

// 1.13c D2Win+0x136A0 (ordinal 10166): pointer to the type-1 control's embedded block at +0x5C.
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t* d2win_sub_136A0(uint8_t* ctrl, const D2WinW2Env* env) {
    if (!ctrl) w2_fatal(env, 0x47e);
    if (rd32(ctrl, 0) != 1) w2_fatal(env, 0x480);
    return ctrl + 0x17 * 4;
}

// 1.13c D2Win+0x17C40 (ordinal 10148): clear 0x6F90149C, then D2Gfx #10044.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_17C40(const D2WinW2Env* env) {
    d2win_w2_g_6f90149c = 0;
    env->d2gfx10044();
}

// 1.13c D2Win+0x135C0 (ordinal 10152): 1 if the first byte of the string is an ASCII digit.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_135C0(uint32_t, uint32_t, const char* s) {
    return (*s > '/' && *s < ':') ? 1 : 0;
}

// 1.13c D2Win+0x17890 (ordinal 10156): PtInRect(rect, mouse). Rect is left, top, right, bottom.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_17890(const int32_t* r) {
    int32_t x = (int32_t)d2win_g_6f901488, y = (int32_t)d2win_g_6f90148c;
    return x >= r[0] && x < r[2] && y >= r[1] && y < r[3];
}

// 1.13c D2Win+0x086E0 (ordinal 10160): type-4 control whose child (+0x90) is type 5: child+0x48 =
// max(value, 0). Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_86E0(uint8_t* ctrl, int32_t value, const D2WinW2Env* env) {
    if (!ctrl) w2_fatal(env, 0x582);
    if (rd32(ctrl, 0) != 4) w2_fatal(env, 0x583);
    uint8_t* child = (uint8_t*)(uintptr_t)rd32(ctrl, 0x24 * 4);
    if (rd32(child, 0) != 5) w2_fatal(env, 0x56);
    wr32(child, 0x12 * 4, value > 0 ? (uint32_t)value : 0);
    return 1;
}

// 1.13c D2Win+0x097C0 (ordinal 10167): widen an ANSI string (max 0x400 units) and append it to a
// textbox via 0x93C0 (param3 = 4, 0, 0). Null box -> fatal 0x109. The 0x93C0 call is a seam.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_97C0(void* box, const char* s,
        void (*addText)(void* box, const uint16_t* text, int32_t p3, int32_t p4, int32_t p5),
        const D2WinW2Env* env) {
    if (!box) w2_fatal(env, 0x109);
    uint16_t buf[0x400] = {};                       // Unicode::win2Unicode(buf, s, 0x400)
    for (int i = 0; i < 0x400 && s[i]; ++i) buf[i] = (uint8_t)s[i];
    addText(box, buf, 4, 0, 0);
}

// 1.13c D2Win+0x0EE50 (ordinal 10164): entry idx of the 0x100-byte-stride table at 0x6F935B60.
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t* d2win_sub_EE50(uint8_t* table, int32_t idx) {
    return table + (uint32_t)idx * 0x100;
}

// 1.13c D2Win+0x0EDA0 (ordinal 10153): toggle the 0x6F9A9E28 mode. On: stamp GetTickCount,
// 0x6F921630 = 2, 0x6F8FFD44 = 1. Off: clear both flags. Then D2Gfx #10061(0x6F922338 block).
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_EDA0(int32_t on, void* block, const D2WinW2Env* env) {
    if (on) {
        d2win_w2_g_6f9a9e28 = 1;
        d2win_w2_g_6f9a9e2c = env->getTickCount();
        d2win_w2_g_6f921630 = 2;
        d2win_w2_g_6f8ffd44 = 1;
    } else {
        d2win_w2_g_6f9a9e28 = 0;
        d2win_w2_g_6f8ffd44 = 0;
    }
    env->d2gfx10061(block);
}
