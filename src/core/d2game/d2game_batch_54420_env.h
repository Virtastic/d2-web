// Seam table of the 1.13c D2Game 54420 batch (d2game_batch_54420.cpp); shared with the FN probes (src/core/fn).
#pragma once
#include <cstdint>
struct D2GameBatch54420Env {
    // sgptDataTables: the DataTables block (count at +0xA80, records at +0xA78, +0xBA0, +0xB98, +0xC4).
    const uint8_t* dataTables;
    // gdwBitMasks: the mask bytes indexed at +0x14 and +0x1C.
    const uint8_t* bitMasks;

    // 0x54420 callees.
    int32_t  (*ordinal10417)(void* self);
    int32_t  (*ordinal11041)(int32_t owner, void* self, int32_t cls);
    int32_t  (*ordinal10405)(int32_t owner, void* self, uint32_t mode);
    void     (*ordinal10058)(void* self, uint32_t cls);
    void     (*ordinal11090)(void* self, uint32_t value);
    void     (*ordinal10012)(void* self, uint32_t value);
    void     (*ordinal10892)(void* self, uint32_t a, uint32_t b);
    void     (*ordinal10050)(int32_t owner, void* self);
    void     (*ordinal10201)(void* param2, uint32_t value);
    void     (*sub345A0)();
    int32_t  (*subD4A70)(uint32_t ebx, void* param2, uint32_t value, uint32_t* out);
    int32_t  (*subD76A0)(uint32_t ebx, void* param2, uint32_t value);
    void     (*ordinal10750)(uint32_t value);
    void     (*ordinal10867)(uint32_t value);

    // 0x771D0, 0x9F2B0 and 0xA1D10 data-table probe.
    int32_t  (*ordinal10494)();

    // 0xDB910 callees.
    int32_t  (*ordinal10494Two)(uint32_t param1, uint32_t value);
    int32_t  (*ordinal10913)(uint32_t param1);
    int32_t  (*sub1450)(uint32_t esi, uint32_t selector); // 0x1450 roll on the seed at ESI (unit + 0x20)
    int32_t  (*sub1560)(uint32_t eax, uint32_t edx); // 0x1560 (EAX = unit +4 index, EDX = 3)

    // 0x777F0 callee.
    void     (*subDEA00)(uint32_t param1, void* self, void* eaxIn);

    // 0x9F2B0 callees.
    int32_t  (*subE03A0)(uint32_t a, uint32_t b);
    void     (*sub9E330)(void* param1);

    // 0x47950 callees.
    int32_t  (*ordinal10826)();
    int32_t  (*ordinal10014)(int32_t value);
    void     (*sub47820)(void* esi, int32_t value);

    // 0x53260 callees.
    int32_t  (*ordinal11017)();
    int32_t  (*ordinal10460)();
    int32_t  (*ordinal11147)(int32_t record);
    int32_t  (*ordinal10464)(int32_t record);

    // 0xA1D10 callees.
    int32_t  (*ordinal10630)();
    int32_t  (*ordinal10306)();
    void     (*subA19A0)(void* ebx, int32_t param2, int32_t value, uint32_t a, uint32_t b, uint32_t c);
    void     (*sub9F3B0)(void* ebx, void* edi);
    void     (*ordinal10945)();
    int32_t  (*ordinal10871)();
    int32_t  (*ordinal11013)(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);
    void     (*ordinal10785)(int32_t unit, int32_t value);
    void     (*ordinal10725)(int32_t unit, int32_t value);
    void     (*ordinal11081)(int32_t unit, int32_t value);
    void     (*ordinal10807)();
    void     (*ordinal10188)(int32_t unit, uint32_t stat, int32_t value, uint32_t zero);

    // 0x24BF0 callees.
    void     (*sub23260)();
    int32_t  (*getUModCount)(void* pc);
    void     (*ordinal10331)();
    void     (*sub248A0)(uint32_t ebx, uint32_t self);

    // 0x24C70 callees.
    void*    (*sub24950)(uint32_t param2, uint32_t ebx, uint32_t esi, void* param1,
                         uint32_t selfLo, uint32_t eaxLo, int32_t neg1);
    void     (*sub23CA0)(uint32_t esi, void* record, int32_t param3);

    // 0x10A20 callees.
    int32_t  (*ordinal10695)(uint32_t value);
    void     (*ordinal11050)(uint32_t unit, uint32_t value);
    void     (*ordinal10312)(uint32_t unit, uint32_t a, int32_t index);
    void     (*ordinal10525)(uint32_t unit, uint32_t a, int32_t index);
    void     (*ordinal10611)(uint32_t unit, uint32_t value);
    void     (*ordinal10883)(uint32_t unit, uint32_t value);
    uint32_t (*subEE90)(uint32_t unit);
    void     (*ordinal10875)(uint32_t unit, uint32_t value);
    int32_t  (*ordinal10710)(uint32_t unit);
    int32_t  (*ordinal10176)(uint32_t unit);
    int32_t  (*ordinal10516)(uint32_t unit);
    int32_t  (*ordinal11036)(uint32_t unit);
    int32_t  (*ordinal10106)(uint32_t unit);
    int32_t  (*ordinal10106NoArg)();
    int32_t  (*ordinal10119)(uint32_t unit);
    int32_t  (*ordinal10556)(uint32_t unit);
    void     (*ordinal10411)(uint32_t unit);
    void     (*ordinal10513)();
    int32_t  (*sub14AF0)();
    void     (*sub109B0)(void* param2);
    void     (*sub107B0)();
    void     (*ordinal10536)(uint32_t unit, uint32_t value);
    int16_t  (*ordinal10718)(uint32_t unit);
    int32_t  (*sub140B0)(uint32_t unit);
    int32_t  (*sub103C0)(void* param2, void* esi);
    int32_t  (*sub101E0)();
    int32_t  (*sub141E0)();
    int32_t  (*ordinal10865)(uint32_t unit);
    int32_t  (*ordinal10973)(uint32_t unit, uint32_t stat, uint32_t zero);
    int32_t  (*ordinal10587)(uint32_t unit, uint32_t stat, uint32_t zero);
    void     (*ordinal10887)(uint32_t unit, uint32_t stat, int32_t value, uint32_t zero);
    int32_t  (*sub104F0)();
    int32_t  (*sub154F0)();
    int32_t  (*subF370)(uint32_t a, uint32_t unit);
    int32_t  (*sub10730)(void* param2);
    int32_t  (*sub14BF0)();
    uint32_t (*sub14B30)();
    void     (*subEBB0)();
    void     (*subEC90)();
    uint32_t (*sub14B50)(uint32_t a, uint32_t b, uint32_t c);
    uint32_t (*ordinal10248)(uint32_t value);
    void     (*ordinal10996)(uint32_t a, uint32_t b, uint32_t unit, uint32_t value,
                             uint32_t c, uint32_t d);
};
