// 1.13c D2Launch+0x12BA0 (sub_12BA0). Register input EAX = i; the handle from D2Win#10098 on
// &DAT_6fa60f20 + i*0x30 is appended to DAT_6fa653c0 (count DAT_6fa65e34). The overflow exit (count > 0x3f) is not reproduced.
#include <cstdint>

extern "C" uint32_t d2l_12ba0(uint32_t eax, uint32_t base, uint32_t* count, uint32_t* table, uint32_t (*imp10098)(uint32_t)) {
    uint32_t h = imp10098(base + eax * 0x30u);
    table[*count] = h;
    *count += 1;
    return h;
}
