// 1.13c D2Win+0x0434E: register-convention fragment (EAX, EBP implicit). Stores [EBP+8], EAX, EBP into
// three globals (0x6F8FF870/86C/874). Wasm has no EBP; the three inputs are explicit.
#include <cstdint>
#include <emscripten.h>
static uint32_t g_6f8ff870, g_6f8ff86c, g_6f8ff874;
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_434E(uint32_t eax, uint32_t ebp, uint32_t ebp_plus8_word) {  // 1.13c D2Win+0x0434E
    g_6f8ff870 = ebp_plus8_word;
    g_6f8ff86c = eax;
    g_6f8ff874 = ebp;
}
