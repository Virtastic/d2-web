// D2CMP batch b72. Skipped (locale/codepage/Win32/string-type CRT): 6891 6A2C 6BC0 780F 7D98 7FE0
#include <cstdint>
#include <initializer_list>

extern uint8_t g_sbcsTable[0x101] __attribute__((weak));  // DAT_6ff13c20
extern int32_t g_mbCodePage __attribute__((weak));        // DAT_6ff13d24
extern int32_t g_mb13c0c __attribute__((weak));           // DAT_6ff13c0c
extern int32_t g_mb13c04 __attribute__((weak));           // DAT_6ff13c04
extern uint16_t g_mbRanges[3] __attribute__((weak));      // DAT_6ff13d30..38 (3 dwords: 6 words)
extern int32_t g_mbInit __attribute__((weak));            // DAT_6ff14fb4
extern "C" void d2cmp_2dd8(int32_t) __attribute__((weak));
extern "C" int32_t d2cmp_setmbcp(int32_t) __attribute__((weak));

// 1.13c D2CMP+0x6868
void d2cmp_6868() {
    for (int i = 0; i < 0x101; i++) g_sbcsTable[i] = 0;
    g_mbCodePage = 0;
    g_mb13c0c = 0;
    g_mb13c04 = 0;
    uint32_t* r = reinterpret_cast<uint32_t*>(g_mbRanges);
    r[0] = 0; r[1] = 0; r[2] = 0;
}

// 1.13c D2CMP+0x6D07
void d2cmp_6d07() { d2cmp_2dd8(0xd); }

// 1.13c D2CMP+0x6D10
int32_t d2cmp_6d10() {
    if (g_mbInit == 0) {
        d2cmp_setmbcp(-3);
        g_mbInit = 1;
    }
    return 0;
}

// 1.13c D2CMP+0x8641
// ponytail: isspace via C-locale set (retail reads the locale ctype table, bit 8).
int32_t d2cmp_8641(const char* s) {
    auto sp = [](uint8_t c) { return c == ' ' || (c >= 9 && c <= 13); };
    while (sp((uint8_t)*s)) s++;
    uint32_t c = (uint8_t)*s++;
    uint32_t d = c;
    if (c == '-' || c == '+') d = (uint8_t)*s++;
    int32_t n = 0;
    while (d >= '0' && d <= '9') { n = (int32_t)((uint32_t)n * 10u + (d - '0')); d = (uint8_t)*s++; }
    return c == '-' ? -n : n;
}
