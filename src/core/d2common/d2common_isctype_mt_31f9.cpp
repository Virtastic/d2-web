#include <cstdint>

// 1.13c D2Common+0x031F9 -- statically linked VS2003 CRT ___isctype_mt(locinfo, c, mask).
// locinfo fields: +04 code page, +14 LCID, +48 ctype table (uint16 per char).
// Callee is a seam: the GetStringTypeA wrapper (+0x64D3), writes one uint16 type word.
typedef int32_t (*D2CommonGetStrTypeFn)(uint32_t infoType, const char* src, int32_t srcLen,
                                        uint16_t* out, uint32_t codePage, uint32_t lcid, int32_t);

extern "C" uint32_t D2Common_Isctype_mt_31f9(const uint8_t* loc, int32_t c, uint32_t mask,
                                             D2CommonGetStrTypeFn getStrType)
{
    auto u32 = [&](int o) { return *reinterpret_cast<const uint32_t*>(loc + o); };
    const uint8_t* ctype = reinterpret_cast<const uint8_t*>(uintptr_t(u32(0x48)));
    uint16_t t;
    if (uint32_t(c) + 1u < 0x101u) {
        t = *reinterpret_cast<const uint16_t*>(ctype + c * 2);
    } else {
        char in[4] = {};
        int32_t n;
        if ((ctype[1 + ((c >> 8) & 0xff) * 2] & 0x80) == 0) { in[0] = char(c); n = 1; }
        else { in[0] = char(c >> 8); in[1] = char(c); n = 2; }
        t = 0;
        if (getStrType(1, in, n, &t, u32(4), u32(0x14), 1) == 0) return 0;
    }
    return t & mask;
}
