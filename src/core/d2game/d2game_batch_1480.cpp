// src/core/d2game/d2game_batch_1480.cpp -- twelve 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c), in RVA order: the
// multiplicand RNG roll (0x1480), the stat setter with gold/stash caps (0x1670), the unique-item
// special-flag test (0x17E0), the MonStats base-id and MonProp accessors (0x1870, 0x19F0), the
// variable-width bit writer (0x1CD0), the nNextInClass chain walk (0x1D30), the CRT critical-section
// initialiser (0x1DF0), the SEH-wrapped TSExplicitList inplace-new (0x26F0), the monster-type bit
// test (0x2790) and the CRT _lock-wrapper and __stricmp (0x28E5, 0x2813). Stage A batch lane
// (OBJECTIVE v9): implementation + marker only, no spec, no vector.
//
// Host model: a unit, an item, a bit buffer or a data-table record is opaque and addressed by retail
// 32-bit byte offsets (rd8/rd16/rd32/rdptr). Register-only arguments (Ghidra's in_EAX/in_ECX/
// unaff_EBX/unaff_EDI) are explicit parameters here and are named in each function's comment. The
// resolved sgptDataTables block and the gdwBitMasks image are Env pointers because retail reaches
// them through the D2Game+0xF829C / +0xF8BE0 slot chain; every callee outside this batch is a seam
// pointer on D2GameBatch17Env in retail argument order. The fatal path Fog #10265(id) -> #10024(text)
// -> _exit(-1) is not reached by any of these functions.
// Prior art: D2MOO 1.10f reference/D2MOO/source/D2Game/src/PLAYER/Player.cpp
// (D2GAME_SetStatOrResetGold_6FC7CA70, 0x6FC7CA70 = this 0x1670), MONSTER/Monster.cpp
// (MONSTER_GetMonPropTxtRecord), GAME/Game.cpp
// (D2GameDataTable_TSExplicitList_InplaceNew_WithList, 0x6FC81E70 = this 0x22740); the RNG step is
// the same multiplier as src/core/fn/d2game_1450.cpp (0x6AC690C5), and the CRT routines share
// src/core/d2game/d2game_batch_2a6d.cpp's model of __lock.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <Windows.h>   // CRITICAL_SECTION no-op

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }

// Host seams for the callees and module tables the cached decompiles name. Retail RVAs are in the
// comments; the D2Common/Fog ordinals are 1.13c.
struct D2GameBatch17Env {
    // sgptDataTables (D2Game+0xF829C slot -> variable -> D2DataTablesStrc): MonStats rows at +0xA78,
    // count at +0xA80 (stride 0x1A8); UniqueItems rows at +0xC24, count at +0xC28 (stride 0x14C);
    // MonProp rows at +0xC30, count at +0xC34 (stride 0x138); the monster-type bit table at +0xC44
    // with its row stride at +0xC48 and its bound at +0xC40.
    const uint8_t* tables;
    // gdwBitMasks (D2Game+0xF8BE0 slot, 32 dwords, 1 << i; retail indexes it by byte offset).
    const uint8_t* bitMasks;
    int32_t  (*getUnitStat)(void* unit, int32_t stat, int32_t layer);              // 0xA42A D2Common #10973
    void     (*setUnitStat)(void* unit, int32_t stat, int32_t value, int32_t layer); // 0xA472 D2Common #10887
    int32_t  (*getInventoryGoldLimit)(void* unit);                                 // 0xA45A D2Common #10049
    int32_t  (*getStashGoldLimit)(void* unit);                                     // 0xA4A8 D2Common #11060
    void     (*setFinalStat)(void* unit, int32_t stat, int32_t value, int32_t layer); // 0xA4D2 D2Common #10551
    int32_t  (*getItemQuality)(void* unit);                                        // 0xA898 D2Common #10106
    int32_t  (*getItemUniqueId)(void* unit);                                       // 0xA874 D2Common #10620
    void*    (*uniqueItemsRow)(int32_t uniqueId);                                  // 0x16F0 (eax = id)
    void     (*bitWrite)(void* buffer, uint32_t value, uint32_t bits);             // 0xA2CE Fog #10128
    void     (*inplaceNew)(void* node, const void* list);                          // 0x22740
    void     (*lock)(int32_t file);                                                // 0x402B __lock
    // CRT __stricmp's tiddata/locale path (0x2813). PTR_DAT_6FD1F0CC is the startup ptlocinfo.
    void*       (*getptd)();                                                       // 0x3B93
    void*       (*updateTlocinfo)();                                               // 0x3B1F
    int32_t     (*asciiStricmp)(const char* a, const char* b);                     // 0x3940
    int32_t     (*tolowerMt)(void* locinfo, uint32_t ch);                          // 0x3853
    const void* currentTlocinfo;
};

