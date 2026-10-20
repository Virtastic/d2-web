// Implementation batch 5d1d0: two twin helpers spawning object 0x79 (as 0x3c-class room object) near a unit,
// written from the cached 1.13c decompiles. Callees by retail RVA (rNNNN) / ordinal (oNNNNN); in_EAX /
// unaff_* registers are explicit parameters. Unverified seams: register args of o10331/o10346/o10111/o10258,
// and the state struct the decompile's garbled "uVar10+0x89" tail writes to (taken as the struct already
// tested: piVar5[6] / param_1+0x18). RE-DONE only, not linked or vectored.
#include <cstdint>
#include <cstdlib>
using i32 = int32_t; using u32 = uint32_t;
extern "C" {
i32 o10331(i32); i32 o10826(i32); char o10459(i32); i32 o10750(i32); i32 o10867(i32); i32 o10265(i32);
void o10024(const void*, i32); i32 r_21280(); i32* r_E1D90(i32, i32, uint8_t, i32, i32);
void o11090(i32*, i32); i32* r_A22E0(i32, i32*, i32*); uint8_t o10258(i32*); void o10111(i32*); void o10346(i32, i32);
}
static i32 unitX(i32* u) { return (*u == 2 || (*u > 3 && *u < 6)) ? *(i32*)(intptr_t)(u[0xb] + 0x10) : (u[0xb] ? o10750(u[0xb]) : 0); }
static i32 unitY(i32* u) { return (*u == 2 || (*u > 3 && *u < 6)) ? *(i32*)(intptr_t)(u[0xb] + 0xc) : (u[0xb] ? o10867(u[0xb]) : 0); }
[[noreturn]] static void die(i32 line) { o10024((void*)0, o10265(line)); std::_Exit(-1); }

// shared tail: spawn 0x79 at (x+5,y+10) in room, link it, return false if creation failed
static bool spawn79(i32* unit, i32 room, uint8_t* st, bool full)
{
    i32 x = unitX(unit), y = unitY(unit);
    i32 owner = o10331(0);
    if (!owner) die(0xe42);
    o10459(0x79);
    char a = o10459(0x79), b = o10459(o10826(owner));
    if (a != b) die(0xe5c);
    i32 h = r_21280();
    if (!h) return false;
    i32* o = r_E1D90(2, 0x3c, (uint8_t)(y + 10), x + 5, room);
    o11090(o, 1);
    o = r_A22E0(room, o, (i32*)(intptr_t)0x79);
    if (!o) return false;
    o10258(o); o10111(o); o10346(h, 0); o10346(o10331(0), 0); // unverified arg order
    if (full) { st[0x88] = 1; st[0x87] = 0; } else st[0x89] = 0;
    return true;
}

// 1.13c D2Game+0x5D1D0 (eax = unit, param_1 = game/owner with list at +0x10f4)
void D2GAME_5D1D0(i32* unit, uint8_t* p1)
{
    i32* it = nullptr;
    if (*(i32***)(p1 + 0x10f4)) for (it = **(i32***)(p1 + 0x10f4); it; it = (i32*)(intptr_t)it[0x3d]) if (*it == 0x22) break;
    uint8_t* st = (uint8_t*)(intptr_t)it[6];
    if (!st[0x89]) return;
    if (o10459(o10826(o10331(0))) != 4) return;
    spawn79(unit, (i32)(intptr_t)st, st, false); // room arg = piVar5[6] (iVar1) per decompile
}

// 1.13c D2Game+0x5D380 (eax = unit, cl = unaff_retaddr char, param_1 = struct)
bool D2GAME_5D380(i32* unit, uint8_t* p1)
{
    uint8_t* st = *(uint8_t**)(p1 + 0x18);
    if (st[0x88]) return false;
    return spawn79(unit, *(i32*)(p1 + 4), st, true);
}
