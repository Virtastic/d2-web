// 1.13c D2Win+0x0B4C0 (sub_B4C0), stdcall(a, b) ret 8: DAT_6f9a9e3c = a, DAT_6f9a9e40 = D2CMP #10086(a);
// DAT_6f9a9e48 = b, DAT_6f9a9e4c = D2CMP #10086(b). The second pair reads the SECOND argument ([esp+8] after
// the stdcall #10086 has popped its own word), not the first.
#include <cstdint>

uint32_t g_d2winB4C0[4];   // DAT_6f9a9e3c, 40, 48, 4c

void d2win_sub_b4c0(uint32_t a, uint32_t b, uint32_t (*d2cmp10086)(uint32_t)) {
    g_d2winB4C0[0] = a;
    g_d2winB4C0[1] = d2cmp10086(a);
    g_d2winB4C0[2] = b;
    g_d2winB4C0[3] = d2cmp10086(b);
}
