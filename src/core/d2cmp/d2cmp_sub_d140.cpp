// D2CMP frame-record walk: sprite table (EAX) + data base (EBX) + request (ECX). Returns record ptr offset or 0.
#include <cstdint>

// 1.13c D2CMP+0x0d140
// lookup = FUN_12240(param_1[0x10], mask) (global table, unmapped). *req < tbl[4] so the loop always runs.
int d2cmp_d140(const uint8_t* tbl, int base, const uint32_t* req, uint32_t (*lookup)(int, uint32_t))
{
    if (!base || !tbl) return 0;
    if (tbl[4] <= req[0]) return 0;
    uint32_t k = req[2], idx;
    if (k == 6 && (req[1] & 2)) idx = lookup((int)req[0x10], tbl[3] - 1u);
    else if (k == 2) idx = 0;
    else if (k == 4) idx = req[0x10];
    else idx = lookup((int)req[0x10], tbl[3] - 1u);
    if ((int)(tbl[3] - 1) <= (int)idx) return 0;
    int off = *(const int*)(tbl + 0xc + idx * 4);
    if (off < 0) return 0;
    int rec = 0;
    for (int i = 0; i <= (int)req[0]; i++) {
        rec = off + base;
        if (*(int*)rec != 0 || *(uint32_t*)(rec + 4) > 0x100 || *(uint32_t*)(rec + 8) > 0x100) return 0;
        off = off + 0x23 + *(int*)(rec + 0x1c) + *(int*)(rec + 0x14);
        if (off < 0) return 0;
    }
    return rec;
}
