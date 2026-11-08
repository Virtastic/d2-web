// Seam table of the 1.13c D2Game ed540 batch (d2game_batch_ed540.cpp); shared with the FN probes (src/core/fn).
#pragma once
#include <cstdint>
struct D2GameBatchED540Env {
    // ---- 0xED540 ------------------------------------------------------------------------------
    uint32_t (*ordinal10265)(uint32_t id);
    void     (*ordinal10024)(const char* module, uint32_t id);   // tail is _exit(-1)
    void*    (*ordinal11153)(uint32_t a, uint32_t b);
    uint32_t (*ordinal10826)(uint32_t v);
    uint8_t  (*ordinal10459)(uint32_t v);
    void     (*sub38620)(int32_t v);                   // 0x38620
    void     (*subEFED0)();                            // 0xEFED0
    void     (*subEBA40)(void* v);                     // 0xEBA40
    void     (*sub7B0A0)(void* edi, void* v);          // 0x7B0A0
    void     (*subEB2C0)(void* v);                     // 0xEB2C0
    void     (*sub385C0)(void* v, void* edi);          // 0x385C0
    int32_t  (*ordinal10920)(void* unit);              // the visible-argument form
    int32_t  (*ordinal10630)();
    void     (*subA2100)(void* unit, uint32_t a, void* p, uint32_t d);  // 0xA2100
    void     (*sub35090)(void* edi, void* v, int32_t one);               // 0x35090
    const uint8_t* dataTables;                         // *sgptDataTables: rows at +0xBC4, count +0xBC8
    // ---- 0x70B70 ------------------------------------------------------------------------------
    int32_t  (*ordinal10920_v)();                      // the no-visible-argument form
    void     (*sub8A3E0)(void* buf, uint32_t len);     // 0x8A3E0
    // ---- 0xD22C0 ------------------------------------------------------------------------------
    void*    (*ordinal10858_v)();
    void*    (*ordinal11034)(int32_t a, uint32_t size);
    int32_t  (*subD1A30)(void* esi);                   // 0xD1A30
    // ---- 0xD2310 ------------------------------------------------------------------------------
    void     (*ordinal10180_v)();
    void     (*ordinal10289_v)();
    void*    (*ordinal10708)(void* esi);
    int32_t  (*ordinal10787_v)();
    // ---- 0xDA9E0 ------------------------------------------------------------------------------
    int32_t  (*ordinal10989)(uint32_t v);
    // ---- 0xDAA40 ------------------------------------------------------------------------------
    int32_t  (*sub14D0)(uint32_t a, int32_t b);        // 0x14D0
    const int32_t* table1A048;                         // 0x6FD1A048
    const int32_t* table1A01C;                         // 0x6FD1A01C
    // ---- 0xDB6B0 ------------------------------------------------------------------------------
    void*    (*subE03A0)(uint32_t eax, uint32_t ecx, uint32_t edx); // 0xE03A0 (register args)
    void*    (*sub96570_v)(uint32_t ebx);              // 0x96570 (EBX = the unit's +0xC word)
    void*    (*ordinal11156)(uint32_t a, uint32_t b, int32_t c);
    int32_t  (*ordinal10448)(int32_t a, uint32_t b);
    // ---- 0x9C070 ------------------------------------------------------------------------------
    void*    (*ordinal10331_v)();
    void*    (*ordinal10331)(void* v);                 // the visible-argument form
    int16_t  (*sub9BEB0_v)();                          // 0x9BEB0
    void*    (*sub9BD00)(int16_t idx);                 // 0x9BD00
    // ---- 0xF4FE8 ------------------------------------------------------------------------------
    void     (*fdivpSTiST)();                          // 0xF4EF6 __fdivp_sti_st
    // ---- 0xF01D0 ------------------------------------------------------------------------------
    int32_t  (*ordinal10920r_v)();                     // no-visible-argument form returning the record
    // ---- 0x78B60 ------------------------------------------------------------------------------
    uint32_t (*ordinal10829)(uint32_t v);
    uint8_t  (*ordinal10020)(void* v);
    void*    (*ordinal10391)(int32_t a, void* b);
    void*    (*subD2610)(uint32_t unit, uint32_t* p, uint32_t zero);  // 0xD2610
    int32_t  (*ordinal11139)(int32_t a, uint32_t b);
    void     (*subD57F0)(void* ctx, uint32_t v);       // 0xD57F0
    void     (*ordinal11055)(uint32_t unit, uint32_t zero);
};
