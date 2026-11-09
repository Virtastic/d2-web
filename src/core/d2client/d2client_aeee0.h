// D2Client+0xAEEE0 env seam (packet handler: creates the unit through D2Client+0xB30F0 and sets its stats).
#pragma once
#include <cstdint>
struct D2cAEEE0Env {
    uint32_t  curUnit;                                                    // global DAT 0x11BBFC (6fbcbbfc)
    uintptr_t (*fB30F0)(const uint32_t* ecx_req);                         // D2Client+0xB30F0 (ECX = 0x17-dword request), returns the unit
    void      (*o10069)(uintptr_t unit, uint32_t b, uint32_t c);          // Ordinal_10069 thunk 0xC532 (3 stack words)
    void      (*o10887)(uintptr_t unit, uint32_t stat, uint32_t v, uint32_t layer); // thunk 0xC40C
    void      (*o10970)(uintptr_t unit, uint32_t w);                      // thunk 0xC2C8 (2 stack words)
    uint32_t  (*f1380)(uintptr_t ecx);                                    // D2Client+0x1380 (ECX = unit + 0x20)
};
extern "C" void d2c_aeee0(const D2cAEEE0Env* e, const uint8_t* pkt);
