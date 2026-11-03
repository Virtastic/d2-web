// Implementation batch 5d1d0: two twin helpers spawning object 0x79 (as 0x3c-class room object) near a unit,
// written from the cached 1.13c decompiles. Callees by retail RVA (rNNNN) / ordinal (oNNNNN); in_EAX /
// unaff_* registers are explicit parameters. Unverified seams: register args of o10331/o10346/o10111/o10258,
// and the state struct the decompile's garbled "uVar10+0x89" tail writes to (taken as the struct already
// tested: piVar5[6] / param_1+0x18). RE-DONE only, not linked or vectored.
#include <cstdint>
#include <cstdlib>
using i32 = int32_t; using u32 = uint32_t;
extern "C" {
intptr_t o10331(...); i32 o10826(i32); char o10459(i32); intptr_t o10750(...); intptr_t o10867(...); intptr_t o10265(...);
intptr_t o10024(...); i32 r_21280(...); intptr_t r_E1D90(...);
void o11090(i32*, i32); i32* r_A22E0(...); uint8_t o10258(i32*); void o10111(i32*, i32); void o10346(i32, i32);
}
// NB: unitX reads +0x10 / #10750 which is the unit Y; unitY reads +0xc / #10867 which is the unit X (5D960 stores them as nTargetY / nTargetX). The (y+10, x+5) call below is therefore (X+10, Y+5).
static i32 unitX(i32* u) { return (*u == 2 || (*u > 3 && *u < 6)) ? *(i32*)(intptr_t)(u[0xb] + 0x10) : (u[0xb] ? (i32)o10750((intptr_t)(u32)u[0xb]) : 0); }
static i32 unitY(i32* u) { return (*u == 2 || (*u > 3 && *u < 6)) ? *(i32*)(intptr_t)(u[0xb] + 0xc) : (u[0xb] ? (i32)o10867((intptr_t)(u32)u[0xb]) : 0); }
[[noreturn]] static void die(i32 line) { o10024((void*)0, o10265(line)); std::_Exit(-1); }

// shared tail (disassembly-checked): spawn 0x79 at (x+5,y+10) near the unit's room, link it, false if creation failed.
// 1D90: ECX=2, EDX=0x3c, stack (y+10, x+5, game, h, 1, 1, 0); A22E0: stack (game, obj, 0x79, owner class id).
static bool spawn79(i32* unit, i32 game, uint8_t* st, bool full)
{
    i32 x = unitX(unit), y = unitY(unit);
    i32 owner = (i32)o10331(unit);
    if (!owner) die(0xe42);
    char b = o10459(0x79);
    i32 cls = o10826(owner);
    if (b != o10459(cls)) die(0xe5c);
    i32 h = r_21280(owner);
    if (!h) return false;
    i32* o = (i32*)r_E1D90(2, 0x3c, y + 10, x + 5, game, h, 1, 1, 0);
    o11090(o, 1);
    o = r_A22E0(game, o, 0x79, cls);
    if (!o) return false;
    o10111(o, (uint8_t)(o10258(o) | 3));
    o10346(h, 0); o10346((i32)o10331(o), 0);
    if (full) { st[0x88] = 1; st[0x87] = 0; } else st[0x89] = 0;
    return true;
}

// 1.13c D2Game+0x5D1D0 (EAX = unit, stack = game; ret 4)
void D2GAME_5D1D0(i32* unit, uint8_t* p1)
{
    i32* it = nullptr;
    if (*(i32***)(p1 + 0x10f4)) for (it = **(i32***)(p1 + 0x10f4); it; it = (i32*)(intptr_t)it[0x3d]) if (*it == 0x22) break;
    if (!it) return;
    uint8_t* st = (uint8_t*)(intptr_t)it[6];
    if (!st[0x89]) return;
    if (o10459(o10826((i32)o10331(unit))) != 4) return;
    spawn79(unit, (i32)(intptr_t)p1, st, false);
}

// 1.13c D2Game+0x5D380 (EAX = unit, stack = p1; game = p1[1], state = p1[6]; returns 1 on spawn; ret 4)
bool D2GAME_5D380(i32* unit, uint8_t* p1)
{
    uint8_t* st = *(uint8_t**)(p1 + 0x18);
    if (st[0x88]) return false;
    return spawn79(unit, *(i32*)(p1 + 4), st, true);
}
