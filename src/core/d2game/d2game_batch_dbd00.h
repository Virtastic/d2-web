// Seams of the D2Game 0xDBD00 batch (src/core/d2game/d2game_batch_dbd00.cpp): callees and tables the ports reach, supplied by the caller.
#pragma once
#include <cstdint>

struct D2GameBatchDBD00Env {
    // 0xDBD00
    int32_t  (*subDB3C0)();                                   // no visible args
    int32_t  (*sub14D0)(int32_t pct, int32_t v);
    int32_t  (*subDAD00)(uint32_t thisp);
    // 0xDC940 / 0xDCAB0
    int32_t  (*ordinal10871)(uint32_t unit);
    uint32_t (*ordinal10265)(uint32_t size);
    void     (*ordinal10024)(const char* file, uint32_t v);
    const char* assertFile;                                   // DAT_6FD19FF9
    void     (*sub8E6F0)();
    void     (*sub8EA80)(uint32_t game, uint32_t unit, uint32_t type, uint32_t frame, uint32_t a, uint32_t b, uint32_t c);
    void     (*ordinal10945)(uint32_t unit, uint32_t skill, uint32_t one);
    uint32_t (*ordinal11013)(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
    void     (*ordinal10261)(uint32_t h, uint32_t stat, int32_t val, uint32_t z);
    void     (*ordinal10785)(uint32_t h, uint32_t skill);
    void     (*ordinal10911)(uint32_t h, uint32_t fn);
    void     (*ordinal10807)(uint32_t unit, uint32_t h, uint32_t one);
    int32_t  (*ordinal10680)(int32_t a, uint32_t stat, uint32_t z);
    void     (*ordinal11043)(uint32_t game, uint32_t frame);
    void     (*ordinal10188)(uint32_t unit, uint32_t stat, int32_t val, uint32_t z);
    uint32_t subBF7A0;                                        // retail fn address 0x6FCBF7A0
    // 0xDCD80
    int32_t  (*ordinal10973)();
    uint32_t (*subDBED0)(uint32_t a, int32_t b, uint32_t edi, int32_t d, uint32_t esi);
    // 0xDEBA0
    void*    (*sub572C0)(uint32_t p);
    uint32_t (*sub32EE0)();
    uint32_t weightCount;                                     // DAT_6FD2E628
    const uint32_t* weightTable;                              // DAT_6FD2E62C: {id, weight} pairs
    uint32_t (*ordinal11139)(uint32_t a, uint32_t id);
    int32_t  (*ordinal10744)(uint32_t a, uint32_t b);
    // 0xDFDC0
    uint32_t (*ordinal10867)(uint32_t data);                   // takes the unit's data record ([unit+0x2c])
    uint32_t (*ordinal10750)(uint32_t data);
    uint32_t (*ordinal10331)(uint32_t unit);
    void     (*ordinal10561)(uint32_t h, uint32_t out[8]);          // fills a 32-byte local; DFDC0 reads words 4 and 5
    // 0xE0410
    uint32_t (*ordinal10920)(uint32_t unit);
    void     (*sub8E8C0)(uint32_t fn);
    // 0xE0570
    uint32_t (*subE03A0)(uint32_t a, uint32_t b);
    uint32_t (*ordinal10666)(uint32_t a, uint32_t b);
    int16_t  (*subBBEB0)(uint32_t a);
    // 0xE2600
    void     (*ordinal10153)(uint32_t h, uint32_t pos, uint32_t* out);
    // 0xE2900
    uint32_t (*ordinal10826)(uint32_t h);
    // 0xE2A40
    uint32_t (*sub53F30)(uint32_t id);
    int32_t  (*isBadCodePtr)(uint32_t fn);
    uint32_t (*callPtr)(uint32_t fn);
};
