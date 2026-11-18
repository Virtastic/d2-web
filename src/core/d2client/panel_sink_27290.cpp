// The browser's control-panel base and stamina bar: D2Client+0x27290 (d2client_batch_27290.cpp) and D2Client+0x278B0
// (d2client_278b0.cpp) run against a recording dispatcher, their D2Gfx calls returned as a list the platform layer replays.
// Nothing here decides what is drawn or where; the ports do. Records are 8 words, as draw_sink.cpp:
//   cel  [0xD096, cell, frame, x, y(bottom row), -1, mode, 0]       cell 5 = the panel cell (800ctrlpnl7.dc6)
//   box  [0xD0BA, colour, mode, x1, y1, x2, y2, 0]                  the stamina bar (a D2Gfx #10055 box)
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_callee.h"

// The State / Env declarations of d2client_batch_27290.cpp, word for word (the same namespace, so one definition each).
namespace client27290 {
using I=int32_t; using U=uint32_t; using P=void*;
struct Cell { I frame{}; U reserved[12]{}; P file{}; U tail[4]{}; };
#pragma pack(push,1)
struct Key { U action; int16_t key; U side; };
#pragma pack(pop)
struct Threshold { U type,frame,high,medium,low; };
struct State {
 I width,height; P panel{},widePanel{},oldPanel{},button{},warning{},durability{},bars{},barFrame{};
 P statusPanel{},player{},hover{}; I selected{},hoverX{},hoverY{},mouseX{},mouseY{};
 I inventoryOpen{},hoverEnabled{},hoverA{},hoverB{},buttonPressed{},buttonEnabled{};
 I expansion{},blocked[5]{},panelOpen{},panelMode{}; uint8_t occupied[6]{};
 Key* keys; U keyCount; I* slots; I* ammoSlots; U ammoCount; Threshold* thresholds; U thresholdCount;
 I selectionSource{},selectionPage{};
};
struct Env {
 U (*word)(P,U); P (*pointer)(P,U); U (*retailAddress)(P); P (*resolveRetail)(U);
 I (*resolution)(); I (*renderer)(); P (*load)(U); void (*unload)(P); void (*freeFile)(P,U,U,U);
 void (*draw)(const Cell*,I,I,I,I,U); void (*clip)(const Cell*,I,I,I,I,I);
 I (*current)(I); I (*maximumLife)(P); I (*maximumMana)(P); I (*state)(P,I); I (*stat)(P,I,I);
 I (*isExpansion)(); P (*cursor)(P); void (*selectionInfo)(I,I,uint8_t*);
 I (*selectionBlocked)(I,I); I (*cursorValid)(P); I (*selectionTest)(I,P,I,U*); void (*select)(I,I);
 I (*buttonTest)(I);
 P (*equipped)(P,I); I (*durable)(P); I (*excluded)(P); I (*maxDurability)(P); I (*itemFlag)(P,I); I (*special)(P);
 I (*ammoStat)(P); uint8_t (*ammoType)(P); I (*singleStack)(P);
 P (*itemRecord)(U); P (*first)(P); P (*next)(P); P (*item)(P); uint8_t (*skip)(P);
 const char16_t* (*localized)(U); U (*keyName)(U,U);
 void (*formatKey)(char16_t*,U,const char16_t*,U);
 void (*text)(const char16_t*,I,I,I,I);
 void (*name)(P,char16_t*,U); I (*quality)(P); void (*description)(P,I,P,char16_t*);
 const char16_t* (*colorText)(uint8_t,const char16_t*); I (*extraEnabled)(); I (*extra)(P,I,char16_t*,U,I*);
 void (*fatal)(U,U,I);
};
extern "C" void d2_client_27290(State* s, Env* e, I ebx);   // defined in d2client_batch_27290.cpp inside this namespace, C linkage
}
extern "C" void d2c_278b0(const D2cSvc& v, uint32_t unit, int32_t w, int32_t h, int32_t mouseX, int32_t mouseY, uint8_t* pal);