// 1.13c D2Game+0x01480
// Multiplicand RNG roll: ECX = the 64-bit state {lo,hi}, ESI = count, EDI = base. count <= 0 returns
// base without advancing the state; otherwise one LCG step (state = lo*0x6AC690C5 + hi, both halves
// stored back) and the result is base plus lo % count, or base plus (count-1 & lo) when count is a
// power of two.
int32_t d2game_roll_1480(uint32_t* seed, int32_t count, int32_t base)
{
    if (count < 1) return base;
    const uint64_t v = (uint64_t)seed[0] * 0x6AC690C5u + seed[1];
    seed[0] = (uint32_t)v;
    seed[1] = (uint32_t)(v >> 32);
    const uint32_t lo = (uint32_t)v;
    if ((count & (count - 1)) != 0)
        return (int32_t)(lo % (uint32_t)count) + base;
    return (int32_t)(((uint32_t)count - 1) & lo) + base;
}

// 1.13c D2Game+0x1670
// Add delta to a stat. ESI = unit, EBX = stat, stack = delta. A null unit is ignored. The result is
// the current value (D2Common #10973) plus delta; a negative result stores 0. For a player
// (unit+0 type 0), stat 0xE (gold) and 0xF (stash gold) additionally store 0 when the result exceeds
// the matching gold limit; every other path writes the result through D2Common #10551.
void d2_fn_d2game_1670(void* unit, int32_t stat, int32_t delta, const D2GameBatch17Env* env)
{
    if (!unit) return;
    const int32_t value = env->getUnitStat(unit, stat, 0) + delta;
    if (value < 0) { env->setUnitStat(unit, stat, 0, 0); return; }
    if (rd32(unit, 0) == 0) {                              // unit type 0 = player
        if (stat == 0xE) {
            if (env->getInventoryGoldLimit(unit) < value) { env->setUnitStat(unit, stat, 0, 0); return; }
        } else if (stat == 0xF) {
            if (env->getStashGoldLimit(unit) < value) { env->setUnitStat(unit, stat, 0, 0); return; }
        }
    }
    env->setFinalStat(unit, stat, value, 0);
}

// 1.13c D2Game+0x17E0
// ESI = unit. True only for an item (unit+0 type 4) of quality 7 (unique) with a non-negative
// D2Common #10620 unique id whose UniqueItems row exists and has the bit at byte +0x2C that retail
// tests with the byte at gdwBitMasks+8 (mask 0x04). #10620 is called twice, as in retail.
uint32_t d2game_17e0_impl(void* unit, const D2GameBatch17Env* env)
{
    if (!unit || rd32(unit, 0) != 4) return 0;
    if (env->getItemQuality(unit) != 7) return 0;
    if (env->getItemUniqueId(unit) < 0) return 0;
    const void* row = env->uniqueItemsRow(env->getItemUniqueId(unit));
    if (!row) return 0;
    return (rd8(row, 0x2C) & rd8(env->bitMasks, 8)) != 0;
}

// 1.13c D2Game+0x1870
// EAX = monster class id. Returns the MonStats row's int16 at +2 (nBaseId), or the id unchanged when
// it is negative, out of range (>= the MonStats count), or the row address is null. The cached
// decompile shows an empty body for this RVA; the retail bytes at entry 0x6FC21870 are the accessor
// above (objdump-verified), so the implementation follows the bytes.
int32_t d2_fn_d2game_1870(int32_t monsterId, const D2GameBatch17Env* env)
{
    const uint8_t* dt = env->tables;
    if (monsterId < 0 || monsterId >= (int32_t)rd32(dt, 0xA80)) return monsterId;
    const uint32_t row = rd32(dt, 0xA78) + (uint32_t)monsterId * 0x1A8;
    if (!row) return monsterId;
    return (int16_t)rd16((const void*)(uintptr_t)row, 0x2);
}

