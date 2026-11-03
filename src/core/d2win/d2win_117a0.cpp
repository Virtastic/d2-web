// 1.13c D2Win+0x117A0 -- draw a multi-cell image on one row.
// ECX = this (the sprite), EDX = x (the screen x of the first cell), stack: y, align, a2, a3 (ret 0x10).
// n = cellCount(this) (D2CMP #10086). A 72-byte record is zeroed; rec[0] = cell index, rec[13] = this.
// align 1 (centre) or 2 (right): sum the cell widths (#10060 of cellData #10021) over all cells and shift x by -1 - (sum >> 1) / -sum.
// Each cell then draws at x, x + 0x100, ...: a3 == -1 calls drawCell (#10041) (rec, x, y, -1, a2, 0), else drawCellTrans (#10019) (rec, x, y, -1, a2, a3).
// The D2CMP imports are seams (src/core/fn/d2win_117a0.cpp mirrors them).
#include <cstdint>
#include <emscripten.h>

struct D2Win117A0Env {
    uint32_t (*cellCount)(void* thisp);                                   // 0x774A
    void* (*cellData)(uint32_t* rec);                                     // 0x7744
    int32_t (*cellWidth)(void* cell);                                     // 0x773E
    void (*drawCell)(uint32_t* rec, int32_t x, int32_t y, int32_t a, uint32_t b, uint32_t c);       // 0x77CE
    void (*drawCellTrans)(uint32_t* rec, int32_t x, int32_t y, int32_t a, uint32_t b, uint32_t c);  // 0x782E
};

extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2win_117a0(void* thisp, int32_t x, int32_t y, int32_t align, uint32_t a2, int32_t a3, const D2Win117A0Env* env) {
    uint32_t rec[18] = {};
    uint32_t n = env->cellCount(thisp);
    rec[13] = (uint32_t)(uintptr_t)thisp;
    if (align == 1 || align == 2) {
        uint32_t sum = 0;
        for (uint32_t i = 0; i < n; i++) { rec[0] = i; sum += env->cellWidth(env->cellData(rec)); }
        x += align == 1 ? -1 - (int32_t)(sum >> 1) : -(int32_t)sum;
    }
    for (uint32_t i = 0; i < n; i++, x += 0x100) {
        rec[0] = i;
        if (a3 == -1) env->drawCell(rec, x, y, -1, a2, 0);
        else env->drawCellTrans(rec, x, y, -1, a2, (uint32_t)a3);
    }
    return n;   // retail leaves the cell count in EAX (reloaded from its stack slot after the last draw)
}
