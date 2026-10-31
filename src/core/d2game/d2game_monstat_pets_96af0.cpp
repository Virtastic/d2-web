// 1.13c D2Game pet, monstat and unit-state helpers, rows 0x96AF0..0x9F210 (eight functions, one file).
// Host seams carry the D2Common ordinals and the D2Game callees the decompile names; they are passed in an Env in retail
// argument order. Register-only inputs (Ghidra's in_EAX / unaff_EBX / unaff_EDI / this) are explicit parameters at each
// function. Pointer-valued struct fields are read as 32-bit retail values and widened (the same convention the other
// D2Game batch files use). Fatal paths go to Env::fatal(code), which stands for Ordinal 10265 -> 10024 -> _exit(-1).
#include <cstdint>
#include <cstddef>
#include <cstring>

namespace d2g_mp {

struct Env {
    const uint8_t* tables;                                     // sgptDataTables target: +0xB98 monstat base, +0xBA0 monstat count, +0xBE8 / +0xBF0 pet tables
    void (*fatal)(uint32_t code);
    void* (*ord10920)(void* unit);                             // player data block of a unit
    uint32_t (*ord11085)(void);
    int32_t (*ord10494)(void* unit, int32_t a);
    int32_t (*ord10973)(void* unit, int32_t a, int32_t b);
    void (*ord10551)(void* unit, int32_t a, int32_t b, int32_t c);
    uint32_t (*fn2BE20)(void);
    void (*ord10510)(void);
    int32_t (*ord10121)(void);
    void (*ord11091)(void);
    int32_t (*ord10567)(void);
    void (*ord10297)(void);
    uint32_t (*fn211D0)(void* p);                              // D2Game 0x211D0 over the 8-byte record at unit+0x20
    int32_t (*fnBEFE0)(uint32_t id, uint32_t flags, void* unit, uint32_t value, int32_t one, void* a, void* b);
    void* (*fn572C0)(void* p);
    int32_t (*ord10511)(void);
    int32_t (*ord10543)(void* p);
    int32_t (*ord10601)(int32_t a, const char* file, int32_t line);
    int32_t (*ord10910)(void* p, int32_t a, int32_t b);
    void* (*ord11139)(int32_t a, int32_t b);
    int32_t (*ord10048)(void* p);
    int32_t (*ord10711)(void* p);
    uint32_t (*ord10104)(void* p);
    void* (*fn0F0)(void* unit);
    void* (*fn13220)(void* unit, uint32_t* a, int32_t b, void* c);
    void* (*fn1C90)(void* unit, uint32_t* a, void* c);
    void* (*fn13150)(void* c, uint32_t* a, int32_t b, void* unit);
    void* (*fn67B0)(void);                                     // D2Game 0x67B0: next pet unit to drop
    void (*fn7B50)(uint32_t retArg, void* ebx, void* unit);    // D2Game 0x7B50 (retArg and ebx as the decompile passes them)
};

static inline uint32_t rd32(const uint8_t* p, uint32_t o) { uint32_t v; memcpy(&v, p + o, 4); return v; }
static inline void wr32(uint8_t* p, uint32_t o, uint32_t v) { memcpy(p + o, &v, 4); }
static inline void* rdp(const uint8_t* p, uint32_t o) { return (void*)(uintptr_t)rd32(p, o); }

// 1.13c D2Game+0x96AF0 (stack: player; EDI = game; EBX = callback): walks the player's pet table (player+0x44), 12-byte rows
// from index 1 up to the pet-type count, each row a list of pet nodes (flag byte at +0, unit id at +4, next at +0x14). For each
// node whose bit 0 is clear, finds the unit with that id in the game's type-1 bucket (game+0x1320) and calls cb(unit, player).
void D2Game_96AF0_PetsForEachCallback(const Env& e, void* player, uint8_t* game, void (*cb)(void*, void*))
{
    void* pd = e.ord10920(player);
    if (!cb) { e.fatal(0x5B5); return; }
    uint8_t* pets = (uint8_t*)rdp((const uint8_t*)pd, 0x44);
    if (!pets) return;
    uint32_t count = rd32(e.tables, 0xBF0);
    if (count <= 1) return;
    uint8_t* rows = (uint8_t*)rdp(pets, 0);
    for (uint32_t i = 1; i < count; i++) {
        uint32_t off = i * 12;
        uint8_t* row = rows + off;   // the decompile's puVar5 = *pets + off
        if (!row) continue;
        for (uint8_t* node = (uint8_t*)rdp(row, 0); node; node = (uint8_t*)rdp(node, 0x14)) {
            if (node[0] & 1) continue;
            if (!game) { e.fatal(0x2D3); return; }
            uint32_t id = rd32(node, 4);
            uint8_t* u = (uint8_t*)rdp(game, 0x1320 + (id & 0x7F) * 4);
            for (; u; u = (uint8_t*)rdp(u, 0xE4)) {
                if (rd32(u, 0xC) == id) { cb(u, player); break; }
            }
        }
    }
}

// 1.13c D2Game+0x96EA0 (EAX = pet type, param_2 = value; stack: player; EBX and the return address reach the FUN_7B50 call as the decompile
// passes them): sets the row's +8 to value, then drops pets while the row's count (+4) exceeds value. The pick and remove calls are seams.
void D2Game_96EA0_PetTypeSetMaxAndTrim(const Env& e, void* player, uint32_t eax, int32_t value, uint32_t retArg, void* ebx)
{
    void* pd = e.ord10920(player);
    if (!pd) { e.fatal(0x200); return; }
    uint8_t* pets = (uint8_t*)rdp((const uint8_t*)pd, 0x44);
    if (!((eax != 1 || value == 1) && pets && (int32_t)eax >= 0 && eax < rd32(e.tables, 0xBF0))) return;
    uint8_t* row = (uint8_t*)rdp(pets, 0) + eax * 12;
    if (!row) return;
    int32_t cur = (int32_t)rd32(row, 4);
    wr32(row, 8, (uint32_t)value);
    while (value < cur && (int32_t)rd32(row, 4) > 0 && eax != 7) {
        void* u = e.fn67B0();
        e.fn7B50(retArg, ebx, u);
        cur = (int32_t)rd32(row, 4);
    }
}

// 1.13c D2Game+0x9C9A0 (this = unit type in ECX, param_1 stack, EAX = pointer): types 10..12 consult the pointer's flag word
// (type 1 with a record whose +0x16 byte has bit 1); the answer is Ordinal 11085 unless that flag vetoes it.
uint32_t D2Game_9C9A0_UnitStateFlagGuard(const Env& e, uint32_t thisType, int32_t param1, const int32_t* eax)
{
    if (thisType == 0 || (thisType != 0xA && thisType != 0xB && thisType != 0xC)) return 1;
    if (param1 != 0 && eax && eax[0] == 1 && eax[5] != 0 && (((const uint8_t*)(uintptr_t)eax[5])[0x16] & 2)) return 0;
    return e.ord11085();
}

// 1.13c D2Game+0x9D160 (EBX = unit, ESI = record of five-plus dwords, param_1 stack): dispatch on the kind at *FUN_D0F0(unit); the
// result pointer is stored at record+8 (kinds 1, 2, 4, 5). Kind 0 returns false; kind 3 and any other kind return true.
bool D2Game_9D160_ObjectCreateByKind(const Env& e, void* unit, uint32_t* esi, int32_t param1)
{
    int32_t* kind = (int32_t*)e.fn0F0(unit);
    switch (*kind) {
    case 0: return false;
    case 1: { void* r = e.fn13220(unit, (uint32_t*)(uintptr_t)esi[0], param1, esi + 5); esi[2] = (uint32_t)(uintptr_t)r; return r == nullptr; }
    case 2: { void* r = e.fn1C90(unit, (uint32_t*)(uintptr_t)esi[0], esi + 5); esi[2] = (uint32_t)(uintptr_t)r; return false; }
    case 4: { void* r = e.fn13150(esi + 5, (uint32_t*)(uintptr_t)esi[0], param1, unit); esi[2] = (uint32_t)(uintptr_t)r; return r == nullptr; }
    case 5: { void* r = e.fn13150(esi + 5, (uint32_t*)(uintptr_t)esi[0], param1, unit); esi[2] = (uint32_t)(uintptr_t)r; return false; }
    default: return true;
    }
}

// 1.13c D2Game+0x9ED20 (ECX = unit, EAX = amount): a unit of type 0 with no flag 0x72 gives up `amount` of its stat 8, if it has that much.
uint32_t D2Game_9ED20_UnitStatDrainByAmount(const Env& e, int32_t* unit, int32_t amount)
{
    if (unit && unit[0] == 0) {
        if (e.ord10494(unit, 0x72)) return e.fn2BE20();
        if (e.ord10973(unit, 8, 0) < amount) return 0;
        e.ord10551(unit, 8, -amount, 0);
    }
    return 1;
}

// 1.13c D2Game+0x9EB80 (EAX = source pointer; no stack arguments): resolves the unit that a skill event sources from, through the
// skill tables and the ordinals in the decompile. Returns 1 when the source is settled, 0 when it is not.
// assertFile is the source-file name the caller passes to the D2Common assert seam (the retail path string is not kept in the tree).
uint32_t D2Game_9EB80_SkillResolveSourceUnit(const Env& e, int32_t* src, const char* assertFile)
{
    if (!src) return 0;
    if (src[0] != 0) return 1;
    void* p2 = e.fn572C0(src);
    if (!p2) return 0;
    int32_t game = e.ord10511();
    if (!game) return 0;
    uint32_t sameFlag = 1;
    if (e.ord10543(p2)) {
        sameFlag = 0;
        if (e.ord10601(game, assertFile, 0x640) == 0 && e.ord10910(p2, 0x9D, 0)) return 1;
    }
    int32_t t = src[0x18];
    if (!t) return 0;
    void* p5 = e.ord11139(t, 4);
    if (!p5 || (sameFlag && p5 != p2) || (e.ord10048(p5) == 0 && e.ord10711(p5) == 0 && e.ord10910(p5, 0x7D, 0) == 0)) {
        p5 = e.ord11139(t, 5);
        if (!p5) return 0;
        if (sameFlag && p5 != p2) return 0;
        if (e.ord10048(p5) == 0 && e.ord10711(p5) == 0 && e.ord10910(p5, 0x7D, 0) == 0) return 0;
    }
    return e.ord10104(p5);
}

// 1.13c D2Game+0x9F1A0 (EAX = monstat id, EBX = unit): a valid monstat row adds the unit's time term to the unit's +8 field.
void D2Game_9F1A0_MonstatAddTimeOnUnit(const Env& e, int32_t id, uint8_t* unit)
{
    uint32_t count = rd32(e.tables, 0xBA0);
    uintptr_t base = (uintptr_t)rdp(e.tables, 0xB98);
    if (!(id >= 0 && (uint32_t)id < count && (uintptr_t)((uint32_t)id * 0x23C) + base != 0)) return;
    int32_t a = e.ord10567();
    e.ord10297();
    uint32_t b = e.fn211D0(unit + 0x20);
    wr32(unit, 8, rd32(unit, 8) + b + (uint32_t)a);
}

// 1.13c D2Game+0x9F210 (EAX = monstat id, EDI = unit): applies the monstat's row to the unit and returns the time term.
int32_t D2Game_9F210_MonstatApplyOnUnit(const Env& e, int32_t id, uint8_t* unit)
{
    uint32_t count = rd32(e.tables, 0xBA0);
    const uint8_t* base = (const uint8_t*)rdp(e.tables, 0xB98);
    if (!(id >= 0 && (uint32_t)id < count && (uintptr_t)((uint32_t)id * 0x23C) + (uintptr_t)base != 0)) return 0;
    const uint8_t* rec = base + (uint32_t)id * 0x23C;
    e.ord10510();
    int32_t a = e.ord10121();
    e.ord11091();
    uint32_t b = e.fn211D0(unit + 0x20);
    e.fnBEFE0((uint32_t)id, rec[0x1DC], unit, b + (uint32_t)a, 1, nullptr, nullptr);
    return (int32_t)(b + (uint32_t)a);
}

}  // namespace d2g_mp
