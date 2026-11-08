// Seam table of the 1.13c D2Game item-use / unit-damage helpers in d2game_batch_d9bf0.cpp; shared with the FN probes (src/core/fn).
#pragma once
#include <cstdint>
struct D2GameBatchD9BF0Env {
    void*    (*at)(uint32_t guest);                                   // guest address -> host pointer
    void     (*fatal)(uint32_t line);                                 // Ordinal_10265(line); Ordinal_10024; _exit(-1)
    // D2Common ordinals
    int32_t  (*o10973)(uint32_t unit, uint32_t stat, uint32_t layer); // stat get
    void     (*o10551)(uint32_t unit, uint32_t stat, int32_t v, uint32_t layer); // stat add
    int32_t  (*o10910)(uint32_t unit, uint32_t stat, uint32_t layer); // base-stat get
    void     (*o10887)(uint32_t unit, uint32_t stat, int32_t v, uint32_t layer);
    int32_t  (*o10174)(uint32_t unit, uint32_t a, uint32_t b);
    void     (*o10593)(uint32_t unit, uint32_t a, uint32_t b);
    void     (*o10003)(uint32_t unit, uint32_t a, uint32_t b);
    int32_t  (*o11017)(uint32_t inv);
    int32_t  (*o10478)(uint32_t item);
    int32_t  (*o11088)(uint32_t item);
    uint32_t (*o10920)(uint32_t unit);                                // player data of a player unit
    uint32_t (*o10695)(uint32_t classId);                             // item record by class
    uint32_t (*o10708)(uint32_t unit, uint32_t idx, int32_t m1);      // ecx = unit, edx = idx, stack = -1
    int32_t  (*o10353)(uint32_t item);
    uint32_t (*o10507)(uint32_t unit);
    void     (*o10252)(uint32_t item, int32_t v);
    uint32_t (*o10331)(uint32_t eaxArg);                              // register argument (eax at entry of DAB00)
    void     (*o10141)(uint32_t unit, int32_t* outXY);                // writes {x,y} (8 bytes)
    void     (*o11090)(uint32_t unit, uint32_t v);
    void     (*o10408)(uint32_t unit);
    uint32_t (*o10867)(uint32_t p);
    uint32_t (*o10750)(uint32_t p);
    uint32_t (*o10927)(uint32_t unit);
    uint32_t (*o10195)(uint32_t unit, uint32_t v);
    uint32_t (*o11131)(uint32_t v);
    uint32_t (*o10473)(uint32_t v);
    int32_t  (*o10212)(uint32_t esiUnit, uint32_t v);
    int32_t  (*o10143)(uint32_t unit);
    uint32_t (*o10237)(uint32_t classByte);
    int32_t  (*o11104)(uint32_t unit);
    void     (*o10350)(uint32_t unit, int32_t* pA, int32_t* pB, int32_t* pC, uint32_t file, uint32_t line);
    int32_t  (*o10913)(uint32_t unit);
    void     (*o10689)(uint32_t unit, uint32_t ev, uint32_t z);
    uint32_t fileSUnitDmg;                                            // guest address of "..\\Source\\D2Game\\UNIT\\SUnitDmg.cpp"
    // D2Game internals named by RVA
    int32_t  (*sub1100)(uint32_t unitEcx);                            // 0x1100
    int32_t  (*sub1130)(uint32_t unitEcx);                            // 0x1130
    int32_t  (*sub14D0)(uint32_t eaxV, uint32_t edxV, uint32_t ecx100); // 0x14D0 (eax, edx, ecx)
    int32_t  (*sub1180)(uint32_t eaxN, uint32_t ecxSeedPtr);          // 0x1180
    uint32_t (*sub1020)(uint32_t eaxUnit, uint32_t game);             // 0x1020 (eax = unit, stack = game)
    void     (*sub29F0)(uint32_t unit);                               // 0xD29F0
    void     (*sub39B0)(uint32_t eaxItem, uint32_t ecxUnit, uint32_t game); // 0xD39B0
    void     (*subDFFD0)(uint32_t code, uint32_t unit);               // 0xDFFD0
    int32_t  (*sub70D60)(uint32_t ecxUnit, uint32_t game);            // 0x70D60
    int32_t  (*subAD0F0)(uint32_t game, uint32_t item, uint32_t a1, uint32_t a2); // 0xAD0F0
    int32_t  (*subD17B0)(uint32_t esiItem);                           // 0xD17B0 (esi = item)
    void     (*sub8B010)(uint32_t eaxItem, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4); // 0x8B010
    void     (*sub8C3B0)(uint32_t alCode, uint32_t clType, uint32_t edxId, uint32_t stackV);    // 0x8C3B0
    void     (*subD8DE0)(uint32_t eaxUnit, uint32_t esiV);            // 0xD8DE0 (eax = unit, stack = value)
    void     (*sub8D570)(uint32_t eaxType, uint32_t ecxIdx, uint32_t edxId, uint32_t esiV, uint32_t stackS); // 0x8D570
    void     (*subA2100)(uint32_t ecxUnit, uint32_t edxZero, uint32_t z, int32_t m1); // 0xA2100
    void     (*sub34B20)(uint32_t eaxUnit, uint32_t code);            // 0x34B20
    void     (*sub5B0B0)(uint32_t eaxUnit, uint32_t game);            // 0x5B0B0
    void     (*subA1280)(uint32_t eaxUnit, uint32_t game);            // 0xA1280
    void     (*subEAF80)(uint32_t eaxUnit);                           // 0xEAF80
    uint32_t (*sub3A0)(uint32_t eaxV, uint32_t edxV, uint32_t ecxV);  // RVA 0xE03A0 (eax, edx, ecx)
    uint32_t (*subDB030)(uint32_t eaxUnit, uint32_t arg);             // 0xDB030
    void     (*subDAE70)(uint32_t v);                                 // 0xDAE70
    void     (*subDB570)(uint32_t v);                                 // 0xDB570
};
