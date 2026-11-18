// 1.13c D2Client+0x6C3B0: EAX = esi value, stack = arg (ret 4). For i < count (re-read
// every iteration): 0x6BF30 with ESI = EAX, EDI = table[i], stack word = arg. The table is dwords at 0xE288C, the
// count the dword at 0xE28AC (table[8]). 0x6BF30 is injected.
#include <cstdint>

extern "C" void d2c_port_6c3b0(uint32_t eax, uint32_t arg, const volatile uint32_t* tab,
                               void (*c6bf30)(uint32_t esi, uint32_t edi, uint32_t arg)) {
    for (uint32_t i = 0; i < tab[8]; ++i) c6bf30(eax, tab[i], arg);
}
