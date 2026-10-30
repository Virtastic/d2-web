// src/core/d2client/d2client_a4b60_env.h -- host seams (D2ClientA4b60Env) shared by the 1.13c D2Client batch and the 0xA4B60 replay probe.
// in d2client_batch_a4b60.cpp and the 0xA4B60 replay probe.
#pragma once
#include <cstdint>

struct D2ClientA4b60Env {
    uint32_t  (*rd32)(uintptr_t addr);
    uintptr_t (*rd_ptr)(uintptr_t addr);
    void      (*wr32)(uintptr_t addr, uint32_t v);
    // Fog!#10265(line) + Fog!#10024 + _exit(-1): the assertion-failure path. Does not return.
    void      (*fatal)(uint32_t line);
    int32_t   (*o10494)();                              // 0xBF80-area thunk used by 0xA4B90
    int32_t   (*o10511)();                              // 0xC640
    uintptr_t (*o10858)();                              // 0xC310
    uintptr_t (*o10093)(uintptr_t a, uint32_t b);       // 0xC214
    int32_t   (*o10014)(uintptr_t a);                   // 0xC1A2
    int32_t   (*o10750)(uintptr_t a);                   // unit y
    int32_t   (*o10867)(uintptr_t a);                   // unit x
    void      (*o10739)(uintptr_t a, int32_t x, int32_t y, uint32_t z);
    void      (*o11090)();
    void      (*o11087)(int32_t* x, int32_t* y);
    uintptr_t (*o10331)(uintptr_t unit);                // 0xC1F0
    uintptr_t (*o10633)(uintptr_t a);
    void      (*o10345)(uintptr_t a);
    void      (*o10184)(uintptr_t a);
    void      (*o10704)();
    uint32_t  (*a4a90)(uintptr_t unit, uintptr_t arg);  // 0xA4A90
    void      (*a4da0)(uint32_t table_rva);             // 0xA4DA0
    // Globals (RVA): heads 0x10A608 / 0x10A808 (2 x 0x80 uintptr_t), cur unit ptr 0x11BBFC,
    // sgptDataTables, fn table 0xDE838, count 0xDE854.
    const uintptr_t* heads[2];
    uintptr_t cur;
    uintptr_t data_tables;
    uint32_t  nclass;
    uint32_t  (*classify[64])();
};
