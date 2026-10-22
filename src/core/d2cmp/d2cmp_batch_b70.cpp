// D2CMP batch b70. Skipped (SEH/CRT/direct-Win32): 6E88 56A5 26E7 27A3 2A01 2AA2 2C23 2D83 4876 4E3A 4E5D 4E7E 53C6 6330
#include <cstdint>
#include <initializer_list>

// 1.13c D2CMP+0x6839
// EAX hidden input -> explicit codepage param: ANSI codepage -> LANGID.
uint32_t d2cmp_6839(int32_t codepage) {
    if (codepage == 0x3a4) return 0x411;
    if (codepage == 0x3a8) return 0x804;
    if (codepage == 0x3b5) return 0x412;
    if (codepage != 0x3b6) return 0;
    return 0x404;
}

extern "C" int32_t crtInitCritSecAndSpinCount(void* cs, uint32_t spin) __attribute__((weak));

struct LockSlot { void* cs; int32_t preallocated; };  // DAT_6fe32060 pairs
extern LockSlot g_lockTable[0x24] __attribute__((weak));   // DAT_6fe32060
extern uint8_t g_lockPool[] __attribute__((weak));         // DAT_6fe34660, 0x18-byte sections

// 1.13c D2CMP+0x2D3A
int32_t d2cmp_2d3a() {
    uint8_t* pool = g_lockPool;
    for (int i = 0; i < 0x24; i++) {
        if (g_lockTable[i].preallocated == 1) {
            g_lockTable[i].cs = pool;
            pool += 0x18;
            if (crtInitCritSecAndSpinCount(g_lockTable[i].cs, 4000) == 0) {
                g_lockTable[i].cs = nullptr;
                return 0;
            }
        }
    }
    return 1;
}
