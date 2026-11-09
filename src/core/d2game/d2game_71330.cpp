// src/core/d2game/d2game_71330.cpp -- 1.13c D2Game.dll PLRTRADE_RequestTrade (sub_71330), transcribed from the cached decompile
// and resolved against its disassembly (register inputs, which packet goes to which client, the dead local after the F62E0 call).
// Convention as d2game_batch_aea40.cpp: raw retail records, register inputs explicit, callees weak externs.
#include <cstdint>

typedef intptr_t I;
typedef int32_t S;
typedef uint32_t U;
#define O(n) extern "C" __attribute__((weak)) I Ord##n(...)
#define F(n) extern "C" __attribute__((weak)) I D2GAME_##n(...)
#define R32(b, o) (*(uint32_t*)((uintptr_t)(b) + (o)))
#define S32(b, o) (*(int32_t*)((uintptr_t)(b) + (o)))
#define R8(b, o)  (*(uint8_t*)((uintptr_t)(b) + (o)))

extern "C" __attribute__((weak)) char D2GAME_DAT_F9FF9[];   // the fatal-error source-file string (retail .rdata 0x6FD19FF9)
extern "C" __attribute__((weak)) uint32_t GetTickCount();
O(10920); O(10265); O(10024);
F(8A3E0); F(D62E0); F(6FFA0); F(70B70); F(2A3C);

// The client record the packet goes to: only a unit whose first word is 0 has one, found through 10920 at +0x9C.
static inline I clientOf(I u) { return R32(u, 0) == 0 ? (I)R32(Ord10920(u), 0x9c) : 0; }
static inline void send2(I u, uint8_t a, uint8_t b) { uint8_t p[2] = { a, b }; D2GAME_8A3E0(clientOf(u), p, 2); }
static inline void send6(I u, uint8_t k) { uint8_t p[6] = { 0x79, k, 0, 0, 0, 0 }; D2GAME_8A3E0(clientOf(u), p, 6); }

// 1.13c D2Game+0x71330 -- ECX = game, EDX = the requesting player unit, stack = the target unit record (or 0). ret 4.
extern "C" void d2game_71330_env(I game, I unit, I target)
{
    const U id = R32(unit, 0x64);
    if (R8(unit, 0x6c) != 0 && R32(unit, 0x68) != 0) { send2(unit, 0x77, 0x0c); return; }
    if (!game) {
        Ord10024(D2GAME_DAT_F9FF9, Ord10265(0x2d3));   // fatal diagnostic, then _exit(-1)
        D2GAME_2A3C(0xffffffff);
        return;
    }
    I other = (I)R32(game, 0x1120 + (id & 0x7f) * 4);
    while (other && R32(other, 0xc) != id) other = (I)R32(other, 0xe4);
    if (!other) { send2(unit, 0x77, 0x0c); return; }
    S key = target ? S32(target, 0xc) : -1;
    I a = Ord10920(unit);
    if (a && R32(a, 0x5c)) {
        for (I n = (I)R32(R32(a, 0x5c), 8); n; n = (I)R32(n, 8)) {
            if (S32(n, 0) == key) {
                uint32_t scratch[4];
                if (D2GAME_D62E0(other, 0, game, R32(n, 4), scratch, 1, 1, 1)) D2GAME_6FFA0(unit);
                break;
            }
        }
    }
    I pa = Ord10920(unit);
    I pb = Ord10920(other);
    uint32_t until = GetTickCount() + 10000;
    R32(R32(pa, 0x5c), 0x14) = until;
    R32(R32(pb, 0x5c), 0x14) = until;
    send2(unit, 0x77, 0x0e);
    send2(other, 0x77, 0x0e);
    if (S32(pa, 0x50) == 4 || S32(pb, 0x50) == 4) {
        send6(unit, 0);
        send6(unit, 1);
        send6(other, 0);
        send6(other, 1);
        D2GAME_70B70(unit, pa, pb);
    }
}
