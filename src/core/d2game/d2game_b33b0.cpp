// src/core/d2game/d2game_b33b0.cpp -- 1.13c D2Game.dll AttachUnitAsFollower (sub_B33B0), transcribed from the cached decompile and
// its disassembly. Same shape as d2game_batch_aea40.cpp: retail 32-bit records read as raw bytes, register inputs as explicit
// parameters, callees as weak externs (Ord<N> = D2Common ordinal, D2GAME_<RVA> = D2Game internal).
#include <cstdint>
#include <cstdlib>

typedef intptr_t I;
typedef uint32_t U;
#define W extern "C" __attribute__((weak))
#define R32(b, o) (*(uint32_t*)((uintptr_t)(b) + (o)))
// Fixed signatures (stdcall thunks, stack words; the D2GAME_ ones take EAX / EBX first as noted at the calls) so an FN probe can define them.
W I Ord10331(I unit); W I Ord10973(I u, I a, I b); W I Ord10887(I u, I a, I b, I c); W I Ord10826(I room); W I Ord10014(I area);
W I Ord11056(I own, I kind); W I Ord10704(I unit); W I Ord10750(I own); W I Ord10867(I own); W I Ord10123(I unit, I kind);
W I Ord10565(I rec, I x, I y, I pat);
W I D2GAME_CF920(I owner, I unit); W I D2GAME_EA490(I unit, I owner_f0); W I D2GAME_55890(I unit, I f, I owner);
W I D2GAME_EBE70(I a, I msg, I b, I owner);
extern "C" __attribute__((weak)) U D2GAME_DAT_F31C20;   // DAT_6fd31c20, error-log enable flag
static const char kAttachErr[] = "Error attaching to unit";

// 1.13c D2Game+0xB33B0 -- EAX = owner (game-side unit holder, kept in EDI), ESI = the new unit. Returns 1, or 0 when 10331(unit) is null.
extern "C" U d2_fn_d2game_B33B0(I owner, I unit)
{
    I rec = Ord10331(unit);   // retail: the entry's `push esi` is the argument 0xA478 (10331) pops, so the unit, not the owner
    if (!rec) return 0;
    Ord10887(unit, 6, Ord10973(unit, 7, 0), 0);   // A42A (10973) result is the value A472 (10887) stores
    U kind = 0x80;
    if (unit) {
        U fl = R32(unit, 0xc4) | 0xe;
        R32(unit, 0xc4) = fl;
        if (R32(unit, 0) == 1) {
            R32(unit, 0xc4) = fl | 0x4020000;
            D2GAME_CF920(owner, unit);                // EAX = owner, EBX = unit
            D2GAME_EA490(unit, owner + 0xf0);         // EAX = unit, stack = owner+0xF0
            I area = Ord10826(rec);
            if (area) {
                I ar = Ord10014(area);                // 0xA74E
                uint8_t f = *(uint8_t*)((uintptr_t)ar + 0x30);
                if (f && D2GAME_55890(unit, (U)f, owner) == 1 && D2GAME_DAT_F31C20)
                    D2GAME_EBE70(0, (I)kAttachErr, 4, owner);   // EDX = message
            }
            kind = 0x100;
        }
    }
    Ord11056(R32(unit, 0x2c), kind);
    Ord10704(unit);
    U t = R32(unit, 0), ebx, edi;
    const bool inl = (t == 2) || (t > 3 && t <= 5);   // types 2, 4, 5 read the owner record directly
    U own = R32(unit, 0x2c);
    ebx = inl ? R32(own, 0x10) : own ? (U)Ord10750(own) : 0;
    edi = inl ? R32(own, 0xc) : own ? (U)Ord10867(own) : 0;
    Ord10565(rec, edi, ebx, Ord10123(unit, kind));   // AD00 = 10123, AD2A = 10565

    return 1;
}
