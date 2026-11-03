#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
// 1.13c D2Client+0x6A0C0: EBX = monster-type index, stack = param. rec = sgptDataTables +0xA78 table + idx*0x1A8
// (idx < +0xA80); st = 0x1260(idx). For every id in the DAT_E28B0 list (count DAT_E28EC) whose bit is set in st[0xF0 + (id>>3)] (mask *(gdwBitMasks + (id&7)*4)):
// 0x69CC0(rec, id, &local{short idx, rest 0}, param).
extern "C" EMSCRIPTEN_KEEPALIVE void d2cf_6a0c0(uint32_t idx, uint32_t param, uint32_t tab, uint32_t bm, uint32_t gl, const Env* e) {
    if ((int32_t)idx < 0 || (int32_t)idx >= (int32_t)R(tab, 0xA80)) return;
    uint32_t rec = R(tab, 0xA78) + idx * 0x1A8;
    if (!rec) return;
    uint32_t st = e->call(0x1260, idx, 0, 0, 0, 0, 0, 0);
    if (!st) return;
    for (int32_t i = 0; i < (int32_t)R(gl, 0); ++i) {
        uint32_t id = R(gl, 4 + i * 4);
        if (B(bm, (id & 7) * 4) & B(st, 0xF0 + ((int32_t)id >> 3)))
            e->call4(0x69CC0, 0, 0, 0, 0, 0, rec, id, 0xB3000000u, param);
    }
}
