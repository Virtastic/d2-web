// Prior art: none vendored; cached 1.13c decompiles 0x7C060 and 0x7C1D0 (byte-identical bodies).
// Seam style as d2game_batch_181a0.cpp: retail byte offsets, callees injected via Env.
// GUESS marks arguments the decompile lost (register/stack args not shown).
#include <cstdint>
#include <cstring>
namespace d2game_batch7c060 {
using U = uint32_t; using I = int32_t; using P = void*;
static U r(P p,U o) { U v; std::memcpy(&v,(uint8_t*)p+o,4); return v; }
static I h(P p,U o) { int16_t v; std::memcpy(&v,(uint8_t*)p+o,2); return v; }
static uint8_t b(P p,U o) { return ((uint8_t*)p)[o]; }
static void w(P p,U o,U v) { std::memcpy((uint8_t*)p+o,&v,4); }
struct Env {
    P (*ptr)(P,U);                 // retail pointer dword -> native object
    void (*skill)(P unit);         // F2380; GUESS: AL mode/skill register arg not shown
    void (*idle)(I frames);        // F22C0; GUESS: game/unit implicit regs
    void (*reset)(P unit,I);       // D2Common 11103
    void (*change)(P game,P unit); // 75E10; GUESS: stack0xffffffd8 Mode block built from unit
};
static void body(Env& e,P game,P unit,P a) {
    P st=e.ptr(a,0), sk=e.ptr(a,0x1c);
    auto fail=[&]{ if(unit) w(unit,0xc4,r(unit,0xc4)|0x20000); e.reset(unit,0); e.change(game,unit); };
    if (r(a,8)==0) return fail();
    U g=b(game,0x6d);
    if (I(r(a,0x14))<=h(sk,0x56+2*g) && I(r(st,0x14))<h(sk,0x5c+2*g)) {
        if (h(sk,0x170)>=0 && r(st,0x18)==0) {
            e.skill(unit); w(st,0x14,r(st,0x14)+1); w(st,0x18,1); return;
        }
        w(st,0x18,0); e.idle(h(sk,0x62+2*g)); return;
    }
    fail();
}
// 1.13c D2Game+0x7C060
void sub_7C060(Env& e,P game,P unit,P a) { body(e,game,unit,a); }
// 1.13c D2Game+0x7C1D0
void sub_7C1D0(Env& e,P game,P unit,P a) { body(e,game,unit,a); }
}
