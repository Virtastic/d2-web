// src/core/d2cmp/d2cmp_2c23.cpp -- 1.13c D2CMP.dll 0x2C23 (DllMain CRT entry), from the cached Ghidra decompile.
// Alpha lane. Seams: crtInit = __CRT_INIT_12, userEntry = DAT_6ff14fa4 (may be null), body = 0x14EB0 (returns 1), loaded = DAT_6fe34648.
#include <cstdint>

struct D2CMP2C23Env {
    int (*crtInit)(void* inst, int reason);
    int (*userEntry)(void* inst, int reason, void* res);
    int (*body)(void* inst, int reason, void* res);
    int loaded;
};

// 1.13c D2CMP+0x02C23
extern "C"
int d2cmp_2c23(const D2CMP2C23Env* e, void* inst, int reason, void* res) {
    int r = 1;
    if (reason == 0 && e->loaded == 0) return 0;
    if (reason == 1 || reason == 2) {
        if (e->userEntry) r = e->userEntry(inst, reason, res);
        if (r == 0 || e->crtInit(inst, reason) == 0) return 0;
    }
    r = e->body(inst, reason, res);
    if (reason == 1 && r == 0) e->crtInit(inst, 0);
    if (reason == 0 || reason == 3) {
        if (e->crtInit(inst, reason) == 0) r = 0;
        if (r != 0 && e->userEntry) r = e->userEntry(inst, reason, res);
    }
    return r;
}
