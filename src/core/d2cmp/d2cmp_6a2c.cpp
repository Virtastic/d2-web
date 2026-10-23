// 1.13c D2CMP+0x6A2C  (MSVC CRT sub-function of setmbcp: select codepage tables)
#include <cstdint>
extern uint8_t g_sbcsTable[0x101] __attribute__((weak));  // DAT_6ff13c20 (ctype flags, index c+1)
extern int32_t g_mbCodePage __attribute__((weak));        // DAT_6ff13d24
extern int32_t g_mb13c0c __attribute__((weak));           // DAT_6ff13c0c
extern int32_t g_mb13c04 __attribute__((weak));           // DAT_6ff13c04
extern uint32_t g_mbRanges[3] __attribute__((weak));      // DAT_6ff13d30..38
extern int32_t g_mbHaveFallback __attribute__((weak));    // DAT_6fe3493c
extern uint8_t g_cpFlagBits[4] __attribute__((weak));     // DAT_6fe328a8
struct CpEntry { uint32_t cp, r[3]; uint8_t ranges[4][8]; };  // 0x30 at DAT_6fe328b0 x5
extern CpEntry g_cpTable[5] __attribute__((weak));
struct CpInfo { uint32_t maxChar; uint8_t def[2]; uint8_t lead[12]; };
extern "C" int32_t GetCPInfo(uint32_t, CpInfo*) __attribute__((weak));
uint32_t d2cmp_6839(int32_t);
void d2cmp_6868();
void d2cmp_6891();

// ponytail: extraout_ECX_00 in the GetCPInfo path is an unrecovered register leftover; 0 used.
int32_t d2cmp_6a2c(uint32_t cp) {
    if (cp) {
        for (int i = 0; i < 5; i++) {
            if (g_cpTable[i].cp != cp) continue;
            for (int k = 0; k < 0x101; k++) g_sbcsTable[k] = 0;
            for (int t = 0; t < 4; t++)
                for (const uint8_t* p = g_cpTable[i].ranges[t]; p[0] && p[1]; p += 2)
                    if (p[0] <= p[1])
                        for (uint32_t c = p[0]; c <= p[1]; c++) g_sbcsTable[c + 1] |= g_cpFlagBits[t];
            g_mbCodePage = cp;
            g_mb13c0c = 1;
            g_mb13c04 = (int32_t)d2cmp_6839(cp);
            g_mbRanges[0] = g_cpTable[i].r[0]; g_mbRanges[1] = g_cpTable[i].r[1]; g_mbRanges[2] = g_cpTable[i].r[2];
            d2cmp_6891();
            return 0;
        }
        CpInfo ci;
        if (GetCPInfo(cp, &ci) == 1) {
            for (int k = 0; k < 0x101; k++) g_sbcsTable[k] = 0;
            g_mbCodePage = cp;
            g_mb13c04 = 0;
            if (ci.maxChar < 2) g_mb13c0c = 0;
            else {
                for (const uint8_t* p = ci.lead; p[0] && p[1]; p += 2)
                    for (uint32_t c = p[0]; c <= p[1]; c++) g_sbcsTable[c + 1] |= 4;
                for (uint32_t c = 1; c < 0xff; c++) g_sbcsTable[c + 1] |= 8;
                g_mb13c04 = (int32_t)d2cmp_6839(cp);
                g_mb13c0c = 0;
            }
            g_mbRanges[0] = g_mbRanges[1] = g_mbRanges[2] = 0;
            d2cmp_6891();
            return 0;
        }
        if (!g_mbHaveFallback) return -1;
    }
    d2cmp_6868();
    d2cmp_6891();
    return 0;
}
