// Alpha batch b70: D2Game+0x75CE0, 0x79730, 0x79850 from the cached 1.13c decompile; no test runner run.
// 0x7E070 / 0x7EA10 skipped (hidden stack/EBX/EBP inputs not recoverable). GUESS = arg Ghidra dropped.
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <initializer_list>
#include <unistd.h>

#define W extern "C" __attribute__((weak))
W int32_t  Ordinal_10511(void* unit);
W uint32_t Ordinal_10397(int32_t item);
W void     Ordinal_10137(int32_t item, uint32_t flags);
W void     Ordinal_10853(void* unit);
W int32_t  Ordinal_10426(void* unit);
W void*    Ordinal_10392(uint32_t id);
W intptr_t Ordinal_10750(...);
W intptr_t Ordinal_10867(...);
W intptr_t Ordinal_10331(...);  // variadic like d2game_batch_b63 (same wasm signature)
W intptr_t Ordinal_10920(...);
W intptr_t Ordinal_11090(...);
W uint32_t Ordinal_10265(int32_t id);
W void     Ordinal_10024(const void* s, uint32_t v);
W int32_t  Ordinal_10601(int32_t v, const char* file, int32_t line);
W uint32_t D2Game_74F20(void* unit);
W void     D2Game_1930(void);                     // 0xE1930
W int32_t  D2Game_E1930_ret(void);                // GUESS: returns int in 75CE0
W uint32_t D2Game_75C60(void);                    // GUESS: register args hidden
W void     D2Game_77790(void* game);
W void     D2Game_E07A0(void* game);
W void     D2Game_DED30(void* game, void* a, void* b);
W void     D2Game_A2060(void* game);
W void     D2Game_79210(void* game, void* unit, uint32_t a, int32_t b);
W void     D2Game_97730(void* game);
W void     D2Game_DFF10(void);
W void     D2Game_78230(void);
W void     D2Game_DA750(void);
W void     D2Game_54320(void);
W void     D2Game_E0750(void);
W void     D2Game_782F0(void* game, void* unit);
W void     D2Game_79100(void* game);
W void     D2Game_78A90(int32_t c);
W void     D2Game_78550(void* game, void* unit, int32_t a, int32_t b, int32_t c, int32_t d, int32_t e);
W uint8_t* sgptDataTables;
W uint32_t* gdwBitMasks;

static inline uint32_t r32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }

// 1.13c D2Game+0x75CE0
extern "C" uint32_t D2Game_75CE0(int32_t* game, int32_t* unit)
{
    int32_t item = Ordinal_10511(unit);
    if (item == 0) {
        if (D2Game_74F20(unit) != 0) {
            D2Game_1930();
            Ordinal_10853(unit);
            if (Ordinal_10426(unit) != 0) return 2;
            if (!unit || unit[4] != 0xe) { if (*((char*)unit + 0x4e) != 1) return 1; }
            else if (((uint32_t)unit[0x31] >> 6 & 1) == 0) return 1;
        }
        if (D2Game_75C60() == 0) {
            D2Game_77790(game);
            D2Game_E07A0(game);
            int32_t* o = (int32_t*)Ordinal_10392(unit[0xb]);
            if (o != unit && o) D2Game_DED30(game, unit, o);
        }
        return 1;
    }
    uint32_t f = Ordinal_10397(item);
    if (f & 1) {
        D2Game_E07A0(game);
        if (D2Game_E1930_ret() == 2) {
            Ordinal_10137(item, f | 2);
            D2Game_A2060(game);
            if (!unit || unit[4] != 0xe) {
                if (*((char*)unit + 0x4e) != 1) { Ordinal_10853(unit); return 1; }
            } else if (((uint32_t)unit[0x31] >> 6 & 1) == 0) { Ordinal_10853(unit); return 1; }
        }
    }
    D2Game_A2060(game);
    Ordinal_10853(unit);
    return 1;
}

// 1.13c D2Game+0x79730
extern "C" void D2Game_79730(int32_t* game, int32_t* unit)
{
    int32_t t = unit[0];
    int32_t a, b;
    bool pl = (t == 2 || (t > 3 && t < 6));
    if (pl) a = (int32_t)r32((void*)(uintptr_t)unit[0xb], 0x10);
    else a = unit[0xb] ? Ordinal_10750(unit[0xb]) : 0;
    if (pl) b = (int32_t)r32((void*)(uintptr_t)unit[0xb], 0xc);
    else b = unit[0xb] ? Ordinal_10867(unit[0xb]) : 0;
    Ordinal_10331(unit);
    D2Game_79210(game, unit, (uint32_t)b, a);
    D2Game_97730(game);
    uint8_t* p = 0;
    if (unit[0] == 0) p = (uint8_t*)(uintptr_t)r32((void*)Ordinal_10920(unit), 0x9c);
    p[10] |= 8;
    // GUESS: DAT_6fd31c3c is a global flag, modelled as gdwBitMasks-independent extern below
    extern uint32_t D2Game_DAT_31c3c __attribute__((weak));
    bool ok = (&D2Game_DAT_31c3c && D2Game_DAT_31c3c != 0 && (*(uint16_t*)(p + 10) & 4) == 0);
    if (!ok && unit[0] == 0 && r32((void*)Ordinal_10920(unit), 0x9c) != 0) { D2Game_54320(); ok = true; }
    if (!ok) { Ordinal_10024(nullptr, Ordinal_10265(0x9d9)); _exit(-1); }
    D2Game_E0750();
    Ordinal_11090(unit, 0x11);
    D2Game_DFF10();
    D2Game_78230();
    D2Game_DA750();
}

// 1.13c D2Game+0x79850
extern "C" void D2Game_79850(int32_t* game, int32_t* unit)
{
    int32_t t = 0;
    bool run = true;
    if (!unit) {
        t = 0;
        int32_t it = Ordinal_10511(unit);
        run = false;
        if (it) {
            int32_t idx = Ordinal_10601(it, "..\\Source\\D2Game\\PLAYER\\PlrModes.cpp", 0x778);
            uint8_t* dt = (uint8_t*)(uintptr_t)r32(sgptDataTables, 0);
            run = idx >= 0 && (int32_t)r32(dt, 0xba0) > idx;
            if (run) { uint8_t* rec = (uint8_t*)(uintptr_t)r32(dt, 0xb98) + idx * 0x23c; run = rec && (gdwBitMasks[8] & rec[6]); }
        }
    } else {
        t = unit[4];
        if (t < 7 || (t > 8 && t != 0xb)) {
            int32_t it = Ordinal_10511(unit);
            run = false;
            if (it) {
                int32_t idx = Ordinal_10601(it, "..\\Source\\D2Game\\PLAYER\\PlrModes.cpp", 0x778);
                uint8_t* dt = (uint8_t*)(uintptr_t)r32(sgptDataTables, 0);
                run = idx >= 0 && (int32_t)r32(dt, 0xba0) > idx;
                if (run) { uint8_t* rec = (uint8_t*)(uintptr_t)r32(dt, 0xb98) + idx * 0x23c; run = rec && (gdwBitMasks[8] & rec[6]); }
            }
        }
    }
    if (run) { D2Game_79100(game); D2Game_78A90(0 /* GUESS: extraout_ECX */); }
    if (t == 0) { D2Game_79730(game, unit); return; }
    if (t != 0x13) { D2Game_782F0(game, unit); return; }
    D2Game_78550(game, unit, 0, 4, -1, 0, 0);
}

// 1.13c D2Game+0x79850 (alias of the body above is the same function; kept single)
