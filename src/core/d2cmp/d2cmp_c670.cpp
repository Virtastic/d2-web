// D2CMP sprite-cache record -> path string. out = EAX in retail; gFlag = DAT_6fe32cc8 (dword at D2CMP+0x32cc8).
#include <cstdint>
#include <cstring>

static uint8_t* c670_name(uint8_t* o, uint32_t v)
{
    for (int i = 0; i < 3; i++) {  // up to 3 chars, stop at first ' '
        uint8_t c = (uint8_t)(v >> (8 * i));
        if (c == ' ') break;
        *o++ = c;
    }
    return o;
}

// 1.13c D2CMP+0x0C670
uint32_t d2cmp_c670(char* out, uint8_t* rec, int32_t gFlag)
{
    auto u32 = [&](int off) { return *(uint32_t*)(rec + off); };
    auto mark = [&]() { if (gFlag) *(uint32_t*)(rec + 0x44) |= 1; return 1u; };
    uint32_t kind = u32(8);
    const char* s;
    switch (kind) {
    case 0: case 1: case 2: {
        uint8_t* o = (uint8_t*)out;
        for (int off = 0x18; off <= 0x28; off += 4) o = c670_name(o, u32(off));
        *o = 0;
        int32_t id = (int32_t)u32(0xc);
        if (kind == 1) {
            if (id > 0x209) {
                if (id == 0x2c0) return 1;
                if (id == 0x220 || id == 0x22f || id == 0x23a || id == 0x2c1 || id == 0x2c5) {
                    if (u32(0x10) == 0) return 1;
                }
            } else if (id == 0x209 || id == 0xf2 || id == 0xfb || id == 0x16f) {
                return 1;
            } else if (id == 0xf3 || id == 0x11c || id == 0x14d) {
                if (u32(0x10) == 0) return 1;
            }
        } else if (kind == 2) {
            if (id == 0x156 || id == 0x233) return 1;
        }
        return mark();
    }
    case 3:
        s = *(const char**)(rec + 0x2c);
        if (s) { if (gFlag) *(uint32_t*)(rec + 0x44) |= 1; strcpy(out, s); return 1; }
        break;
    case 4:
        s = *(const char**)(rec + 0x2c);
        if (s) { strcpy(out, s); return 1; }
        break;
    case 6:
        s = *(const char**)(rec + 0x2c);
        if ((rec[4] & 2) && s) {
            if (u32(0x14) != 0x7a && gFlag) *(uint32_t*)(rec + 0x44) |= 1;
            strcpy(out, s);
            return 1;
        }
    }
    *out = 0;
    return 0;
}
