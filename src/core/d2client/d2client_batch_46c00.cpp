// src/core/d2client/d2client_batch_46c00.cpp -- 9 UNMAPPED 1.13c D2Client.dll rows (0x46C00..0x47CA0)
// from the cached decompiles. Alpha batch lane: implementation + marker, no spec/vector.
// The pop-up/dialog "wait box" state machine around DAT 0x6FBC9729 (mode 1..5) and the unit hash at
// 0x6FBBAE08. Env seam: unrecovered callees by RVA via fn(), ordinals via ord(), image globals by
// address via g(rva); the retail-layout memory is read directly through those addresses.
// guess: widths of packed globals (DAT_6fbc9729 u32, 6fbc9663 u8, 6fbc978e u8, 471F0 stores u32 except
// 6fba3edd u16); register-passed args lost by the decompiler (Ordinal_10003 index, 0x46230 result fed to
// the "%d" wsprintf) are marked guess.
#include <cstdint>
#include <cstdio>
#include <emscripten.h>

struct D2Client46c00Env {
    int64_t (*ord)(uint32_t n, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d);
    int64_t (*fn)(uint32_t rva, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e, uintptr_t f, uintptr_t g, uintptr_t h, uintptr_t i);
    uintptr_t (*g)(uint32_t rva);          // address of a D2Client image address (global or code)
    void (*callp)(uintptr_t fnaddr, uintptr_t a);  // indirect call through a vtable slot
    uint32_t (*tick)(void);                // GetTickCount
    void (*die)(void);                     // _exit(-1)
};
static D2Client46c00Env e_;
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_46c00_set_env(const D2Client46c00Env* e) { e_ = *e; }

typedef uintptr_t U;
#include "d2client_47500.h"
static const uint32_t IB = 0x6FAB0000;
#define A(a) e_.g((uint32_t)((a) - IB))
#define D32(a) (*(volatile uint32_t*)A(a))
#define D16(a) (*(volatile uint16_t*)A(a))
#define D8(a)  (*(volatile uint8_t*)A(a))
static inline uint32_t R32(U p) { return *(volatile uint32_t*)p; }
static inline void W32(U p, uint32_t v) { *(volatile uint32_t*)p = v; }
static inline int64_t F(uint32_t r, U a = 0, U b = 0, U c = 0, U d = 0, U e = 0, U f = 0, U g = 0, U h = 0, U i = 0) { return e_.fn(r, a, b, c, d, e, f, g, h, i); }
static inline int64_t O(uint32_t n, U a = 0, U b = 0, U c = 0, U d = 0) { return e_.ord(n, a, b, c, d); }
static inline void fatal(uint32_t strId) { O(10024, A(0x6fb8359f), (U)O(10265, strId)); e_.die(); }

// unit hash lookup: DAT_6fbbae08[(id&0x7f)] chain via +0xE4, id at +0x0C
static U unitById(uint32_t id) {
    U p = R32(A(0x6fbbae08) + (id & 0x7f) * 4);
    while (p && R32(p + 0xc) != id) p = R32(p + 0xe4);
    return p;
}
static void destroyDialog() {   // shared by 46C00 / 47810 prologue
    if (D32(0x6fbc976f)) {
        F(0xA3D20);
        U o = D32(0x6fbc977b);
        if (o) e_.callp(R32(R32(o)), 1);
        D32(0x6fbc977b) = 0;
        D32(0x6fbc976f) = 0;
    }
}

