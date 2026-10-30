// Batch 9c4f0: 11 D2Game 1.13c functions from the cached decompile.
// Guessed inputs (register-only args): in_EAX / unaffected EDI/ESI/EBX are explicit parameters.
// 1.13c structs hold 32-bit retail pointers; they are NOT ported as host pointers. Offsets are
// read from a byte base with 32-bit pointer fields kept as uint32_t. Callees that are UNMAPPED or
// ordinals are passed in as function pointers (the caller binds them), so no link dependency.
#include <cstdint>
#include <cstring>

template <class T> static T rd(const uint8_t* b, uint32_t o) { T v; memcpy(&v, b + o, sizeof v); return v; }
template <class T> static void wr(uint8_t* b, uint32_t o, T v) { memcpy(b + o, &v, sizeof v); }

// 1.13c D2Game+0x9CA00 (in_ECX/param_1 unused)
int D2GAME_9CA00(const uint8_t* a, uint8_t* b, int (*getRoom)(const uint8_t*), int (*isFlagged)(uint8_t*),
                 int (*getVal)())
{
    if (b && rd<int>(b, 0) == 1 && a != b && getRoom(a) == getRoom(b) && isFlagged(b) == 0) {
        int v = getVal();
        if (v < rd<int>(b, 4)) { wr<uint32_t>(b, 0, (uint32_t)(uintptr_t)b); wr<int>(b, 4, v); }
    }
    return 0;
}

// 1.13c D2Game+0x9CA60 (in_EAX=a, EDI=u; 0xfa = skill id, 0xa80 = table count read via cnt)
int D2GAME_9CA60(const uint8_t* a, uint8_t* u, int cnt, int (*ord10064)(int))
{
    bool ok;
    if (a && rd<int>(a, 0) == 1 && rd<int>(a, 20) && (rd<uint8_t>((const uint8_t*)(uintptr_t)rd<uint32_t>(a, 20), 0x16) & 8)) ok = true;
    else ok = ord10064(0) != 0;
    int s = a ? rd<int>(a, 4) : -1;
    if (((a && s >= 0 && s < cnt && s == 0xfa) || ok) && rd<uint32_t>(u, 20) < 0x14) {
        uint8_t* p = (uint8_t*)(uintptr_t)rd<uint32_t>(u, 8);
        if (p && rd<int>(p, 0) == 0) {
            uint8_t* q = (uint8_t*)(uintptr_t)rd<uint32_t>(u, 0);
            if (!(rd<uint8_t>(q, 8) & 0x10)) { wr<uint8_t>(q, 8, rd<uint8_t>(q, 8) | 0x10); return 1; } // FUN_fffd0/122c0 side effects bound by caller
        }
    }
    return 0;
}

// 1.13c D2Game+0x9CCF0
int D2GAME_9CCF0(const uint8_t* unit, const uint8_t* p, uint32_t mask12, uint32_t (*ord11009)(uint32_t),
                 void (*fn)(const uint8_t*))
{
    if ((mask12 & rd<uint8_t>((const uint8_t*)(uintptr_t)rd<uint32_t>(unit, 0x1c), 0xc)) && (ord11009(rd<uint32_t>(p, 0x2c)) & 0x800)) fn(p);
    return 0;
}

// 1.13c D2Game+0x9CD90
bool D2GAME_9CD90(int p1, int (*o10494)(), void (*f122c0)(int), int (*f9ccf0)(), int (*f9cb30)(int))
{
    if (o10494() == 0) { if (f9ccf0() == 0) return f9cb30(p1) != 0; }
    else f122c0(3);
    return true;
}

// 1.13c D2Game+0x9CFE0 (callee results precomputed by the caller in order; same short-circuits)
int D2GAME_9CFE0(const uint8_t* a, int (*f9ca60)(), int (*f9cde0)(), uint32_t (*c215d0)(int), uint32_t (*c21560)(int),
                 int (*inUnit)(), uint8_t (*tableByte)(int), bool (*ce90)(), void (*act)())
{
    if (rd<int>(a, 8) == 0) return 0;
    if (f9ca60() || f9cde0()) return 1;
    if (c215d0(1) && c21560(2)) {
        if (inUnit()) {
            uint8_t b = tableByte(0x2f);
            if (b && (uint32_t)b < rd<uint32_t>(a, 20) && !ce90()) { act(); return 1; }
        }
        return 0;
    }
    return 0;
}

// 1.13c D2Game+0x9DDA0 (game ptr in_EAX; unit list at +0x74, next at +0xe8)
void D2GAME_9DDA0(const uint8_t* game, int (*o10596)(const uint8_t*), void (*f6b100)(const uint8_t*), bool* fatal)
{
    if (!game) { *fatal = true; return; }
    for (const uint8_t* u = (const uint8_t*)(uintptr_t)rd<uint32_t>(game, 0x74); u;
         u = (const uint8_t*)(uintptr_t)rd<uint32_t>(u, 0xe8))
        if (o10596(u) && !((rd<uint32_t>(u, 0xc4) >> 4) & 1)) f6b100(u);
}

// 1.13c D2Game+0x9E420 (table of 0xbf handlers at DAT_6fd274a8)
void D2GAME_9E420(int idx, uint32_t p1, uint32_t p2, void (*const* tab)(uint32_t, uint32_t))
{
    if (idx >= 0 && idx < 0xbf && tab[idx]) tab[idx](p2, p1);
}

// 1.13c D2Game+0x9E4F0 (EDI/ESI/EBX all nonzero)
void D2GAME_9E4F0(int edi, int esi, int ebx, void (*seq)())
{
    if (edi && esi && ebx) seq(); // Ordinals 10973,10228,10887,10228,10551 in that order
}

// 1.13c D2Game+0x9E570 (Skills.cpp:1099 alloc, zero 0x38 bytes, ordinal 10591 fill, 10052 return)
uint32_t D2GAME_9E570(uint32_t alloc, uint32_t unitRef, uint32_t owner, uint32_t (*o10331)(uint32_t),
                      void (*o10591)(uint32_t, uint32_t, uint32_t), void (*o10052)(uint32_t))
{
    o10591(owner, alloc, o10331(unitRef)); o10052(alloc); return alloc;
}

// 1.13c D2Game+0x9E6B0
void D2GAME_9E6B0(int (*a)(), int (*b)(), int (*c)(), int (*lim)(), void (*clamp)(), void (*fin)())
{
    if (a() < lim()) clamp();
    if (b() < lim()) clamp();
    if (c() < lim()) clamp();
    fin();
}

// 1.13c D2Game+0x9E970
int D2GAME_9E970(uint32_t p1, const uint8_t* p2, uint32_t p4, int (*o10786)(uint32_t, uint32_t, uint32_t))
{
    uint32_t base = rd<uint32_t>(p2, 0x128);
    int n = o10786(p4, base, p1);
    if (n < 6) n = 5;
    return ((rd<int>((const uint8_t*)(uintptr_t)base, 0xa8) - 1 + n) / n) * n + 1;
}
