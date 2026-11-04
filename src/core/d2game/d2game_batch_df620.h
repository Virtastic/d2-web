// 1.13c D2Game small unit helpers (0xDF620 .. 0xE2780): callee and global environment, shared with the function probes.
#pragma once
#include <cstdint>

struct D2GameBatchDF620Env {
    void*    (*at)(uint32_t guest);
    void     (*fatal)(uint32_t line);                        // Ordinal_10265(line); Ordinal_10024; _exit(-1)
    uint32_t bitMasksSlot;                                   // guest address 0x6FD18BE0 (pointer to the mask table)
    uint32_t tableDataTables;                                // guest address 0x6FD1829C
    // DF620
    uint32_t (*o10950)(uint32_t v);
    uint32_t (*o10331)(uint32_t unit);
    uint32_t (*o10826)(uint32_t v);
    uint32_t (*o10459)(uint32_t v);                          // result used as a byte
    uint32_t (*o10717)(uint32_t v);
    void     (*o10451)(uint32_t v);
    // E0210 / E0660
    void     (*sub20D50)(uint32_t eaxUnit, uint32_t* edxId, uint32_t* stackFlag);
    uint32_t (*sub1420)(uint32_t eaxV);
    int32_t  (*subDFA60)(uint32_t eaxUnit);                  // returns int16
    uint32_t (*sub3A0)(uint32_t eaxV, uint32_t edxV, uint32_t ecxV);
    // E0D00
    void     (*o10692)(uint32_t a, uint32_t b);
    void     (*o11003)(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t f, uint32_t g);
    void     (*sub37E20)(uint32_t ediRec, uint32_t esiGame, uint32_t a, uint32_t b);
    void     (*subAFAB0)(uint32_t eaxRec, uint32_t game, uint32_t a, uint32_t b);
    uint32_t (*subDF9A0)(uint32_t eaxRec);
    void     (*subAF920)(uint32_t eaxGame);
    void     (*subE0860)(uint32_t eaxRec, uint32_t game);
    uint32_t (*o10648)(uint32_t v);
    void     (*o10775)(uint32_t unit, uint32_t v);
    void     (*o10671)(uint32_t unit, uint32_t v);
    void     (*o11056)(uint32_t a, uint32_t b);
    uint32_t (*o10688)(uint32_t v);
    void     (*o10574)(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
    void     (*o10032)(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
    void     (*o10857)(uint32_t a, uint32_t b);
    uint32_t (*subE0A0)(uint32_t esiRec, uint32_t ediGame);
    void     (*o10158)(uint32_t unit);
    void     (*o10067)(uint32_t unit, uint32_t v);
    // E2310 / E2780
    void     (*sub8E6F0)(uint32_t eaxP, uint32_t ediGame);
    void     (*sub8EA80)(uint32_t edxUnit, uint32_t ecxGame, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t f);
    void     (*o10153)(uint32_t a, uint32_t b, uint32_t* out);
    uint32_t (*o10686)(uint32_t ctx, uint32_t x, uint32_t y, uint32_t flags);
};