extern "C" {

// 1.13c D2Client+0x46C00
EMSCRIPTEN_KEEPALIVE void d2c_46c00() {
    uint8_t buf[16] = {0};
    buf[0] = '6';
    *(uint32_t*)(buf + 1) = D32(0x6fbc9658);
    *(uint32_t*)(buf + 5) = *(volatile uint16_t*)(A(0x6fbc967d) + D32(0x6fbc9654) * 8);
    F(0x143E0, (U)buf);
    destroyDialog();
    D8(0x6fbc9663) = 10;
    F(0x46880);
    U box = (U)F(0xA3E40, 0, 0, A(0x6faf5bf0), 0, 0, 0, 1, 0, 0);
    D32(0x6fbc975f) = (uint32_t)box;
    D32(0x6fbc974b) = e_.tick() + 60000;
    D32(0x6fbc974f) = 0;
    if (!box) fatal(0x529);
    W32(box + 0x5c, 0);
    F(0xA36F0, box, (U)O(10003), 0xf, 0, 1, 0, 0);
}

// 1.13c D2Client+0x46E00
EMSCRIPTEN_KEEPALIVE void d2c_46e00() {
    for (int32_t i = 0; i < (int32_t)D32(0x6fba3b9c); i++) F(0x46910, A(0x6fba37a8) + (U)i * 0x16);   // EDI = &table[i] (record stride 0x16)
    D8(0x6fbc9662) = 1;
}

// 1.13c D2Client+0x46EE0 (stdcall, ret 8: param_1 dword, param_2 byte). The leaf calls carry their register/stack inputs as retail does
// (F args = the probe's FnMap order): 0x88A70 EBX + 4 stack words; ord 0xC202/0xC1BA/0xC1C6 1/1/3 stack words; 0x8BB00/0x8BB20 EAX = param_1;
// 0x148A0 ECX EDX + 2 stack words (AL = the packet code '2'/'3'/'5' is not passed: its upper bytes are stale in retail); 0x46880 ESI = &out[2];
// 0xA3E40 11 stack words (the port passes the first 9); 0xA3340 EAX + 1 stack word; 0xD1EC ECX; 0xA36F0 EAX ECX + 6 stack words.
EMSCRIPTEN_KEEPALIVE void d2c_46ee0(int32_t param_1, uint8_t param_2) {
    if (D8(0x6fbd22f8) == 0) { if (D32(0x6fbcbc34) == 5) F(0x8D300); return; }
    char ch;
    uint32_t mode = D32(0x6fbc9729);
    if (mode == 1) ch = '2';
    else if (mode == 2) ch = '3';
    else if (mode == 3) { ch = '5'; F(0x88A70, 0xf, 0, 0, 0, 0); }
    else {
        D32(0x6fbc9663) = 3;
        if (D32(0x6fbcbc34) == 5) F(0x8D300);
        return;
    }
    uint32_t u6 = 0;
    U p = 0;
    D32(0x6fbc978e) = 0;
    uint32_t id = D32(0x6fbd22fd);
    if (D32(0x6fbcbc90) == 0) {
        p = unitById(id);
        if (p && R32(p) != 4) fatal(0x5a);
        if (!p) {
            D32(0x6fbc9663) = 3;
            F(0x92850);
            D32(0x6fbd2301) = e_.tick();
            D32(0x6fbd2305) = D32(0x6fbd2305) + 1;
            D8(0x6fbd22f8) = 0;
            return;
        }
        u6 = R32(p + 0x10);
        if (mode == 2) {
            U q = (U)(uint32_t)F(0xC202, R32(D32(0x6fbcc338) + 0x60));
            D32(0x6fbc9737) = (q == p);
        }
    } else {
        if (e_.tick() - D32(0x6fbd3484) < 2000) { if (D32(0x6fbcbc34) == 5) F(0x8D300); return; }
        D32(0x6fbd3484) = e_.tick();
    }
    F(param_1 ? 0x8BB00 : 0x8BB20, (U)(uint32_t)param_1);
    D32(0x6fbc9663) = 5;
    D32(0x6fbd2309) = e_.tick();
    if (ch == '2') {
        u6 = (u6 & 0xffff) << 16;
        if (D32(0x6fbc97a8)) u6 |= 2;
        if (param_2 & 4) {
            U r = (U)(uint32_t)F(0xC1BA, p ? R32(p + 4) : 0xffffffffu);
            if (r && *(volatile uint8_t*)(r + 0x1a5)) u6 |= 0x80000000u;
        }
        F(0x148A0, D32(0x6fbc971d), D32(0x6fbd22fd), u6, D32(0x6fbc9777));
        D32(0x6fbc9753) = (uint32_t)F(0x81E40);
    } else {
        uint32_t v = D32(0x6fbc9777);
        if (ch == '5') v = D32(0x6fbcbc90) ? 0x80000000u : (p ? (uint32_t)F(0xC1C6, p, 0x48, 0) : 0);
        F(0x148A0, D32(0x6fbc971d), D32(0x6fbd22fd), u6 & 0xffff, v);
    }
    if (D32(0x6fbc975f) != 0) fatal(0x11b6);
    uint32_t loc[2] = { 0, 0 };
    F(0x46880, (U)loc);
    U box = (U)F(0xA3E40, loc[0], loc[1], A(0x6faf5bf0), 0, 0, 0, 1, 0, 0);
    D32(0x6fbc975f) = (uint32_t)box;
    D32(0x6fbc974b) = e_.tick() + 60000;
    D32(0x6fbc974f) = 0;
    F(0xA3340, D32(0x6fbc975f), 0);
    uint32_t s = (uint32_t)F(0xD1EC, 0xd19);
    F(0xA36F0, 0, D32(0x6fbc975f), s, 0xf, 0, 1, 0, 0);
}

// 1.13c D2Client+0x471F0
EMSCRIPTEN_KEEPALIVE void d2c_471f0() {
    // widths follow the retail stores: dword for the 2/3/4 values and the pointer, word (AX) for the zeroed halves and the 0xfb4 field
    static const struct { uint32_t a; uint32_t v; bool w; } s[] = {
        {0x6fba3d51,2,0},{0x6fba3d59,0,1},{0x6fba3d67,0,0},{0x6fba3d9f,2,0},{0x6fba3da7,0,1},{0x6fba3db5,0,0},
        {0x6fba3ded,2,0},{0x6fba3df5,0,1},{0x6fba3e03,0,0},{0x6fba3c19,3,0},{0x6fba3c23,0,1},{0x6fba3c33,0,0},
        {0x6fba4231,2,0},{0x6fba4239,0,1},{0x6fba4247,0,0}};
    const bool eq = D32(0x6fba4243) == 0x6faf6d10;
    for (auto& x : s) { if (x.w) D16(x.a) = (uint16_t)x.v; else D32(x.a) = x.v; }
    if (eq) D32(0x6fba4231) = 3;
    D16(0x6fba419f) = 0; D32(0x6fba41af) = 0; D16(0x6fba41ef) = 0; D32(0x6fba4201) = 0;
    D32(0x6fba3ed7) = 2; D16(0x6fba3edf) = 0; D32(0x6fba3eed) = 0;
    D16(0x6fba3edd) = 0; D32(0x6fba3ee9) = 0;
    D32(0x6fba4195) = 3; D32(0x6fba41e3) = 4;
    static const uint32_t b[] = {0x6fba3e14, 0x6fba3e3b, 0x6fba3e62, 0x6fba3e89, 0x6fba416e};
    for (uint32_t a : b) {   // {+0: 3, +6 (word): 0xfb4, +0x12: &LAB_6faf6af0}
        D32(a) = 3; D16(a + 6) = 0xfb4; D32(a + 0x12) = (uint32_t)A(0x6faf6af0);
    }
}

// env-seam adapter for the typed port in d2client_47500.cpp (1.13c D2Client+0x47500); F() = (rva, eax, ecx, stack words...)
EMSCRIPTEN_KEEPALIVE void d2c_47500() {
    struct S { static const char16_t* str(void*, uint32_t id) { return (const char16_t*)(U)O(10003, id); }
        static uint32_t menu(void*, const uint32_t w[11]) { return (uint32_t)F(0xA3E40, 0, 0, w[0], w[1], w[2], w[3], w[4], w[5], w[6]); }
        static void line(void*, uint32_t col, uint32_t m, const uint32_t w[6]) { F(0xA36F0, col, m, w[0], w[1], w[2], w[3], w[4], w[5]); }
        static void name(void*, uint32_t u, char16_t* b, uint32_t n) { F(0x914F0, 0, 0, u, (U)b, n); }
        static void cost(void*, uint32_t* o, uint32_t u, uint32_t c, char16_t* b, uint32_t n) { F(0x46230, (U)o, u, c, (U)b, n); }
        static void die(void*, uint32_t code) { fatal(code); } };
    d2client_47500::Env en = { nullptr, S::str, S::menu, S::line, S::name, S::cost, S::die };
    d2client_47500::State st = { D32(0x6fbc9757), D32(0x6fbc9663), D32(0x6fbc9729), D32(0x6fbcb824), D32(0x6fbcb828), D32(0x6fbd22fd),
        (char16_t*)A(0x6fbd230d), (const uint32_t*)A(0x6fbbae08) };
    d2client_47500::sub_47500(en, st);
    D32(0x6fbc9757) = st.box; D32(0x6fbc9663) = st.g9663;
}

// (env-seam copy; verified body: dialog_47810_4b420.cpp)
EMSCRIPTEN_KEEPALIVE void d2c_47810() {
    destroyDialog();
    D32(0x6fbc9729) = 5;
    d2c_47500();
}

// dup of D2CLIENT+0x47920
EMSCRIPTEN_KEEPALIVE void d2c_47920() {
    if (D32(0x6fbc975f)) { F(0xA3D20); D32(0x6fbc975f) = 0; }
    D32(0x6fbc9663) = 3;
    F(0x8F660);
    F(0x94970);
    D32(0x6fbcc2f8) = 0;
    D32(0x6fbcc2f4) = 0;
}

// 1.13c D2Client+0x47A70
EMSCRIPTEN_KEEPALIVE void d2c_47a70(int32_t param_1) {
    U a = (U)F(0x51A80);
    if (!a) return;
    U b = (U)F(0x46150);
    if (b && R32(a) == 4 && (int32_t)R32(a + 0xc) == param_1) {
        D32(0x6fbcc2f8) = 0;
        D32(0x6fbcc2f4) = 0;
        F(0x8BBF0);
    }
}

// 1.13c D2Client+0x47CA0
EMSCRIPTEN_KEEPALIVE void d2c_47ca0(int32_t param_1) {
    if (D32(0x6fbc978e) && D8(0x6fbc9781) == 0 && (uint32_t)param_1 == D32(0x6fbc9786)) F(0x47AB0);
}

}

extern "C" {
// 1.13c D2Client+0x47AB0
EMSCRIPTEN_KEEPALIVE void d2c_47ab0() {
    uint32_t t = e_.tick();
    if (t - D32(0x6fbd2301) < 0x3e9) {
        uint32_t n = D32(0x6fbd2305);
        if (n > 9) n = 10;
        D32(0x6fbc974f) = (n + 0x1e) * 500 + t;
    } else {
        D32(0x6fbc974f) = t + 100;
    }
    D32(0x6fbc978e) = 0;
    D8(0x6fbd22f8) = 0;
    D32(0x6fbd22f9) = 0;
    D32(0x6fbd22fd) = 0;
    if (D32(0x6fbcc394) == 0) F(0x47920);   // tail jump to 0x47920
}
}