// 1.13c D2Game+0x19F0
// EAX = monster class id. Returns the MonProp row (0x138 stride) selected by the MonStats int16 at
// +0x1A (wMonProp), or null when the class id is negative/out of range, the class row is null, the
// property id is negative/out of range, or the base row address is null.
void* d2_fn_d2game_19f0(int32_t monsterId, const D2GameBatch17Env* env)
{
    const uint8_t* dt = env->tables;
    if (monsterId < 0 || monsterId >= (int32_t)rd32(dt, 0xA80)) return nullptr;
    const uint32_t row = rd32(dt, 0xA78) + (uint32_t)monsterId * 0x1A8;
    if (!row) return nullptr;
    const int16_t prop = (int16_t)rd16((const void*)(uintptr_t)row, 0x1A);
    if (prop < 0 || prop >= (int32_t)rd32(dt, 0xC34)) return nullptr;
    return (void*)(uintptr_t)(rd32(dt, 0xC30) + (uint32_t)prop * 0x138);
}

// 1.13c D2Game+0x1CD0
// Variable-width bit writer: ESI = bit buffer, EDI = value. Below 0x100 write flag 0 then 8 bits;
// below 0x10000 write flag 1, flag 0, then 16 bits; otherwise write flag 1, flag 1, then 32 bits.
// Fog #10128 BITMANIP_Write takes (buffer, value, bit count).
void d2_fn_d2game_1cd0(void* buffer, uint32_t value, const D2GameBatch17Env* env)
{
    if (value < 0x100) {
        env->bitWrite(buffer, 0, 1);
        env->bitWrite(buffer, value, 8);
    } else if (value < 0x10000) {
        env->bitWrite(buffer, 1, 1);
        env->bitWrite(buffer, 0, 1);
        env->bitWrite(buffer, value, 16);
    } else {
        env->bitWrite(buffer, 1, 1);
        env->bitWrite(buffer, 1, 1);
        env->bitWrite(buffer, value, 32);
    }
}

// 1.13c D2Game+0x01d30
// Walk the MonStats nNextInClass chain: ECX = starting class id, EBX = step limit. Returns -1 when
// the start is negative, out of range or its row address is null. Otherwise it starts at the row's
// int16 +2 (nBaseId) and, while fewer than EBX steps have been taken, moves to int16 +4
// (nNextInClass); a negative or out-of-range next id, or a null next row address, is returned as is.
int32_t d2_fn_d2game_1d30(int32_t monsterId, int32_t maxSteps, const D2GameBatch17Env* env)
{
    const uint8_t* dt = env->tables;
    if (monsterId < 0) return -1;
    const int32_t count = (int32_t)rd32(dt, 0xA80);
    if (monsterId >= count) return -1;
    const uint32_t base = rd32(dt, 0xA78);
    uint32_t row = base + (uint32_t)monsterId * 0x1A8;
    if (!row) return -1;
    int32_t id = (int16_t)rd16((const void*)(uintptr_t)row, 0x2);
    for (int32_t i = 0; i < maxSteps; ++i) {
        id = (int16_t)rd16((const void*)(uintptr_t)row, 0x4);
        if (id < 0 || id >= count) return id;
        row = base + (uint32_t)id * 0x1A8;
        if (!row) return id;
    }
    return id;
}

// 1.13c D2Game+0x1DF0
// Initialise and return the CRITICAL_SECTION passed in ECX; the initialiser is a host no-op.
void* d2_fn_d2game_1df0(void* criticalSection)
{
    InitializeCriticalSection((LPCRITICAL_SECTION)criticalSection);
    return criticalSection;
}

