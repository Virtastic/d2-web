// src/core/d2cmp/d2cmp_sub_d560.cpp -- 1.13c D2CMP.dll sub_D560 (ordinal 10104) from the cached decompile.
// Callees 0xCE40, 0x175C0 and the critical section are seams. No vector: needs the DLL's globals.
#include <cstdint>
#include <cstring>

struct D2CMPD560Env {
    void (*ce40)();                 // 0xCE40
    void (*c175c0)();               // 0x175C0
    uint32_t* tab0;                 // DAT_6fe77240, 15 dwords
    uint32_t* tab1;                 // DAT_6fe7727c, 13 dwords
    uint32_t* initFlag;             // DAT_6fe87ef4
    void (*enterCs)();              // EnterCriticalSection(6fe6fba8)
    void (*deleteCs)();             // DeleteCriticalSection(6fe6fba8)
};

// 1.13c D2CMP+0x0D560
extern "C" void d2cmp_sub_d560(int param_1, const D2CMPD560Env* e) {
    if (param_1) e->ce40();
    std::memset(e->tab0, 0, 15 * 4);
    if (param_1) e->c175c0();
    std::memset(e->tab1, 0, 13 * 4);
    if (*e->initFlag) {
        e->enterCs();
        *e->initFlag = 0;
        e->deleteCs();
    }
}
