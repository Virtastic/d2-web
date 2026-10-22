// 1.13c D2Common+0x42A70: stamp a table entry's footprint onto a tile grid. Callees are
// unpinned function-pointer globals, so they are explicit seams here.
#include <cstdint>

struct D2c42A70Seams {
    int32_t (*dataRowCount)();                      // sgptDataTables+0xC68
    const uint8_t* (*dataRow)(int32_t idx);         // table base + idx*0x1B0, nullptr if out of range
    void (*prepare)(void* ctx, int32_t idx);        // 0x42920 (hidden regs: EAX=idx, EDI=ctx)
    void (*slotA)(void* ctx, int32_t x, int32_t y); // PTR 6fdeb258
    void (*slotB)(void* ctx, int32_t x, int32_t y); // PTR 6fdeb244
    void (*slotC)(void* ctx);                       // PTR 6fdeb250
};

static int32_t rdi(const uint8_t* p, int o) { return p ? *(const int32_t*)(p + o) : 0; }

// 1.13c D2Common+0x42A70: ECX=ctx, EDX=x0, stack y0, idx, flag, extra.
void D2Common_42A70_impl(const D2c42A70Seams& s, void* ctx, int32_t x0, int32_t y0, int32_t idx, int32_t flag, int32_t extra)
{
    const uint8_t* row = (idx < 0 || idx >= s.dataRowCount()) ? nullptr : s.dataRow(idx);
    const int32_t w = rdi(row, 0x28), h = rdi(row, 0x2c);
    if (flag == -1) s.prepare(ctx, idx);
    const int32_t yEnd = ((h + (h >> 31 & 7)) >> 3) + y0;
    const int32_t xEnd = ((w + (w >> 31 & 7)) >> 3) + x0;
    for (int32_t y = y0; y < yEnd; ++y)
        for (int32_t x = x0; x < xEnd; ++x) {
            s.slotA(ctx, x, y);
            s.slotB(ctx, x, y);
            if (extra != 0 && ((idx > 3 && idx < 0x10) || (idx > 0x16b && idx < 0x178))) s.slotB(ctx, x, y);
            s.slotC(ctx);
        }
    s.slotC(ctx);
}
