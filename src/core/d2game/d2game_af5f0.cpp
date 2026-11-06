// 1.13c D2Game+0xAF5F0: EAX = index; below 9 (signed) the nine-entry table at DAT_6FD1B614, else (index - 2) * 0x32.
#include <cstdint>
int32_t g_af5f0(int32_t index, const int32_t* table) {
    if (index < 9) return table[index];
    return (index - 2) * 0x32;
}