namespace {
constexpr int kFields = 8, kMax = 16;
int32_t g_list[kMax * kFields];
int g_count = 0;
void put(int32_t a, int32_t b, int32_t c, int32_t d, int32_t e, int32_t f, int32_t g, int32_t h) {
    if (g_count >= kMax) return;
    int32_t* r = g_list + g_count++ * kFields;
    r[0] = a; r[1] = b; r[2] = c; r[3] = d; r[4] = e; r[5] = f; r[6] = g; r[7] = h;
}

// 0x27290's environment: the panel cell is one handle (5), the screen is the 800x600 resolution (query 2).
using namespace client27290;
P loadCell(U) { return (P)(uintptr_t)5; }
void noUnload(P) {}
void noFree(P, U, U, U) {}
I resolution() { return 2; }
void drawCel(const Cell* c, I x, I y, I, I mode, U) { put(0xd096, (int32_t)(uintptr_t)c->file, c->frame, x, y, -1, mode, 0); }
State g_state;
Env g_env;

struct Stamina { int32_t cur, max, over; uint8_t ci[3]; };
uint32_t staminaCallee(void* c, uint32_t rva, const uint32_t* r, const uint32_t* s) {
    const Stamina* x = (const Stamina*)c;
    switch (rva) {
    case 0x26b20: return (uint32_t)x->cur;                              // smoothed stat 10 (stamina)
    case 0xcf34: return (uint32_t)x->max;                               // stamina maximum
    case 0xcbda: return (uint32_t)x->over;                              // state 0x18 test
    case 0xd390: return s[0] == 0xff ? x->ci[0] : s[0] == 0xf4 ? x->ci[1] : x->ci[2];   // D2Win #10190: red, gold, blue
    case 0xd0ba: put(0xd0ba, (int32_t)s[4], (int32_t)s[5], (int32_t)s[0], (int32_t)s[1], (int32_t)s[2], (int32_t)s[3], 0); return 0;
    default: return 0;                                                  // Unicode ctor, 0x88 state test: nothing set
    }
}
}  // namespace

extern "C" {
// D2Client+0x27290 at screen width w (the 800x600 base plus the free-resolution edges), height h; `ebx` is 0x29250's panel flag.
// Returns the record count; `out` takes 8 words per record (up to 16).
EMSCRIPTEN_KEEPALIVE int32_t d2_client_draw_panel_base(int32_t w, int32_t h, int32_t ebx, int32_t* out) {
    g_count = 0;
    if (!g_env.draw) { g_env.resolution = resolution; g_env.load = loadCell; g_env.unload = noUnload; g_env.freeFile = noFree; g_env.draw = drawCel; }
    g_state.width = w; g_state.height = h;
    d2_client_27290(&g_state, &g_env, ebx);
    std::memcpy(out, g_list, sizeof(int32_t) * kFields * g_count);
    return g_count;
}

// D2Client+0x278B0 (the stamina bar) with the smoothed current / maximum raw values and the 0x18-state flag; ci0..ci2 are the
// palette indices D2Win #10190 returns for red, gold and blue. The pointer is off the bar, so no hover text is built.
// One box record; returns 1.
EMSCRIPTEN_KEEPALIVE int32_t d2_client_draw_stamina(int32_t cur, int32_t max, int32_t over, int32_t w, int32_t h,
                                                    int32_t ci0, int32_t ci1, int32_t ci2, int32_t* out) {
    g_count = 0;
    Stamina st = { cur, max, over, { (uint8_t)ci0, (uint8_t)ci1, (uint8_t)ci2 } };
    uint8_t pal[3] = { 0, 0, 0 };
    const D2cSvc v = { &st, staminaCallee };
    d2c_278b0(v, 0, w, h, -1, -1, pal);
    std::memcpy(out, g_list, sizeof(int32_t) * kFields * g_count);
    return g_count;
}
}
