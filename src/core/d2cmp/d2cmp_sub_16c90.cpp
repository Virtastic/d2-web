#include <cstdint>
#include <cstring>
uint32_t d2cmp_12240(int k, uint32_t mask);

static uint32_t r32(const uint8_t* p, int o) { uint32_t v; memcpy(&v, p + o, 4); return v; }

// 1.13c D2CMP+0x16C90
// Register-ABI in retail: ecx=ctx, edi=cel/frame header, esi=out divisor. Returns size / *divisor.
uint32_t d2cmp_16c90(const uint8_t* ctx, const uint8_t* edi, uint32_t* esi)
{
    uint32_t v = 0, mode = r32(ctx, 8);
    *esi = 0;
    if (mode == 0) {
        uint32_t f = r32(ctx, 0x10);
        *esi = (f == 0 || f == 0x11 || (ctx[0x44] & 2)) ? 1 : 8;
        if (!(edi[0] & 1)) v = d2cmp_12240((int)r32(ctx, 0x40), r32(edi, 0x14));
    } else if (mode == 1) {
        if (edi[0] & 1) return 0;
        *esi = r32(edi, 0x14) != 8 ? 1 : 4;
        v = d2cmp_12240((int)r32(ctx, 0x40), r32(edi, 0x14));
    } else {
        *esi = 1;
        if (mode == 4) return r32(ctx, 0x40);
        if (mode == 3 || (mode == 6 && (ctx[4] & 2))) {
            if (!(edi[0] & 1)) v = d2cmp_12240((int)r32(ctx, 0x40), r32(edi, 0x14));
        }
        return v;
    }
    return *esi ? v / *esi : 0;
}
