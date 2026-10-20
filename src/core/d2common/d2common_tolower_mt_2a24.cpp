#include <cstdint>

// 1.13c D2Common+0x02A24 -- statically linked VS2003 CRT ___tolower_mt(locinfo, c).
// locinfo fields: +04 code page, +14 LCID, +24 C-locale-ctype flag, +28 mb_cur_max,
// +48 ctype table. Callees are seams: ___isctype_mt (+0x31F9) and the LCMapString
// wrapper (+0x4EF4); the latter writes up to 3 bytes into out.
typedef int32_t (*D2CommonIsctypeFn)(const uint8_t* loc, uint32_t c, int32_t mask);
typedef int32_t (*D2CommonLcMapFn)(uint32_t lcid, uint32_t flags, const char* src, int32_t srcLen,
                                   uint8_t* out, int32_t outLen, uint32_t codePage, int32_t);

extern "C" uint32_t D2Common_Tolower_mt_2a24(const uint8_t* loc, uint32_t c,
                                             D2CommonIsctypeFn isctype, D2CommonLcMapFn lcmap)
{
    auto u32 = [&](int o) { return *reinterpret_cast<const uint32_t*>(loc + o); };
    if (u32(0x14) == 0 || (u32(0x24) != 0 && c < 0x80))
        return (int32_t(c) > 0x40 && int32_t(c) < 0x5b) ? c + 0x20 : c;
    const uint8_t* ctype = reinterpret_cast<const uint8_t*>(uintptr_t(u32(0x48)));
    if (c < 0x100) {
        const uint32_t upper = int32_t(u32(0x28)) < 2 ? (ctype[c * 2] & 1) : uint32_t(isctype(loc, c, 1));
        if (!upper) return c;
    }
    char in[4] = {};
    int32_t n;
    if ((ctype[1 + ((c >> 8) & 0xff) * 2] & 0x80) == 0) { in[0] = char(c); n = 1; }
    else { in[0] = char(c >> 8); in[1] = char(c); n = 2; }
    uint8_t out[4] = {};
    const int32_t r = lcmap(u32(0x14), 0x100, in, n, out, 3, u32(4), 1);
    if (r == 0) return c;
    if (r != 1) return (uint32_t(out[0]) << 8) | out[1];
    return out[0];
}
