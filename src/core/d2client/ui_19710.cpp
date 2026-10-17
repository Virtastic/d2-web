// 1.13c D2Client+0x19710: unit-release helper. ECX=unit, EAX=edi, stack=flag.
// Callees are returned as events {rva,eax,ecx,edx,s0,s1,s2,s3} so the caller supplies the world (probe/hook log).
#include <cstdint>
extern "C" uint32_t d2_client_19710(uint32_t unit, uint32_t edi, uint32_t flag, uint32_t is_sel, uint32_t type,
                                    uint32_t sub, uint32_t sub_c, uint32_t sub_10, const uint32_t* rv,
                                    uint32_t* flags, uint32_t* ev) {
    uint32_t n = 0;
    auto E = [&](uint32_t rva, uint32_t a, uint32_t c, uint32_t d, uint32_t s0, uint32_t s1) {
        uint32_t* w = ev + n++ * 8; w[0] = rva; w[1] = a; w[2] = c; w[3] = d; w[4] = s0; w[5] = s1; w[6] = w[7] = 0;
    };
    if (unit) *flags &= 0xFFFFFFBFu;
    E(0xC43C, 0, 0, 0, unit, edi);
    if (rv[0]) {
        E(0x6A220, unit, 0, 0, 0, 0);
        E(0x2A590, 0, edi, unit, 0, 0);
        E(0x24490, unit, 0, 0, edi, 0);
    }
    if (flag && is_sel) {
        bool viaSub = type == 2 || (type > 3 && type < 6);
        uint32_t a, b;
        if (viaSub) a = sub_c; else if (!sub) a = 0; else { E(0xC3BE, 0, 0, 0, sub, 0); a = rv[1]; }
        if (viaSub) b = sub_10; else if (!sub) b = 0; else { E(0xC2EC, 0, 0, 0, sub, 0); b = rv[2]; }
        if (a && b) E(0x2A1B0, a, 0, 0, b, 0);
    }
    return n;
}