// 1.13c D2Game+0x26F0
// SEH wrapper: ECX = list, stack = node. When the node is non-null, link it onto the list through
// 0x22740 (D2MOO's D2GameDataTable_TSExplicitList_InplaceNew_WithList: node+0 := list link offset,
// node+4 := &node+4, node+8 := ~&node+4). The host has no SEH chain to install or restore.
void d2_fn_d2game_26f0(void* list, void* node, const D2GameBatch17Env* env)
{
    if (node) env->inplaceNew(node, list);
}

// 1.13c D2Game+0x2790
// EAX = unit, EDI = row index. Zero unless the unit is a monster (type 1) whose MonStats record has
// a non-negative wMonType (+0x1C) below the bit-table bound at dt+0xC40 and the index is also in
// [0, that bound). Returns the dword at dt[0xC44] + (dt[0xC48]*index + (wMonType>>5))*4 ANDed with
// gdwBitMasks[wMonType & 0x1F] (dword-indexed, i.e. 1 << (wMonType & 0x1F)).
uint32_t d2_fn_d2game_2790(const void* unit, int32_t index, const D2GameBatch17Env* env)
{
    if (!unit || rd32(unit, 0) != 1) return 0;
    const int32_t monsterId = (int32_t)rd32(unit, 4);
    const uint8_t* dt = env->tables;
    if (monsterId < 0 || monsterId >= (int32_t)rd32(dt, 0xA80)) return 0;
    const uint32_t row = rd32(dt, 0xA78) + (uint32_t)monsterId * 0x1A8;
    if (!row) return 0;
    const int32_t monType = (int16_t)rd16((const void*)(uintptr_t)row, 0x1C);
    const int32_t bound = (int32_t)rd32(dt, 0xC40);
    if (monType < 0 || monType >= bound) return 0;
    if (index < 0 || index >= bound) return 0;
    const uint32_t word = rd32((const void*)(uintptr_t)rd32(dt, 0xC44),
                               (rd32(dt, 0xC48) * (uint32_t)index + ((uint32_t)monType >> 5)) * 4);
    return word & rd32(env->bitMasks, ((uint32_t)monType & 0x1F) * 4);
}

// 1.13c D2Game+0x2813 (__stricmp)
// Case-insensitive string compare through the CRT thread data: the ptlocinfo at tiddata+0x64 is
// refreshed through 0x3B1F when it is not the startup locale; with a null wlocale (C locale,
// locinfo+0x14) it forwards to 0x3940 ___ascii_stricmp, else it folds each byte with 0x3853
// ___tolower_mt and returns the first difference, stopping at a zero fold (0 - other on a prefix).
int32_t d2_fn_d2game_2813(const char* str1, const char* str2, const D2GameBatch17Env* env)
{
    void* locinfo = rdptr(env->getptd(), 0x64);
    if (locinfo != env->currentTlocinfo) locinfo = env->updateTlocinfo();
    if (!rdptr(locinfo, 0x14)) return env->asciiStricmp(str1, str2);
    for (;;) {
        const int32_t a = env->tolowerMt(locinfo, (uint8_t)*str1++);
        const int32_t b = env->tolowerMt(locinfo, (uint8_t)*str2++);
        if (a == 0 || a != b) return a - b;
    }
}

// FN-probe seam for 0x2813: C-locale tiddata (locinfo wlocale null) so the call forwards to 0x3940.
int d2_fn_d2game_3940(const char*, const char*);
int32_t d2game_stricmp_probe(const char* a, const char* b)
{
    static uint8_t locinfo[0x40];
    D2GameBatch17Env env = {};
    env.getptd = []() -> void* { static uint8_t ptd[0x80]; *(void**)(ptd + 0x64) = locinfo; return ptd; };
    env.updateTlocinfo = []() -> void* { return locinfo; };
    env.asciiStricmp = d2_fn_d2game_3940;
    return d2_fn_d2game_2813(a, b, &env);
}

// 1.13c D2Game+0x28E5
// CRT _lock wrapper: acquire file lock 8 (0x402B __lock, which lazily creates the lock then enters it).
void d2_fn_d2game_28e5(const D2GameBatch17Env* env)
{
    env->lock(8);
}
